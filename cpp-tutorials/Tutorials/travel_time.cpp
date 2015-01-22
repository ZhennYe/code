// travel_time.cpp -- using structures and functions
#include <iostream>
using namespace std;

struct travel_time
{
	int hours;
	int mins;
};
const int Mins_per_hr = 60;

// function with output type (struct) travel_time
travel_time sum(travel_time t1, travel_time t2); // constructors
void show_time(travel_time t);

int main()
{
	travel_time day1 = {5, 45};
	travel_time day2 = {4, 55};
	
	travel_time trip = sum(day1, day2);
	cout << "2-day travel time: ";
	show_time(trip);
	
	travel_time day3 = {4, 32};
	cout << "3-day travel time: ";
	show_time(sum(trip, day3));
	
	return 0;
}

travel_time sum(travel_time t1, travel_time t2)
{
	travel_time total; // init of type travel_time
	
	total.mins = (t1.mins + t2.mins) % Mins_per_hr; // % modulo (rem.)
	total.hours = t1.hours + t2.hours + 
					(t1.mins + t2.mins) / Mins_per_hr;
	return total;
}

void show_time(travel_time t)
{
	cout << t.hours << " hours, ";
	cout << t.mins << " mins.\n";
}
	 
