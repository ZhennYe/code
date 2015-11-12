
#include <iostream>


int main() {
  int j = 1;
  for (int i=1; i<7; ++i) {
    j = j * i;
    // std::cout << j << std::endl;
  }
  std::cout << "6 factorial: " << j << std::endl;
  return 0;
}

