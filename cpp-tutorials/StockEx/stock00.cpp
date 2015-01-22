// stock00.cpp -- implementing the stock class
// version 00
#include <iostream>
#include "stock00.h"

void Stock:: acquire(const std::string & co, long n, double pr)
{
  company = co;
  if (n < 0)
  {
    std::cout << "Numer of shares can't be negative; "
              << company << " shares set to 0.\n";
    shares = 0;
  }
  else
    shares = n;
  share_val = pr;
  set_tot();
}

void Stock::buy(long num, double price)
{
  if (num < 0)
  {
    std::cout << "Number of shares purcahsed can't be neagtive."
              << "Transaction aborted.\n";
  }
  else
  {
    shares += num;
    share_val = price;
    set_tot();
  }
}

void Stock::sell(long num, double price)
{
  using std::cout;
  if (num < 0)
  {
    cout << "Number of shares sold can't be negative."
         << "Transaction aborted.\n";
  }
  else if (num > shares)
  {
    cout << "You can't sell more shares than you have."
         << "Transaction aborted.\n";
  }
  else
  {
    shares -= num;
    share_val = price;
    set_tot();
  }
}

void Stock::update(double price)
{
  share_val = price;
  set_tot();
}

void Stock::show()
{
  std::cout << "Company: " << company
            << " Shares: " << shares << '\n'
            << " Share Price: $" << share_val
            << " Total worth: $" << total_val << '\n';
}
