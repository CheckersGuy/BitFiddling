//
// Created by robin on 03.11.21.
//

#ifndef READING_BITPATTERN_H
#define READING_BITPATTERN_H

#include "types.h"
#include <array>
#include <cstring>
#include <fstream>
#include <immintrin.h>
#include <iostream>
#include <memory>

uint64_t get_left_mask(size_t size) {
  size_t left_over = size % 64ull;
  left_over = ((left_over == 0) ? 64 : left_over);
  uint64_t maske = 0ull;
  for (auto i = 0; i < left_over; ++i) {
    maske |= 1ull << i;
  }
  return maske;
}

class bit_pattern;

struct BitSetIterator {
  const bit_pattern &bits;
  size_t index;

  // std::array<uint64_t, get_num_fields(board_size)> fields{0};

  using iterator_category = std::forward_iterator_tag;
  using difference_type = std::ptrdiff_t;
  using value_type = size_t;
  using pointer = value_type *;
  using reference = value_type &;

  // stuff goes here

  BitSetIterator(const BitSetIterator &other) : bits(other.bits) {
    index = other.index;
  }

  BitSetIterator(const bit_pattern &pattern);

  BitSetIterator &operator++();

  BitSetIterator operator++(int);

  bool operator*() const;

  bool operator==(const BitSetIterator &other) const;
  // std::array<uint64_t, get_num_fields(board_size)> fields{0};

  bool operator!=(const BitSetIterator &other) const;
};

struct bit_pattern {
  std::unique_ptr<uint64_t[]> fields;
  size_t num_bits;
  size_t num_fields;
  const uint64_t LEFT_MASK;

  bit_pattern(size_t num_bits) : LEFT_MASK(get_left_mask(num_bits)) {
    this->num_bits = num_bits;
    this->num_fields = num_bits / 64 + 1;

    fields = std::make_unique<uint64_t[]>(this->num_fields);
  }

  size_t get_num_fields() { return num_fields; }

  bit_pattern(const bit_pattern &other) : LEFT_MASK(other.LEFT_MASK) {
    num_fields = other.num_fields;
    num_bits = other.num_bits;
    fields = std::make_unique<uint64_t[]>(other.num_fields);
    std::memcpy(fields.get(), other.fields.get(),
                sizeof(uint64_t) * num_fields);
  }

  bit_pattern &operator=(const bit_pattern &other) {
    num_fields = other.num_fields;
    num_bits = other.num_bits;
    fields = std::make_unique<uint64_t[]>(num_fields);
    std::memcpy(fields.get(), other.fields.get(),
                sizeof(uint64_t) * num_fields);
    return *this;
  }

  void set_bit(size_t idx) {
    size_t field_index = idx / 64ull;
    size_t sub_index = idx & (63ull);
    fields[field_index] |= 1ull << sub_index;
  }

  void clear_bit(size_t idx) {
    size_t field_index = idx / 64ull;
    size_t sub_index = idx & (63ull);
    fields[field_index] &= ~(1ull << sub_index);
  }

  bool is_set(size_t idx) const {
    size_t field_index = idx / 64ull;
    size_t sub_index = idx & (63ull);
    return (fields[field_index] & (1ull << sub_index)) != 0;
  }

  friend bit_pattern operator&(bit_pattern one, bit_pattern two) {
    bit_pattern next(one.num_bits);
    const size_t last = one.get_num_fields() - 1;
    for (auto i = 0; i < last; ++i) {
      next.fields[i] = (one.fields[i] & two.fields[i]);
    }
    next.fields[last] = (one.fields[last] & two.fields[last]);
    next.fields[last] &= one.LEFT_MASK;
    return next;
  }

  bit_pattern &operator&=(bit_pattern &other) {
    const size_t last = other.get_num_fields() - 1;
    for (auto i = 0; i < last; ++i) {
      fields[i] = (fields[i] & other.fields[i]);
    }
    fields[last] = (fields[last] & other.fields[last]);
    fields[last] &= LEFT_MASK;
    return *this;
  }

  friend bit_pattern operator|(bit_pattern one, bit_pattern two) {
    bit_pattern next(one.num_bits);
    const size_t last = one.get_num_fields() - 1;
    for (auto i = 0; i < last; ++i) {
      next.fields[i] = (one.fields[i] | two.fields[i]);
    }
    next.fields[last] = (one.fields[last] | two.fields[last]);
    next.fields[last] &= one.LEFT_MASK;
    return next;
  }

  bit_pattern &operator|=(bit_pattern &other) {
    const size_t last = other.get_num_fields() - 1;
    for (auto i = 0; i < last; ++i) {
      fields[i] = (fields[i] | other.fields[i]);
    }
    fields[last] = (fields[last] | other.fields[last]);
    fields[last] &= LEFT_MASK;
    return *this;
  }

  friend bit_pattern operator~(bit_pattern other) {
    bit_pattern next(other.num_bits);
    const size_t last = other.get_num_fields() - 1;
    for (auto i = 0; i < last; ++i) {
      next.fields[i] = ~(other.fields[i]);
    }
    next.fields[last] = ~(other.fields[last]);
    next.fields[last] &= other.LEFT_MASK;
    return next;
  }

