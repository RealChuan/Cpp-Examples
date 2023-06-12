#include <algorithm>
#include <iostream>
#include <iterator>
#include <vector>

// std::sort
void test_sort()
{
    std::cout << "----------std::sort----------" << std::endl;
    std::vector<int> v{3, 1, 4, 1, 5, 9};
    std::sort(v.begin(), v.end());
    std::copy(v.begin(), v.end(), std::ostream_iterator<int>(std::cout, " "));
    std::cout << '\n';
}

// std::stable_sort
void test_stable_sort()
{
    std::cout << "----------std::stable_sort----------" << std::endl;
    std::vector<int> v{3, 1, 4, 1, 5, 9};
    std::stable_sort(v.begin(), v.end());
    std::copy(v.begin(), v.end(), std::ostream_iterator<int>(std::cout, " "));
    std::cout << '\n';
}

// std::partial_sort
void test_partial_sort()
{
    std::cout << "----------std::partial_sort----------" << std::endl;
    std::vector<int> v{3, 1, 4, 1, 5, 9};
    std::partial_sort(v.begin(), v.begin() + 3, v.end());
    std::copy(v.begin(), v.end(), std::ostream_iterator<int>(std::cout, " "));
    std::cout << '\n';
}

// std::partial_sort_copy
void test_partial_sort_copy()
{
    std::cout << "----------std::partial_sort_copy----------" << std::endl;
    std::vector<int> v{3, 1, 4, 1, 5, 9};
    std::vector<int> v2(3);
    std::partial_sort_copy(v.begin(), v.end(), v2.begin(), v2.end());
    std::copy(v2.begin(), v2.end(), std::ostream_iterator<int>(std::cout, " "));
    std::cout << '\n';
}

// std::sort_heap
void test_sort_heap()
{
    std::cout << "----------std::sort_heap----------" << std::endl;
    std::vector<int> v{3, 1, 4, 1, 5, 9};
    std::make_heap(v.begin(), v.end());
    std::sort_heap(v.begin(), v.end());
    std::copy(v.begin(), v.end(), std::ostream_iterator<int>(std::cout, " "));
    std::cout << '\n';
}

int main()
{
    test_sort();
    test_stable_sort();
    test_partial_sort();
    test_partial_sort_copy();
    test_sort_heap();
    return 0;
}
