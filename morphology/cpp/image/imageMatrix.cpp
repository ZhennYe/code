#include <fstream>
#include <iostream>
#include <vector>
#include <cassert>
#include <string>
#include <boost/tuple/tuple.hpp>
#include <boost/multi_array.hpp>
#include <eigen3/Eigen/Dense>


using namespace std;
using boost::tuple;
using namespace Eigen;



//////////////////////////////////////////////////////////////////////////
//                       Loading the matrix


int load_matrix(const char* fname, vector<int> dims) {
  int data[dims[0]][dims[1]];
  ifstream file(fname);

  for (int i = 0; i < dims[0]; i++) {
    for (int j = 0; j < dims[1]; j++) {
      file >> data[i][j];
    }
  }
  // show the matrix
  for (int i = 0; i < dims[0]; i++) {
    for (int j = 0; j < dims[1]; j++) {
      cout << data[i][j];
    }
    cout << endl;
  }
  return 0;
}



// test version
int** get_matrix(const char* fname, vector<int> dims) {
  ifstream file(fname);
  
  int ** data = 0;
  data = new int*[dims[0]];
  for (int h = 0; h < dims[0]; h++) {
    data[h] = new int[dims[1]];
    for (int w = 0; w < dims[1]; w++) {
      file >> data[h][w];
    }
  }
  // show the matrix
  for (int i = 0; i < dims[0]; i++) {
    for (int j = 0; j < dims[1]; j++) {
      cout << data[i][j];
    }
    cout << endl;
  }
  return data;
}



// Put the data into an Eigen MatrixXi
MatrixXi eigen_matrix(int** data, vector<int> dims) {
  MatrixXi mat(dims[0], dims[1]);
  // row-major order of both is assumed
  for (int i = 0; i < dims[0]; i++) {
    for (int j = 0; j < dims[1]; j++) {
      mat(i,j) = data[i][j];
    }
  }
  return mat;
}



// Load array data from a list of images passed from an .txt containing a
// list of image files, in order
typedef boost::multi_array<int, 3> array3d;
typedef array3d::index idx;

array3d get_stack(const char* fileName, vector<int> dims) {
  // get the list of image files
  vector<string> files;
  ifstream file(fileName);
  while (file) {
    string tmp;
    file >> tmp;
    files.push_back(tmp);
  }
  // add each image matrix to the stack
  array3d stack(boost::extents[dims[0]][dims[1]][dims[2]]);
  int numfiles = files.size();
  assert(dims[2] == numfiles);
  int cnt = 0;
  for (int k = 0; k < numfiles; k++) {
    string tmpstr = files[k];
    char* fname = new char[tmpstr.size()+1];
    // fname[tmpstr.size()] = 0;
    strcpy(fname, tmpstr.c_str());
    // const char* arrfile = new char(fname);
    int** tmp = get_matrix(fname, dims);
    // get x and y from this matrix, add to boost array
    for (int i = 0; i < dims[0]; i++) {
      for (int j = 0; j < dims[1]; j++) {
        stack[i][j][k] = tmp[i][j];
      }
    }
    cnt++;
  }
  cout << "Should have " << dims[2] << " slices; found " << cnt << " files.\n";
  return stack;
}





//////////////////////////////////////////////////////////////////////////
//                        Load the skeleton

typedef vector< tuple<int, int, int> > tuple_list;

tuple_list get_skeleton(const char* skelfile) {
  // Here the skelfile must have an <x,y,z> tuple on each line, nothing more
  tuple_list skel;
  ifstream file(skelfile);
  while (file) {
    int x, y, z;
    file >> x;
    file >> y;
    file >> z;
    skel.push_back( tuple<int, int, int>(x, y, z) );
  }
  // Show the first and last 5 elements of the array
  cout << "Skeleton has " << skel.size() << " nodes.\n";
  for (tuple_list::const_iterator i = skel.begin(); i != skel.begin()+5; ++i) {
    cout << i->get<0>() << " " << i->get<1>() << " " << i->get<2>() << endl;
  }
  cout << "... \n";

  return skel;
}


///////////////////////////////////////////////////////////////////////
//                      Image Processing stuff

// Generate the normal vector for the plane
vector<float> gen_vector(vector<int> pt0, vector<int> pt1) {
  vector<float> vec(3);
  for (int i = 0; i < 3; i++) {
    vec.push_back(pt1[i]-pt0[i]);
  }
  return vec;
}



// inline solve-plane equation
inline int solve_z(vector<float> vec, int x, int y) {
  return -((vec[0]*x + vec[1]*y)/vec[2]);
}


// Generate the plane of the normal vector
tuple_list gen_plane_index(vector<int> sk0, vector<int> sk1, int npts=50) {
  vector<float> vec = gen_vector(sk0, sk1);
  tuple_list planinds((npts*2)*(npts*2)); // pre-allocate to save time
  vector<int> xs (npts*2);
  vector<int> ys (npts*2);
  // populate the x and y vectors
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
  cout << "Plane indices should be " << (npts*2)*(npts*2) << " and is " << planinds.size() << endl;
  return planinds;
}



// Generate the cross-sections





//////////////////////////////////////////////////////////////////////////
//                             Testing

int test_multi_array() {
  // Create a 3D array of 3 x 4 x 2
  typedef boost::multi_array<double, 3> array_type;
  typedef array_type::index index;
  array_type A(boost::extents[3][4][2]);
  // Assign values to the elements
  int values = 0;
  for (index i = 0; i != 3; ++i) {
    for (index j = 0; j != 4; ++j) {
      for (index k = 0; k != 2; ++k) {
        A[i][j][k] = values++;
      }
    }
  }
  // Verify values
  int verify = 0;
  for (index i = 0; i != 3; ++i) {
    for (index j = 0; j != 4; ++j) {
      for (index k = 0; k != 2; ++k) {
        assert(A[i][j][k] == verify++);
      }
    }
  }
  return 0;
}



//////////////////////////////////////////////////////////////////////////
//                               Main


int main() {
  // version 1 
  
  vector<int> dims;
  dims.push_back(96);
  dims.push_back(96);
  dims.push_back(96);
  const char* fname = "imlist.txt";
  /*
  int goat = load_matrix(fname, dims);
  */
  // version 2
  //int** goat = get_matrix(fname, dims);
  tuple_list skel = get_skeleton("skel.txt");
  //int boat = test_multi_array();
  array3d varr = get_stack(fname, dims);
  return 0;
}




