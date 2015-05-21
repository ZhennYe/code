// This will load a hoc file and pass its points as a 3D array, 
// with or without the radius, as needed.
// usage: g++ loadHoc.cpp hocfile.hoc True(optional: default=False)


#include <iostream>
#include <cmath>
#include <vector>
#include <stdio.h>
#include <fstream>
#include <sstream>
#include <string>



// load the file
int main(int argc, char *argv[])
{
  using namespace std;
  // if no filename provided, exit
  if (argc <= 1)
  {
    cout << "No hoc file provided" << endl;
    exit(1);
  }
  char *pFilename = argv[1];
  pFile = fopen(pFilename, "r");

  string line;
  while (getline(pFile, line))
  {
    isstringstream iss(line);

