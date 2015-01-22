// constructor and destructor example
#include <iostream>
using namespace std;

class CRectangle {
		int *width, *height;
	public:
		CRectangle (int, int);	// constructor prototype
		~CRectangle ();			// destructor prototype
		int area () {return (*width * *height);}
};

CRectangle::CRectangle (int a, int b) {	// constructor
	width = new int;			// allocates dynamic memory off the heap
	height = new int;
	*width = a;
	*height = b;
}

CRectangle::~CRectangle () {	// destructor (~)
	delete width;				// returns dynamic memory to the heap
	delete height;
}

int main () {
	CRectangle recta (3,4), rectb (5,6);
	cout << "rect a area: " << recta.area() << endl;
	cout << "rect b area: " << rectb.area() << endl;
	return 0;
}

