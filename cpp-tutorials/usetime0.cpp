// usetime0.cpp -- using the first draft of the Time class
// compile usetime0.cpp and mytime0.cpp together
#include <iostream>
#include "mytime0.h"

int main()
{
  using std::cout;
  using std::endl;
  Time planning;
  Time coding(2, 40);
  Time fixing(5, 55);
  Time total;
  
  cout << "Planning time = ";
  planning.Show();
  cout << endl;
  
  cout << "Coding time = ";
  coding.Show();
  cout << endl;
  
  cout << "Fixint time = ";
  fixing.Show();
  cout << endl;
  
  total = coding.Sum(fixing);
  cout << "coding.Sum(fixing) = ";
  total.Show();
  cout << endl;
  
  return 0;
}

