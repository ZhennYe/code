// stack.h -- class definition for the stack abstract data type (ADT)
#ifndef STACK_H_
#define STACK_H_

typedef unsigned long Item;

class Stack
{
  private:
    enum {MAX = 10}; // constant specific to class
    Item items[MAX]; // holds stack items
    int top;         // index for top stack items
  public:
    Stack();
    bool isempty() const; // implicit call of const item
    bool isfull() const;
    // push() returns false if stack is already full, otherwise true
    bool push(const Item & item); // add item to stack
    // pop() returns false if stack is emtpy
    bool pop(Item & item); // pop top item
};
#endif
