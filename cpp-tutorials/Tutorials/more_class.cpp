// one class, 2 objects
#include <iostream>
using namespace std;

class CRectangle {
		int x, y;
	public:
		void set_values (int, int);
		int area () {return (x*y);}
};

void CRectangle::set_values (int a, int b) {	// scope operator
	x = a;
	y = b;
}

int main ()
{
	CRectangle recta, rectb;	// two objects of class CRectangle
	recta.set_values (3,4);
	rectb.set_values (5,6);
	cout << "rect a area: " << recta.area () << endl;
	cout << "rect b area: " << rectb.area () << endl;
	return 0;
}

