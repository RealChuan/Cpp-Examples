#include "search.hpp"

#include <gtest/gtest.h>

TEST(test_algorithm_search, test_sequential_search)
{
    std::vector<int> v{1, 2, 3, 4, 5, 6, 7, 8, 9};
    EXPECT_EQ(sequential_search(v, 1), 0);
    EXPECT_EQ(sequential_search(v, 2), 1);
    EXPECT_EQ(sequential_search(v, 3), 2);
    EXPECT_EQ(sequential_search(v, 4), 3);
    EXPECT_EQ(sequential_search(v, 5), 4);
    EXPECT_EQ(sequential_search(v, 6), 5);
    EXPECT_EQ(sequential_search(v, 7), 6);
    EXPECT_EQ(sequential_search(v, 8), 7);
    EXPECT_EQ(sequential_search(v, 9), 8);
    EXPECT_EQ(sequential_search(v, 10), -1);
}

TEST(test_algorithm_search, test_binary_search)
{
    std::vector<int> v{1, 2, 3, 4, 5, 6, 7, 8, 9};
    EXPECT_EQ(binary_search(v, 1), 0);
    EXPECT_EQ(binary_search(v, 2), 1);
    EXPECT_EQ(binary_search(v, 3), 2);
    EXPECT_EQ(binary_search(v, 4), 3);
    EXPECT_EQ(binary_search(v, 5), 4);
    EXPECT_EQ(binary_search(v, 6), 5);
    EXPECT_EQ(binary_search(v, 7), 6);
    EXPECT_EQ(binary_search(v, 8), 7);
    EXPECT_EQ(binary_search(v, 9), 8);
    EXPECT_EQ(binary_search(v, 10), -1);
}

TEST(test_algorithm_search, test_fibonacci_search)
{
    std::vector<int> v{1, 2, 3, 4, 5, 6, 7, 8, 9};
    EXPECT_EQ(fibonacci_search(v, 1), 0);
    EXPECT_EQ(fibonacci_search(v, 2), 1);
    EXPECT_EQ(fibonacci_search(v, 3), 2);
    EXPECT_EQ(fibonacci_search(v, 4), 3);
    EXPECT_EQ(fibonacci_search(v, 5), 4);
    EXPECT_EQ(fibonacci_search(v, 6), 5);
    EXPECT_EQ(fibonacci_search(v, 7), 6);
    EXPECT_EQ(fibonacci_search(v, 8), 7);
    EXPECT_EQ(fibonacci_search(v, 9), 8);
    EXPECT_EQ(fibonacci_search(v, 10), -1);
}

TEST(test_algorithm_search, test_linear_index_search)
{
    std::vector<int> v{1, 2, 3, 4, 5, 6, 7, 8, 9};
    EXPECT_EQ(linear_index_search(v, 1), 0);
    EXPECT_EQ(linear_index_search(v, 2), 1);
    EXPECT_EQ(linear_index_search(v, 3), 2);
    EXPECT_EQ(linear_index_search(v, 4), 3);
    EXPECT_EQ(linear_index_search(v, 5), 4);
    EXPECT_EQ(linear_index_search(v, 6), 5);
    EXPECT_EQ(linear_index_search(v, 7), 6);
    EXPECT_EQ(linear_index_search(v, 8), 7);
    EXPECT_EQ(linear_index_search(v, 9), 8);
    EXPECT_EQ(linear_index_search(v, 10), -1);
}

TEST(test_algorithm_search, test_kmp_search)
{
    std::vector<int> v{1, 2, 3, 4, 5, 6, 7, 8, 9};
    std::vector<int> pattern{4, 5, 6};
    EXPECT_EQ(kmp_search(v, pattern), 3);
}

TEST(test_algorithm_search, test_bm_search)
{
    std::vector<int> v{1, 2, 3, 4, 5, 6, 7, 8, 9};
    std::vector<int> pattern{4, 5, 6};
    EXPECT_EQ(bm_search(v, pattern), 3);
}

TEST(test_algorithm_search, test_sunday_search)
{
    std::vector<int> v{1, 2, 3, 4, 5, 6, 7, 8, 9};
    std::vector<int> pattern{4, 5, 6};
    EXPECT_EQ(sunday_search(v, pattern), 3);
}

TEST(test_algorithm_search, test_rabin_karp_search)
{
    std::vector<int> v{1, 2, 3, 4, 5, 6, 7, 8, 9};
    std::vector<int> pattern{4, 5, 6};
    EXPECT_EQ(rabin_karp_search(v, pattern), 3);
}

int main(int argc, char *argv[])
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
