// usetime0.cpp -- using the first draft of the Time class
// compile usetime0.cpp and mytime0.cpp together
#include <iostream>
#include "mytime2.h"

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
  cout << "\nAdding 5 hrs and 25 mins to planning time.\n";
  planning.AddMin(25);
  planning.AddHr(5);
  cout << "New amount: ";
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
  
  cout << "Now, the additions. Before set_later: ";
  cout << planning.set_later << endl;
  cout << "After set_now: ";
  planning.set_now(55);
  cout << planning.set_later << endl;
  
  cout << "Using Min2. Adding 15 x 2 mins to planning.\nBefore: ";
  planning.Show();
  cout << endl << "After: ";
  planning.Min2(15);
  planning.Show();
  cout << "\ngoat\n";
  
  return 0;
}

