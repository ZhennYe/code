// static_cast
// can perform conversions between pointers to related classes, and in both directions
// between base and derived classes; no safety check is performed during run-time.
// the 'overhead' of the type-safety checks of dynamic_cast is avoided:

// 		class CBase {};
// 		class CDerived: public CBase {};
// 		CBase * a = new CBase;
// 		CDerived * b = static_cast<CDerived*>(a);

//	this is valid, although 'b' would point to an incomplete object pf the class CDerived
//	and could lead to runtime errors if dereferenced

// static_cast can also perform non-pointer conversions that can be performed implicitly,
//	such as standard conversion between fundamental types:
//		double d = 3.14159265;
//		int i = static_case <int> (d);


