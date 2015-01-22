// euler003.cpp -- find all prime factors of a given number

#include <iostream>
#include <cmath>
#define NUMMAX 800000000000

int main()
{
  using namespace std;
  
  unsigned long int num = 0;
  std::cout << "What is number to be factored? ";
  std::cin >> num;
  if (num > NUMMAX) {
    num = NUMMAX;
    std::cout << "Input too large, replaced by 8e11.\n";
  }
  
  int c = 1;
  while (num > 1) {
    if (num % c == 0) {
      num = num / c;
      std::cout << c << "\n";
    }
    c = c + 1;
  }
  
  return 0;
}

