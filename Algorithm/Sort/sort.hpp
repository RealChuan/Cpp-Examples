#pragma once

#include <algorithm>
#include <iterator>
#include <random>
#include <vector>

// generate random vector
template<typename T>
std::vector<T> generate_random_vector(int size)
{
    std::vector<T> v;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, size);
    for (int i = 0; i < size; ++i) {
        v.push_back(dis(gen));
    }
    return v;
}

// 1. 插入排序
template<typename T>
void insertion_sort(std::vector<T> &v)
{
    for (int i = 1; i < v.size(); ++i) {
        T temp = v[i];
        int j = i - 1;
        while (j >= 0 && v[j] > temp) {
            v[j + 1] = v[j];
            --j;
        }
        v[j + 1] = temp;
    }
}

// 2. 选择排序
template<typename T>
void selection_sort(std::vector<T> &v)
{
    for (int i = 0; i < v.size() - 1; ++i) {
        int min = i;
        for (int j = i + 1; j < v.size(); ++j) {
            if (v[j] < v[min]) {
                min = j;
            }
        }
        std::swap(v[i], v[min]);
    }
}

// 3. 归并排序
template<typename T>
void merge_sort(std::vector<T> &v)
{
    if (v.size() <= 1) {
        return;
    }
    std::vector<T> left(v.begin(), v.begin() + v.size() / 2);
    std::vector<T> right(v.begin() + v.size() / 2, v.end());
    merge_sort(left);
    merge_sort(right);
    std::merge(left.begin(), left.end(), right.begin(), right.end(), v.begin());
}

// 4. 快速排序
template<typename T>
int quick_sort(std::vector<T> &v)
{
    // base case: if the vector has one or zero elements, it is already sorted
    if (v.size() <= 1)
        return 0;     // recursive case: choose a pivot element and partition the vector around it
    int pivot = v[0]; // choose the first element as the pivot
    int i = 1, j = v.size() - 1; // indices for the left and right subvectors
    while (
        i <= j) { // find the first element from the left that is greater than or equal to the pivot
        while (i <= j && v[i] < pivot)
            i++;  // find the first element from the right that is less than or equal to the pivot
        while (i <= j && v[j] > pivot)
            j--;  // swap the elements if they are out of order
        if (i <= j) {
            std::swap(v[i], v[j]); // swap the elements using the standard library function
            i++;                   // move to the next element from the left
            j--;                   // move to the previous element from the right
        }
    }                              // swap the pivot with the last element from the left subvector
    std::swap(v[0], v[j]);         // swap the elements using the standard library function
    // sort the left and right subvectors recursively
    std::vector<T> left(v.begin(), v.begin() + j); // copy the left subvector
    std::vector<T> right(v.begin() + i, v.end());  // copy the right subvector
    int left_count = quick_sort(left);   // sort the left subvector and count the number of swaps
    int right_count = quick_sort(right); // sort the right subvector and count the number of swaps
    // copy the sorted subvectors back to the original vector
    std::copy(left.begin(),
              left.end(),
              v.begin()); // copy the left subvector using the standard library function
    std::copy(right.begin(),
              right.end(),
              v.begin() + i); // copy the right subvector using the standard library function
    // return the total number of swaps performed in this function call
    return left_count + right_count + 1;
}

// 5. 堆排序
template<typename T>
void heap_sort(std::vector<T> &v)
{
    std::make_heap(v.begin(), v.end());
    std::sort_heap(v.begin(), v.end());
}

// 6. 希尔排序
template<typename T>
void shell_sort(std::vector<T> &v)
{
    for (int gap = v.size() / 2; gap > 0; gap /= 2) {
        for (int i = gap; i < v.size(); ++i) {
            T temp = v[i];
            int j = i - gap;
            while (j >= 0 && v[j] > temp) {
                v[j + gap] = v[j];
                j -= gap;
            }
            v[j + gap] = temp;
        }
    }
}

// 7. 冒泡排序
template<typename T>
void bubble_sort(std::vector<T> &v)
{
    for (int i = 0; i < v.size() - 1; ++i) {
        for (int j = 0; j < v.size() - i - 1; ++j) {
            if (v[j] > v[j + 1]) {
                std::swap(v[j], v[j + 1]);
            }
        }
    }
}

// 8. 基数排序
template<typename T>
void radix_sort(std::vector<T> &v)
{
    int max = *std::max_element(v.begin(), v.end());
    for (int exp = 1; max / exp > 0; exp *= 10) {
        std::vector<T> temp(v.size());
        std::vector<T> count(10);
        for (int i = 0; i < v.size(); ++i) {
            ++count[(v[i] / exp) % 10];
        }
        for (int i = 1; i < count.size(); ++i) {
            count[i] += count[i - 1];
        }
        for (int i = v.size() - 1; i >= 0; --i) {
            temp[--count[(v[i] / exp) % 10]] = v[i];
        }
        for (int i = 0; i < v.size(); ++i) {
            v[i] = temp[i];
        }
    }
}

// 9. 计数排序
template<typename T>
void counting_sort(std::vector<T> &v)
{
    T max = *std::max_element(v.begin(), v.end());
    std::vector<T> temp(max + 1);
    for (int i = 0; i < v.size(); ++i) {
        ++temp[v[i]];
    }
    for (int i = 1; i < temp.size(); ++i) {
        temp[i] += temp[i - 1];
    }
    std::vector<T> result(v.size());
    for (int i = v.size() - 1; i >= 0; --i) {
        result[--temp[v[i]]] = v[i];
    }
    for (int i = 0; i < v.size(); ++i) {
        v[i] = result[i];
    }
}

// 10. 桶排序
template<typename T>
void bucket_sort(std::vector<T> &v)
{
    T max = *std::max_element(v.begin(), v.end());
    std::vector<std::vector<T>> temp(v.size());
    for (int i = 0; i < v.size(); ++i) {
        int index = v[i] * v.size() / (max + 1);
        temp[index].push_back(v[i]);
    }
    for (int i = 0; i < temp.size(); ++i) {
        std::sort(temp[i].begin(), temp[i].end());
    }
    int index = 0;
    for (int i = 0; i < temp.size(); ++i) {
        for (int j = 0; j < temp[i].size(); ++j) {
            v[index++] = temp[i][j];
        }
    }
}