#include <algorithm>
#include <iostream>
#include <iterator>
#include <vector>

// std::find
void test_find()
{
    std::cout << "----------std::find----------" << std::endl;
    std::vector<int> v{1, 2, 3, 4, 5, 6, 7, 8};
    auto it = std::find(v.begin(), v.end(), 5);
    if (it != v.end()) {
        std::cout << "5 is found at position " << std::distance(v.begin(), it) << '\n';
    } else {
        std::cout << "5 is not found\n";
    }
}

// std::find_if
void test_find_if()
{
    std::cout << "----------std::find_if----------" << std::endl;
    std::vector<int> v{1, 2, 3, 4, 5, 6, 7, 8};
    auto it = std::find_if(v.begin(), v.end(), [](int i) { return i % 2 == 0; });
    if (it != v.end()) {
        std::cout << "even number " << *it << " is found at position "
                  << std::distance(v.begin(), it) << '\n';
    } else {
        std::cout << "even number is not found\n";
    }
}

// std::find_if_not
void test_find_if_not()
{
    std::cout << "----------std::find_if_not----------" << std::endl;
    std::vector<int> v{1, 2, 3, 4, 5, 6, 7, 8};
    auto it = std::find_if_not(v.begin(), v.end(), [](int i) { return i % 2 == 0; });
    if (it != v.end()) {
        std::cout << "odd number " << *it << " is found at position "
                  << std::distance(v.begin(), it) << '\n';
    } else {
        std::cout << "odd number is not found\n";
    }
}

// std::find_end
void test_find_end()
{
    std::cout << "----------std::find_end----------" << std::endl;
    std::vector<int> v{1, 2, 3, 4, 5, 6, 7, 8};
    std::vector<int> sub{3, 4, 5};
    auto it = std::find_end(v.begin(), v.end(), sub.begin(), sub.end());
    if (it != v.end()) {
        std::cout << "sub is found at position " << std::distance(v.begin(), it) << '\n';
    } else {
        std::cout << "sub is not found\n";
    }
}

// std::find_first_of
void test_find_first_of()
{
    std::cout << "----------std::find_first_of----------" << std::endl;
    std::vector<int> v{1, 2, 3, 4, 5, 6, 7, 8};
    std::vector<int> sub{3, 4, 5};
    auto it = std::find_first_of(v.begin(), v.end(), sub.begin(), sub.end());
    if (it != v.end()) {
        std::cout << "sub is found at position " << std::distance(v.begin(), it) << '\n';
    } else {
        std::cout << "sub is not found\n";
    }
}

// std::adjacent_find
void test_adjacent_find()
{
    std::cout << "----------std::adjacent_find----------" << std::endl;
    std::vector<int> v{1, 2, 3, 3, 4, 5, 6, 7, 8};
    auto it = std::adjacent_find(v.begin(), v.end());
    if (it != v.end()) {
        std::cout << "adjacent pair " << *it << " " << *(it + 1) << " is found at position "
                  << std::distance(v.begin(), it) << '\n';
    } else {
        std::cout << "adjacent pair is not found\n";
    }
}

// std::count
void test_count()
{
    std::cout << "----------std::count----------" << std::endl;
    std::vector<int> v{1, 2, 3, 3, 4, 5, 6, 7, 8};
    auto n = std::count(v.begin(), v.end(), 3);
    std::cout << "3 is found " << n << " times\n";
}

// std::count_if
void test_count_if()
{
    std::cout << "----------std::count_if----------" << std::endl;
    std::vector<int> v{1, 2, 3, 3, 4, 5, 6, 7, 8};
    auto n = std::count_if(v.begin(), v.end(), [](int i) { return i % 2 == 0; });
    std::cout << "even number is found " << n << " times\n";
}

// std::mismatch
void test_mismatch()
{
    std::cout << "----------std::mismatch----------" << std::endl;
    std::vector<int> v1{1, 2, 3, 3, 4, 5, 6, 7, 8};
    std::vector<int> v2{1, 2, 3, 4, 5, 6, 7, 8};
    auto p = std::mismatch(v1.begin(), v1.end(), v2.begin(), v2.end());
    if (p.first != v1.end()) {
        std::cout << "first mismatch is " << *p.first << " " << *p.second << '\n';
    } else {
        std::cout << "no mismatch\n";
    }
}

// std::equal
void test_equal()
{
    std::cout << "----------std::equal----------" << std::endl;
    std::vector<int> v1{1, 2, 3, 3, 4, 5, 6, 7, 8};
    std::vector<int> v2{1, 2, 3, 4, 5, 6, 7, 8};
    auto b = std::equal(v1.begin(), v1.end(), v2.begin(), v2.end());
    if (b) {
        std::cout << "v1 and v2 are equal\n";
    } else {
        std::cout << "v1 and v2 are not equal\n";
    }
}

// std::is_permutation
void test_is_permutation()
{
    std::cout << "----------std::is_permutation----------" << std::endl;
    std::vector<int> v1{1, 2, 3, 3, 4, 5, 6, 7, 8};
    std::vector<int> v2{1, 2, 3, 4, 5, 6, 7, 8};
    auto b = std::is_permutation(v1.begin(), v1.end(), v2.begin(), v2.end());
    if (b) {
        std::cout << "v1 and v2 are permutation\n";
    } else {
        std::cout << "v1 and v2 are not permutation\n";
    }
}

// std::search
void test_search()
{
    std::cout << "----------std::search----------" << std::endl;
    std::vector<int> v{1, 2, 3, 3, 4, 5, 6, 7, 8};
    std::vector<int> sub{3, 4, 5};
    auto it = std::search(v.begin(), v.end(), sub.begin(), sub.end());
    if (it != v.end()) {
        std::cout << "sub is found at position " << std::distance(v.begin(), it) << '\n';
    } else {
        std::cout << "sub is not found\n";
    }
}

// std::search_n
void test_search_n()
{
    std::cout << "----------std::search_n----------" << std::endl;
    std::vector<int> v{1, 2, 3, 3, 4, 5, 6, 7, 8};
    auto it = std::search_n(v.begin(), v.end(), 2, 3);
    if (it != v.end()) {
        std::cout << "3 is found at position " << std::distance(v.begin(), it) << '\n';
    } else {
        std::cout << "3 is not found\n";
    }
}

auto main() -> int
{
    test_find();
    test_find_end();
    test_find_first_of();
    test_adjacent_find();
    test_count();
    test_count_if();
    test_mismatch();
    test_equal();
    test_is_permutation();
    test_search();
    test_search_n();

    return 0;
}