// basic file operations
#include <iostream>
#include <fstream>
using namespace std;

int main () {
	ofstream myfile;
	myfile.open ("example.txt");
	myfile << "Writing this to a file.\n";
	myfile.close();
	return 0;
}

// This code creates a file called example.txt and inserts a sentence into it in the same 
//	way we are used to do with cout, but using the file stream myfile instead.
