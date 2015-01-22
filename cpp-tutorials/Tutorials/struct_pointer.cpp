// struct_pointer.cpp - functions with pointer to structure arguments
#include <iostream>
#include <cmath>
using namespace std;

// structure templates
struct polar
{
	double distance; 	// from origin
	double angle;		
};
struct rect
{
	double x;
	double y;
};

// prototypes
void rect_to_polar(const rect * pxy, polar * pda);
void show_polar(const polar * pda);

int main()
{
	rect rplace;
	polar pplace;
	
	cout << "Enter x and y values: ";
	while (cin >> rplace.x >> rplace.y)
	{
		rect_to_polar(&rplace, &pplace);	// pass addresses
		show_polar(&pplace);				// pass address
		cout << "Next two numbers (q to quit): ";
	}
	cout << "Done.\n";
	return 0;
}

// show polar coordinates
void show_polar(const polar * pda)
{
	const double Rad_to_deg = 57.29;
	
	cout << "Distance = " << pda->distance;
	cout << ", angle = " << pda->angle * Rad_to_deg;
	cout << " degrees.\n";
}

// convert rect to polar coords
void rect_to_polar(const rect * pxy, polar * pda)
{
	pda->distance = 
		sqrt(pxy->x * pxy->x + pxy->y * pxy->y);
	pda->angle = atan2(pxy->y, pxy->x);
}

