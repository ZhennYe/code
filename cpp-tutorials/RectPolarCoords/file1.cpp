// file1.cpp -- example of three-file program
#include <iostream>
#include "coord_in.h" // quotes for local files, <> for system files
using namespace std;
int main()
{
  rect rplace;
  polar pplace;
  
  cout << "Enter the x and y values: ";
  while (cin >> rplace.x >> rplace.y) // get values
  {
    pplace = rect_to_polar(rplace);
    show_polar(pplace);
    cout << "Next two numbers (q to quit): ";
  }
  cout << "Bye \n";
  return 0;
}

