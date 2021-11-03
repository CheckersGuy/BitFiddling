#include <iostream>
#include <array>
#include <immintrin.h>
#include <random>
#include "Union.h"
#include <chrono>
#include "Prng.h"
//functions to check if stone is placed on an edge

template<size_t board_size>
bool is_north_edge(size_t idx) {
    size_t row = idx / board_size;
    return row == 0;
}

template<size_t board_size>
bool is_south_edge(size_t idx) {
    size_t row = idx / board_size;
    return row == board_size - 1;
}

template<size_t board_size>
bool is_west_edge(size_t idx) {
    size_t col = idx % board_size;
    return col == board_size - 1;
}

template<size_t board_size>
bool is_east_edge(size_t idx) {
    size_t col = idx % board_size;
    return col == 0;
}

constexpr size_t get_num_fields(size_t board_size) {
    size_t num_squares = board_size * board_size;
    size_t num_fields = num_squares / 64;
    if ((num_squares % 64ull) == 0)
        return
                num_fields;
    else
        return num_fields + 1;
}


template<size_t board_size>
constexpr uint64_t get_board_mask() {
    size_t num_squares = board_size * board_size;
    size_t left_over = num_squares % 64ull;
    left_over = ((left_over == 0) ? 64 : left_over);
    uint64_t maske = 0ull;
    for (auto i = 0; i < left_over; ++i) {
        maske |= 1ull << i;
    }
    return maske;
}

template<size_t board_size> constexpr uint64_t LEFT_MASK = get_board_mask<board_size>();

template<size_t board_size>
struct bit_pattern {


    static constexpr size_t num_squares = board_size * board_size;
    std::array<uint64_t, get_num_fields(board_size)> fields{0};

    constexpr size_t get_num_squares() {
        return num_squares;
    }

    constexpr void set_bit(size_t idx) {
        size_t field_index = idx / 64ull;
        size_t sub_index = idx & (63ull);
        fields[field_index] |= 1ull << sub_index;
    }

    constexpr void clear_bit(size_t idx) {
        size_t field_index = idx / 64ull;
        size_t sub_index = idx & (63ull);
        fields[field_index] &= ~(1ull << sub_index);
    }

    constexpr bool is_set(size_t idx) {
        size_t field_index = idx / 64ull;
        size_t sub_index = idx & (63ull);
        return (fields[field_index] & (1ull << sub_index)) != 0;
    }
};



constexpr Color operator~(Color color) {
    return (color == BLACK) ? WHITE : BLACK;
}

template<size_t board_size>
struct Position {
    bit_pattern<board_size> BP;
    bit_pattern<board_size> WP;
    Color color{BLACK};
    size_t num_empty{board_size * board_size};


    void make_move(int idx) {
        if (color == BLACK) {
            BP.set_bit(idx);
        } else {
            WP.set_bit(idx);
        }
        color = ~color;
        num_empty -= 1;
    }

    size_t get_num_empty() {
        return num_empty;
    }

    uint64_t get_empty_squares(int index) {
        const uint64_t squares = BP.fields[index] | WP.fields[index];
        if (index == BP.fields.size() - 1) {
            return (~squares) & LEFT_MASK<board_size>;
        }
        return (~squares);
    }

    template<typename Generator>
    size_t get_random_empty(Generator &generator) {
        uint64_t rand = generator();
        uint64_t index = rand % num_empty;
        //std::cout << "Index: " << index << std::endl;
        //Now we need to know where the empty square is
        int field_index = 0;
        size_t count = 0;
        for (auto i = 0; i < BP.fields.size(); ++i) {
            size_t l_empt = get_empty_squares(i);
            size_t num = __builtin_popcountll(l_empt);
            count += num;
            if (count >= index + 1) {
                field_index = i;
                break;
            }
            index = index - num;
        }
        //now wee need to select the empty square given by the index
        uint64_t empty_squares = get_empty_squares(field_index);
        uint64_t index_mask = 1ull << index;
        uint64_t empty_square = _pdep_u64(index_mask, empty_squares);
        //std::cout << "OUtput: " << _tzcnt_u64(empty_square) + 64ull * field_index << std::endl;
        return _tzcnt_u64(empty_square) + 64ull * field_index;
    }

    friend std::ostream &operator<<(std::ostream &stream, Position &pos) {
        for (auto i = 0; i < board_size; ++i) {
            for (auto p = 0; p < i; ++p) {
                stream << " ";
            }
            stream << "\\";
            stream << " ";
            for (auto k = 0; k < board_size; ++k) {
                if (pos.BP.is_set(board_size * i + k)) {
                    stream << "B";
                } else if (pos.WP.is_set(board_size * i + k)) {
                    stream << "W";
                } else {
                    stream << ".";
                }
                stream << " ";

            }
            stream << "\\";
            stream << "\n";
        }
        return stream;
    }

};


int main(int argl, const char **argc) {

/*
    Union<11> un;

    un.merge(1, 3);

    std::cout << un.in_same_set(1, 3) << std::endl;

    return 0;*/


    Prng generator(231231ull);

    auto t1 = std::chrono::high_resolution_clock::now();
    int sum = 0;
    for (auto k = 0; k < 1; ++k) {
        Position<11> pos;
        for (auto i = 0; i < 121; ++i) {
            auto index = pos.get_random_empty(generator);
            pos.make_move(index);
            sum += pos.color;
            if ((index % 2) == 0)
                sum += 3;
            //std::cout<<pos<<std::endl;
        }
    }
    auto t2 = std::chrono::high_resolution_clock::now();
    auto count = (t2 - t1).count();
    std::cout << count / 1000000 << std::endl;
    std::cout << "Summe: " << sum << std::endl;

    /*for (auto i = 0; i < empty_squares; ++i) {
        std::cout << pos;
        auto rand_index = pos.get_random_empty(generator);
        pos.make_move(rand_index);
    }*/


    return 0;
}
