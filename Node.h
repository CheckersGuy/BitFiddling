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

enum TerminalState : int8_t {
    WIN, LOSS, UNKNOWN
};

template<size_t board_size>
class Node {

public:
    using node_ptr = std::unique_ptr<Node<board_size>>;
    Node *parent;
    std::unique_ptr<node_ptr[]> children;
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
        //maximally favour unvisited children
        if (num_visits == 0) {
            return std::numeric_limits<float>::max()-1.0f;
        } else {
            //uct formula to be put here
            auto p_visits = static_cast<float>(parent->num_visits);

            if (!use_rave) {
                float uct = q_value / static_cast<float>(num_visits) +
                            std::sqrt(2.4f * std::log(p_visits) / static_cast<float>(num_visits));
                return uct;
            } else {
                const float bias = 0.001;
                auto rave_visits = static_cast<float>(num_rave);
                float rave_value = (q_rave / (1.0 + rave_visits));
                float q = q_value / (static_cast<float>(num_visits));
                float beta = (rave_visits / (rave_visits + static_cast<float>(num_visits) +
                                             bias * rave_visits * static_cast<float>(num_visits)));
                return ((1.0f - beta) * q + beta * rave_value);
            }


        }
    }


    auto begin() {
        return children.get();
    }

    auto end() {
        return children.get() + num_visits;
    }

    float get_win_over_visits() {
        return q_value / ((float) num_visits);
    }

    Node *operator[](int index) {
        return children[index].get();
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
            if (children[i]->is_won())
                return true;
        }
        return false;
    }

    bool has_lost_child() {
        for (auto i = 0; i < num_children; ++i) {
            if (children[i]->is_loss())
                return true;
        }
        return false;
    }

    bool is_terminal() {
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
        //expanding goes here
        auto empty_squares = ~(pos.BP | pos.WP);
        children = std::make_unique<node_ptr[]>(pos.get_num_empty());
        for (auto sq: empty_squares) {
            children[num_children++] = std::make_unique<Node>(sq, this);
        }
    }

    Node *best_child() {
        //for now simply selects the child with the most visits
        Node *max = nullptr;
        uint32_t max_visits = 0;
        for (auto i = 0; i < num_children; ++i) {
            auto &child = children[i];
            if (child->num_visits > max_visits) {
                max_visits = child->num_visits;
                max = child.get();
            }
        }

        return max;
    }

    Node *select(Prng &generator) {
        //select is trickier since we want to uniformly select
        //from all the nodes with max_value
        //we can find the max_value and at the same time count how many times
        //we have seen it

        size_t num_max = 0;
        Node *anymax = nullptr;
        float max = std::numeric_limits<float>::lowest();
        for (auto i = 0; i < num_children; ++i) {
            auto &child = children[i];
            float child_uct = child->operator()();
            if (child_uct > max) {
                anymax = child.get();
                max = child_uct;
                num_max = 1;
            } else if (child_uct == max) {
                num_max++;
            }
        }
        if (num_max == 1) {
            return anymax;
        }

        size_t rand_index = generator() % num_max;
        size_t counter = 0;

        for (auto i = 0; i < num_children; ++i) {
            auto &child = children[i];
            float child_uct = child->operator()();
            if (child_uct == max) {
                if (counter == rand_index) {
                    return child.get();
                }
                counter++;
            }
        }

        return anymax;
    }

    void back_up(Color result, Color turn, bit_pattern<board_size> WP, bit_pattern<board_size> BP) {
        //backup goes here
        float reward = ((result == turn) ? -1.0f : 1.0f);
        Node *current = this;

        while (current != nullptr) {


            if (turn == BLACK && current->num_children > 0) {
                for (auto sq: BP) {
                    for (auto i = 0; i < current->num_children; ++i) {
                        if (current->children[i]->move == sq) {
                            current->children[i]->q_rave += -reward;
                            current->children[i]->num_rave += 1;
                            break;
                        }
                    }
                }
            } else if (turn == WHITE && current->num_children > 0) {
                for (auto sq: WP) {
                    for (auto i = 0; i < current->num_children; ++i) {
                        if (current->children[i]->move == sq) {
                            current->children[i]->q_rave += -reward;
                            current->children[i]->num_rave += 1;
                            break;
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
