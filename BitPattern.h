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
#include <type_traits>
#include <x86intrin.h>

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

  bool operator!=(const BitSetIterator &other) const;
};

struct OneIterator {
  const bit_pattern &bits;
  size_t field_index;
  uint64_t maske;

  // std::array<uint64_t, get_num_fields(board_size)> fields{0};

  using iterator_category = std::forward_iterator_tag;
  using difference_type = std::ptrdiff_t;
  using value_type = size_t;
  using pointer = value_type *;
  using reference = value_type &;

  // stuff goes here

  OneIterator(const OneIterator &other);

  OneIterator(const bit_pattern &pattern);

  OneIterator &operator++();

  OneIterator operator++(int);

  size_t operator*() const;

  bool operator==(const OneIterator &other) const;

  bool operator!=(const OneIterator &other) const;
};

// an Adapter on a bitset;

struct OneAdapter {
  bit_pattern &bits;

  OneAdapter(bit_pattern &pattern);

  OneIterator begin();
  OneIterator end();
};

struct Bit;

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
    for (auto i = 0; i < num_fields; ++i) {
      fields[i] = other.fields[i];
    }
  }

  bit_pattern &operator=(const bit_pattern &other) {
    num_fields = other.num_fields;
    num_bits = other.num_bits;
    fields = std::make_unique<uint64_t[]>(num_fields);
    for (auto i = 0; i < num_fields; ++i) {
      fields[i] = other.fields[i];
    }

    return *this;
  }

  void set_bit(size_t idx);

  void set_bit(size_t idx, bool value);

  void clear_bit(size_t idx);

  bool is_set(size_t idx) const;

  Bit operator[](size_t index);

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

  bit_pattern &operator&=(bit_pattern &other);

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

  bit_pattern &operator|=(bit_pattern &other);

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

  size_t count_bits();

  size_t get_bit_index(int n);

  bool operator==(const bit_pattern &other) const;

  bool operator!=(const bit_pattern &other) const;

  bool is_empty();

  BitSetIterator begin() const;

  BitSetIterator end() const;

  size_t size() const;

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

    for (auto bit : pat) {
      auto token = (bit == 0) ? "0" : "1";
      stream << token;
    }
    return stream;
  }
};

struct Bit {
  bit_pattern &ref;
  size_t index;
  // to be continued

  Bit(bit_pattern &other, size_t index);

  Bit &operator=(bool value);

  template <typename T> operator T() {
    static_assert(std::is_integral_v<T> || std::is_floating_point_v<T>);
    return (ref.is_set(index) ? 1 : 0);
  }

  friend std::ostream &operator<<(std::ostream &stream, Bit bit) {
    bool value = (bit.ref.is_set(bit.index) ? 1 : 0);
    stream << value;
    return stream;
  }

  template <typename T> bool operator==(T &&other) {
    static_assert(std::is_integral_v<T> || std::is_floating_point_v<T>);
    bool my_value = (ref.is_set(index) ? 1 : 0);
    return my_value == other;
  }
  template <typename T> bool operator!=(T &&other) {
    static_assert(std::is_integral_v<T> || std::is_floating_point_v<T>);
    bool my_value = (ref.is_set(index) ? 1 : 0);
    return my_value != other;
  }

  bool operator==(const Bit &other);
  bool operator!=(const Bit &other);
};

// class definition ends here

bool Bit::operator==(const Bit &other) {
  bool my_value = (ref.is_set(index) ? 1 : 0);
  bool other_value = (other.ref.is_set(other.index) ? 1 : 0);
  return my_value == other_value;
}

bool Bit::operator!=(const Bit &other) { return !((*this) == other); }

void bit_pattern::set_bit(size_t idx) {
  size_t field_index = idx / 64ull;
  size_t sub_index = idx & (63ull);
  fields[field_index] |= 1ull << sub_index;
}

void bit_pattern::set_bit(size_t idx, bool value) {
  const uint64_t temp = value;
  size_t field_index = idx / 64ull;
  size_t sub_index = idx & (63ull);
  // clearing out the bit first
  fields[field_index] &= ~(1ull << sub_index);
  fields[field_index] |= temp << sub_index;
}

void bit_pattern::clear_bit(size_t idx) {
  size_t field_index = idx / 64ull;
  size_t sub_index = idx & (63ull);
  fields[field_index] &= ~(1ull << sub_index);
}

bool bit_pattern::is_set(size_t idx) const {
  size_t field_index = idx / 64ull;
  size_t sub_index = idx & (63ull);
  return (fields[field_index] & (1ull << sub_index)) != 0;
}

Bit bit_pattern::operator[](size_t index) {
  Bit bit(*this, index);
  return bit;
}

bit_pattern &bit_pattern::operator&=(bit_pattern &other) {
  const size_t last = other.get_num_fields() - 1;
  for (auto i = 0; i < last; ++i) {
    fields[i] = (fields[i] & other.fields[i]);
  }
  fields[last] = (fields[last] & other.fields[last]);
  fields[last] &= LEFT_MASK;
  return *this;
}

