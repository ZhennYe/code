#include <fstream>
#include <iostream>
#include <vector>
#include <cassert>
#include <string>
#include <boost/tuple/tuple.hpp>
#include <boost/multi_array.hpp>
#include <eigen3/Eigen/Dense>


#ifndef IMAGEMATRIX_H_
#define IMAGEMATRIX_H_

using namespace std;
using boost::tuple;
using namespace Eigen;


typedef boost::multi_array<int, 3> array3d;
typedef array3d::index idx;
typedef vector< tuple<int, int, int> > tuple_list;


class IM {
  private:

  public:
    vector<int> dims;
    const char* fileName;
    const char* skelfile;
    // Public members
    array3d stack;
    tuple_list skeleton;
    // Constructor
    IM(const char* fileName, const char* skelfile, vector<int> ddims); 
    // Loading functions
    array3d get_stack(const char* fileName, vector<int> dims); // For whole stack
    tuple_list get_skeleton();
    // Image processing
    vector<float> gen_vector(vector<int> pt0, vector<int> pt1);
    MatrixXi get_cross_sec(vector<int> sk0, vector<int> sk1,
                           array3d stack, vector<int> dims);
    // Testing
    void test_multi_array();
    void show_slice(array3d stack, vector<int> dims, int slice=0);
};


int** get_matrix(const char* imname, vector<int> dims); // For individual slice
MatrixXi eigen_matrix(int** data, vector<int> dims);
inline int solve_z(vector<int> pt0, vector<int> p1);
tuple_list gen_plane_index(vector<int> sk0, vector<int> sk1, int npts=50);
void test_multi_array();
bool pt_ok(vector<int> pt, vector<int> dims);




#endif

    
    
