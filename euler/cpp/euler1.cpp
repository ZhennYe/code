// euler1.cpp --  project euler problem #1
// find the sum of all multiples of 3 or 5 below 1000

#include <cmath>
#include <iostream>

int main()
{
  using namespace std;
  int* old = new int[2];
  int c = 0;
  for (int n = 0; n < 10; n +1) {
    
    // if n is divisible by 3, add it
    if (n % 3 == 0) {
      c = c + 1;
      int* m = new int[c];
      for (int i = 0; i < c; i + 1) {
        m[i] = old[i];
         }
      m[c] = n; 
      delete[] old;
      int* old = new int[c];
      for (int i = 0; i < c; i + 1) {
        old[i] = m[i];
      }
      delete[] m;
    }
    
    // if n is divisible by 5, add it to the list
    else if (n % 5 == 0) {
      c = c + 1;
      int* m = new int[c];
      for (int i = 0; i < c; i + 1) {
        m[i] = old[i];
         }
      m[c] = n; 
      delete[] old;
      int* old = new int[c];
      for (int i = 0; i < c; i + 1) {
        old[i] = m[i];
      }
      delete[] m;
    }
    else {
      // do nothing
    }
  }
  
  // sum and return
  int s = 0;
  
  for (int i = 0; i < c; i + 1) {
    s = s + old[i];
  }
  std::cout << "Sum is: " << s << '\n';
}








