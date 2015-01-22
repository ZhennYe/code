// exceptions
#include <iostream>
using namespace std;

int main ()	{
	try
	{
		throw 20; // this passes the single value of the throw ('20')
	}	// to the exception handler ('catch') which assigns it an idenitifier
	catch (int e)
	{
		cout << "An exception occurred, number: " << e << endl;
	}
	return 0;
}
// using elipsis (...) as catch param, the handler will catch any exception
// no matter the type of exception thrown; can be used as backup if other
// catches don't catch an unexpected throw:

//	try {
//	 	code here
//	}
//	catch (int param) { cout << "int exception"; }
//	catch (char param) { cout << "char exception"; }
//	catch (...) { cout << "default exception"; }

// after an exception is thrown, execution resumes after 'try' NOT after 'throw' statement

// throw with no arguments throws exception to a 'higher up' catch (in nested 'try' blocks):
//	try {
//	try {
// 		code here
//		}
//		catch (int n) {
//		throw; // throws to catch(...) below
//		}
//	}
//	catch (...) { // elipsis catches all exceptions
//	cout << "Exception occurred";
//  }

// throws can also be appended to function declarations:
// float myfunction (char param) throw (int); // can only throw exceptions of type int
// int myfunction (int param) throw (); // is not allowed to throw exceptions
// int myfunction (int param); // can throw exception of any kind
