#include "search.hpp"

#include <benchmark/benchmark.h>

#include <numeric>

static void BM_sequential_search(benchmark::State &state)
{
    std::vector<int> v(state.range(0));
    std::iota(v.begin(), v.end(), 0);
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(v.begin(), v.end(), g);
    for (auto _ : state) {
        int index = g() % v.size();
        benchmark::DoNotOptimize(sequential_search(v, v[index]));
    }
}

static void BM_binary_search(benchmark::State &state)
{
    std::vector<int> v(state.range(0));
    std::iota(v.begin(), v.end(), 0);
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(v.begin(), v.end(), g);
    for (auto _ : state) {
        int index = g() % v.size();
        benchmark::DoNotOptimize(binary_search(v, v[index]));
    }
}

static void BM_fibonacci_search(benchmark::State &state)
{
    std::vector<int> v(state.range(0));
    std::iota(v.begin(), v.end(), 0);
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(v.begin(), v.end(), g);
    for (auto _ : state) {
        int index = g() % v.size();
        benchmark::DoNotOptimize(fibonacci_search(v, v[index]));
    }
}

static void BM_linear_index_search(benchmark::State &state)
{
    std::vector<int> v(state.range(0));
    std::iota(v.begin(), v.end(), 0);
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(v.begin(), v.end(), g);
    for (auto _ : state) {
        int index = g() % v.size();
        benchmark::DoNotOptimize(linear_index_search(v, v[index]));
    }
}

static void BM_kmp_search(benchmark::State &state)
{
    std::vector<int> v(state.range(0));
    std::iota(v.begin(), v.end(), 0);
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(v.begin(), v.end(), g);
    int index = g() % v.size();
    std::vector<int> pattern(v.begin() + index, v.end());
    for (auto _ : state) {
        benchmark::DoNotOptimize(kmp_search(v, pattern));
    }
}

static void BM_bm_search(benchmark::State &state)
{
    std::vector<int> v(state.range(0));
    std::iota(v.begin(), v.end(), 0);
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(v.begin(), v.end(), g);
    int index = g() % v.size();
    std::vector<int> pattern(v.begin() + index, v.end());
    for (auto _ : state) {
        benchmark::DoNotOptimize(bm_search(v, pattern));
    }
}

static void BM_sunday_search(benchmark::State &state)
{
    std::vector<int> v(state.range(0));
    std::iota(v.begin(), v.end(), 0);
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(v.begin(), v.end(), g);
    int index = g() % v.size();
    std::vector<int> pattern(v.begin() + index, v.end());
    for (auto _ : state) {
        benchmark::DoNotOptimize(sunday_search(v, pattern));
    }
}

static void BM_rabin_karp_search(benchmark::State &state)
{
    std::vector<int> v(state.range(0));
    std::iota(v.begin(), v.end(), 0);
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(v.begin(), v.end(), g);
    int index = g() % v.size();
    std::vector<int> pattern(v.begin() + index, v.end());
    for (auto _ : state) {
        benchmark::DoNotOptimize(rabin_karp_search(v, pattern));
    }
}

BENCHMARK(BM_sequential_search)->Range(8, 8 << 10);
BENCHMARK(BM_binary_search)->Range(8, 8 << 10);
BENCHMARK(BM_fibonacci_search)->Range(8, 8 << 10);
BENCHMARK(BM_linear_index_search)->Range(8, 8 << 10);
BENCHMARK(BM_kmp_search)->Range(8, 8 << 10);
BENCHMARK(BM_bm_search)->Range(8, 2 << 7);
BENCHMARK(BM_sunday_search)->Range(8, 2 << 7);
BENCHMARK(BM_rabin_karp_search)->Range(8, 8 << 10);

BENCHMARK_MAIN();
