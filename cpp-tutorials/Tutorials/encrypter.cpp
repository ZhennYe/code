#include <iostream>
#include <cstdlib>
#include <stdio.h>
#include <cstring>
// #include <conio.h>



int main()
{
  using namespace std;
  c:char crypt[27], cons; 
  int ctr;
  cout << "Input your 26-character cipherstring below.\n\n";
  cout << "ABCDEFGHIJKLMNOPQRSTUVWXYZ\n";
  cin >> crypt;
  e:char string[65535];
  cout << "\nInput your string (input your spaces as any non-alphabet character).\n";
  cin>>string;
  cout<<"\n\nEncrypted string: "; //std::string 
  for(ctr=0;ctr<=strlen(string);ctr++)
  {
    if (isalpha(string[ctr]))
    {
      int index = toupper(string[ctr]) - 'A';
      cout << crypt[index];
    }
  }
  cout<<"\n\n";
  /*cout<<"Input 'c' to re-input your cipherstring.\n 'e' to reuse your cipherstring.\n 'q' to quit. ";
  comm:cout<<"\nCommand: "; cin>>cons;
  switch (cons)
  {
  case('c'): cout<<endl; goto c; break;
  case('C'): goto c; break;
  case('e'): goto e; break;
  case('E'): goto e; break;
  case('q'): break;
  case('Q'): break;
  default: cout<<"Invalid command. Please refer to the command list above.\n";goto comm;
  }*/
  system("PAUSE"); 
  return 0;
}
