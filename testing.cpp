#include "BitPattern.h"
#include "types.h"
#include <iostream>

int main(int argl, const char **argc) {

  std::cout << "Doing some testing to fix my Bitset class" << std::endl;
  bit_pattern pattern(10);
  pattern.set_bit(2);

  std::cout << pattern << std::endl;

  for (auto bit : pattern) {
    std::cout << bit << std::endl;
  }
  std::cout << "Testing my new struct" << std::endl;
  Bit test(pattern, 2);

  double value = test;
  std::cout << "My value: " << value << std::endl;

  return 0;
}
