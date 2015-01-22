// non-type parameters, including sequences of elements
// sequence template

#include <iostream>
using namespace std;

template <class T, int N>
class mysequence {
		T memblock [N];
	public:
		void setmember (int x, T value); // allows a member to be of type T
		T getmember (int x);
};

template <class T, int N>
void mysequence <T,N>::setmember (int x, T value)
{
	memblock[x]=value;
}

template <class T, int N>
T mysequence <T,N> :: getmember (int x) {
	return memblock[x];
}

int main () {
	mysequence <int,5> myints; // creates "function" template of type mysequence
	mysequence <double,5> myfloats; // with ints (5 of them) called myints

	myints.setmember (0,100); // set value of 100 for location 0
	myfloats.setmember (3,3.1416); // set value of pi for location 3

	cout << myints.getmember(0) << '\n';
	cout << myfloats.getmember(3) << '\n';
	return 0;
}

// if template definition were:
// 	template <class T=char, int N=10> class mysequence {...};
// new objects would be created by:
// 	mysequence <> myseq;
// which is equivalent to:
//	mysequence <char, 10> myseq;

