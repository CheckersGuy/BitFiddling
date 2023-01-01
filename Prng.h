//
// Created by robin on 01.11.21.
//

#ifndef READING_PRNG_H
#define READING_PRNG_H
#include <cstdint>
#include <limits>
class Prng {
private:
  uint64_t seed;

public:
  using result_type = uint64_t;
  using value_type = uint64_t;

  Prng(value_type seed) : seed(seed) {}

  uint64_t operator()() {
    seed ^= seed >> 12;
    seed ^= seed << 25;
    seed ^= seed >> 27;
    return seed * 2685821657736338717ull;
  }

  uint64_t getSeed() const { return seed; }
  static uint64_t constexpr max() {
    return std::numeric_limits<uint64_t>::max();
  }

  static uint64_t constexpr min() { return 0ull; }
};

#endif // READING_PRNG_H
