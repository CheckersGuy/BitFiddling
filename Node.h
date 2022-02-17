//
// Created by robin on 07.11.21.
//

#ifndef READING_NODE_H
#define READING_NODE_H

#include <math.h>

#include <cstdint>
#include <limits>
#include <memory>
#include <vector>
#include "Prng.h"
#include "Position.h"
#include <memory>
#include "NodeAllocator.h"

extern bool use_rave;

enum TerminalState : int8_t {
    WIN, LOSS, UNKNOWN
};

template<size_t board_size>
class Node {


public:
    Node<board_size> *parent;
    Node<board_size> *first_child;
    uint32_t num_visits{0};
    uint32_t num_rave{0};
    float q_value{0};
    float q_rave{0};
    SquareType<board_size> move;
    SquareType<board_size> num_children{0};
    TerminalState state{UNKNOWN};

public:

    Node() = default;

    Node(uint16_t move, Node *parent) : move(move), parent(parent) {}


    float operator()() const {
        if (num_visits == 0) {
            return std::numeric_limits<float>::max();
        } else {
            auto p_visits = static_cast<float>(parent->num_visits);

            if (!use_rave) {
                float uct = q_value / static_cast<float>(num_visits) +
                            std::sqrt(2.4f * std::log(p_visits) / static_cast<float>(num_visits));
                return uct;
            }
            const float bias = 0.001f;
            auto rave_visits = static_cast<float>(num_rave);
            float rave_value = (q_rave / (1.0f + rave_visits));
            float q = q_value / (static_cast<float>(num_visits));
            float beta = (rave_visits / (rave_visits + static_cast<float>(num_visits) +
                                         bias * rave_visits * static_cast<float>(num_visits)));
            return ((1.0f - beta) * q + beta * rave_value);


        }
    }

    void operator delete(void *adress) {
        NodeAllocator<board_size>::get_instance().free((Node<board_size> *) adress);
    }

    void *operator new(size_t size) {
        return NodeAllocator<board_size>::get_instance().allocate();
    }


    float get_win_over_visits() {
        return q_value / ((float) num_visits);
    }

    inline Node *operator[](int index) {
        return first_child + index;
    }

    void update(float reward) {
        num_visits++;
        q_value += reward;
    }

    void make_terminal(TerminalState st) {
        state = st;
    }

    uint32_t get_num_visits() {
        return num_visits;
    }

    float get_q() const {
        return q_value;
    }

    float get_q_rave() const {
        return q_rave;
    }

    TerminalState get_terminal_state() const {
        return state;
    }

    bool is_loss() const {
        return state == LOSS;
    }

    bool is_won() const {
        return state == WIN;
    }

    bool has_won_child() {
        for (auto i = 0; i < num_children; ++i) {
            if (first_child[i]->is_won())
                return true;
        }
        return false;
    }

    bool has_lost_child() {
        for (auto i = 0; i < num_children; ++i) {
            if (first_child[i]->is_loss())
                return true;
        }
        return false;
    }

    inline bool is_terminal() {
        return state != UNKNOWN;
    }

    uint32_t get_rave_visits() {
        return num_rave;
    }

    uint16_t get_move() {
        return move;
    }

    Node *get_parent() {
        return parent;
    }


    size_t get_num_children() {
        return num_children;
    }

    void expand(Position<board_size> &pos) {
        auto empty_squares = ~(pos.BP | pos.WP);
        first_child = NodeAllocator<board_size>::get_instance().allocate_children(pos.get_num_empty());
        for (auto sq: empty_squares) {
            first_child[num_children].move = sq;
            first_child[num_children].parent = this;
            num_children++;
        }
    }

    Node *best_child() {
        Node *max = nullptr;
        uint32_t max_visits = 0;
        for (auto i = 0; i < num_children; ++i) {
            Node<board_size> *child = first_child + i;
            if (child->num_visits > max_visits) {
                max_visits = child->num_visits;
                max = child;
            }
        }

        return max;
    }

    Node *select(Prng &generator) {
        size_t num_max = 0;
        Node<board_size> *anymax = first_child;
        float max = std::numeric_limits<float>::lowest();
        for (auto i = 0; i < num_children; ++i) {
            Node<board_size> *child = first_child + i;
            float child_uct = child->operator()();
            if (child_uct > max) {
                anymax = first_child+i;
                max = child_uct;
            }
        }

        return anymax;


    }

    void back_up(Color result, Color turn, bit_pattern<board_size> &WP, bit_pattern<board_size> &BP) {
        //rewrite into a more efficient backup operation
        float reward = ((result == turn) ? -1.0f : 1.0f);
        Node *current = this;

        size_t start = 0;
        size_t length = 0;
        while (current != nullptr) {

            if (use_rave) {
                if (turn == BLACK && current->num_children > 0) {
                    for (auto sq: BP) {
                        for (auto i = 0; i < current->num_children; ++i) {
                            auto &child = current->first_child[i];
                            start++;
                            if (child.move == sq) {
                                child.q_rave += -reward;
                                child.num_rave += 1;
                                break;
                            }
                        }
                    }
                } else if (turn == WHITE && current->num_children > 0) {
                    for (auto sq: WP) {
                        for (auto i = 0; i < current->num_children; ++i) {
                            auto &child = current->first_child[i];
                            start++;
                            if (child.move == sq) {
                                child.q_rave += -reward;
                                child.num_rave += 1;
                                break;
                            }
                        }
                    }
                }

            }
            current->update(reward);
            turn = ~turn;
            reward = -reward;
            current = current->get_parent();
        }
    }

};


#endif //READING_NODE_H
