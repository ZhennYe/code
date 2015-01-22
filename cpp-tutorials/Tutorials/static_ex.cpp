// example of static members

#include <iostream>
using namespace std;

class CDummy {
	public:
		static int n;			// static members have same properties as global
		// variables but they also have scope; therefore the prototype declaration
		// cannot also initialize/define the variable
		CDummy () { n++; };		// contructor and overload operator
		~CDummy () { n++; };	// desctructor
};

int CDummy::n=0;				// scoped definition/initialization

int main () {
	CDummy a;
	CDummy b[5];
	CDummy * c = new CDummy;	// allocate dynamic memory for new CDummy member
	cout << a.n << endl;
	delete c;
	cout << CDummy::n << endl;
	return 0;
}

