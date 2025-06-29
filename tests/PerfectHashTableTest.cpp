#include <gtest/gtest.h>
#include <utility>
#include <string>
#include <vector>
#include <random>
#include <memory>
#include <set>

#include <lib/PerfectHashTable.hpp>

TEST(PerfectHashTableSuite, EmptyTest) {
    PerfectHashTable<std::string, int> table({});
    EXPECT_EQ(table.Find("A"), nullptr);
}

TEST(PerfectHashTableSuite, BasicOperationsTest) {
    std::vector<std::pair<std::string, int>> data = {
        {"best lyceum", 239},
        {"friend lyceum", 30},
        {"good school", 80}
    };
    
    PerfectHashTable<std::string, int> table(data);
    
    EXPECT_NE(table.Find("friend lyceum"), nullptr);
    EXPECT_EQ(*table.Find("best lyceum"), 239);
    EXPECT_EQ(table.Find("best class"), nullptr);
    
    EXPECT_EQ(table.At("good school"), 80);
    EXPECT_THROW(table.At("good student"), std::out_of_range);
}

TEST(PerfectHashTableSuite, DuplicateKeysTest) {
    std::vector<std::pair<std::string, int>> data = {
        {"best lyceum", 239},
        {"good school", 80},
        {"good school", 610}
    };
    
    EXPECT_THROW(
        ([&]() {
            PerfectHashTable<std::string, int> table(data);
        })(),
        std::invalid_argument
    );

}

TEST(PerfectHashTableSuite, IntKeysTest) {
    std::vector<std::pair<int, std::string>> data = {
        {239, "best lyceum"},
        {30, "friend lyceum"},
        {80, "good school"}
    };
    
    PerfectHashTable<int, std::string> table(data);
    
    EXPECT_EQ(table.At(239), "best lyceum");
    EXPECT_EQ(table.At(30), "friend lyceum");
    EXPECT_EQ(table.Find(56), nullptr);
}

TEST(PerfectHashTableSuite, LargeDatasetTest) {
    constexpr size_t kSize = 10000;
    std::vector<std::pair<int, int>> data;
    std::set<int> used_keys;
    
    std::mt19937 rng(239);
    std::uniform_int_distribution<int> dist(1, 10 * kSize);
    
    while (data.size() < kSize) {
        int key = dist(rng);
        if (used_keys.find(key) == used_keys.end()) {
            data.emplace_back(key, key + 80);
            used_keys.insert(key);
        }
    }
    
    PerfectHashTable<int, int> table(data);
    
    for (const auto& [key, value] : data) {
        ASSERT_NE(table.Find(key), nullptr);
        EXPECT_EQ(*table.Find(key), value);
        EXPECT_EQ(table.At(key), value);
    }
    
    for (int i = 1; i <= 100; ++i) {
        int missing_key = kSize * 10 + i;
        ASSERT_EQ(table.Find(missing_key), nullptr);
        EXPECT_THROW(table.At(missing_key), std::out_of_range);
    }
}