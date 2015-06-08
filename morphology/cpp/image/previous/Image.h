// Image.h
// -- this was basically copied from SO question 13750142

#include <fstream>

using namespace std;
typedef unsigned char unchar; // for simplicity

class MImage {

  public:
    MImage(const char* fileName); // constructor
    ~MImage(); // destructor
    //  void write(const char* fileName); // I don't use this since I don't re-write file
    //  void smoothFilter(); // also don't use this
  private:
    ifstream* pInFile;
    ofstream* pOutFile;
    unchar imageHeaderData[1078]; //this is header size for .bmp, need .tif
    unchar** imageData;
};



