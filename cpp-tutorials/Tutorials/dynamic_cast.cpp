// dynamic_cast
// can be used only with pointers and references
// its purpose is to ensure the result of the type conversion is a valid complete
// 	object of the requested class

#include <iostream>
#include <exception>
using namespace std;

class CBase { virtual void dummy () {} };
class CDerived: public CBase { int a; };

int main () {
	try {
		CBase* pba = new CDerived;
		CBase* pbb = new CBase; // points to an object of class CBase, which is an incomplete
		CDerived* pd;			// object of class CDerived

		pd = dynamic_cast <CDerived*> (pba);
		if (pd==0) cout << "Null pointer on first type-cast" << endl;
		
		pd = dynamic_cast <CDerived*> (pbb); // 
		if (pd==0) cout << "Null pointer on second type-cast" << endl;

	} catch ( exception& e) {cout << "Exception: " << e.what ();}
	return 0;
}

// dynamic_cast is always successful when we cast a class to one of its base classes:
// 		class CBase { };
//		class CDerived: public CBase { };

//		CBase b; CBase* pb;
//		CDerived d; CDerived* pd;

//		pb = dynamic_cast<CBase*>(&d);		//ok: derived-to-base
//		pd = dynamic_cast<CDerived*>(&b);  	// wrong: base-to-derived

// dynamic_cast can also cast null pointers even between pointers to an unrelated class,
// amd cam also cast pointers of any type to void pointers (void*).
