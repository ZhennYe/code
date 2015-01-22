// arrfun3.cpp -- array functions and const
#include <iostream>
using namespace std;
const int Max = 5;

// function prototypes
int fill_array(double ar[], int limit);
void show_array(const double ar[], int n); // const prohibits changing data
void revalue(double r, double ar[], int n);

int main()
{
	double properties[Max];
	
	int size = fill_array(properties, Max);
	show_array(properties, size);
	if (size > 0)
	{
		cout << "Enter re-valuation factor: ";
		double factor;
		while (!(cin >> factor)) 	// "bad input"
		{
			cin.clear();
			while (cin.get() != '\n')
				continue;
			cout << "Bad input; enter a number: ";
		}
		revalue(factor, properties, size);
		show_array(properties, size);
	}
	cout << "Done.\n";
	cin.get();
	cin.get();
	return 0;
}

int fill_array(double ar[], int limit)
{
	double temp;
	int i;
	for (i = 0; i < limit; i++)
	{
		cout << "Enter value #" << (i + 1) << ": ";
		cin >> temp;
		if (!cin)	// bad input
		{
			cin.clear();
			while (cin.get() != '\n')
				continue;
			cout << "Bad input; process terminated.\n";
			break;
		}
		else if (temp < 0)	// negative is signal to terminate
			break;
		ar[i] = temp;
	}
	return i;
}

// call to const double ar means this function can read but not alter
// any data
void show_array(const double ar[], int n)
{
	for (int i = 0; i < n; i++)
	{
		cout << "Property #" << (i + 1) << ": $";
		cout << ar[i] << endl;
	}
}

// multiplies each element of ar[] by r
void revalue(double r, double ar[], int n)
{
	for (int i = 0; i < n; i++)
		ar[i] *= r; // short notation, like += or -=
}

