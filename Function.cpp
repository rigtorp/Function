#include "Function.h"
#include <iostream>

int main(int argc, char *argv[]) {
  int foo = 1;
  double bar = 3;

  Function<void(std::ostream &)> f([=](std::ostream &os) {
    os << "test " << foo << " " << bar << std::endl;
  });

  f(std::cout);

  return 0;
}
