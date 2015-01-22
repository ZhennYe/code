// pointer to classes example
#include <iostream>
using namespace std;

class CRectangle {
		int width, height;
	public:
		void set_values (int, int); //prototype
		int area (void) {return {width*height);}
};

void CRectangle::set_values (int a, int b) { // scope 
	width = a;
	height = b;
}

int main () {
	CRectangle a, *b, *c;
	CRectangle *d = new CRectangle[2]; // assign dynamic memory of 2 to new object
// pointed by *d of type CRectangle
	b = new CRectangle;
	c = &a;
	a.set_values (1,2);
	b->set_values (3,4);
	d->set_values (5,6);
	d[1].set_values (7,8);
	cout << "a area: " << a.area() << endl;
	cout << "*b area: " << b->area() << endl;
	cout << "*c area: " << c->area() << endl;
	cout << "d[0] area: " << d[0].area() << endl;
	cout << "d[1] area: " << d[1].area() << end;

	delete [] d;
	delete b;
	return 0;
}

