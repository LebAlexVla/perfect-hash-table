#pragma once

#include <vector>
#include <utility>
#include <string>
#include <functional>
#include <algorithm>
#include <memory>
#include <optional>
#include <stdexcept>
#include <type_traits>

#include "UniversalHash.hpp"

template<typename Key, typename T>
class PerfectHashTable {
public:
    explicit PerfectHashTable(const std::vector<std::pair<Key, T>>& data)
    : size_(data.size()) {
        if (size_ == 0)
            return;

        static_assert(std::is_copy_constructible_v<Key>, "Key must be CopyConstructible");
        static_assert(std::is_copy_constructible_v<T>, "Value must be CopyConstructible");

        std::vector<std::vector<size_t>> indices_in_buckets(size_);
        BuildFirstLevel(data, indices_in_buckets);
        BuildSecondLevel(data, indices_in_buckets);
    }

    T* Find(const Key& key) {
        if (!size_)
            return nullptr;

        size_t first_index = first_hash_(key) % size_;
        if (!buckets_[first_index])
            return nullptr;

        size_t second_index = buckets_[first_index]->second_hash(key) % buckets_[first_index]->data.size();
        auto& opt = buckets_[first_index]->data[second_index];
        if (!opt.has_value() || opt.value().first != key)
            return nullptr;

        return &opt->second;
    }

    T& At(const Key& key) {
        T* ptr = Find(key);
        if (!ptr)
            throw std::out_of_range("No such key in the hash table");
        
        return *ptr;
    }

private:
    static constexpr size_t kMaxFirstLevelAttempts  = 100;
    static constexpr size_t kMaxSecondLevelAttempts = 100;
    static constexpr size_t kMaxLoadFactorSquared   = 4;

    struct Bucket {
        UniversalHash<Key> second_hash;
        std::vector<std::optional<std::pair<Key, T>>> data;
    };

    void BuildFirstLevel(const std::vector<std::pair<Key, T>>& data, std::vector<std::vector<size_t>>& indices_in_buckets) {
        bool is_hashed = false;
        size_t attempt = 0;
        while (!is_hashed && attempt < kMaxFirstLevelAttempts) {
            FirstHash(data, indices_in_buckets);
            if (CheckDistribution(indices_in_buckets)) {
                is_hashed = true;
            } else {
                first_hash_.RegenCoefs();
                indices_in_buckets.assign(size_, {});
            }
            ++attempt;
        }

        if (attempt == kMaxFirstLevelAttempts)
            throw std::runtime_error("Too many hash attempts in the first level of the perfect hash table");
    }

    void BuildSecondLevel(const std::vector<std::pair<Key, T>>& data, std::vector<std::vector<size_t>>& indices_in_buckets) {
        ResizeBuckets(data, indices_in_buckets);
        for (size_t i = 0; i < indices_in_buckets.size(); ++i) {
            size_t attempt = 0;
            while (!SecondHash(data, indices_in_buckets[i], buckets_[i]) && attempt < kMaxSecondLevelAttempts) {
                ++attempt;
                buckets_[i]->second_hash.RegenCoefs();
                for (auto& opt : buckets_[i]->data)
                    opt.reset();
            }
            std::vector<size_t>().swap(indices_in_buckets[i]);

            if (attempt == kMaxSecondLevelAttempts)
                throw std::runtime_error("Too many hash attempts in the second level of the perfect hash table");
        }
    }

    void FirstHash(const std::vector<std::pair<Key, T>>& data, std::vector<std::vector<size_t>>& indices_in_buckets) {
        for (size_t i = 0; i < data.size(); ++i) {
            const auto& key = data[i].first;
            size_t index = first_hash_(key) % size_;
            for (size_t old_i : indices_in_buckets[index])
                if (data[old_i].first == key)
                    throw std::invalid_argument("Duplicate key in the perfect hash table");
            indices_in_buckets[index].push_back(i);
        }
    }

    bool CheckDistribution(const std::vector<std::vector<size_t>>& indices_in_buckets) {
        size_t sum_of_squares = 0;
        for (const auto& bucket : indices_in_buckets)
            sum_of_squares += bucket.size() * bucket.size();
        
        return (sum_of_squares <= kMaxLoadFactorSquared * size_);
    }

    void ResizeBuckets(const std::vector<std::pair<Key, T>>& data, const std::vector<std::vector<size_t>>& indices_in_buckets) {
        buckets_.resize(size_);
        for (size_t i = 0; i < size_; ++i) {
            if (!indices_in_buckets[i].empty()) {
                buckets_[i] = std::make_unique<Bucket>();
                size_t n = indices_in_buckets[i].size();
                buckets_[i]->data.resize(n * n);
            }
        }
    }

    bool SecondHash(const std::vector<std::pair<Key, T>>& data, const std::vector<size_t>& indices_in_bucket, std::unique_ptr<Bucket>& bucket) {
        for (size_t data_index : indices_in_bucket) {
            const auto& [key, value] = data[data_index];
            size_t index = bucket->second_hash(key) % bucket->data.size();
            if (bucket->data[index].has_value()) {
                return false;
            } else {  
                bucket->data[index].emplace(key, value);
            }
        }

        return true;
    }

    UniversalHash<Key> first_hash_;
    std::vector<std::unique_ptr<Bucket>> buckets_;
    size_t size_;
};