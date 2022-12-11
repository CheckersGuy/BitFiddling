#include "BitPattern.h"
#include "types.h"
#include <iostream>
#include <random>
int main(int argl, const char **argc) {

  std::cout << "Doing some testing to fix my Bitset class" << std::endl;
  bit_pattern pattern(150);
  pattern[121] = 1;
  pattern[60] = 1;
  pattern[50] = 1;
  pattern[22] = 1;

  bit_pattern other = ~pattern;
  bit_pattern bla = pattern & (~other);

  std::cout << bla << std::endl;

  OneIterator begin(pattern);

  for (auto value : OneAdapter(pattern)) {
    std::cout << value << std::endl;
  }

  return 0;
}