bit_pattern &bit_pattern::operator|=(bit_pattern &other) {
  const size_t last = other.get_num_fields() - 1;
  for (auto i = 0; i < last; ++i) {
    fields[i] = (fields[i] | other.fields[i]);
  }
  fields[last] = (fields[last] | other.fields[last]);
  fields[last] &= LEFT_MASK;
  return *this;
}

size_t bit_pattern::count_bits() {
  size_t count = 0;
  for (auto i = 0; i < num_fields - 1; ++i) {
    count += __builtin_popcountll(fields[i]);
  }
  count += __builtin_popcountll(fields[num_fields - 1] & LEFT_MASK);
  return count;
}

size_t bit_pattern::size() const { return num_bits; }

size_t bit_pattern::get_bit_index(int n) {
  // returns the index of the nth bith
  size_t total_count = 0;
  size_t field_index = 0;
  size_t count = 0;
  for (auto i = 0; i < num_fields; ++i) {
    // figuring out which bucket this bit is in
    count = __builtin_popcountll(fields[i]);
    if (total_count + count > n) {
      field_index = i;
      break;
    }
    total_count += count;
  }
  auto local_index = n - total_count;
  uint64_t index_mask = 1ull << local_index;
  uint64_t empty_square = _pdep_u64(index_mask, fields[field_index]);

  return _tzcnt_u64(empty_square) + 64ull * field_index;
}

bool bit_pattern::operator==(const bit_pattern &other) const {
  for (auto i = 0; i < num_fields; ++i) {
    if (fields[i] != other.fields[i])
      return false;
  }
  return true;
}

bool bit_pattern::operator!=(const bit_pattern &other) const {
  for (auto i = 0; i < num_fields; ++i) {
    if (fields[i] != other.fields[i])
      return true;
  }
  return false;
}

bool bit_pattern::is_empty() {
  for (auto i = 0; i < num_fields - 1; ++i) {
    if (fields[i] != 0)
      return false;
  }
  return (fields[num_fields - 1] & LEFT_MASK) == 0;
}

BitSetIterator bit_pattern::begin() const {
  BitSetIterator next(*this);
  return next;
}

BitSetIterator bit_pattern::end() const {
  BitSetIterator next(*this);
  next.index = num_bits;
  return next;
}

bool BitSetIterator::operator*() const { return bits.is_set(index); }

BitSetIterator::BitSetIterator(const bit_pattern &pattern) : bits(pattern) {
  index = 0;
}

BitSetIterator &BitSetIterator::operator++() {
  // removes the lsb
  index++;
  return *this;
}

BitSetIterator BitSetIterator::operator++(int) {
  BitSetIterator next(*this);
  next.index++;
  return next;
}

bool BitSetIterator::operator==(const BitSetIterator &other) const {
  return (bits == other.bits && (index == other.index));
}

bool BitSetIterator::operator!=(const BitSetIterator &other) const {
  return (bits != other.bits || index != other.index);
}

Bit::Bit(bit_pattern &other, size_t index) : ref(other) { this->index = index; }

Bit &Bit::operator=(bool value) {
  ref.set_bit(index, value);
  return *this;
}

OneIterator::OneIterator(const OneIterator &other) : bits(other.bits) {
  field_index = other.field_index;
  maske = other.maske;
}

OneIterator::OneIterator(const bit_pattern &pattern) : bits(pattern) {
  maske = bits.fields[0];
  field_index = 0;
  while (maske == 0 && field_index + 1 < bits.num_fields) {
    // getting to the next non_zero mask
    maske = bits.fields[++field_index];
  };
}

OneIterator &OneIterator::operator++() {
  // removing the lsb
  maske &= maske - 1;

  while (maske == 0 && field_index + 1 < bits.num_fields) {
    // getting to the next non_zero mask
    maske = bits.fields[++field_index];
  };

  return *this;
}

OneIterator OneIterator::operator++(int) {
  OneIterator other(*this);
  ++(*this);
  return other;
}

size_t OneIterator::operator*() const {
  return _tzcnt_u64(maske) + field_index * 64;
}

bool OneIterator::operator==(const OneIterator &other) const {
  return (field_index == other.field_index && maske == other.maske);
}

bool OneIterator::operator!=(const OneIterator &other) const {
  return (field_index != other.field_index || maske != other.maske);
}

OneAdapter::OneAdapter(bit_pattern &pattern) : bits(pattern) {}

OneIterator OneAdapter::begin() {
  OneIterator beg(bits);
  return beg;
}

OneIterator OneAdapter::end() {
  OneIterator end(bits);
  end.field_index = bits.num_fields - 1;
  end.maske = 0;
  return end;
}

#endif // READING_BITPATTERN_H
