// coord_in.h -- structure templates and function prototypes
// structure templates
#ifndef COORD_IN_H_
#define COORD_IN_H_

struct polar
{
  double distance;  // distance from origin
  double angle;  // direction from origin
};

struct rect
{
  double x;
  double y;
};

// prototypes
polar rect_to_polar(rect xypos);
void show_polar(polar dapos); // dapos = distance-angle position

#endif

