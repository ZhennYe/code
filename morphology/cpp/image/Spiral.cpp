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
#include "Spiral.h" // This is just constructors for the larger class Spiral,
                    // not all Spiral methods have been added to this yet.


using boost::tuple;
using namespace Eigen;

typedef std::vector< boost::tuple<int, int> > tuple_list;



////////////////////////////////////////////////////////////////////////
// testing



// Constructor
Spiral::Spiral(MatrixXi array, std::vector<int> ddims, boost::tuple<int, int> spt) {
  arr = array;
  for (auto i = ddims.begin(); i != ddims.end(); ++i) {
    int temp = *i;
    dims.push_back(temp);
  }
  int t_x, t_y;
  t_x = boost::get<0>(spt);
  t_y = boost::get<1>(spt);
  startpt = {t_x, t_y};
  x = t_x;
  y = t_y;
  std::cout << "Uninitialized target was: " << tgt << ", new target is: ";
  tgt = arr(t_x, t_y); // Target should be binary, ==1
  std::cout << tgt << std::endl;
  tuple_list dead_pts;
  tuple_list live_pts;
  int delta_dead = 0;
  int delta_live = 0;
  int delta_count = 0;
  int tolerance; // If delta_count > tolerance, spiral exits
  prev = 0; // 0=right->down, 1=left->up, 2=up->right, 3=down->left
  int x, y;
  std::vector<int> prev_pt;
  int area = 0;
  std::cout << arr << std::endl;
  std::cout << "(" << boost::get<0>(startpt) << "," <<
    boost::get<1>(startpt) << ")\n" << std::endl;
  return;
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



void show_tup(boost::tuple<int, int> tup) {
  bool newline = false;
  std::cout << "(" << tup.get<0>() << "," << tup.get<1>() <<")";
  if (newline) {std::cout << "\n";}
}



// Show some elements of a tuple_list
void show_tup_list(tuple_list tlist) {
  std::cout << "Summary of "<< tlist.size() << " elements: \n";
  for (tuple_list::const_iterator i = tlist.begin(); i != tlist.begin()+5; ++i) {
    boost::tuple<int, int> temp_tup (i->get<0>(), i->get<1>());
    show_tup(temp_tup);
    std::cout << "\n";
  }
  std::cout << " ... \n";
  for (tuple_list::const_iterator i = tlist.end()-5; i != tlist.end(); ++i) {
    boost::tuple<int, int> temp_tup (i->get<0>(), i->get<1>());
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



// Make all non-target points dead *unless* that means making >95% live
// (then switch) // This function is currently being tested in main()
void Spiral::default_dead_pts() {
  int dead_count = 0;
  int other;
  // Iterate through the 2-D array
  for (int i = 0; i < dims[0]; i++) { 
    for (int j = 0; j < dims[1]; j++) {
      if (arr(i,j) != tgt) {
        other = arr(i,j);
        // show_tup(boost::tuple<int, int> (i,j));
        dead_pts.push_back(boost::tuple<int, int> (i,j));
        dead_count = dead_count + 1;
      }
    }
  }
  
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
  
  std::cout << "number of dead points: " << dead_pts.size() << std::endl;
  // iterate through dead_pts and show them
  show_tup_list(dead_pts);
}



// Check to see if the next point exists (in the array) and is not
// already in live_pts or dead_pts
boost::tuple<bool, int, int> Spiral::check_next() {
  std::vector<int> next_pt;
  // std::cout << "previous pt: [" << prev_pt[0] <<","<< prev_pt[1] <<"]\n";
  switch (prev) {
    // 0 : prev=right, next=down
    case 0 : next_pt = move_down(prev_pt);
             // prev = 3; // Don't actually advance the next
             break;
    // 1 : prev = left, next=up
    case 1 : next_pt = move_up(prev_pt);
             // prev = 2;
             break;
    // 2 : prev=up, next=right
    case 2 : next_pt = move_right(prev_pt);
             // prev = 0;
             break;
    // 3 : prev=down, next=left
    case 3 : next_pt = move_left(prev_pt);
             // prev = 1;
             break;
  }
  // If point not in live or dead points ...
  boost::tuple <int, int> next_pt_t (next_pt[0], next_pt[1]);
  if (! is_element(dead_pts, next_pt_t)) {
    if (! is_element(live_pts, next_pt_t)) {
      /* find(v.begin(), v.end(), x) == v.end() is true if x NOT IN v
         find(v.begin(), v.end(), x) != v.end() is true if x IN v */
      // ... and point is within the bounds of the array ...
      if (next_pt[0] > 0 && next_pt[0] < dims[0]) {
        if (next_pt[1] > 0 && next_pt[1] < dims[1]) {
          // ... then return that the point is okay.
          return boost::tuple<bool, int, int> (true, boost::get<0>(next_pt_t),
                                    boost::get<1>(next_pt_t)); // Return as std::vector or tuple?
        }
      }
    }
  }
  // Else, return false and some bullshit
  return boost::tuple<bool, int, int> (false, 0,0);
}



/* This is called by log_pt, which tries to add a live_pt if it should
   be added (not already in live_pts or dead_pts). If a false is
   returned, that means the point is already in one of those lists.
   A true returned means add it to live (but there was insufficient info
   for check_live_pt to add it). */
bool Spiral::check_live_pt(std::vector<int> potpt) {
  // If it's off-target, can return it as false immediately
  auto potpt_t = boost::make_tuple(potpt[0], potpt[1]);
  if (arr(potpt[0], potpt[1]) != tgt) {
    // Add it to dead points if it's not already there
    if (! is_element(dead_pts, potpt_t)) {
      dead_pts.push_back(potpt_t);
    }
    return false;
  }
  // Else, check if it's surrounded on any side by live pts
  int live = 0;
  std::vector<int> next_pt1, next_pt2, next_pt3, next_pt4;
  next_pt1 = move_up(potpt);                                     // Up
  auto next_pt_t1 = boost::make_tuple(next_pt1[0], next_pt1[1]);
  if (is_element(live_pts, next_pt_t1)) { live = live + 1;}
  next_pt2 = move_down(potpt);                                   // Down
  auto next_pt_t2 = boost::make_tuple(next_pt2[0], next_pt2[1]);
  if (is_element(live_pts, next_pt_t2)) { live = live + 1;}
  next_pt3 = move_left(potpt);                                   // Left
  auto next_pt_t3 = boost::make_tuple(next_pt3[0], next_pt3[1]);
  if (is_element(live_pts, next_pt_t3)) { live = live + 1;}
  next_pt4 = move_right(potpt);                                  // Right
  auto next_pt_t4 = boost::make_tuple(next_pt4[0], next_pt4[1]);
  if (is_element(live_pts, next_pt_t4)) { live = live + 1;}
  // If at least one side is live and the point itself isn't dead, make live
  // This means the point has already been added, so return false
  if (live >= 1 && ! is_element(dead_pts, potpt_t)) {
    // As long as it's not already in live_pts
    if (! is_element(live_pts, potpt_t)) {
      live_pts.push_back(potpt_t);
      // And increment the area?
    }
    return false;
  }
  return true; // Else, return true (try to add the point)
}



// Log the point as a live_pt
void Spiral::log_pt() {
  // Check current point
  if (arr(x,y) == tgt) {
    // std::cout << "good point found: (" << x << "," << y << ")\n";
    area = area + 1;
    std::vector<int> potpt;
    potpt.push_back(x);
    potpt.push_back(y);
    if (check_live_pt(potpt)) {
      auto potpt_t = boost::make_tuple(x, y);
      if (! is_element(live_pts, potpt_t)) {
        live_pts.push_back(potpt_t);
      }
    }
  }
}



////////////////////////////////////////////////////////////////////////
// Direction in-line functions

// Move up
inline std::vector<int> move_up(std::vector<int> prev_pt) {
  std::vector<int> next_pt = {prev_pt[0], prev_pt[1]-1};
  return next_pt;
}
// Move down
inline std::vector<int> move_down(std::vector<int> prev_pt) {
  std::vector<int> next_pt = {prev_pt[0], prev_pt[1]+1};
  return next_pt;
}
// Move right
inline std::vector<int> move_right(std::vector<int> prev_pt) {
  std::vector<int> next_pt = {prev_pt[0]+1, prev_pt[1]};
  return next_pt;
}
// Move left
inline std::vector<int> move_left(std::vector<int> prev_pt) {
  std::vector<int> next_pt = {prev_pt[0]-1, prev_pt[1]};
  return next_pt;
}



////////////////////////////////////////////////////////////////////////
// Spiral
//      ------->
//     ^   x-->
//     |       |
//      <------
//
// Helper function -- cycle between 0 -(down)-> 3 -(left)-> 1 -(up)-> 2 -(right)-> 0
int cycle(int prev) {
  if (prev > -1 && prev < 4) {
    switch(prev) {
      case 0 : return 3;
      case 3 : return 1;
      case 1 : return 2;
      case 2 : return 0;
    }
  }
  return -1;
}



// Spiral
void Spiral::spiral() {
  srand (time(NULL)); // Seed random number
  tolerance = 2000; // Set some tolerance
  // Add the first area
  area = area + 1;
  // int r_x, r_y; // Random numbers for start points
  x = startpt.get<0>();
  y = startpt.get<1>();
  prev = 2; // Set 'fake' previous move to 'up'
  log_pt(); // Log the start point
  //no_changes(); // Log the state of the lists
  while (live_pts.size() > 0) {
    int count = 0;
    no_changes();
    // Choose a random point
    x = rand() % dims[0];
    y = rand() % dims[1];
    // std::cout << "rand point: (" << x << "," << y << ")\n";
    log_pt();
    prev_pt = {x, y};
    // Check each direction for the new point
    while (count < 4) { 
      auto next_t = check_next();
      // If the next point exists
      if (boost::get<0>(next_t)) {
        x = boost::get<1>(next_t); // Set the new x and y values
        y = boost::get<2>(next_t); 
        log_pt();
        count = 0; // Reset the count -- found a new point
        // Advance the cycles
        prev = cycle(prev);
      }
      // If the next point is not valid, cycle and increment
      else {
        prev = cycle(prev);
        count = count + 1;
      }
      // Check tolerance; if it exceeds then quit, otherwise continue
      if (delta_count > tolerance) return;
    }
  }
}

