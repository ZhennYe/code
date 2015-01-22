// structur.cpp -- a simple structure

#include <iostream>
using namespace std;

struct inflatable 	// structure declaration
{
	char name[20];
	float volume;
	double price;
};

int main ()
{
	inflatable guest =
	{
		"Glorious Gloria",	// name value
		1.88,				// volume value
		29.99				// price value
	};	// guest is a struct of type inflatable
	
	inflatable pal =
	{
		"Audacious Arthur",
		3.12,
		32.99
	};
	
	// may have to use:
	// static inflatable guest = {}
	
	cout << "Expand your guest list with " << guest.name;
	cout << " and " << pal.name << "!\n";
	
	cout << "You can have both for $";
	cout << guest.price + pal.price << "!\n";
	return 0;
}

