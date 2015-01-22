// a test for parallelized c++ functions
#include <iostream>
int main()
{
  #pragma omp parallel
  {
    for (int i = 0; i<10; i++)
    {
      std::cout << "Hello World!\n";
    }
  }
  std::cout << "Non-parallel hello world!\n";
}

