// structures and strings
#include <iostream>
#include <sstream>
#include <string>
using namespace std;

struct movies_t {	// declare structure
	string title;	
	int year;		// objects of type movies_t will have two elements, title and year
} mine, yours;		// optional immediate declaration of objects of type movies_t

void printmovie (movies_t movie); // "blank declaration" so that function can be defined later
	// distinguished by semicolon after function declaration which is normally omitted

int main ()
{
	string mystr;

	mine.title = "2001 - A Space Odysseey"; // nested elements of structures are accessed in this way
	mine.year = 1968;

	cout << "What is your favorite movie? ";
	getline (cin, yours.title);
	cout << "What year? ";
	getline (cin, mystr);
	stringstream(mystr) >> yours.year; // import from string to different type

	cout << "My favorite movie is: ";
	printmovie (mine); 
	cout << "And yours is: ";
	printmovie (yours);
	return 0;
}

void printmovie (movies_t movie)
{
	cout << movie.title;
	cout << " (" << movie.year << ")\n";
}

