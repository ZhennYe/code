// default_args.cpp -- left : string function with default argument
#include <iostream>
const int ArSize = 80;
char * left(const char * str, int n=1); // here is where default is set
int main()
{
  using namespace std;
  char sample[ArSize];
  cout << "Enter a string:\n";
  cin.get(sample, ArSize);
  char *ps = left(sample, 6); // call with 2 args (no default is used)
  cout << ps << endl;
  delete [] ps; // free the old string
  ps = left(sample); // call with just 1 arg
  cout << ps << endl;
  delete [] ps; // free the new string
  return 0;
}

// this function returns a pointer to a new string consisting
// of the first n characters of the str string
char * left(const char * str, int n) // NOTE: int n=1 is not used here
  // only in prototype
{
  if (n<0)
    n = 0;
  char * p = new char[n+1];
  int i;
  for (i = 0; i < n && str[i]; i++) // while str[i] exists (can't exceed)
    p[i] = str[i]; // copy characters
  while (i <= n)
    p[i++] = '\0'; // set the rest of string to '\0'
  return p;
}
