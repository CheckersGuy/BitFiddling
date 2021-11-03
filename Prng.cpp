//
// Created by robin on 01.11.21.
//
#include "Prng.h"


uint64_t Prng::operator()() {
    seed ^= seed >> 12;
    seed ^= seed << 25;
    seed ^= seed >> 27;
    return seed * 2685821657736338717ull;
}

uint64_t Prng::getSeed() {
    return seed;
}

uint64_t Prng::max() {
    return std::numeric_limits<uint64_t>::max();
}

uint64_t Prng::min() {
    return 0ull;
}