// a simple test of stuff

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
#include "Spiral.h"


using boost::tuple;
using namespace Eigen;

typedef std::vector< boost::tuple<int, int> > tuple_list;



////////////////////////////////////////////////////////////////////////
// testing


// Make all non-target points dead *unless* that means making >95% live
// (then switch)
void Spiral::default_dead_pts() {
  int dead_count = 0;
  int other;
  // Iterate through the 2-D array
  for (int i = 0; i < dims[0]; i++) { 
    for (int j = 0; j < dims[1]; j++) {
      if (arr(i,j) != tgt) {
        other = arr(i,j);
        dead_pts.push_back( boost::tuple<int, int>(i,j) );
        dead_count = dead_count + 1;
      }
    }
  }
  // Make sure too many points weren't added
  if (dead_count < (dims[0]*dims[1])*0.15) {
    for (int i = 0; i < dead_pts.size(); i++) {
      dead_pts.pop_back();
    }
    dead_count = 0;
    for (int i = 0; i < dims[0]; i++) {
      for (int j = 0; j < dims[1]; j++) {
        if (arr(i,j) == tgt) {
          dead_pts.push_back( boost::tuple<int, int>(i,j) );
          dead_count = dead_count + 1;
          // Switch target and other
          tgt = other;
        }
      }
    }
  }
}



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




// Constructor
Spiral::Spiral(MatrixXi array, std::vector<int> ddims, boost::tuple<int, int> spt) {
  MatrixXi arr = array;
  std::vector<int> dims;
  for (auto i = ddims.begin(); i != ddims.end(); ++i) {
    int temp = *i;
    dims.push_back(temp);
  }
  int t_x, t_y;
  t_x = boost::get<0>(spt);
  t_y = boost::get<1>(spt);
  boost::tuple<int, int> startpt = {t_x, t_y};
  tgt = arr(t_x, t_y); // Target should be binary, ==1
  tuple_list dead_pts;
  tuple_list live_pts;
  int delta_dead = 0;
  int delta_live = 0;
  int delta_count = 0;
  int tolerance; // If delta_count > tolerance, spiral exits
  int prev; // 0=right->down, 1=left->up, 2=up->right, 3=down->left
  int x, y;
  std::vector<int> prev_pt;
  int area = 0;
}





// Assess whether spiral process is done
void Spiral::no_changes() {
  // Set current length of live and dead pts
  if (delta_dead == 0) {
    delta_dead = dead_pts.size();
  }
  if (delta_live == 0) {
    delta_live = live_pts.size();
  }
  // Check live and dead pts; if they're the same, increment the count
  // Otherwise set the new delta level
  if (dead_pts.size() == delta_dead) {
    delta_count = delta_count + 1;
  }
  else {
    delta_dead = dead_pts.size();
  }
  if (live_pts.size() == delta_live) {
    delta_count = delta_count + 1;
  }
  else {
    delta_live = live_pts.size();
  }
}





bool is_element(tuple_list list, boost::tuple<int, int> tup) {
  std::vector<int> test_vec = {boost::get<0>(tup), boost::get<1>(tup)};
  // Pull out the elements of the test tuple (operator==)
  int tup_0 = boost::get<0>(tup);
  int tup_1 = boost::get<1>(tup);
  for (tuple_list::const_iterator i = list.begin(); i != list.end(); ++i) {
    // Pull out the element of the possible tuple
    int elem_0 = i->get<0>();
    int elem_1 = i->get<1>();
    if (tup_0 == elem_0 && tup_1 == elem_1) {
      return true;
    }
  }
  return false;
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




void show_tup(boost::tuple<int, int> tup) {
  bool newline = false;
  std::cout << "(" << tup.get<0>() << "," << tup.get<1>() <<")";
  if (newline) {std::cout << "\n";}
}


void show_tup_list(tuple_list tlist) {
  for (tuple_list::const_iterator i = tlist.begin(); i != tlist.end(); ++i) {
    boost::tuple<int, int> temp_tup = (i->get<0>(), i->get<1>());
    show_tup(temp_tup);
    std::cout << "\n";
  }
}




void show_vec(std::vector<int> vec) {
  std::cout << "(";
  for (std::vector<int>::const_iterator i = vec.begin(); i != vec.end(); ++i) {
    std::cout << *i << " ";
  }
  std::cout << ")\n";
}



void test_tuple_lust() {
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



int main() {

  // Spiral stuff
  auto arr = gen_array_small();
  std::cout << arr << std::endl;
  std::vector<int> dims = {6,6};
  boost::tuple<int, int> startpt = {2,2};
  
  Spiral spi(arr, dims, startpt);
  std::cout << "Target is: " << spi.tgt << std::endl;
  std::cout << "Start point is: ";
  show_tup(spi.startpt);
  std::cout << std::endl;
  std::cout << "dims are: " << dims[0] << " x " << dims[1] << std::endl;
  // testing
  int dead_count = 0;
  int other;
  // Iterate through the 2-D array
  for (int i = 0; i < dims[0]; i++) { 
    for (int j = 0; j < dims[1]; j++) {
      if (arr(i,j) != spi.tgt) {
        other = arr(i,j);
        spi.dead_pts.push_back( boost::tuple<int, int> (i,j) );
        dead_count = dead_count + 1;
      }
    }
  } /*
  if (dead_count < (dims[0]*dims[1])*0.15) {
    for (int i = 0; i < spi.dead_pts.size(); i++) {
      spi.dead_pts.pop_back();
    }
    dead_count = 0;
    for (int i = 0; i < dims[0]; i++) {
      for (int j = 0; j < dims[1]; j++) {
        if (arr(i,j) == spi.tgt) {
          spi.dead_pts.push_back( boost::tuple<int, int>(i,j) );
          dead_count = dead_count + 1;
          // Switch target and other
          spi.tgt = other;
        }
      }
    }
  } */
  //spi.default_dead_pts();
  std::cout << "number of dead points: " << spi.dead_pts.size() << std::endl;
  
  // iterate through dead_pts and show them
  show_tup_list(spi.dead_pts);
  return 0;
}

