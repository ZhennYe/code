#include <vector>
#include <iostream>
#include <sstream>
#include <fstream>


using namespace std;


// ask user for dimensions of array
vector<int> get_dims() {
  vector<int> dims;
  string row_s = "";
  string col_s = "";
  int row_int = 0;
  int col_int = 0;
  cout << "how many rows? ";
  getline(cin, row_s);
  stringstream(row_s) >> row_int;
  cout << "how many cols? ";
  getline(cin, col_s);
  stringstream(col_s) >> col_int;
  dims.push_back(row_int);
  dims.push_back(col_int);
  cout << "looking for " << row_int << " x " << col_int << " array\n";
  //  istringstream iss(row_s);
  /*
  vector<vector<bool> > m(row_int, vector<bool>(col_int));
  for (int i = 0; i < 10; i++) {
    for (int j = 0; j < 20; j++) {
      m[i].push_back(true);
    }
  }
  cout << "value of m[5][3]: " << m[5][3] << endl;
  cout << "size of m: " << m.size() << endl;
  cout << "size_of m: " << sizeof(m) << endl;
  */
  return dims;
}


// read the file
vector<vector<int> > load_array(const char* fileName, vector<int> dims) {
  vector<vector<int> > arr(dims[0], vector<int>(dims[1]));
  ifstream inFile(fileName);
  string line;
  int row = 0;
  // for each line in the text file
  while (getline(inFile, line)) {
    int col = 0;
    istringstream iss(line);
    cout << "";
    char c;
    while (iss >> c) {
      // cout << c << " ";
      col = col+1;
      int a = c - '0';
      // cout << a << " ";
      arr[row][col] = a;
      cout << arr[row][col] << " ";
    }
    row = row+1;
    cout << "\n";
  }

  // now should have the whole matrix filled in
  /*
  for (int i = 0; i < dims[0]; i++) {
    for (int j = 0; j < dims[1]; j++) {
      cout << arr[i][j] << " ";
    }
    cout << endl;
  }
  */
  return arr;
}


// get the name of the array file
const char* get_filename() {
  const char* fileName;
  string fname;
  cout << "what is file name? ";
  getline(cin, fname);
  fileName = fname.c_str();
  return fileName;
}


// show me the vector
void show_vector(vector<vector<int> > arr, vector<int> dims) {
  for (int i = 0; i < dims[0]; i++) {
    for (int j = 0; j < dims[1]; j++ ) {
      cout << arr[i][j] << " ";
    }
    cout << "\n";
  }
  return;
}


int main() {
  // get the dimensions from the user input
  vector<int> dims = get_dims();
  // get file name -- initially from user, then from list of files
  const char* fileName = get_filename();
  cout << "got filename as: " << fileName << endl;
  // then get the matrix for that file
  vector<vector<int> > arr = load_array(fileName, dims);
  // show_vector(arr, dims);
  return 0;
}











