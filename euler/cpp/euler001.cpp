// euler1.cpp --  project euler problem #1
// find the sum of all multiples of 3 or 5 below 1000

#include <cmath>
#include <iostream>
#define MAXN 100000

int main()
{
  using namespace std;
  
  int N = 0;
  std::cout << "Enter upper limit: ";
  std::cin >> N;
  
  // sanitize
  if (N > MAXN) {
    N = MAXN;
  }
  
  int old[N];
  int c = 0;
  for (int n = 0; n < N; n++) {
    
    // if n is divisible by 3, add it
    if (n % 3 == 0) {
      old[c] = n;
      c = c + 1;
    }
    
    // if n is divisible by 5, add it to the list
    else if (n % 5 == 0) {
      old[c] = n;
      c = c + 1;
    }
    else {
      // do nothing
    }
  }
  
  // sum and return
  int s = 0;
  
  for (int i = 0; i < c; i++) {
    s = s + old[i];
  }
  std::cout << "Sum is: " <<  s << "\n";
  
  return 0;
}








