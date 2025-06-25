#pragma once

#include <string>

template <typename>
inline constexpr bool always_false = false;

template<typename T>
class UniversalHash {
    static_assert(
        always_false<T>,
        "There is no specialization of the UniversalHash for this type"
    );
};

template<>
class UniversalHash<std::string> {
public:
    UniversalHash();
    size_t operator()(const std::string& str) const noexcept;
    void GenCoefs();

private:
    static constexpr size_t kUniversal = 2000000011;

    size_t base_;
};

template<>
class UniversalHash<int> {
public:
    UniversalHash();
    size_t operator()(int num) const noexcept;
    void GenCoefs();

private:
    static constexpr size_t kUniversal = 2000000011;

    size_t coef_a_;
    size_t coef_b_;
};