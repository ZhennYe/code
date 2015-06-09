#include <fstream>
#include <iostream>
#include <vector>
#include <cassert>
#include <string>
#include <algorithm>
#include <stdlib.h>
#include <time.h>
#include <boost/tuple/tuple.hpp>
#include <boost/multi_array.hpp>
#include <eigen3/Eigen/Dense>
// #include "imageMatrix_class.h" // Not needed?
#include "Spiral.h" // This is just constructors for the larger class Spiral,
                    // not all Spiral methods have been added to this yet.


using boost::tuple;
using namespace Eigen;

typedef std::vector< boost::tuple<int, int> > tuple_list;



////////////////////////////////////////////////////////////////////////

// Generate fake matrix
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



// Generate a fake tuple list
tuple_list gen_tuple_list() {
  tuple_list goat;
  boost::tuple<int, int> curr_tup = {55,56};
  goat.push_back(curr_tup);
  goat.push_back( boost::tuple<int, int>(14, 11) );
  goat.push_back( boost::tuple<int, int>(24, 12) );
  goat.push_back( boost::tuple<int, int>(34, 13) );
  goat.push_back( boost::tuple<int, int>(44, 14) );
  goat.push_back( boost::tuple<int, int>(54, 15) );
  goat.push_back( boost::tuple<int, int>(64, 16) );
  return goat;
}



void test_tuple_list() {
  tuple_list goat = gen_tuple_list();
  boost::tuple<int, int> sample_tup = {11, 14};
  std::cout << "Last element of tuple_list: ";
  for (tuple_list::const_iterator i = goat.begin(); i != goat.end(); ++i) {
    std::cout << i->get<0>() << " " << i->get<1>() << std::endl;
  }
  bool in_list = is_element(goat, sample_tup);
  show_tup(sample_tup);
  std::cout << " in list: ";
  if (in_list) { std::cout << "true.\n";}
  else { std::cout << "false.\n"; }
  boost::tuple<int, int> new_tup = {44,14};
  in_list = is_element(goat, new_tup);
  show_tup(sample_tup);
  std::cout << " in list: ";
  if (in_list) { std::cout << "true.\n";}
  else { std::cout << "false.\n"; }
}



////////////////////////////////////////////////////////////////////////
// main

int main() {

  // Spiral stuff
  auto arr = gen_array_large();
  // std::cout << arr << std::endl;
  std::vector<int> dims = {10,12};
  boost::tuple<int, int> startpt = {4,4};
  
  Spiral spi(arr, dims, startpt);
  // std::cout << spi.arr << std::endl;
  std::cout << "Target is: " << spi.tgt << std::endl;
  std::cout << "Start point is: ";
  show_tup(spi.startpt);
  std::cout << std::endl;
  std::cout << "dims are: " << spi.dims[0] << " x " << spi.dims[1] << std::endl;
  spi.default_dead_pts();
  spi.no_changes();
  if (is_element(spi.dead_pts, spi.startpt)) {
    std::cout << "Start pt in dead_pts.\n";
  }
  else {
    std::cout << "Start pt NOT in dead_pts.\n";
  }
  std::cout << "prev is " << spi.prev << std::endl;
  spi.prev_pt = {boost::get<0>(spi.startpt), boost::get<1>(spi.startpt)};
  std::cout << spi.prev_pt.size();
  std::cout << spi.prev_pt[0] << spi.prev_pt[1] << std::endl;
  auto possible = spi.check_next();
  if (boost::get<0>(possible)) {
    std::cout << "next point is a go: " << std::endl;
    show_tup(boost::tuple<int, int> (possible.get<1>(), possible.get<2>()));
    std::cout << std::endl;
  }
  // testing
  std::vector<int> next_vec = {possible.get<1>(), possible.get<2>()};
  bool next_go = spi.check_live_pt(next_vec);
  if (next_go) {std::cout << "check_live_pt returned true\n";}
  spi.log_pt();
  std::cout << "contents of live_pts:";
  int cyc = cycle(spi.prev);
  spi.spiral();
  show_tup_list(spi.live_pts);
  return 0;
}

