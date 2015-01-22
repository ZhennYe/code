// using
#include <iostream>
using namespace std;

namespace first
{
	int x=5;
	int y=10;
}

namespace second
{
	double x=3.1416;
	double y=2.7183;
}

int main () {
	using first::x; // introduces identifier from a namespace into current declarative
	using second::y;	// region
	cout << x << endl;
	cout << y << endl;
	cout << "Now with scopes:\n";
	cout << first::y << endl;
	cout << second::x << endl;
	return 0;
}

// "using" can also be used to introduce an entire namespace:
// using namespace first;
// cout << x << endl;
// cout << y << endl;
	// this introduces all of 'first's identifiers into current declarative region
