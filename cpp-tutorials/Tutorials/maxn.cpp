// maxn.cpp -- returns the max of an array of values of any type
#include <iostream>

template <typename Arr>
void maxn(Arr arr[], int n); // returns max and index of max

// void showLocale (locale L);
// not necessary since maxn will show the max

int main ()
{
  using namespace std;
  int first[10] = 
  {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
  double second[5] = 
  {1.1, 2.2, 3.3, 4.4, 5.5};
  
  maxn(first, 10);
  maxn(second, 5);
  
  return 0;
}

template <typename Arr>
void maxn(Arr arr[], int n)
{
  using namespace std;
  int temp = 0, current_max = 0;
  int index;
  for (int i = 0; i < n; i++)
  {
    temp = arr[i];
    if (temp > current_max)
    {
      current_max = temp;
      index = i;
    }
  }
  
  cout << "Max is "<< current_max << " at location " << index << endl;
}

template <typename Arr>
void maxn(Arr arr[], int n)
{
  using namespace std;
  double temp = 0, current_max = 0;
  int index;
  for (int i = 0; i < n; i++)
  {
    temp = arr[i];
    if (temp > current_max)
    {
      current_max = temp;
      index = i;
    }
  }
  
  cout << "Max is " << current_max << " at location " << index << endl;
}






