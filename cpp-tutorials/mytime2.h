// mytime2.h -- Time class after opertor overloading
#ifndef MYTIME2_H_
#define MYTIME3_H_

class Time
{
  private:
    int hours;
    int minutes;
  
  public:
    int set_later;
    Time();
    Time(int h, int m);
    void AddMin(int m);
    void AddHr(int h);
    void Reset(int h = 0, int m = 0);
    Time Sum(const Time & t) const;
    Time operator+(const Time & t) const;
    Time operator-(const Time & t) const;
    Time operator*(double n) const;
    void Show() const;
    void set_now(int set);
    void Min2(int min2);
};
#endif
