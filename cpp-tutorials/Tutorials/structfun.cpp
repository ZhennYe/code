// structfun.cpp functions that take structure arguments
#include <iostream>
#include <cmath>
using namespace std;

// structure delcarations
struct polar
{
	double distance;
	double angle;
};
struct rect
{
	double x;
	double y;
};

// prototypes
polar rect_to_polar(rect xypos);
void show_polar(polar dapos);

int main()
{
	rect rplace;
	polar pplace;
	
	cout << "Enter X and Y coords: ";
	while (cin >> rplace.x >> rplace.y) // two cin expressions
				// inputs need to be type double for while to continue
				// cin is essentially a boolean value where if input
				// is !double cin evaluates to 0 and control exits while loop
	{
		pplace = rect_to_polar(rplace);
		show_polar(pplace);
		cout << "Next 2 numbers (q to quit): ";
	}
	cout << "Done.\n";
	return 0;
}

// convert rectangular to polar coords
polar rect_to_polar(rect xypos)
{
	polar answer;
	answer.distance = sqrt(xypos.x*xypos.x + xypos.y*xypos.y);
	answer.angle = atan2(xypos.y, xypos.x);
	return answer; // returns a polar struct
}

// show polar coords
void show_polar(polar dapos)
{
	const double Rad_to_deg = 57.29; // 180/pi
	cout << "distance = " << dapos.distance;
	cout << ", angle = " << dapos.angle * Rad_to_deg;
	cout << " degrees\n";
}
