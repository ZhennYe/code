// template specialization

#include <iostream>
using namespace std;

// class template
template <class T>
class mycontainer {
		T element;
	public:
		mycontainer (T arg) {element=arg;}
		T increase () {return ++element;}
	};

// class template specialization
template <> // explicitly delcares as template specialization
class mycontainer <char> { // <char> = specialization parameter idenitifies the type
// we're going to declare a template class specialization
		char element;
	public:
		mycontainer (char arg) {element=arg;}
// must declare all of its members (even if they're exactly equal to generic template)
// because specialization templates don't inherit from generic
		char uppercase ()
		{
			if ((element>='a') && (element<='z'))
			element += 'A'-'a';
			return element;
		}
	};

int main () {
	mycontainer <int> myint (7); // declare instance <type> variable_name (value)
	mycontainer <char> mychar ('j');
	cout << myint.increase() << endl;
	cout << mychar.uppercase() << endl;
	return 0;
}
// difference between template classes is:
// generic template class: template <class T> class mycontainer {...};
// specialization class:   template <> class mycontainer <char> {...};
