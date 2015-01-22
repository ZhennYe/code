// reinterpret_cast

// converts any pointer type to any other pointer type, even of unrelated classes
// all pointer conversions are allowed: neither the content pointed nor the pointer
//	type itself is checked.

// can also cast pointers to or from integer types. the format in which the integet value
// 	represents a pointer is platform-specific; the only guarantee is that a pointer case to 
//	an integet type large enough to contain it is granted to be able to be cast back to a
//	valid pointer.

// conversions performed by reinterpret_case (but not by static_cast) have no specific uses 
//	in c++; they are low-level operations that result in code which is general system-specific
//	and thus not portable:
//		class A {};
//		class B {};
//		A* a = new A;
//		B* b = reinterpret_cast<B*>(a);
// This is valid C++ code, although it does not make much sense, since now we have a pointer
//	 that points to an object of an incompatible class, and thus dereferencing it is unsafe.
