// standard exceptions
#include <iostream>
#include <exception> 
// <exception> contains 'what', a virtual member function that returns a null-terminated
// 		character sequence (char*); as the base, this can then be overwritten in derived
//		classes (such as below: 'myexception') to contain a different description of error
using namespace std;

class myexception: public exception // make public all accessible members of <exception>
{
	virtual const char* what () const throw() // value pointed by 'what' is below
	{
		return "My exception occurred";
	}
} myex; // declare an instance of class 'myexception' called 'myex' with the above properties

int main () {
	try
	{
		throw myex;
	}
	catch (exception& e) // this handler catches exceptions by reference; exception& is
//	(in this case) a reference of the base class <exception>; therefore it will catch
//	all instances of classes derived from <exception>, including myexception and its
// 	member myex
	{
		cout << e.what() << endl;
	}
	return 0;
}

// exceptions thrown by std::exception :
// bad_alloc			thrown by new on allocation failure
// bad_cast				thrown by dynamic_cast when fails with a referenced type
// bad_exception 		thrown when an exception type doesn't match any catch
// bad_typeid			thrown by typeid
// ios_base::failure 	thrown by functions in the iostream library

// example:
// try
//	{
//		int * myarray= new int[1000];
//	}
//	catch (bad_alloc&) // replacing with (exception& e) will catch all standard exceptions
//	{					//	and refer to whichever one is thrown by 'e'
//		cout << "Error allocating memory." << endl;
//	}
