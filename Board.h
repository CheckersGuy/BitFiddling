//
// Created by robin on 05.11.21.
//

#ifndef READING_BOARD_H
#define READING_BOARD_H

#include "Position.h"
#include "Prng.h"
#include "Union.h"
#include <cstdint>
#include <random>
#include <vector>
#include <iostream>
class Board {

private:
  Union un;
  Position position;
  uint16_t last_move;
  const size_t size;
public:


  Board(size_t s):size(s),position(s),un(s){
    last_move = s*s;
  }

  void make_move(int index) {
    position.make_move(index);
    add_to_union(index);
    last_move = index;
  }

  void add_to_union(int index) {
    const Color index_color = ~position.get_mover();

    if (index_color == BLACK && is_north_edge(size,index)) {
      un.merge_north(index);
    }
    if (index_color == BLACK && is_south_edge(size,index)) {
      un.merge_south(index);
    }
    if (index_color == WHITE && is_west_edge(size,index)) {
      un.merge_west(index);
    }
    if (index_color == WHITE && is_east_edge(size,index)) {
      un.merge_east(index);
    }

    if (!is_north_edge(size,index) &&
        get_position().get_square(index - size) == index_color) {
      un.merge(index - size, index);
    }
    if (!is_south_edge(size,index) &&
        get_position().get_square(index + size) == index_color) {
      un.merge(index + size, index);
    }
    if (!is_west_edge(size,index) &&
        get_position().get_square(index - 1) == index_color) {
      un.merge(index - 1, index);
    }
    if (!is_west_edge(size,index) && !is_south_edge(size,index) &&
        get_position().get_square(index + size - 1) == index_color) {
      un.merge(index + size - 1, index);
    }

    if (!is_east_edge(size,index) &&
        get_position().get_square(index + 1) == index_color) {
      un.merge(index + 1, index);
    }
    if (!is_east_edge(size,index) && !is_north_edge(size,index) &&
        get_position().get_square(index - (size - 1)) == index_color) {
      un.merge(index - (size - 1), index);
    }
  }

  Color get_winner() {

    if(un.is_white_win())
      return WHITE;

    if(un.is_black_win())
      return BLACK;

    return EMPTY;

   }

  Union& get_union() { return un; }

  Color play_out(Prng &source) {
    Color winner = get_winner();
    bit_pattern empty_squares = ~(get_position().WP | get_position().BP);

    while (winner == EMPTY) {

      std::uniform_int_distribution<int> distrib(
          0, get_position().get_num_empty());

      auto index = empty_squares.get_bit_index(distrib(source));
      empty_squares[index] = 0;
      make_move(index);

      auto ret = save_bridge(source, index);
      if (ret != -1) {
        empty_squares[ret] = 0;
      }

      winner = get_winner();
    }
    return winner;
  }

  Position &get_position() { return position; }

  const Position& get_position()const{
    return position;
  }

  int save_bridge(Prng &random_source, size_t hex_point) {
    int value = -1;
    if (is_on_edge(size,hex_point)) {
      // will be done later
      return value;
    }
    Position neigh(size);
    std::uniform_int_distribution<int> distrib(0, 5);
    size_t rand_start = distrib(random_source);
    Color opp_color = ~position.get_square(hex_point);
    auto neigh_bours = position.get_neighbours(hex_point);

    size_t ret = 0;
    size_t state = 0;
    for (auto i = 0; i < 6; ++i) {
      auto index = rand_start + i;
      index = index % 6;
      auto value = neigh_bours[index];
      Color current;
      if (value == un.NORTH || value == un.SOUTH) {
        current = BLACK;
      } else if (value == un.WEST || value == un.EAST) {
        current = WHITE;
      } else {
        current = position.get_square(value);
      }

      if (state == 0 && opp_color == current) {
        state = 1;
      } else if (state == 1 && current == EMPTY) {
        ret = neigh_bours[index];
        state = 2;
      } else if (state == 2 && current == opp_color) {

        make_move(ret);
        return ret;
        break;
      } else {
        state = 0;
      }
    }
    return value;
    ;
  }

  friend std::ostream &operator<<(std::ostream &stream, const Board &other) {
     stream<<other.get_position();
    return stream;
  }





};

#endif // READING_BOARD_H
