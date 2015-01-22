// euler004.cpp -- find the largest palindrome for two numbers having
//                 ndigits digits (i.e.: for ndigits=2, 9009 is largest)

#include <iostream>
#include <cmath>
#include <vector>


std::vector<int> breaknum(int num)
{
  std::vector<int> nums;
  int go = 1;
  int r;
  int count = 0; // keep track of how many digits palindrome is
  
  while (go == 1) {
    r = num % 10;
    
    if (r != 0) {
      nums.push_back(r);
      count = count + 1;
    }
    
    else {
      nums.push_back(0);
      count = count + 1;
    }
    
    num = num / 10;
    if (num == 1) {
      nums.push_back(1);
      count = count + 1;
      go = 0;
    }
    else if (num == 0) {
      go = 0;
    }
  }
    
  return nums;
}




int main()
{
  int num = 8888;
  std::vector<int> nums = breaknum(num);
  
  std::cout << "nums:";
  for(int & n : nums) {
    std::cout << ' ' << n;
  }
  std::cout << '\n';
  
  return 0;
}






  
