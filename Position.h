//
// Created by robin on 03.11.21.
//

#ifndef READING_POSITION_H
#define READING_POSITION_H

#include "BitPattern.h"
#include "Prng.h"
#include "types.h"
#include <cstdint>
#include <cstdio>
#include <immintrin.h>
#include <iostream>
#include <ostream>
#include <iomanip>
struct Position {
  const size_t board_size;
  bit_pattern BP;
  bit_pattern WP;
  Color color{BLACK};

  Position(const size_t size):board_size(size),  BP(board_size * board_size), WP(board_size * board_size) {}

  size_t num_empty{board_size * board_size};

  static Position get_empty_position(size_t size) {
    Position pos(size);
    return pos;
  }

  void make_move(int idx) {
    if (color == BLACK) {
      BP.set_bit(idx);
    } else {
      WP.set_bit(idx);
    }
    color = ~color;
    num_empty -= 1;
  }

  void unmake_move(int idx) {
    if (color == BLACK) {
      BP.clear_bit(idx);
    } else {
      WP.clear_bit(idx);
    }
    color = ~color;
    num_empty += 1;
  }

  size_t get_num_empty() { return num_empty; }

  inline Color get_square(int index) {
    if (BP.is_set(index))
      return BLACK;
    if (WP.is_set(index))
      return WHITE;

    return EMPTY;
  }

  inline uint64_t get_empty_squares(int index) {
    const uint64_t squares = BP.fields[index] | WP.fields[index];
    if (index == BP.get_num_fields() - 1) {
      return (~squares) & get_left_mask(board_size * board_size);
    }
    return (~squares);
  }

  inline Color operator[](int index) { return get_square(index); }

  constexpr std::array<size_t, 6> get_neighbours(uint16_t hex_point) {
    std::array<size_t, 6> result{};
    result[0] = hex_point - board_size;
    result[1] = hex_point - 1;
    result[2] = hex_point + (board_size - 1);
    result[3] = hex_point + board_size;
    result[4] = hex_point + 1;
    result[5] = hex_point - ((board_size - 1));

    return result;
  }

   Color get_mover() { return color; }

  friend std::ostream &operator<<(std::ostream &stream, const Position &other) {

    for (auto i = 0; i < other.board_size; ++i) {
      for (auto p = 0; p < i; ++p) {
        stream << " ";
      }
      stream << "\\";
      stream << " ";
      for (auto k = 0; k < other.board_size; ++k) {
        if (other.BP.is_set(other.board_size * i + k)) {
          stream << "B";
        } else if (other.WP.is_set(other.board_size * i + k)) {
          stream <<"W";
        } else {
          stream << ".";
        }
        stream <<" ";
      }
      stream << "\\";
      stream<<"\n";
    }
    return stream;
  }

  friend std::ofstream &operator<<(std::ofstream &stream, const Position &pos) {
    stream << pos.BP;
    stream << pos.WP;
    stream.write((char *)&pos.color, sizeof(Color));
    return stream;
  }

  friend std::ifstream &operator>>(std::ifstream &stream, const Position &pos) {
    stream >> pos.BP;
    stream >> pos.WP;
    stream.read((char *)&pos.color, sizeof(Color));
    return stream;
  }
};

#endif // READING_POSITION_H
