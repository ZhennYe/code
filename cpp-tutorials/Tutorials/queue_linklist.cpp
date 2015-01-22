// queue_linklist.cpp - queues using linked lists
#include <iostream>
using namespace std;

struct node
{
	int data;
	node *link;
};

class lqueue
{
	private:
		node *front, *rear;
	
	public:
		lqueue()
		{
			front = NULL;
			rear = NULL;
		}
		
		void add (int n)
		{
			node *tmp;
			tmp = new node;
			if (tmp == NULL)
				cout << "\n Queue full.\n";
			
			tmp->data = n;
			tmp->link = NULL;
			if (front = NULL)
			{
				rear = front = tmp;
				return;
			}	// else
				rear->link = tmp;
				rear = rear->link;
		}
		
		int del()
		{
			if (front == NULL)
			{
				cout << "\n Queue empty.\n";
				return 0;
			}
			node *tmp;
			int n;
			n = front->data;
			tmp = front;
			front = front->link;
			delete tmp;
			return n;
		}
		
		~lqueue()
		{
			if (front == NULL)
				return;
			node *tmp;
			while (front != NULL)
			{
				tmp = front;
				front = front->link;
				delete tmp;
			}
		}
};

int main()
{
	lqueue q;
	q.add(11);
	q.add(22);
	q.add(33);
	q.add(44);
	q.add(55);
	cout << "\nItem deleted = " << q.del();
	cout << "\nItem deleted = " << q.del();
	cout << "\nItem deleted = " << q.del();
	return 0;
}
		
			
