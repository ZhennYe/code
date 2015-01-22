// friend classes example
#include <iostream>
using namespace std;

class CSquare;	//"empty" declaration of CSquare; required since CRectangle refers to CSquare.
	// since definition of CSquare is later, an empty declaration is needed before CRectangle.

class CRectangle {
		int width, height;
	public:
		int area ()
			{return (width*height);}
		void convert (CSquare a);
};		// class terminates with semicolon

class CSquare {
	private:
		int side;
	public:
		void set_side (int a)
			{side =a;}
		friend class CRectangle;	// allows CRectangle to access private members of CSquare
									// BUT CSquare CANNOT access private members of CRectangle
	// nor are friendships transitive -- the friend of a friend is not a friend to the 1st class
};		// classes terminate with semicolon

void CRectangle::convert (CSquare a) {
	width = a.side;
	height = a.side;
}	// functions do not have semicolons except for prototype declarations

int main () {
	CSquare sqr;
	CRectangle rect;
	sqr.set_side (4);
	rect.convert(sqr);
	cout << rect.area() << endl;
	return 0;
}

