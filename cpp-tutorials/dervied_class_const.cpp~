//constructor and derived classes
#include <iostream>
using namespace std;

class mother {
	public:
		mother ()
			{ cout << "mother: no params \n"; }
		mother (int a)
			{ cout << "mother: int params \n";}
};

class daughter : public mother {
	public:
		daughter (int a)	// no constructor specified - call default const
			{ cout << "daughter: int params \n \n";}
};

class son : public mother {
	public:
		son (int a) : mother (a)	// constructor specified - call this
			{ cout << "son: int params \n \n";}
};

int main () {
	daughter cynthia (0);
	son daniel(0);

	return 0;
}

