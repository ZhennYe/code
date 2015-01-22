// euler002.cpp -- sum the even-valued fibonacci numbers of value
//                 less than 4 million

#include <iostream>
#include <cmath>
#define FMAX 4000000 // no fibonocci value over 4 million allowed

int main()
{
  using namespace std;
  
  int M = 0;
  std::cout << "What is max value? (less than 4e6): ";
  std::cin >> M;
  if (M > FMAX) {
    M = FMAX;
  }
  
  int curr = 0;
  int f1 = 1;
  int f2 = 2;
  int sum = 2;
  
  while (curr < M) {
    curr = f1 + f2;
    // if is even, add to sum
    if (curr % 2 == 0) {
      sum = sum + curr;
    }
    // else, just advance fib sequence
    f1 = f2;
    f2 = curr;
  }
  
  std::cout << "Sum is: " << sum << "\n";
  return 0;
}

