// friend functions example
#include <iostream>
using namespace std;

class CRectangle {
		int width, height;		// private members that are accessible by CRectangle duplicate
								// due to friend call
	public:
		void set_values (int, int);	//prototype declaration (semicolon used)
		int area () { return (width*height);}	//output is area
	// input for area must be width and height, not a, b; hence the need to access private members
		friend CRectangle duplicate (CRectangle);
};

void CRectangle::set_values (int a, int b) { 	// full declaration for set_values 
					// notice scope operator
	width = a;
	height = b;
}

CRectangle duplicate (CRectangle rectparam)	// not a member of CRectangle, just has its access
{
	CRectangle rectres;
	rectres.width = rectparam.width*2;
	rectres.height = rectparam.height*2;
	return (rectres);
}

int main () {
	CRectangle recta, rectb;
	recta.set_values (2,3);
	rectb = duplicate (recta);
	cout << rectb.area() << endl;
	return 0;
}

// friend functions are not consistent with OOP, so best to use members of the same class
// to operations with them
