// swaps.cpp -- swapping variables with references and pointers
#include <iostream>

void swapr(int & a, int & b); // a and b are references of ints
void swapp(int * p, int * q); // p and q are addresses of ints
void swapv(int a, int b); // a and b are new variables

int main()
{
	using namespace std;
	int wallet1 = 300;
	int wallet2 = 350;
	cout << "wallet1 = $" << wallet1;
	cout << ", wallet2 = $" << wallet2 << endl;
	
	cout << "Using references to swap contents:\n";
	swapr(wallet1, wallet2); // pass variables
	cout << "wallet1 = $" << wallet1;
	cout << ", wallet2 = $" << wallet2 << endl;
	
	cout << "Using pointers to swap contents:\n";
	swapp(&wallet1, &wallet2); // pass addresses of variables
	cout << "wallet1 = $" << wallet1;
	cout << ", wallet2 = $" << wallet2 << endl;
	
	cout << "Try to pass by value:\n"; // does not work
	swapv(wallet1, wallet2); // pass values of variables
	cout << "wallet1 = $" << wallet1;
	cout << ", wallet2 = $" << wallet2 << endl;
	return 0;
}

void swapr(int & a, int & b)
{
	int temp;
	temp = a; //use refs a and b for values of variables
	a = b;
	b = temp;
}

void swapp(int * p, int * q)
{
	int temp;
	temp = *p; // use *p and *q for values of variables
	*p = *q;
	*q = temp;
}

void swapv(int a, int b) // this does not work; a and b are swapped
{						 // but not wallet1 and wallet2
	int temp;
	temp = a; // use and and b for values of variables
	a = b;
	b = temp;
}
