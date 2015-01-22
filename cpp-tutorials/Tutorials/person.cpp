// person.cpp -- person example (from scratch)
#include <iostream>
#include <string>

class Person {
  private:
    static const int LIMIT = 25;
    std::string lname;
    char fname[LIMIT];
  public:
    Person() {lname = "", fname[0] = '\0'; } // constructor 1
    Person(const std::string & ln, const char * fn = "Jack"); // const 2
    void Show() const; // firstname lastname format
    void FormalShow() const; // lastname, firstname format
};

void Person::Show() const
{
  using namespace std;
  cout << fname << lname << endl;
}

void Person::FormalShow() const 
{
  using namespace std;
  cout << lname << ", " << fname << endl;
}

int main()
{
  Person one;
  Person two("Smythe");
  Person three("Dimwiddy", "Sam");
  one.Show();
  one.FormalShow();
  two.Show();
  two.FormalShow();
  three.Show();
  three.FormalShow();
  return 0;
}

