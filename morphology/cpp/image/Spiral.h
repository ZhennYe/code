#include <fstream>
#include <iostream>
#include <vector>
#include <cassert>
#include <string>
// #include <boost/tuple/tuple.hpp>
#include <boost/multi_array.hpp>
#include <eigen3/Eigen/Dense>


#ifndef SPIRAL_H_
#define SPIRAL_H_

using namespace Eigen;


typedef boost::multi_array<int, 3> array3d;
typedef array3d::index idx;
typedef std::vector< std::pair<int, int> > tuple_list;

////////////////////////////////////////////////////////////////////////

class Spiral {
  private:

  public:
    std::vector<int> dims; // Here dims is len=2
    MatrixXi arr; // arr is 2-D
    std::pair<int, int> startpt; // <x,y>
    // Variables
    tuple_list dead_pts;
    tuple_list live_pts;
    int delta_dead;
    int delta_live;
    int delta_count;
    int tolerance; // If delta_count > tolerance, spiral exits
    int prev; // 0=right->down, 1=left->up, 2=up->right, 3=down->left
    int x, y;
    std::vector<int> prev_pt;
    int tgt = 0;
    int area;
    // Spiral functions
    Spiral(MatrixXi array, std::vector<int> ddims, boost::tuple<int, int> spt);
    void default_dead_pts();
    void no_changes();
    std::tuple<bool, int, int> check_next();
    bool check_live_pt(std::vector<int> potpt);
    void log_pt();
    bool retire_pts();
    void spiral();
};

void show_pair(std::pair<int, int> tup);
void show_pair_list(tuple_list tlist);
void show_vec(std::vector<int> vec);
bool is_element(tuple_list list, std::pair<int, int> tup);
int cycle(int p);

inline std::vector<int> move_up(std::vector<int> prev_pt);
inline std::vector<int> move_down(std::vector<int> prev_pt);
inline std::vector<int> move_left(std::vector<int> prev_pt);
inline std::vector<int> move_right(std::vector<int> prev_pt);


#endif
