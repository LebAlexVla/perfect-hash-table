#include <iostream>
#include <vector>
#include <string>

#include "lib/PerfectHashTable.hpp"

int main() {
    std::vector<std::pair<std::string, int>> data{
        {"best lyceum", 239},
        {"good lyceum", 30},
        {"good school", 80},
    };
    PerfectHashTable<std::string, int> hash_table(data);
}