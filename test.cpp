#include <iostream>
#include <array>
#include <immintrin.h>
#include <random>


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
    size_t left_over =num_squares % 64ull;
    left_over = (left_over == 0) ? 64 : left_over;
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
        return (fields[field_index] & (1ull << sub_index));
    }
};

enum Color {
    BLACK, WHITE
};

constexpr Color operator~(Color color) {
    return (color == BLACK) ? WHITE : BLACK;
}

template<size_t board_size>
struct Position {
    bit_pattern<board_size> BP;
    bit_pattern<board_size> WP;
    Color color{BLACK};


    void make_move(int idx) {
        if (color == BLACK) {
            BP.set_bit(idx);
        } else {
            WP.set_bit(idx);
        }
        color = ~color;
    }

    size_t get_num_empty() {
        size_t count = 0;
        for (auto i = 0; i < BP.fields.size() - 1; ++i) {
            count += __builtin_popcountll(~((BP.fields[i] | WP.fields[i])));
        }
        size_t last = BP.fields.size() - 1;
        count += __builtin_popcountll((~(BP.fields[last] | WP.fields[last])) & LEFT_MASK<board_size>);
        return count;
    }

    uint64_t get_empty_squares(int index) {
        if (index < BP.fields.size() - 1) {
            return ~(BP.fields[index] | WP.fields[index]);
        } else {
            return (~(BP.fields[index] | WP.fields[index])) & LEFT_MASK<board_size>;
        }
    }

    template<typename Generator>
    size_t get_random_empty(Generator &generator) {
        uint64_t rand = generator();
        int num_empty = get_num_empty();
        uint64_t index = rand % num_empty;
        std::cout << num_empty << std::endl;
        std::cout << index << std::endl;
        //Now we need to now where the empty square is
        int square = index;
        int field_index = 0;
        for (auto i = 0; i < BP.fields.size(); ++i) {
            int l_empt = get_empty_squares(i);
            int num = __builtin_popcountll(l_empt);
            if (square - num < 0) {
                std::cout << "Square" << square << std::endl;
                field_index = i;
                break;
            }
            square = square - num;
        }
        //now wee need to select the empty square given by the index
        uint64_t empty_squares = get_empty_squares(field_index);
        uint64_t index_mask = 1ull << square;
        std::cout << "Mask: " << index_mask << std::endl;
        uint64_t empty_square = _pdep_u64(index_mask, empty_squares);
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

    Position<11> pos;
    std::mt19937_64 generator(331231);


    auto empty_squares = pos.get_num_empty();

    for (auto i = 0; i < empty_squares; ++i) {
        std::cout << pos;
        auto rand_index = pos.get_random_empty(generator);
        pos.make_move(rand_index);
    }


    return 0;
}
