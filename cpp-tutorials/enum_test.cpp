// enum test
#include <iostream>

int main()
{
  using namespace std;
  enum Mos {May, July, Oct};
  Mos bday = May;
  switch(bday) {
    case May : std::cout << "May - Amy's birthday\n"; break;
    case July : std::cout << "July - Engagement Anniversary\n"; break;
    case Oct : std::cout << "October - First data\n"; break;
  }
}
