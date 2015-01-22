// circ_linklist.cpp - circulat linked list example
#include <iostream>
using namespace std;	// omit #include <windows.h>, #include <conio.h>

class CL_list
{
	private:
		struct node
			{
				int data;
				node *link;
			};
			struct node *p;
	
	public:
		CL_list();
		CL_list(CL_list& l);
		~CL_list();
		void add (int);
		void del();
		void addatbeg (int);
		void display();
		int count();
		bool operator == (CL_list);
		bool operator != (CL_list);
		void operator = (CL_list);
	}; 	// end class CL_list
	
	CL_list::CL_list()
	{
		p = NULL;
	}
	
	CL_list::CL_list(CL_list& l)
	{
		node *x;
		p = NULL;
		x = l.p;
		if (x == NULL)
			return;
		for (int i = 1; i <= l.count(); i++)
		{
			add (x->data);
				x = x->link;
		}
	}
	
	CL_list::~CL_list()
	{
		node *q, *t;
			q = p;
			t = p;
			if (p == NULL)
				return;
			while (q->link != t)
			{
				p = q;
				q = q->link;
				delete p;
			}
			p = q;
			delete p;
	}
	
	void CL_list::add(int n)
	{
		if (p == NULL)
			{
				node *q;
				q = new node;
				q->data = n;
				q->link = q;
				p = q;
				return;
			}
			node *q;
			q = p;
			while (q->link != p)
				q = q->link;
			
			node *t;
			t = new node;
			t->data = n;
			t->link = p;
			q->link = t;
	}
		
	void CL_list::display()
	{
		if (p == NULL)
		{
				cout << "\nEmpty list\n";
					return;
		}
		node *q;
		q = p;
		for (int i = 1; i <= this->count(); i++)
		{
			cout << q->data << endl;
			q = q->link;
		}
	}
	
	int CL_list::count()
	{
		node *q;
		q = p;
		int c = 0;
		if (p == NULL)
			return 0;
		else
			c++;
		while (q->link != p)
		{
			c++;
			q = q->link;
		}
		return c;
	}
	
	void CL_list::del()
	{
		if (p == NULL)
			return;
		if (p->link == p)
		{
			p = NULL;
		}
		else
		{
			node *q;
			q = p;
			while (q->link != p)
				q = q->link;
			
			q->link = p->link;
			q = p;
			p = (q->link == NULL ? NULL : p->link);
			delete q;
		}
	}
	
	void CL_list::addatbeg (int n)
	{
		node *q, *t;
		q = p;
		while (q->link != p)
			q = q->link;
		
		t = new node;
		t->data = n;
		t->link = p;
		q->link = t;
		p = t;
	}
	
	// void CL_list::slideshow (float dlay, int x, int y)
	// omitted from non-windows code		

	// void CL_list::wait (float t)
	// omitted from non-windows code

	bool CL_list::operator ==(CL_list t)
	{
		if(t.p == NULL && p == NULL)
			return 1;
		
		if (this->count() != t.count())
			return 0;
		node *q;
		q = p;
		bool flag;
		flag = 1;
		node *a;
		a = t.p;
		for (int i = 1; i <= count(); i++)
		{
			if(a->data != q->data)
				flag = 0;
			a = a->link;
			q = q->link;
		}
		if (a->data != q->data)
			flag = 0;
		return flag;
	}
	
	bool CL_list::operator != (CL_list t)
	{
		return !(this->operator == (t));
	}
	
	int main()
	{
		CL_list a;
		a.add(1);
		a.add(2);
		a.add(3);
		a.add(4);
		a.addatbeg(128);
		a.del(); // 128 deleted;
		cout << "\nList data:\n";
		a.display();
		
		CL_list b = a;
		if (b != a)
			cout << endl << "Not equal" << endl;
		else
			cout << endl << "Equal" << endl;
		// a.slideshow(1, 13, 13);
		
		return 0;
}
































		
