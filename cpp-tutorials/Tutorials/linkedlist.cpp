// linkedlist.cpp - linked list example
#include <iostream>
using namespace std;

class linklist
{
	private:
	
		struct node
		{
			int data;
			node *link;
		} *p;
	
	public: 
		linklist();
		void append (int num);	// prototype declarations
		void add_as_first (int num);
		void addafter (int c, int num);
		void del (int num);
		void display();
		int count();
		~linklist(); // destructor
};

linklist::linklist()
{
	p = NULL;
}

void linklist::append(int num)
{
	node *q, *t;
	
	if (p == NULL)
	{
		p = new node;	// allocate dynamic memory
		p->data = num; // = (*p).data
		p->link = NULL;
	}
	else
	{
		q = p;
		while (q->link != NULL)
			q = q->link;
		
		t = new node;
		t->data = num;
		t->link = NULL;
		q->link = t;
	}
}

void linklist::add_as_first (int num)
{
	node *q;
	
	q = new node;
	q->data = num;
	q->link = p;
	p = q;
}

void linklist::addafter (int c, int num)
{
	node *q, *t;
	int i;
	for (i = 0, q = p; i < c; i++)
	{
		q = q->link;
		if (q == NULL)
		{
			cout << "\nThere are fewer than " << c << " elements.\n";
			return;
		}
	}
	
	t = new node;
	t->data = num;
	t->link = q->link;
	q->link = t;
}

void linklist::del (int num)
{
	node *q, *r;
	q = p;
	if (q->data == num)
	{
		p = q->link;
		delete q;
		return;
	}
	
	r = q;
	while (q != NULL)
	{
		if (q->data == num)
		{
			r->link = q->link;
			delete q;
			return;
		}
		
		r = q;
		q = q->link;
	}
	cout << "\nElement " << num << " not found.\n";
}

void linklist::display()
{
	node *q;
	cout << endl;
	
	for (q = p; q != NULL; q = q->link)
		cout << endl << q->data;
}

int linklist::count()
{
	node *q;
	int c = 0;
	for (q = p; q != NULL; q = q->link)
		c++;
	
	return c;
}

linklist::~linklist()	// destructor - while any objects p, link them to q
{
	node *q;
	if (p == NULL)
		return;
		
	while (p != NULL)
	{
		q = p->link;
		p = q;
	}
}

int main()
{
	linklist ll;
	cout << "Number of elements = " << ll.count();
	ll.append(12);
	ll.append(13);
	ll.append(23);
	ll.append(43);
	ll.append(44);
	ll.append(50);
	
	ll.add_as_first(2);
	ll.add_as_first(1);
	
	ll.display();
	
	cout << "\n Number of elements: " << ll.count();
	
	ll.del(333);
	ll.del(12);
	ll.del(98);
	cout << "\n Number of elements: " << ll.count();
	return 0;
}




































	
