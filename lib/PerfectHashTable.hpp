#pragma once

#include <vector>
#include <utility>
#include <string>
#include <functional>
#include <algorithm>
#include <stdexcept>
#include <memory>
#include <optional>

#include "UniversalHash.hpp"

template<typename Key, typename T>
class PerfectHashTable {
public:
    explicit PerfectHashTable(const std::vector<std::pair<Key, T>>& data)
    : size_(data.size()) {
        if (size_ == 0)
            return;

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
        auto& ptr = buckets_[first_index]->data[second_index];
        if (!ptr || ptr->first != key)
            return nullptr;

        return &ptr->second;
    }

    T& At(const Key& key) {
        T* ptr = Find(key);
        if (!ptr)
            throw std::out_of_range("No such key in the hash table");
        
        return *ptr;
    }

private:
    struct Bucket {
        UniversalHash<Key> second_hash;
        std::vector<std::unique_ptr<std::pair<Key, T>>> data;
    };

    void BuildFirstLevel(const std::vector<std::pair<Key, T>>& data, std::vector<std::vector<size_t>>& indices_in_buckets) {
        bool is_hashed = false;
        size_t attempt = 0;
        while (!is_hashed && attempt < 100) {
            FirstHash(data, indices_in_buckets);
            if (CheckDistribution(indices_in_buckets)) {
                is_hashed = true;
            } else {
                first_hash_.RegenCoefs();
                indices_in_buckets.assign(size_, {});
            }
            ++attempt;
        }

        if (attempt == 100)
            throw std::runtime_error("Too many hash attempts in the first level of the perfect hash table");
    }

    void BuildSecondLevel(const std::vector<std::pair<Key, T>>& data, std::vector<std::vector<size_t>>& indices_in_buckets) {
        ResizeBuckets(data, indices_in_buckets);
        for (size_t i = 0; i < indices_in_buckets.size(); ++i) {
            size_t attempt = 0;
            while (!SecondHash(data, indices_in_buckets[i], i) && attempt < 100) {
                ++attempt;
            }
            std::vector<size_t>().swap(indices_in_buckets[i]);

            if (attempt == 100)
                throw std::runtime_error("Too many hash attempts in the second level of the perfect hash table");
        }
    }

    void FirstHash(const std::vector<std::pair<Key, T>>& data, std::vector<std::vector<size_t>>& indices_in_buckets) {
        size_t i = 0;
        for (const auto& [key, value] : data) {
            size_t index = first_hash_(key) % size_;
            for (size_t old_i : indices_in_buckets[index])
                if (data[old_i].first == key)
                    throw std::invalid_argument("Duplicate key in the perfect hash table");
            indices_in_buckets[index].push_back(i++);
        }
    }

    bool CheckDistribution(const std::vector<std::vector<size_t>>& indices_in_buckets) {
        size_t sum_of_squares = 0;
        for (const auto& bucket : indices_in_buckets)
            sum_of_squares += bucket.size() * bucket.size();
        if (sum_of_squares > 4 * size_)
            return false;
        
        return true;
    }

    void ResizeBuckets(const std::vector<std::pair<Key, T>>& data, const std::vector<std::vector<size_t>>& indices_in_buckets) {
        buckets_.resize(size_);
        for (size_t i = 0; i < indices_in_buckets.size(); ++i) {
            if (indices_in_buckets[i].size() > 0) {
                buckets_[i] = std::make_unique<Bucket>();
                buckets_[i]->data.resize(indices_in_buckets[i].size() * indices_in_buckets[i].size());
            }
        }
    }

    bool SecondHash(const std::vector<std::pair<Key, T>>& data, const std::vector<size_t>& indices_in_bucket, const size_t bucket_index) {
        for (size_t i = 0; i < indices_in_bucket.size(); ++i) {
            const auto& [key, value] = data[indices_in_bucket[i]];
            size_t index = buckets_[bucket_index]->second_hash(key) % buckets_[bucket_index]->data.size();
            if (!buckets_[bucket_index]->data[index]) {
                buckets_[bucket_index]->data[index] = std::make_unique<std::pair<Key, T>>(key, value);
            } else {
                buckets_[bucket_index]->second_hash.RegenCoefs();
                for (auto& ptr : buckets_[bucket_index]->data)
                    ptr.reset();
                
                return false;
            }
        }

        return true;
    }

    UniversalHash<Key> first_hash_;
    std::vector<std::unique_ptr<Bucket>> buckets_;
    size_t size_;
};