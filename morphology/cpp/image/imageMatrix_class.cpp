#include <fstream>
#include <iostream>
#include <vector>
#include <cassert>
#include <string>
#include <boost/tuple/tuple.hpp>
#include <boost/multi_array.hpp>
#include <eigen3/Eigen/Dense>
#include "imageMatrix_class.h"
#include "Spiral.h"


// using namespace std;
using boost::tuple;
using namespace Eigen;



//////////////////////////////////////////////////////////////////////////
//                       Loading the matrix


IM:IM(const char* fname, const char* sfile, std::vector<int> ddims) {
  fileName = fname;
  skelfile = sfile;
  dims = ddims;
}



// Load the matrix
// This method takes the specific image file name and the dims,
// it returns the 2-D <int> array (which is then turned into an eigen matrix).
int** get_matrix(const char* imname, std::vector<int> dims) {
  ifstream file(fileName);
  int** data = 0;
  data = new int*[dims[0]];
  for (int i = 0; i < dims[0]; i++) {
    data[h] = new int[dims[1]];
    for (int j = 0; j < dims[1]; j++) {
      file >> data[i][j];
    }
  }
  return data;
}



// Turn the matrix into an eigen matrix
// This method is independent of IM objects *even though* it uses
// dims objects; the dims vector must be passed explicitly.
MatrixXi eigen_matrix(int** data, std::vector<int> dims) {
  MatrixXi mat(dims[0], dims[1]);
  // Assume row-major order
  for (int i = 0; i < dims[0]; i++) {
    for (int j = 0; j < dims[1]; j++) {
      mat(i,j) = data[i][j];
    }
  }
  return mat;
}



// Load the stack from the file-list (fileName)
void IM::get_stack() {
  // Get the file list from fileName
  std::vector<string> files;
  ifstream file(fileName);
  while (file) {
    string tmp;
    file >> tmp;
    files.push_back(tmp); // This vector is only 100-500 items
  }
  // Add each image matrix to the stack
  stack(boost::extents[dims[0]][dims[1]][dims[2]]); // Not sure this will work
  int numfiles = files.size() - 1;
  cout << "Looking for " << dims[2] << "files.\n";
  cout << "Looks like there are " << files.size() << endl;
  int cnt = 0;
  for (int k = 0; k < numfiles; k++) {
    string tmpstr = files[k];
    char* fname = new char[tmpstr.size() + 1];
    strcpy(fname, tmpstr.c_str());
    int** tmp = get_matrix(fname, dims);
    // Add this slice to the stack object
    for (int i = 0; i < dims[0]; i++) {
      for (int j = 0; j < dims[1]; j++) {
        stack[i][j][k] = tmp[i][j];
      }
    }
    cnt++;
  }
  cout << "Should have " << dims[2] << "slices; found " << cnt << " files.\n";
}



// Load the skeleton
// Reminder: skelfile must be <x,y,z> tuple<int> on each line, nothing more
void IM::get_skeleton() {
  ifstream file(skelfile);
  while (file) {
    int x, y, z;
    file >> x;
    file >> y;
    file >> z;
    skeleton.push( tuple<int, int, int>(x, y, z) );
  }
  // Show the first 5 elements of skeleton as sanity check
  cout << "Skeleton has " << skeleton.size() << " nodes.\n";
  for (tuple_list::const_iterator i = skeleton.begin(); 
       i != skeleton.begin()+5; i++) {
         cout << i->get<0>() << " " << i->get<1>() << " " << i->get<2>() << endl;
  }
  cout << "... \n";
}



////////////////////////////////////////////////////////////////////////
//                     Image processing stuff

// Generate normal vector for plane
vector<float> gen_vector(vector<int> pt0, vector<int> pt1) {
  vector <float> vec(3);
  for (int i = 0; i < 3; i++) {
    vec.push_back(pt1[i] - pt0[i]);
  }
  return vec;
}



// Inline - solve plane equation
inline int solve_z(vector<float> vec, int x, int y) {
  return -((vec[0]*x + vec[1]*y) / vec[2]);
}



// Check if point is within the bounds
bool pt_ok(vector<int> pt, vector<int> dims) {
  if (pt[0] < 0 || pt[0] > dims[0]) {
    return false; 
  }
  else {
    if (pt[1] < 0 || pt[1] > dims[1]) {
      return false;
    }
    else {
      if (pt[2] < 0 || pt[2] > dims[2]) {
        return false;
      }
      else {
        return true;
      }
    }
  }
}



// Generate the plane indices of the cross section
tuple_list gen_plane_index(vector<int> sk0, vector<int> sk1, int npts=50) {
  vector<float> vec = gen_vector(sk0, sk1);
  tuple_list planinds( (npts*2)*(npts*2) ); // allocate
  vector<int> xs (npts*2);
  vector<int> ys (npts*2);
  // populate x and y vectors
  int xstart = vec[0] - npts;
  int ystart = vec[1] - npts;
  for (int i = 0; i < (npts*2); i++) {
    xs[i] = xstart;
    ys[i] = ystart;
    xstart++;
    ystart++;
  }
  // 
  int cnt = 0;
  for (int i = 0; i < (npts*2); i++) { // x loop
    for (int j = 0; j < (npts*2); j++) { // y loop
      int z = solve_z(vec, xs[i], ys[j]); // solve for z
      planinds[cnt] = (xs[i], ys[j], z);
      cnt++;
    }
  }
  cout << "Plane indices should be " << (npts*2)*(npts*2) << " and is " 
    << planinds.size() << endl;
  return planinds;
}



// Generate the cross-section and run spiral on each one (??) or return it?
tuple<MatrixXi, vector<int> >  IM::get_cross_sec(vector<int> sk0, vector<int> sk1) {
  int numpts = 50;
  tuple_list planinds = gen_plane_index(sk0, sk1, numpts);
  MatrixXi cs(numpts*2, numpts*2);
  int x_ind = 0;
  int y_ind = 0;
  vector<int> startpt = {numpts, numpts};
  // planinds should be length (numpts*2)*(numpts*2) = (numpts*2)^2
  for (tuple_list::const_iterator i = planinds.begin();
       i != planinds.end(); ++i) {
         // if y_ind (column) is beyond the range, reset it and move down one row
         if (y_ind >= numpts*2) {
           y_ind = 0; 
           x_ind = x_ind + 1;
         }
         // if the point is within the bounds, get its value from stack
         if (pt_ok(i, dims)) {
           cs(x_ind, y_ind) = stack [i->get<0>()] [i->get<1>()] [i->get<2>()];
           // check for the starting point (the skeleton point)
           if (i == sk0) {
             startpt = {x_ind, y_ind};
           }
         }
         y_ind = y_ind + 1; // increment the column by default
  }
  // Here either pass it to Spiral or send it back to main
  


    







////////////////////////////////////////////////////////////////////////
//                          Main

int main() {
  return 0;


}










