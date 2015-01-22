// queue1.cpp - queue example
// shows that addition is done at the end of the queue while
// deletions occur at the beginning of the queue

#include <iostream>
using namespace std;
#define MAX 5		// maximum constants in queue

class queue
{
	private:
		int t[MAX];
		int a1;	// addition end ("back" of queue)
		int d1;	// deletion end ("front" of queue)
		
	public:
		queue()
		{
			d1 = -1;
			a1 = -1;
		}
		
		void del()
		{
			int tmp;
			if(d1==-1)
			{
				cout << "Queue is empty\n";
			}
			else
			{
				for( int j=0; j <= a1; j++ )
				{
					if( (j+1) <= a1)	// if there is room
					{
						tmp = t[j+1];	// move t[j+1] value up to t[j] via tmp
						t[j] = tmp;		// 
					}
					else   	// if no room
					{
						a1--;	// decrement the addition end (queue is full)
					
					if (a1==-1)
						d1 = -1;
					else
						d1=0;
					}
				}
			}
		}
		
void add(int item)
{
	if (d1==-1 && a1==-1)
	{
		d1++;
		a1++;
	}
	else
	{
		a1++;
		if(a1==MAX)
		{
			cout << "Queue is full\n";
			a1--;
			return;
		}
	}
	t[a1] = item;
}

void display()
{
	if (d1 != -1)
	{
		for (int iter=0; iter<=a1; iter++)
			cout << t[iter] << " ";
		}
		else
			cout << "Queue is empty\n";
		}
	}; 	// closes class queue
	
	int main()
	{
		queue a;
		int data[5] = {32, 23, 45, 99, 24};
		int iter;
		
		cout << "Queue before adding elements: ";
		a.display();
		cout << endl << endl;
		
		for (iter=0; iter <5; iter++) // wanted to initialize iter here
		{															// with int iter = 0
			a.add(data[iter]);					// but this creates a scope that sucks
			cout << "Addition number : " << (iter+1) << " : ";
			a.display();
			cout << endl;
		}
		
		cout << endl;
		cout << "Queue after adding elements: ";
		a.display();
		cout << endl << endl;
		
		for (iter=0; iter < 5; iter++)
		{
			a.del();
			cout << "Deletion number : " << (iter+1) << " : ";
			a.display();
			cout << endl;
		}
		return 0;
}

