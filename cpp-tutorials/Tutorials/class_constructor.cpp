// class constructor example
#include <iostream>
using namespace std;

class CRectangle {
		int width, height;
	public:
		CRectangle (int, int);
		int area () {return (width*height);}
};

// a constructor is delcared as a function with the same name as the class
// and the scope operator; it returns no output - not even void.
// constructors are run when the class is called.
CRectangle::CRectangle (int a, int b) {
	width = a;
	height = b;
}

int main ()
{
	CRectangle recta (3,4); //contructor called here
	CRectangle rectb (5,6); // constructor called here too
	cout << "rect a area: " << recta.area() << endl;
	cout << "rect b area: " << rectb.area() << endl;
	return 0;
}

