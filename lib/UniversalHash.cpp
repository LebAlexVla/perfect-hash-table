#include "UniversalHash.hpp"

#include <random>

UniversalHash<std::string>::UniversalHash() {
    RegenCoefs();
}

size_t UniversalHash<std::string>::operator()(const std::string& str) const noexcept {
    size_t hash = 0;
    for (auto c : str) {
        size_t symbol = static_cast<size_t>(c) + 1;
        hash = (hash * base_ + symbol) % kUniversal;
    }

    return hash;
}

void UniversalHash<std::string>::RegenCoefs() {
    static std::mt19937 gen(std::random_device{}());
    static std::uniform_int_distribution<size_t> dist_base(1, kUniversal - 1);
    
    base_ = dist_base(gen);
}

UniversalHash<int>::UniversalHash() {
    RegenCoefs();
}

size_t UniversalHash<int>::operator()(int num) const noexcept {
    uint64_t num_casted = static_cast<uint64_t>(num);
    return static_cast<size_t>(coef_a_ * num_casted + coef_b_) % kUniversal;
}

void UniversalHash<int>::RegenCoefs() {
    static std::mt19937 gen(std::random_device{}());
    static std::uniform_int_distribution<size_t> dist_a(1, kUniversal - 1);
    static std::uniform_int_distribution<size_t> dist_b(0, kUniversal - 1);
    
    coef_a_ = dist_a(gen);
    coef_b_ = dist_b(gen);
}