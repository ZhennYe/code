// function template
#include <iostream>
using namespace std;

template <class T> // structure of template: template <class name> (parameters)
T GetMax (T a, T b) {
	T result;	// by using a value of type T to be returned, it ensures that
	// the return call will be of the same data type as the data passed to GetMax
	result = (a>b)? a:b; // result = a if a>b and b if b>a
	return (result);
}

int main () {
	int i=5, j=6, k;
	long l=10, m=5, n; // compiler will run GetMax with whichever data type is called
	k = GetMax <int> (i,j);
	n = GetMax <long> (l,m);
	cout << k << endl;
	cout << n << endl;
	return 0;
}


