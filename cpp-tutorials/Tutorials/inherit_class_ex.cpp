// inheritance class example
#include <iostream>
using namespace std;

class CPolygon {
	protected:	// similar to "private:" specifier but differs with inheritance;
	// an inherited class can access protected members of its parent class but
	// not private members; ALWAYS USE PROTECTED SPCIFIER WITH INHERITANCE if 
	// you want inherited classes to be able to access "private" members.
		int width, height;
	public:
		void set_values (int a, int b)
			{ width = a; height = b;}
	};

class CRectangle: public CPolygon { // CRectangle inherits properties of CPolygon and
	// establishes them as PUBLIC, even the protected members; you can still
	// create protected and private members, but the members inherited from
	// CPolygon will be public in this case.
	public:
		int area ()
			{ return (width * height);}
	};

class CTriangle: public CPolygon {
	public:
		int area ()
			{ return (width * height /2);}
	};

int main () {
	CRectangle rect;
	CTriangle trgl;
	rect.set_values (4,5);
	trgl.set_values (4,5);
	cout << "rect area = " << rect.area() << endl;
	cout << "trinagle area = " << trgl.area() << endl;
	return 0;
}

// default access level for inheritance is private for class
// and public for struct

// classes inherit everything except specified constructor and destructor, operator =() 
// members and friends; but default const and destruct are always called

