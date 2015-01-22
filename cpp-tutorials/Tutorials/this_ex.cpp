// example for keyword 'this'
// 'this' represents a pointer to the object whose member function is being executed
// it is a pointer to the object itself
#include <iostream>
using namespace std;

class CDummy {
	public:
		int isitme (CDummy& param);
};

int CDummy::isitme (CDummy& param)	
{
	if (&param == this) return true;	// checks whether the parameter passed --
	// in this case &param -- to a member function is the object itself;
	// is &param == 'this'?
	else return false;
}

int main () {
	CDummy a;
	CDummy* b = &a;
	if ( b->isitme(a) )
		cout << "yes - &a is b \n";
	return 0;
}

// also used in 'operator=' member functions, such as:
// CVector& CVector::operator= (const CVector& param)
// {
//		x=param.x;
//		y=param.y;
//		return *this;
// }

