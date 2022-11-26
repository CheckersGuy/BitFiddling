#include "BitPattern.h"
#include "types.h"
#include <iostream>

int main(int argl, const char **argc) {

  std::cout << "Doing some testing to fix my Bitset class" << std::endl;
  bit_pattern pattern(10);

  std::cout << pattern << std::endl;
  std::cout << pattern.get_num_fields() << std::endl;
  return 0;
}
