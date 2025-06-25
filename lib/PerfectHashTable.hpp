#pragma once

#include <vector>
#include <utility>
#include <string>
#include <optional>
#include <functional>
#include <algorithm>

#include "UniversalHash.hpp"

template<typename T>
class PerfectHashTable {
public:
    explicit PerfectHashTable(const std::vector<std::pair<std::string, T>>& data)
    : size_(data.size()) {
        std::vector<std::vector<size_t>> buckets_indices(size_);
        BuildFirstLevel(buckets_indices);
        BuildSecondLevel(data, buckets_indices);
    }

    ~PerfectHashTable();

    std::optional<std::reference_wrapper<const T>> Find(const std::string& key);
    std::optional<std::reference_wrapper<T>> At(const std::string& key);
    bool Erase(const std::string& key);

    PerfectHashTable(const PerfectHashTable&) = delete;
    PerfectHashTable& operator=(const PerfectHashTable&) = delete;

private:
    struct Bucket {
        UniversalHash<T> second_hash;
        std::vector<std::pair<std::string, T>> data;
    };

    void BuildFirstLevel(std::vector<std::vector<size_t>>& buckets_indices) {
        bool is_hashed = false;
        while (!is_hashed) {
            size_t i = 0;
            for (const auto& [key, value] : data) {
                size_t index = first_hash_(key) % size_;
                buckets_indices[index].push_back(i++);
            }

            size_t sum_of_squares = 0;
            for (const auto& bucket : buckets_indices)
                sum_of_squares += bucket.size() * bucket.size();
            if (sum_of_squares <= 4 * size_) {
                is_hashed = true;
            } else {
                first_hash_.GenCoefs();
                for (auto& bucket : buckets_indices)
                    bucket.clear();
            }
        }
    }

    void BuildSecondLevel(const std::vector<std::pair<std::string, T>>& data, std::vector<size_t>& buckets_indices) {
        for (size_t i = 0; i < buckets_indices.size(): ++i) {
            buckets_[i].resize(buckets_indices[i].size() * buckets_indices[i].size());
        }

        // TODO
    }

    UniversalHash<T> first_hash_;
    std::vector<Bucket> buckets_;
    size_t size_;
};