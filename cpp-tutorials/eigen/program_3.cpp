#include <iostream>
#include "Eigen/Dense"

using namespace Eigen;

int main()
{
  MatrixXd m(2,5);
  std::cout << "rows of m: " << m.rows() << " cols of m: " << m.cols()
            << " size of m: " << m.size() << std::endl;
  m.resize(4,3);
  std::cout << "rows of m: " << m.rows() << " cols of m: " << m.cols()
            << " size of m: " << m.size() << std::endl;
  
  VectorXd v(2);
  v.resize(5);
  std::cout << "As a matrix, v is of size " << v.rows() << " x "
            << v.cols() << std::endl;
}

