#include <fstream>
#include <iostream>
#include <vector>

using namespace std;

//////////////////////////////////////////////////////////////////////////
//                       Loading the matrix


int load_matrix(const char* fname, vector<int> dims) {
  int data[dims[0]][dims[1]];
  ifstream file(fname);

  for (int i = 0; i < dims[0]; i++) {
    for (int j = 0; j < dims[1]; j++) {
      file >> data[i][j];
    }
  }
  // show the matrix
  for (int i = 0; i < dims[0]; i++) {
    for (int j = 0; j < dims[1]; j++) {
      cout << data[i][j];
    }
    cout << endl;
  }
  return 0;
}



// test version
int** get_matrix(const char* fname, vector<int> dims) {
  ifstream file(fname);
  
  int ** data = 0;
  data = new int*[dims[0]];
  for (int h = 0; h < dims[0]; h++) {
    data[h] = new int[dims[1]];
    for (int w = 0; w < dims[1]; w++) {
      file >> data[h][w];
    }
  }
  // show the matrix
  for (int i = 0; i < dims[0]; i++) {
    for (int j = 0; j < dims[1]; j++) {
      cout << data[i][j];
    }
    cout << endl;
  }
  return data;
}




int main() {
  // version 1 
  
  vector<int> dims;
  dims.push_back(96);
  dims.push_back(96);
  const char* fname = "test.txt";
  /*
  int goat = load_matrix(fname, dims);
  */
  // version 2
  int** goat = get_matrix(fname, dims);
  return 0;
}




