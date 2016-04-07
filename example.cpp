#include "Function.h"
#include <iostream>

int main(int argc, char *argv[]) {
  int foo = 1;
  double bar = 3;

  Function<int (std::ostream &), 128> f([=](std::ostream &os) {
    os << "test " << foo << " " << bar << std::endl;
    return foo;
  });

  f(std::cout);
  std::cout << "Size of Function: " << sizeof(f) << '\n';

  return 0;
}
