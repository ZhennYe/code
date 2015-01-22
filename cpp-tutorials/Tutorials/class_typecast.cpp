// class type-casting
#include <iostream>
using namespace std;

class CDummy {
	float i, j;
};

class CAddition {
		int x, y;
	public:
		CAddition (int a, int b) {x=a; y=b; }
		int result () {return x+y;}
};

int main () {
	CDummy d;
	CAddition* padd;
	padd = (CAddition*) &d;
	cout << padd->result () << endl; // this returns an unexpected result since it is a pointer
	return 0;	// which is then cast as a reference
}

// Traditional explicit type-casting allows to convert any pointer into any other pointer type, 
// independently of the types they point to. To control these types of conversions,
// use one of these casting operators:
//	 	dynamic_cast <new_type> (expression)
//		reinterpret_cast <new_type> (expression)
//		static_cast <new_type> (expression)
//	 	const_cast <new_type> (expression)
// which differ from 'traditional' type-casting, such as:
// 		(new_type) expression
//		new_type (expression)
