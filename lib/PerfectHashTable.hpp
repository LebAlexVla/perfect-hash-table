#pragma once

#include <vector>
#include <utility>
#include <string>
#include <functional>
#include <algorithm>
#include <stdexcept>
#include <memory>

#include "UniversalHash.hpp"

template<typename Key, typename T>
class PerfectHashTable {
public:
    explicit PerfectHashTable(const std::vector<std::pair<Key, T>>& data)
    : size_(data.size()) {
        if (size_ == 0)
            return;

        std::vector<std::vector<size_t>> indices_in_bucket(size_);
        BuildFirstLevel(data, indices_in_bucket);
        BuildSecondLevel(data, indices_in_bucket);
    }

    T* Find(const Key& key) {
        if (!size_)
            return nullptr;

        size_t first_index = first_hash_(key) % size_;
        if (!buckets_[first_index])
            return nullptr;

        size_t second_index = buckets_[first_index]->second_hash(key) % buckets_[first_index]->data.size();
        if (!buckets_[first_index]->data[second_index])
            return nullptr;

        return &buckets_[first_index]->data[second_index]->second;
    }

    T& At(const Key& key) {
        if (!size_)
            throw std::out_of_range("No such key in the hash table");

        size_t first_index = first_hash_(key) % size_;
        if (!buckets_[first_index])
            throw std::out_of_range("No such key in the hash table");

        size_t second_index = buckets_[first_index]->second_hash(key) % buckets_[first_index]->data.size();
        if (!buckets_[first_index]->data[second_index])
            throw std::out_of_range("No such key in the hash table");

        return buckets_[first_index]->data[second_index]->second;
    }

    bool Erase(const Key& key) {
        if (!size_)
            return false;

        size_t first_index = first_hash_(key) % size_;
        if (!buckets_[first_index])
            return false;

        size_t second_index = buckets_[first_index]->second_hash(key) % buckets_[first_index]->data.size();
        if (!buckets_[first_index]->data[second_index])
            return false;

        buckets_[first_index]->data[second_index].reset();
        return true;
    }

private:
    struct Bucket {
        UniversalHash<Key> second_hash;
        std::vector<std::unique_ptr<std::pair<Key, T>>> data;
    };

    void BuildFirstLevel(const std::vector<std::pair<Key, T>>& data, std::vector<std::vector<size_t>>& indices_in_bucket) {
        bool is_hashed = false;
        while (!is_hashed) {
            size_t i = 0;
            for (const auto& [key, value] : data) {
                size_t index = first_hash_(key) % size_;
                indices_in_bucket[index].push_back(i++);
            }

            size_t sum_of_squares = 0;
            for (const auto& bucket : indices_in_bucket)
                sum_of_squares += bucket.size() * bucket.size();
            if (sum_of_squares <= 4 * size_) {
                is_hashed = true;
            } else {
                first_hash_.GenCoefs();
                indices_in_bucket.assign(size_, {});
            }
        }
    }

    void BuildSecondLevel(const std::vector<std::pair<Key, T>>& data, std::vector<std::vector<size_t>>& indices_in_bucket) {
        buckets_.resize(size_);
        for (size_t i = 0; i < indices_in_bucket.size(); ++i) {
            if (indices_in_bucket[i].size() > 0) {
                buckets_[i] = std::make_unique<Bucket>();
                buckets_[i]->data.resize(indices_in_bucket[i].size() * indices_in_bucket[i].size());
            }
        }

        for (size_t i = 0; i < indices_in_bucket.size(); ++i) {
            bool is_hashed = false;
            while (!is_hashed) {
                is_hashed = true;
                for (size_t j = 0; j < indices_in_bucket[i].size(); ++j) {
                    const auto& [key, value] = data[indices_in_bucket[i][j]];
                    size_t index = buckets_[i]->second_hash(key) % buckets_[i]->data.size();
                    if (!buckets_[i]->data[index]) {
                        buckets_[i]->data[index] = std::make_unique<std::pair<Key, T>>(key, value);
                    } else {
                        is_hashed = false;
                        buckets_[i]->second_hash.GenCoefs();
                        for (auto& ptr : buckets_[i]->data)
                            ptr = nullptr;
                        break;
                    }
                }
            }
            std::vector<size_t>().swap(indices_in_bucket[i]);
        }
    }

    UniversalHash<Key> first_hash_;
    std::vector<std::unique_ptr<Bucket>> buckets_;
    size_t size_;
};