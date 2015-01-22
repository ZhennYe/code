// reading a text file
#include <iostream>
#include <fstream>
#include <string>
using namespace std;

int main () {
	string line;
	ifstream myfile ("example.txt");
	if (myfile.is_open())
	{
		while (! myfile.eof() ) // returns true if end of file has been reached (state flag)
		{
			getline (myfile,line);
			cout << line << endl;
		}
		myfile.close();
	}

	else cout << "Unable to oprn file.\n";
	return 0;
}

// other state flags
// bad() - returns true if reading or writing operation fails
// fail() - returns true in same cases as bad(), but also in case that a format error
//		happens, such as when an alphabetical char is extracted when trying to read an int
// good() - returns false in same cases as bad() and fail() return true
// clear () - resets state flags; no params accepted

// 'get' and 'put' stream pointers:
//		ifstream and istream have a 'get pointer' that points to the elements to be read
//			in the next input operation
//		ofstream and ostream have 'put pointer' that points to location where next
//			elements has to be written
//		fstream inherits both 'get' and 'put' pointers from iostream
//			iostream is derived from both istream and ostream

