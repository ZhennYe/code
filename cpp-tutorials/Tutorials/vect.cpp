// vect.cpp -- methods for the Vector class 
#include <cmath>
#include "vect.h" // includes <iosteam>
using std::sqrt;
using std::sin;
using std::cos;
using std::atan;
using std::atan2;
using std::cout;

namespace VECTOR
{
  // computer degrees in one radian
  const double Rad_to_deg = 45.0 / atan(1.0);
  // should be about 57.2957795130823
  
  // private methods
  // calculates magnitude from x and y
  void Vector::set_mag()
  {
    mag = sqrt(x * x + y * y);
  }
  
  void Vector::set_ang()
  {
    if (x == 0.0 && y == 0.0)
      ang = 0.0;
    else
      ang = atan2(y,x); // prevent undefined
  }
  
  // set x from polar coordinate
  void Vector::set_x()
  {
    x = mag * cos(ang);
  }
  
  // set y from polar coordinate
  void Vector::set_y()
  {
    y = mag * sin(ang);
  }
  
  // public methods
  Vector::Vector() // default constructor
  {
    x = y = mag = ang = 0.0;
    mode = RECT;
  }
  
  // construct vector from the rectangular coordinates if form is r
  // (default) or else from polar coords if form is p
  Vector::Vector(double n1, double n2, Mode form)
  {
    mode = form;
    if (form == RECT)
    {
      x = n1;
      y = n2;
      set_mag();
      set_ang();
    }

    else if (form == POL)
    {
      mag = n1;
      ang = n2 / Rad_to_deg;
      set_x();
      set_y();
    }
    else
    {
      cout << "Incorrect number/types of arguments; set to 0\n";
      x = y = mag = ang = 0.0;
      mode = RECT;
    }
  }

// reset vector from rectangular coordinates if form is RECT (default)
  void Vector::reset(double n1, double n2, Mode form)
  {
    mode = form;
    if (form == RECT)
    {
      x = n1;
      y = n2;
      set_mag();
      set_ang();
    }
    else if (form == POL)
    {
      mag = n1;
      ang = n2 / Rad_to_deg;
      set_x();
      set_y();
    }
    else
    {
      cout << "Incorrect number of arguments; set to 0\n";
      x = y = mag = ang = 0.0;
      mode = RECT;
    }
  }
  
  Vector::~Vector() // destructor
  {
  }
  
  void Vector::polar_mode() // set to poalr mode
  {
    mode = POL;
  }
  
  void Vector::rect_mode() // set to rect mode
  {
    mode = RECT;
  }
  
  // operator overloading
  // add two vectors
  Vector Vector::operator+(const Vector & b) const
  {
    return Vector(x + b.x, y + b.y);
  }
  
  // subtract Vector b from a
  Vector Vector::operator-(const Vector & b) const
  {
    return Vector(x - b.x, y - b.y);
  }
  
  // reverse sign of Vector
  Vector Vector::operator-() const
  {
    return Vector(-x, -y);
  }
  
  // multiple vector by n
  Vector Vector::operator*(double n) const
  {
    return Vector(n * x, n * y);
  }
  
  // friend methods
  // multiply n ny Vector a
  Vector operator*(double n, const Vector & a)
  {
    return a * n;
  }
  
  // display rect or pol coords depending on which mode
  std::ostream & operator<<(std::ostream & os, const Vector & v)
  {
    if (v.mode == Vector::RECT)
      os << "(x,y) = (" << v.x << ", " << v.y << ")";
    else if (v.mode == Vector::POL)
    {
      os << "(m,a) = (" << v.mag << ", "
         << v.ang * Rad_to_deg << ")";
    }
    else 
      os << "Vector object mode is invalid";
    return os;
  }

} // end namespace VECTOR
