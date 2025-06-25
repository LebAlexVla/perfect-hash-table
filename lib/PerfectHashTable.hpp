#pragma once

#include <vector>
#include <utility>
#include <string>
#include <optional>
#include <functional>
#include <algorithm>

#include "UniversalHash.hpp"

template<typename Key, typename T>
class PerfectHashTable {
public:
    explicit PerfectHashTable(const std::vector<std::pair<Key, T>>& data)
    : size_(data.size()) {
        std::vector<std::vector<size_t>> buckets_indices(size_);
        BuildFirstLevel(buckets_indices);
        BuildSecondLevel(data, buckets_indices);
    }

    ~PerfectHashTable();

    std::optional<std::reference_wrapper<const T>> Find(const Key& key);
    std::optional<std::reference_wrapper<T>> At(const Key& key);
    bool Erase(const Key& key);

    PerfectHashTable(const PerfectHashTable&) = delete;
    PerfectHashTable& operator=(const PerfectHashTable&) = delete;

private:
    struct Bucket {
        UniversalHash<T> second_hash;
        std::vector<std::pair<Key, T>> data;
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

    void BuildSecondLevel(const std::vector<std::pair<Key, T>>& data, std::vector<size_t>& buckets_indices) {
        for (size_t i = 0; i < buckets_indices.size(): ++i) {
            buckets_[i].resize(buckets_indices[i].size() * buckets_indices[i].size());
        }

        for (auto& bucket : buckets_indices) {
            UniversalHash<T>
            bool is_hashed = false;
            int i = 0;
            while (!is_hashed) {
                auto& [key, value] = data[bucket[i]];
                if ()
            }
            std::vector<size_t>().swap(bucket);
        }
        // TODO
    }

    UniversalHash<T> first_hash_;
    std::vector<Bucket> buckets_;
    size_t size_;
};