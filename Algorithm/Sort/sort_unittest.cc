#include "sort.hpp"

#include <gtest/gtest.h>

#include <iostream>

// use gtest to test sort algorithm
TEST(SortTest, InsertionSort)
{
    std::vector<int> v = generate_random_vector<int>(10);
    auto v1 = v;
    std::sort(v1.begin(), v1.end());
    insertion_sort(v);
    EXPECT_EQ(v, v1);
}

TEST(SortTest, SelectionSort)
{
    std::vector<int> v = generate_random_vector<int>(10);
    auto v1 = v;
    std::sort(v1.begin(), v1.end());
    selection_sort(v);
    EXPECT_EQ(v, v1);
}

TEST(SortTest, MergeSort)
{
    std::vector<int> v = generate_random_vector<int>(10);
    auto v1 = v;
    std::sort(v1.begin(), v1.end());
    merge_sort(v);
    EXPECT_EQ(v, v1);
}

TEST(SortTest, QuickSort)
{
    std::vector<int> v = generate_random_vector<int>(10);
    auto v1 = v;
    std::sort(v1.begin(), v1.end());
    quick_sort(v, 0, v.size() - 1);
    EXPECT_EQ(v, v1);
}

TEST(SortTest, HeapSort)
{
    std::vector<int> v = generate_random_vector<int>(10);
    auto v1 = v;
    std::sort(v1.begin(), v1.end());
    heap_sort(v);
    EXPECT_EQ(v, v1);
}

TEST(SortTest, ShellSort)
{
    std::vector<int> v = generate_random_vector<int>(10);
    auto v1 = v;
    std::sort(v1.begin(), v1.end());
    shell_sort(v);
    EXPECT_EQ(v, v1);
}

TEST(SortTest, BubbleSort)
{
    std::vector<int> v = generate_random_vector<int>(10);
    auto v1 = v;
    std::sort(v1.begin(), v1.end());
    bubble_sort(v);
    EXPECT_EQ(v, v1);
}

TEST(SortTest, CountingSort)
{
    std::vector<int> v = generate_random_vector<int>(10);
    auto v1 = v;
    std::sort(v1.begin(), v1.end());
    counting_sort(v);
    EXPECT_EQ(v, v1);
}

TEST(SortTest, BucketSort)
{
    std::vector<int> v = generate_random_vector<int>(10);
    auto v1 = v;
    std::sort(v1.begin(), v1.end());
    bucket_sort(v);
    EXPECT_EQ(v, v1);
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
