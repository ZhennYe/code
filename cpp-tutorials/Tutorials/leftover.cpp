// leftover.cpp -- overloading the left function to count number of digits
#include <iostream>
unsigned long left(unsigned long num, unsigned ct);
char * left(const char * str, int n = 1);

int main()
{
  using namespace std;
  char * trip = "Hawaii!"; // test value
  unsigned long n = 12345678; // test value
  int i;
  char * temp;
  
  for (i = 1; i < 10; i++)
  {
    cout << left(n, i) << endl;
    temp = left(trip, i);
    cout << temp << endl;
    delete [] temp; // point to temporary storage
  }
  return 0;
}

// overload 1 (int)
// this function returns the first ct digits of the number num
unsigned long left(unsigned long num, unsigned ct)
{
  unsigned digits = 1;
  unsigned long n = num;
  
  if (ct == 0 || num == 0)
    return 0; // return 0 if not digits
  while (n /= 10)
    digits++;
  if (digits > ct)
  {
    ct = digits - ct;
    while (ct--)
      num /= 10;
    return num;  // return left ct digists of num
  }
  else // if ct >= number of digits in num
    return num; // return the whole number
}

// overload 2 (str/char)
// this function returns a pointer to a new string consisting of the
  // first n characters in the string
char * left(const char * str, int n) //not int n = 1 (only default in prototype)
{
  if (n < 0)
    n = 0;
  char * p = new char[n+1];
  int i;
  for (i = 0; i < n && str[i]; i++)
    p[i] = str[i]; // copy chars
  while (i <= n)
    p[i++] = '\0';
  return p;
}

