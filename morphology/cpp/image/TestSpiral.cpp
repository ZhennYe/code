// TestSpiral.cpp
// Test Spiral.cpp and Spiral.h functionalit


#include <fstream>
#include <iostream>
#include <vector>
#include <cassert>
#include <string>
#include <boost/tuple/tuple.hpp>
#include <boost/multi_array.hpp>
#include <eigen3/Eigen/Dense>
#include "Spiral.h"


using boost::tuple;
using namespace Eigen;


typedef boost::multi_array<int, 3> array3d;
typedef array3d::index idx;
typedef std::vector< tuple<int, int> > tuple_list;


MatrixXi gen_array_large() {
  MatrixXi arr(10,12);
  arr.row(0) << 0,0,0,0,0,0,0,0,0,0,0,0;
  arr.row(1) << 0,0,0,0,0,1,1,1,1,0,0,0;
  arr.row(2) << 0,0,1,1,1,1,1,1,1,1,0,0;
  arr.row(3) << 1,1,1,1,1,1,1,1,1,1,1,0;
  arr.row(4) << 1,1,1,1,1,1,1,1,1,1,1,0;
  arr.row(5) << 0,1,1,1,1,1,1,1,1,1,0,0;
  arr.row(6) << 0,0,1,1,0,1,1,1,1,1,1,0;
  arr.row(7) << 0,0,0,0,0,0,0,1,1,1,1,0;
  arr.row(8) << 0,0,0,0,0,0,0,0,1,1,0,0;
  arr.row(9) << 0,0,0,0,0,0,0,0,0,0,0,0;
  return arr;
  }
MatrixXi gen_array_small() {
  MatrixXi arr(6,6);
  arr.row(0) << 0, 0, 0, 0, 0, 0;
  arr.row(1) << 0, 0, 1, 1, 0, 0;
  arr.row(2) << 0, 1, 1, 1, 1, 0;
  arr.row(3) << 1, 1, 1, 1, 1, 0;
  arr.row(4) << 0, 0, 1, 1, 0, 0;
  arr.row(5) << 0, 0, 0, 0, 0, 0;
  return arr;
}


int main() {
  MatrixXi arr = gen_array(true);
  std::vector<int> dims;
  dims.push_back(10);
  dims.push_back(12);
  std::vector<int> startpt;
  startpt.push_back(5);
  startpt.push_back(5);
  Spiral slice(arr, dims, startpt);
  slice.default_dead_pts();
  slice.spiral();
  
  std::cout << "Area of cross-section is: " << slice.area << std::endl;
  return 0;
}













