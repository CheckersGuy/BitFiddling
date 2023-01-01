#include "BitPattern.h"
#include "Prng.h"
#include "Util/ThreadPool.h"
#include "types.h"
#include <iostream>
#include <random>
#include <thread>

void test_patterns() {
  std::cout << "Doing some testing to fix my Bitset class" << std::endl;
  bit_pattern pattern(150);
  pattern[121] = 1;
  pattern[60] = 1;
  pattern[50] = 1;
  pattern[22] = 1;
  pattern[149] = 1;
  bit_pattern other = ~pattern;
  bit_pattern bla = pattern & (~other);

  std::cout << bla << std::endl;

  OneIterator begin(pattern);
  auto input = 4;
  auto index = pattern.get_bit_index(input);
  std::cout << "Index of the " << input << "'s one bit at:" << index
            << std::endl;

  // Testing some random number generation

  Prng generator(231231ull);
  std::uniform_int_distribution<int> distrib(0, 100);

  auto rand_value = distrib(generator);

  std::cout << "Random: " << rand_value << std::endl;
}

void test_pool() {
  ThreadPool pool(2);
  auto task = pool.submit([]() { std::cout << "Test" << std::endl; });
  auto task2 = pool.submit([]() {
    for (auto i = 0; i < 10; ++i) {
      std::this_thread::sleep_for(std::chrono::milliseconds(1000));
      std::cout << "Slept: " << i << std::endl;
    }
  });
  task.get();
}

int main(int argl, const char **argc) {
  test_pool();

  return 0;
}
