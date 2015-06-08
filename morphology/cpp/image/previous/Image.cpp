// Image.cpp
// also basically from SO question 13750142
//

#ifndef _Image_h
#define _Image_h
#define WIDTH 96 // these will change based on the image size
#define HEIGHT 96 // wonder if I can set them at run time...


#include "Image.h"
#include <cmath>

#endif


using namespace std;
typedef unsigned char unchar;


// constructor
MImage::MImage(const char* fileName) {
  imageData = new unchar* [HEIGHT]; // create new array size of height whatever

  for (int i = 0; i < HEIGHT; i++) {
    imageData[i] = new unchar [WIDTH]; // this is the matrix part
  }

  // reading the image
  pInFile = new ifstream;
  pInFile->open(fileName, ios::in | ios::binary); // open file, read as binary
  pInFile->seekg(0, ios::beg); // position at beginning of file
  pInFile->read(reinterpret_cast<char*>(imageHeaderData),1078); // read header
                                                                // into array

  for (int i = 0; i < HEIGHT; i++) {
    pInFile->read(reinterpret_cast<char*>(imageData[i]),WIDTH); // read row into array
  }
  
  for (int i = 0; i < HEIGHT; i++) {
    for (int j = 0; j < WIDTH; j++) {
      cout << " " << imageData[i][j] << " ";
    }
    cout << "\n";
  }

  cout << imageHeaderData << endl;

  pInFile->close();
  // apply any filtering here?
}



MImage::~MImage() {
  delete pInFile;
  // delete pOutFile;
  for (int i = 0; i < HEIGHT; i++) {
    delete[] imageData[i];
  }

  delete[] imageData;
}


/*
// don't need this
void MImage::write(const char* fileName) {
  smoothFilter(); // constructor
  pOutFile = new ofstream;
  pOutFile->open(fileName, ios::out | ios::trunc | ios::binary);
  pOutFile->write(reinterpret_cast<char*>(imageHeaderData), 1078); //write header data onto output

  for(int i = 0; i < HEIGHT; i++){
    pOutFile->write(reinterpret_cast<char*>(filteredData[i]),WIDTH); // write new image data.
  }

  pOutFile->close(); //close stream
}
*/


// here is where the image smoothing occurs
//
// then EOF
