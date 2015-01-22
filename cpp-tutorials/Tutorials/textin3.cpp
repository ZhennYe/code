// textin3.coo -- reading chars to end of file
#include <iostream>
using namespace std;

int main ()
{
	char ch;
	int count = 0;
	cin.get(ch);	// attempt to read a char
	while (cin.fail() == false)	// use failbit to test for EOF
	{
		cout << ch;	 	// echo character
		++count;
		cin.get(ch);	// attempt to read another char
	}
	cout << endl << count << " characters have been read. \n";
	return 0;
}

