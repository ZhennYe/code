// writing a text file
// don't include ios::binary flag in opening mode

#include <iostream>
#include <fstream>
using namespace std;

int main () {
	ofstream myfile("example.txt");
	if (myfile.is_open())
	{
		myfile << "This is a line.\n";
		myfile << "This is another line.\n";
		myfile.close();
	}
	else cout << "Unable to open file.\n";
	return 0;
}

