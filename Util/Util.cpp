//
// Created by robin on 18.02.22.
//

#include "Util.h"

std::vector<std::string> Util::split_by_delimiter(std::string text, std::string delim) {
    std::vector<std::string> result;
    size_t res;
    do {
        res = text.find(delim);
        result.emplace_back(text.substr(0, res));
        text.erase(0, delim.size() + res);
    } while (res != std::string::npos);

    return result;
}
