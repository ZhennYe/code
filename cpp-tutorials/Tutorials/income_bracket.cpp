// income_bracket.cpp -- calculates taxes based on salary
// structure:
// first $5000 - 0% tax
// next $10000 - 10% tax
// next $20000 - 15% tax
// any additional - 20% tax

#include <iostream>
#include <cstdlib>
using namespace std;

int main()
{
	double income;
	cout << "Enter income (no $ sign):";
	cin >> income;
	double total = 0.0;
	
	double fourth;
	if (income-35000 >= 0)
	{
		total = total + (income-35000)*0.2;
		fourth = income-35000;
		cout << "Income in 4th tax bracket: $" << fourth << endl;
		fourth = 35000;
	}
	else
	{
		fourth = income;
		cout << "No income in 4th tax brakcet.\n";
	}
	double third;
	if (fourth-15000 >= 0)
		{
		third = fourth-15000;
		total = total + third*0.15;
		cout << "Income in 3rd tax bracket: $" << third << endl;
		third = 15000;
		}
	else
	{
		third = income;
		cout << "No income in 3rd tax bracket.\n";
	}
	double second;
	if (third-5000 >= 0)
	{
		second = third-5000;
		total = total + second*0.1;
		cout << "Income in 2nd tax bracket: $" << second << endl;
	}
	else
	{
		second = income;
		cout << "No income in 2nd tax bracket.\n";
	}
	cout << "Total taxes owed: $" << total << endl;
	
	return 0;
}
