#include <benchmark/benchmark.h>
#include <unordered_map>
#include <unordered_set>
#include <random>
#include <vector>
#include <stdexcept>
#include <algorithm>

#include <lib/PerfectHashTable.hpp>

static std::vector<std::pair<int, int>> GenerateData(int size, int max_key) {
    if (max_key < size) {
        throw std::invalid_argument("Max key must be >= size");
    }
    
    std::mt19937_64 gen(239);
    std::uniform_int_distribution<int> dist(0, max_key);
    std::unordered_set<int> used_keys;
    std::vector<std::pair<int, int>> data;
    data.reserve(size);

    while (data.size() < size) {
        int key = dist(gen);
        if (used_keys.insert(key).second) {
            data.emplace_back(key, data.size());
        }
    }

    return data;
}

static void BM_PerfectHashTableBuild(benchmark::State& state) {
    int size = state.range(0);
    int max_key = 2 * size;
    auto data = GenerateData(size, max_key);

    for (auto _ : state) {
        PerfectHashTable<int, int> table(data);
        benchmark::DoNotOptimize(table);
    }

    state.SetComplexityN(size);
}

static void BM_UnorderedMapBuild(benchmark::State& state) {
    int size = state.range(0);
    int max_key = 2 * size;
    auto data = GenerateData(size, max_key);

    for (auto _ : state) {
        std::unordered_map<int, int> map;
        map.reserve(size);
        for (const auto& [key, value] : data) {
            map.emplace(key, value);
        }
        benchmark::DoNotOptimize(map);
    }

    state.SetComplexityN(size);
}

static void BM_PerfectHashTableFind(benchmark::State& state) {
    int size = state.range(0);
    int max_key = 2 * size;
    auto data = GenerateData(size, max_key);
    PerfectHashTable<int, int> table(data);
    
    std::vector<int> keys;
    keys.reserve(size);
    for (const auto& [key, _] : data) {
        keys.push_back(key);
    }
    std::shuffle(keys.begin(), keys.end(), std::mt19937{239});

    int i = 0;
    for (auto _ : state) {
        benchmark::DoNotOptimize(table.Find(keys[i]));
        i = (i + 1) % keys.size();
    }
    state.SetComplexityN(size);
}

static void BM_UnorderedMapFind(benchmark::State& state) {
    int size = state.range(0);
    int max_key = 2 * size;
    auto data = GenerateData(size, max_key);
    std::unordered_map<int, int> map;
    map.reserve(size);
    for (const auto& [key, value] : data) {
        map.emplace(key, value);
    }
    
    std::vector<int> keys;
    keys.reserve(size);
    for (const auto& [key, _] : data) {
        keys.push_back(key);
    }
    std::shuffle(keys.begin(), keys.end(), std::mt19937{239});

    int i = 0;
    for (auto _ : state) {
        benchmark::DoNotOptimize(map.find(keys[i]));
        i = (i + 1) % keys.size();
    }
    state.SetComplexityN(size);
}

BENCHMARK(BM_PerfectHashTableBuild)
    ->RangeMultiplier(10)
    ->Range(100, 1'000'000)
    ->Unit(benchmark::kMillisecond)
    ->Complexity();

BENCHMARK(BM_UnorderedMapBuild)
    ->RangeMultiplier(10)
    ->Range(100, 1'000'000)
    ->Unit(benchmark::kMillisecond)
    ->Complexity();

BENCHMARK(BM_PerfectHashTableFind)
    ->RangeMultiplier(10)
    ->Range(100, 1'000'000)
    ->Unit(benchmark::kNanosecond)
    ->Complexity();

BENCHMARK(BM_UnorderedMapFind)
    ->RangeMultiplier(10)
    ->Range(100, 1'000'000)
    ->Unit(benchmark::kNanosecond)
    ->Complexity();

BENCHMARK_MAIN();