  void print() {
    for (auto i = 0; i < num_bits; ++i) {
      if (is_set(i)) {
        std::cout << "1";
      } else {
        std::cout << "0";
      }
    }
    std::cout << "\n";
  }

  size_t count_bits() {
    size_t count = 0;
    for (auto i = 0; i < num_fields - 1; ++i) {
      count += __builtin_popcountll(fields[i]);
    }
    count += __builtin_popcountll(fields[num_fields - 1] & LEFT_MASK);
    return count;
  }

  size_t get_bit_index(int n) {
    // returns the index of the nth bith
    size_t total_count = 0;
    size_t field_index = 0;
    for (auto i = 0; i < num_fields; ++i) {
      // figuring out which bucket this bit is in
      auto count = __builtin_popcountll(fields[i]);
      total_count += count;
      if (total_count >= n) {
        field_index = i;
        break;
      }
    }
    const size_t local_index = n - total_count;
  }

  template <typename Prng>
  size_t get_random_bit(Prng &generator, size_t num_bits) {
    // REQUIRES UNIT TESTING
    // This function needs to be reworked
    uint64_t rand = generator();
    uint64_t index = rand % num_bits;
    uint64_t local_index = index;
    int field_index = 0;
    size_t count = 0;
    for (auto i = 0; i < num_fields; ++i) {
      size_t l_empt = fields[i];
      size_t num = __builtin_popcountll(l_empt);
      count += num;
      if (count >= index + 1) {
        field_index = i;
        break;
      }
      local_index -= num;
    }
    uint64_t empty_squares = fields[field_index];
    uint64_t index_mask = 1ull << local_index;
    uint64_t empty_square = _pdep_u64(index_mask, empty_squares);
    return _tzcnt_u64(empty_square) + 64ull * field_index;
  }

  template <typename Prng> size_t get_random_bit(Prng &generator) {
    // above version is for when we are incrementally making moves
    // so we can keep track of the number of 1 bits without computing them
    return get_random_bit(generator, count_bits());
  }

  bool operator==(const bit_pattern &other) const {
    for (auto i = 0; i < num_fields; ++i) {
      if (fields[i] != other.fields[i])
        return false;
    }
    return true;
  }

  bool operator!=(const bit_pattern &other) const {
    for (auto i = 0; i < num_fields; ++i) {
      if (fields[i] != other.fields[i])
        return true;
    }
    return false;
  }

  bool is_empty() {
    for (auto i = 0; i < num_fields - 1; ++i) {
      if (fields[i] != 0)
        return false;
    }
    return (fields[num_fields - 1] & LEFT_MASK) == 0;
  }

  BitSetIterator begin() const {
    BitSetIterator next(*this);
    return next;
  }

  BitSetIterator end() const {
    BitSetIterator next(*this);
    next.field_index = num_fields - 1;
    next.mask = 0;
    return next;
  }

  friend std::ofstream &operator<<(std::ofstream &stream,
                                   const bit_pattern &pat) {
    stream.write((char *)&(pat.fields[0]), sizeof(uint64_t) * pat.num_fields);
    return stream;
  }

  friend std::ifstream &operator>>(std::ifstream &stream,
                                   const bit_pattern &pat) {
    stream.read((char *)&(pat.fields[0]), sizeof(uint64_t) * pat.num_fields);
    return stream;
  }

  friend std::ostream &operator<<(std::ostream &stream,
                                  const bit_pattern &pat) {

    std::cout << "Test" << std::endl;
    for (auto bit : pat) {
      auto token = (bit == 0) ? "0" : "1";
      stream << token;
    }
    return stream;
  }
};

bool BitSetIteartor::operator*() const { return bits.is_set(index); }

BitSetIterator::BitSetIterator(const bit_pattern &pattern) : bits(pattern) {
  field_index = bits.num_fields;
  for (auto i = 0; i < pattern.num_fields; ++i) {
    field_index = i;
    if (pattern.fields[i] != 0) {
      break;
    }
  }
  mask = pattern.fields[field_index];
}

BitSetIterator &BitSetIterator::operator++() {
  // removes the lsb
  mask &= mask - 1;
  if (mask == 0 && field_index < bits.num_fields - 1) {
    field_index++;
    mask = bits.fields[field_index];
  }
  return *this;
}

BitSetIterator BitSetIterator::operator++(int) {
  BitSetIterator next(*this);
  mask &= mask - 1;
  if (mask == 0 && field_index < bits.num_fields - 1) {
    field_index++;
    mask = bits.fields[field_index];
  }
  return next;
}

bool BitSetIterator::operator==(const BitSetIterator &other) const {
  return (bits == other.bits && (field_index == other.field_index) &&
          (mask == other.mask));
}

bool BitSetIterator::operator!=(const BitSetIterator &other) const {
  return (bits != other.bits || field_index != other.field_index ||
          mask != other.mask);
}

#endif // READING_BITPATTERN_H
