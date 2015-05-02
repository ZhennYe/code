/* example.cpp */

#include <Python.h>
#include <iostream>

double Foo = 7.5;

int fact(int n) {
  if (n <= 1) return 1;
  else return n*fact(n-1);
}

int main() {
  using namespace std;
  cout << "Shit worked?" << "\n";
  return 0;
}

