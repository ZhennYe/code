// euler1.cpp --  project euler problem #1
// find the sum of all multiples of 3 or 5 below 1000

#include <iostream>

int main()
{
  using namespace std;
  int c = 0;
  for (int n = 0; n < 10; n++) {
    c = c + n;
  }

  std::cout <<  c;
  
  return 0;
}

