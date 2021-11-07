#include <iostream>
#include <chrono>
#include <iterator>
#include "Prng.h"
#include "Position.h"
#include "Union.h"
#include "Board.h"

int main(int argl, const char **argc) {


    auto t1 = std::chrono::high_resolution_clock::now();
    int sum = 0;
    for (auto k = 0; k < 880000; ++k) {
        Board<11> pos;
        auto result = pos.play_out();
        sum += result;
    }
    auto t2 = std::chrono::high_resolution_clock::now();
    auto count = (t2 - t1).count();
    std::cout << count / 1000000 << std::endl;
    std::cout << "Summe: " << sum << std::endl;





    return 0;
}
