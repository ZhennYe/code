// function macro, define, undef, ifdef, ifndef
#include <iostream>
using namespace std;

#define getmax(a,b) ((a)>(b)?(a):(b))
int main() {
	int x=5, y;
	y= getmax(x,2);
	cout << y << endl;
	cout << getmax(7,x) << endl;
	return 0;
}

// can also use "#undef" to undefine

// '#' can also be used to replace parameter with a string literal (as though in quotes)
//		#define str(x) #x
//		cout << str(test);	// this is same as cout << "test";

// '##' concatenates two arguments together
//		#define glue(a,b) a##b
//		glue(c, out) << "test"; //	same as cout << "test"

// Conditional inclusions: include or discard part of code if certain conditions are met.
// #ifdef allows a section to be compiled only if something is defined (regardless of value)
//		#ifdef TABLE_SIZE
//		int table[TABLE_SIZE];
//		#endif
// #ifndef does the opposite: code is only executed if specifier identifier is undefined
//		#ifndef TABLE_SIZE
//		#define TABLE_SIZE 100
//		#endif
//		int table[TABLE_SIZE];
// #if, #else and #elif (elseif) specify conditions to be met in order for code to be run.
// all chained directives end with #endif

// #if defined  is the same as  #ifdef
// #if !defined  is the same as  #ifndef
