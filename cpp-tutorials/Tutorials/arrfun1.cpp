// arrfun1.cpp -- function with an array argument
#include <iostream>
using namespace std;
const int ArSize = 8;
int sum_arr(int arr[], int n);	// prototype
int main()
{
	int cookies[ArSize] = {1,2,3,4,5,6,7,8};
	// may need to use static array initialization
	
	cout << "'value' of cookies: " << cookies << endl;
	cout << "cookies[0]: " << cookies[0] << endl;
	
	int sum = sum_arr(cookies,ArSize);
	cout << "Total cookies eaten: " << sum << endl;
	return 0;
}

// this function returns the sum of an int array
int sum_arr(int arr[], int n)
{
	int total = 0;
	
	for (int i = 0; i < n; i++)
		total = total + arr[i];
	return total;
}
