// remember-o-matic - functions new, delete and nothrow
#include <iostream>
#include <new>
using namespace std;

int main ()
{
	int i, n;
	int *p;
	cout << "How many numbers to enter?";
	cin >> i;
	p= new (nothrow) int[i]; // creates array at *p with dynamic memory 
	// (created by 'new' function); but does not throw an error, or
	// end the program, if no memory is available; the thrown error is suppressed
	if (p==0)
		cout << "Error - memory could not be allocated.\n";
	else
	{
		for (n=0;n<i;n++)
		{
			cout << "Enter number: ";
			cin >> p[n];
		}
		cout << "You entered: ";
		for (n=0;n<i;n++)
			cout << p[n] << ", ";
		delete [] p;
	}
	return 0;
}
