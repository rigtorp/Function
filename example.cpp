#include "Function.h"
#include <iostream>

int function(std::ostream& os)
{
  os << "Free function\n";
  return 0;
}

struct Class
{
  int operator()(std::ostream& os)
  {
    os << "Class::operator()\n";
    return 1;
  }

  int fun(std::ostream& os)
  {
    os << "Class::fun()\n";
    return 2;
  }
};

int main(int argc, char *argv[]) {
  int foo = 1;
  double bar = 3;

  Function<int (std::ostream &), 128> f([=](std::ostream &os) {
    os << "test " << foo << " " << bar << std::endl;
    return foo;
  });
  f(std::cout);

  f = &function;
  f(std::cout);

  f = function;
  f(std::cout);

  Class cl;
  f = std::bind(&Class::fun, &cl, std::placeholders::_1);
  f(std::cout);

  f = cl;
  f(std::cout);

  f(std::cout);
  std::cout << "Size of Function: " << sizeof(f) << '\n';

  return 0;
}
