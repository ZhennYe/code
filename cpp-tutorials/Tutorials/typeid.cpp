// typeid

// allows to check the type of an expression
//		typeid (expression)
// returns a reference to a const object of type type_info, defined in the standard
//	header file <typeinfo>. This returned value can be compared with another one using
// 	== and != operators, or can obtain null-terminated char sequences representing the data
//	type or class name by using it's 'name()' member.

#include <iostream>
#include <typeinfo>
using namespace std;

int main () {
	int* a, b;
	a=0; b=0;
	if (typeid(a) != typeid(b))
	{
		cout << "a and b are of different types:\n";
		cout << "a is: " << typeid(a).name() << "\n"; // should be int * or Pi
		cout << "b is: " << typeid(b).name() << "\n"; // should be int or i
	}
	return 0;
}


