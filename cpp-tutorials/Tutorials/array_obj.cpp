// array_obj.cpp - functions with array objects (c++11 ?)
// must run with -std=c++11 option !!!!!!!!!!!
// useful for data input and storage

#include <iostream>
#include <array>
#include <string>
using namespace std;

// constant data
const int Seasons = 4;
const array<string, Seasons> Snames =
	{"Spring","Summer","Fall","Winter"};

// function prototypes
void fill(array<double, Seasons> * pa);
void show(array<double, Seasons> da);

int main()
{
	array<double, Seasons> expenses;
	fill (&expenses);
	show(expenses);
	return 0;
}

void fill(array<double, Seasons> * pa)
{
	for (int i = 0; i < Seasons; i++)
	{
		cout << "enter " << Snames[i] << " expenses: ";
		cin >> (*pa)[i];
	}
}

void show(array<double, Seasons> da)
{
	double total = 0.0;
	cout << "\nExpenses\n";
	for (int i = 0; i < Seasons; i++)
	{
		cout << Snames[i] << ": $" << da[i] << endl;
		total += da[i];
	}
	cout << "Total expenses: $" << total << endl;
}

