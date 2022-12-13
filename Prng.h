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

  uint64_t operator()();

  uint64_t getSeed() const;

  static constexpr uint64_t min();

  static constexpr uint64_t max();
};

#endif // READING_PRNG_H
