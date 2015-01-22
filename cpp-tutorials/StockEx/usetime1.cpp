// usetime1.cpp -- using the second draft of the Time class
// still compiled with mytime0.cpp
# include <iostream>
# include "mytime1.h"

int main()
{
  using std::cout;
  using std::endl;
  Time planning;
  Time coding(2, 40);
  Time fixing(5, 55);
  Time total;
  
  cout << "planning time = ";
  planning.Show();
  cout << endl;
  
  cout << "coding time = ";
  coding.Show();
  cout << endl;
  
  cout << "ficing time = ";
  fixing.Show();
  cout << endl;
  
  total = coding + fixing; // invokes overloaded + operator
  cout << "coding + fixing = ";
  total.Show();
  cout << endl;
  
  Time morefixing(3, 28);
  cout << " more fixing time = ";
  morefixing.Show();
  cout << endl;
  total = morefixing.operator+(total); // ?
  // function notation instead of operator notation
  cout << "morefixing.operator+(total) = ";
  total.Show();
  cout << endl;
  
  return 0;
}
