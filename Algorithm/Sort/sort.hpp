#pragma once

#include <algorithm>
#include <iterator>
#include <random>
#include <vector>

// generate random vector
template<typename T>
auto generate_random_vector(int size) -> std::vector<T>
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

// 插入排序
template<typename T>
void insertion_sort(std::vector<T> &v)
{
    for (int i = 1; i < v.size(); ++i) {
        T temp = v[i];                  // 保存当前元素
        int j = i - 1;                  // 从当前元素的前一个元素开始比较
        while (j >= 0 && v[j] > temp) { // 如果当前元素小于前一个元素，就将前一个元素后移
            v[j + 1] = v[j];
            --j;
        }
        v[j + 1] = temp; // 将当前元素插入到合适的位置
    }
}

// 选择排序
template<typename T>
void selection_sort(std::vector<T> &v)
{
    for (int i = 0; i < v.size() - 1; ++i) {
        int min = i;                             // 保存最小元素的下标
        for (int j = i + 1; j < v.size(); ++j) { // 从当前元素的下一个元素开始比较
            if (v[j] < v[min]) { // 如果当前元素小于最小元素，就更新最小元素的下标
                min = j;
            }
        }
        std::swap(v[i], v[min]); // 将最小元素与当前元素交换
    }
}

// 归并排序
template<typename T>
void merge_sort(std::vector<T> &v)
{
    if (v.size() <= 1) { // 如果只有一个元素，就不需要排序
        return;
    }
    std::vector<T> left(v.begin(), v.begin() + v.size() / 2); // 复制左半部分
    std::vector<T> right(v.begin() + v.size() / 2, v.end());  // 复制右半部分
    merge_sort(left);                                         // 递归排序左半部分
    merge_sort(right);                                        // 递归排序右半部分
    std::merge(left.begin(), left.end(), right.begin(), right.end(), v.begin()); // 合并左右两部分
}

// 快速排序
template<typename T>
void quick_sort(std::vector<T> &v, int left, int right)
{
    if (left >= right) { // 如果只有一个元素，就不需要排序
        return;
    }
    int i = left;      // 保存左边界
    int j = right;     // 保存右边界
    T pivot = v[left]; // 保存基准元素
    while (i < j) {
        while (i < j && v[j] >= pivot) { // 从右边开始找到第一个小于基准元素的元素
            --j;
        }
        if (i < j) { // 如果找到了，就将该元素放到左边
            v[i++] = v[j];
        }
        while (i < j && v[i] <= pivot) { // 从左边开始找到第一个大于基准元素的元素
            ++i;
        }
        if (i < j) { // 如果找到了，就将该元素放到右边
            v[j--] = v[i];
        }
    }
    v[i] = pivot;                // 将基准元素放到合适的位置
    quick_sort(v, left, i - 1);  // 递归排序左边部分
    quick_sort(v, i + 1, right); // 递归排序右边部分
}

// 堆排序
template<typename T>
void heap_sort(std::vector<T> &v)
{
    std::make_heap(v.begin(), v.end()); // 将数组转换为堆
    std::sort_heap(v.begin(), v.end()); // 将堆排序
}

// 希尔排序
template<typename T>
void shell_sort(std::vector<T> &v)
{
    for (int gap = v.size() / 2; gap > 0; gap /= 2) { // gap 为步长
        for (int i = gap; i < v.size(); ++i) { // 从 gap 开始，对每个元素进行插入排序
            T temp = v[i];                     // 保存当前元素
            int j = i - gap;                   // 从当前元素的前一个元素开始比较
            while (j >= 0 && v[j] > temp) { // 如果当前元素小于前一个元素，就将前一个元素后移
                v[j + gap] = v[j];
                j -= gap;
            }
            v[j + gap] = temp; // 将当前元素插入到合适的位置
        }
    }
}

// 冒泡排序
template<typename T>
void bubble_sort(std::vector<T> &v)
{
    for (int i = 0; i < v.size() - 1; ++i) {
        for (int j = 0; j < v.size() - i - 1; ++j) { // 从第一个元素开始比较
            if (v[j] > v[j + 1]) { // 如果当前元素大于后一个元素，就交换
                std::swap(v[j], v[j + 1]);
            }
        }
    }
}

// 计数排序
template<typename T>
void counting_sort(std::vector<T> &v)
{
    int max = *std::max_element(v.begin(), v.end()); // 找到最大值
    for (int exp = 1; max / exp > 0; exp *= 10) { // 从最低位开始，对每一位进行计数排序
        std::vector<T> temp(v.size());            // 临时数组
        std::vector<T> count(10);                 // 计数数组
        for (int i = 0; i < v.size(); ++i) {      // 统计每个元素出现的次数
            ++count[(v[i] / exp) % 10];
        }
        for (int i = 1; i < count.size(); ++i) { // 计算每个元素的位置
            count[i] += count[i - 1];
        }
        for (int i = v.size() - 1; i >= 0; --i) { // 将元素放到临时数组中
            temp[--count[(v[i] / exp) % 10]] = v[i];
        }
        for (int i = 0; i < v.size(); ++i) { // 将临时数组中的元素复制到原数组中
            v[i] = temp[i];
        }
    }
}

// 桶排序
template<typename T>
void bucket_sort(std::vector<T> &v)
{
    T max = *std::max_element(v.begin(), v.end()); // 找到最大值
    std::vector<std::vector<T>> temp(v.size());    // 临时数组
    for (int i = 0; i < v.size(); ++i) {           // 将元素放到临时数组中
        int index = v[i] * v.size() / (max + 1);   // 计算元素的位置
        temp[index].push_back(v[i]);
    }
    for (int i = 0; i < temp.size(); ++i) { // 对每个桶进行排序
        std::sort(temp[i].begin(), temp[i].end());
    }
    int index = 0;
    for (int i = 0; i < temp.size(); ++i) { // 将临时数组中的元素复制到原数组中
        for (int j = 0; j < temp[i].size(); ++j) {
            v[index++] = temp[i][j];
        }
    }
}
