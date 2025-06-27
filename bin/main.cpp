#include <iostream>

#include "lib/PerfectHashTable.hpp"

int main() {
    std::vector<std::pair<std::string, int>> data{
        {"best lyceum", 239},
        {"good lyceum", 30},
        {"good school", 80},
    };
    PerfectHashTable<std::string, int> hash_table(data);

    std::cout << *hash_table.Find("best lyceum") << "\n";
    std::cout << *hash_table.Find("good lyceum") << "\n";
    std::cout << *hash_table.Find("good school") << "\n";
    std::cout << (hash_table.Find("good") == nullptr ? "nullptr" : "!?!?") << "\n";
    hash_table.At("good lyceum") = 366;
    std::cout << *hash_table.Find("good lyceum") << "\n";
}