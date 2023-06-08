#include "sort.hpp"

#include <benchmark/benchmark.h>

static void BM_InsertionSort(benchmark::State &state)
{
    std::vector<int> v = generate_random_vector<int>(state.range(0));
    for (auto _ : state) {
        insertion_sort(v);
    }
}
BENCHMARK(BM_InsertionSort)->RangeMultiplier(2)->Range(1 << 10, 1 << 15);

static void BM_SelectionSort(benchmark::State &state)
{
    std::vector<int> v = generate_random_vector<int>(state.range(0));
    for (auto _ : state) {
        selection_sort(v);
    }
}
BENCHMARK(BM_SelectionSort)->RangeMultiplier(2)->Range(1 << 10, 1 << 15);

static void BM_MergeSort(benchmark::State &state)
{
    std::vector<int> v = generate_random_vector<int>(state.range(0));
    for (auto _ : state) {
        merge_sort(v);
    }
}
BENCHMARK(BM_MergeSort)->RangeMultiplier(2)->Range(1 << 10, 1 << 15);

static void BM_QuickSort(benchmark::State &state)
{
    std::vector<int> v = generate_random_vector<int>(state.range(0));
    for (auto _ : state) {
        quick_sort(v);
    }
}
BENCHMARK(BM_QuickSort)->RangeMultiplier(2)->Range(1 << 10, 1 << 15);

static void BM_HeapSort(benchmark::State &state)
{
    std::vector<int> v = generate_random_vector<int>(state.range(0));
    for (auto _ : state) {
        heap_sort(v);
    }
}
BENCHMARK(BM_HeapSort)->RangeMultiplier(2)->Range(1 << 10, 1 << 15);

static void BM_ShellSort(benchmark::State &state)
{
    std::vector<int> v = generate_random_vector<int>(state.range(0));
    for (auto _ : state) {
        shell_sort(v);
    }
}
BENCHMARK(BM_ShellSort)->RangeMultiplier(2)->Range(1 << 10, 1 << 15);

static void BM_BubbleSort(benchmark::State &state)
{
    std::vector<int> v = generate_random_vector<int>(state.range(0));
    for (auto _ : state) {
        bubble_sort(v);
    }
}
BENCHMARK(BM_BubbleSort)->RangeMultiplier(2)->Range(1 << 10, 1 << 15);

static void BM_RadixSort(benchmark::State &state)
{
    std::vector<int> v = generate_random_vector<int>(state.range(0));
    for (auto _ : state) {
        radix_sort(v);
    }
}
BENCHMARK(BM_RadixSort)->RangeMultiplier(2)->Range(1 << 10, 1 << 15);

static void BM_CountingSort(benchmark::State &state)
{
    std::vector<int> v = generate_random_vector<int>(state.range(0));
    for (auto _ : state) {
        counting_sort(v);
    }
}
BENCHMARK(BM_CountingSort)->RangeMultiplier(2)->Range(1 << 10, 1 << 15);

static void BM_BucketSort(benchmark::State &state)
{
    std::vector<double> v = generate_random_vector<double>(state.range(0));
    for (auto _ : state) {
        bucket_sort(v);
    }
}
BENCHMARK(BM_BucketSort)->RangeMultiplier(2)->Range(1 << 10, 1 << 15);

static void BM_StdSort(benchmark::State &state)
{
    std::vector<int> v = generate_random_vector<int>(state.range(0));
    for (auto _ : state) {
        std::sort(v.begin(), v.end());
    }
}
BENCHMARK(BM_StdSort)->RangeMultiplier(2)->Range(1 << 10, 1 << 15);

BENCHMARK_MAIN();
