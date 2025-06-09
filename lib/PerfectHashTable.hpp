#pragma once

#include <vector>
#include <utility>
#include <string>
#include <optional>
#include <functional>

template<typename T>
class PerfectHashTable {
public:
    explicit PerfectHashTable(const std::vector<std::pair<std::string, T>>& data);
    ~PerfectHashTable();

    std::optional<std::reference_wrapper<const T>> Find(const std::string& key);
    std::optional<std::reference_wrapper<T>> At(const std::string& key);
    bool Erase(const std::string& key);

    PerfectHashTable(const PerfectHashTable&) = delete;
    PerfectHashTable& operator=(const PerfectHashTable&) = delete;
};