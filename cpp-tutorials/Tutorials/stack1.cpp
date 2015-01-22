// string / stack example
#include <iostream>
using namespace std;

#define MAX 10	// maximum stack content

class stack
{
	private:
		int arr[MAX];	// contains all of data
		int top;		// contains location of topmost data pushed onto stack
		
	public:
		stack()		// constructor
		{
			top = -1; 		// sets top location to -1 indicating an empty stack
		}
		
		void push (int a)	// push - add value function
		{
			top++;
			if (top<MAX)
			{
				arr[top] = a; 	// if stack is vacant store value in array
			}
			else
			{
				cout << "Stack is full!" << endl;
				top--;
			}
		}
		
		int pop()			// push adds, pop deletes items; returns item to user
		{
			if(top==-1)
			{
				cout << "Stack is empty!" << endl;
			}
			else
			{
				int data = arr[top];		// set data = topmost value in array
				arr[top] = NULL;		// set original position to null
				// this throws a non-fatal error
				top--;						// decrement
				return data;		// return deleted item
			}
		}
};

int main()
{
	stack a;
	a.push(3);
	cout << "3 is pushed\n";
	a.push(10);
	cout << "10 is pushed\n";
	a.push(1);
	cout << "1 is pushed\n\n";
	
	cout << a.pop() << " is popped\n";
	cout << a.pop() << " is popped\n";
	cout << a.pop() << " is popped\n";
	return 0;
}

		
