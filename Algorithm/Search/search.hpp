#pragma once

#include <algorithm>
#include <iostream>
#include <iterator>
#include <random>
#include <vector>

// 顺序查找
template<typename T>
auto sequential_search(const std::vector<T> &v, const T &value) -> int
{
    for (int i = 0; i < v.size(); ++i) { // 从头到尾遍历
        if (v[i] == value) {             // 如果找到了，就返回下标
            return i;
        }
    }
    return -1; // not found
}

// 二分查找
template<typename T>
auto binary_search(const std::vector<T> &v, const T &value) -> int
{
    int low = 0;                    // 最小下标
    int high = v.size() - 1;        // 最大下标
    while (low <= high) {           // 当最小下标小于等于最大下标时
        int mid = (low + high) / 2; // 取中间下标
        if (v[mid] == value) {      // 如果中间元素等于要查找的元素，就返回下标
            return mid;
        } else if (v[mid] < value) { // 如果中间元素小于要查找的元素，就在右半部分查找
            low = mid + 1;
        } else { // 如果中间元素大于要查找的元素，就在左半部分查找
            high = mid - 1;
        }
    }
    return -1; // not found
}

// 斐波那契查找
template<typename T>
auto fibonacci_search(const std::vector<T> &v, const T &value) -> int
{
    std::vector<int> fibonacci{1, 1};     // 斐波那契数列
    while (fibonacci.back() < v.size()) { // 生成斐波那契数列
        fibonacci.push_back(fibonacci[fibonacci.size() - 1] + fibonacci[fibonacci.size() - 2]);
    }
    int low = 0;                              // 最小下标
    int high = v.size() - 1;                  // 最大下标
    int k = fibonacci.size() - 1;             // 斐波那契数列的下标
    while (low <= high) {                     // 当最小下标小于等于最大下标时
        int mid = low + fibonacci[k - 1] - 1; // 取黄金分割点
        if (mid >= v.size()) {                // 如果黄金分割点大于等于数组长度
            mid = v.size() - 1; // 就将黄金分割点设置为数组最后一个元素的下标
        }
        if (v[mid] == value) {  // 如果中间元素等于要查找的元素，就返回下标
            return mid;
        } else if (v[mid] < value) { // 如果中间元素小于要查找的元素，就在右半部分查找
            low = mid + 1;
            k -= 2;
        } else {            // 如果中间元素大于要查找的元素，就在左半部分查找
            high = mid - 1; // 最大下标为黄金分割点的前一个元素
            k -= 1;
        }
    }
    return -1; // not found
}

// 线性索引查找
template<typename T>
auto linear_index_search(const std::vector<T> &v, const T &value) -> int
{
    std::vector<T> index;                // 索引
    index.push_back(v[0]);               // 第一个索引为第一个元素
    for (int i = 1; i < v.size(); ++i) { // 生成索引
        if (v[i] > index.back()) {       // 如果当前元素大于索引的最后一个元素
            index.push_back(v[i]);
        }
    }
    int low = 0;                    // 最小下标
    int high = index.size() - 1;    // 最大下标
    while (low <= high) {           // 当最小下标小于等于最大下标时
        int mid = (low + high) / 2; // 取中间下标
        if (index[mid] == value) {  // 如果中间元素等于要查找的元素，就返回下标
            return mid;
        } else if (index[mid] < value) { // 如果中间元素小于要查找的元素，就在右半部分查找
            low = mid + 1;
        } else {
            high = mid - 1; // 如果中间元素大于要查找的元素，就在左半部分查找
        }
    }
    return -1; // not found
}

// KMP查找
template<typename T>
auto kmp_search(const std::vector<T> &v, const std::vector<T> &pattern) -> int
{
    std::vector<int> next(pattern.size());     // next数组
    next[0] = -1;                              // next数组的第一个元素为-1
    int k = -1;                                // next数组的下标
    for (int i = 1; i < pattern.size(); ++i) { // 生成next数组
        while (k > -1 && pattern[k + 1] != pattern[i]) { // 如果k大于-1且下一个元素不等于当前元素
            k = next[k];
        }
        if (pattern[k + 1] == pattern[i]) { // 如果下一个元素等于当前元素
            ++k;
        }
        next[i] = k;                               // 将k赋值给next数组的当前元素
    }
    k = -1;                                        // next数组的下标
    for (int i = 0; i < v.size(); ++i) {
        while (k > -1 && pattern[k + 1] != v[i]) { // 如果k大于-1且下一个元素不等于当前元素
            k = next[k];
        }
        if (pattern[k + 1] == v[i]) { // 如果下一个元素等于当前元素
            ++k;
        }
        if (k == pattern.size() - 1) { // 如果k等于模式串的最后一个元素的下标
            return i - k;
        }
    }
    return -1; // not found
}

auto move_by_suffix(int j, const std::vector<int> &suffix, const std::vector<bool> &prefix) -> int
{
    int k = suffix.size() - 1 - j; // 好后缀长度
    if (suffix[k] != -1) {         // 如果好后缀在模式串中存在
        return j - suffix[k] + 1;
    }
    for (int r = j + 2; r <= suffix.size() - 1; ++r) {
        // 如果好后缀的后缀子串在模式串的前缀子串中存在，就将模式串向后移动到好后缀的后缀子串与模式串的前缀子串相等的位置
        if (prefix[suffix.size() - r]) {
            return r; // 好后缀的后缀子串的长度
        }
    }
    return suffix.size();
}
// BM查找
template<typename T>
auto bm_search(const std::vector<T> &v, const std::vector<T> &pattern) -> int
{
    std::vector<int> bc(256, -1);              // 坏字符哈希表
    for (int i = 0; i < pattern.size(); ++i) { // 生成坏字符哈希表
        bc[pattern[i]] = i;
    }

    std::vector<int> suffix(pattern.size(), -1); // 好后缀
    std::vector<bool> prefix(pattern.size(), false); // 好后缀的前缀子串是否在模式串的前缀子串中存在
    for (int i = 0; i < pattern.size() - 1; ++i) {
        int j = i;
        int k = 0;
        while (j >= 0 && pattern[j] == pattern[pattern.size() - 1 - k]) { // 生成好后缀
            --j;
            ++k;
            suffix[k] = j + 1;
        }
        if (j == -1) { // 如果好后缀的前缀子串在模式串的前缀子串中存在
            prefix[k] = true;
        }
    }
    int i = 0;
    while (i <= v.size() - pattern.size()) {       // 从头到尾遍历
        int j = pattern.size() - 1;
        while (j >= 0 && v[i + j] == pattern[j]) { // 从后往前比较
            --j;
        }
        if (j == -1) { // 如果模式串的所有元素都匹配了
            return i;
        }
        i += std::max(move_by_suffix(j, suffix, prefix), j - bc[v[i + j]]); // 将模式串向后移动
    }
    return -1;                                                              // not found
}

// Sunday查找
template<typename T>
auto sunday_search(const std::vector<T> &v, const std::vector<T> &pattern) -> int
{
    std::vector<int> bc(256, -1);              // 坏字符哈希表
    for (int i = 0; i < pattern.size(); ++i) { // 生成坏字符哈希表
        bc[pattern[i]] = i;
    }
    int i = 0;
    while (i <= v.size() - pattern.size()) {                   // 从头到尾遍历
        int j = 0;
        while (j < pattern.size() && v[i + j] == pattern[j]) { // 从前往后比较
            ++j;
        }
        if (j == pattern.size()) { // 如果模式串的所有元素都匹配了
            return i;
        }
        if (i + pattern.size() == v.size()) { // 如果模式串的最后一个元素在主串中不存在
            return -1;                        // not found
        }
        i += pattern.size() - bc[v[i + pattern.size()]]; // 将模式串向后移动
    }
    return -1;                                           // not found
}

// Rabin-Karp查找
template<typename T>
auto rabin_karp_search(const std::vector<T> &v, const std::vector<T> &pattern) -> int
{
    int pattern_hash = 0; // 模式串的哈希值
    for (int i = 0; i < pattern.size(); ++i) {
        pattern_hash += pattern[i];
    }
    int v_hash = 0; // 主串的哈希值
    for (int i = 0; i < pattern.size(); ++i) {
        v_hash += v[i];
    }
    int i = 0;
    while (i <= v.size() - pattern.size()) { // 从头到尾遍历
        if (v_hash == pattern_hash) {        // 如果模式串的哈希值等于主串的哈希值
            int j = 0;
            while (j < pattern.size() && v[i + j] == pattern[j]) { // 从前往后比较
                ++j;
            }
            if (j == pattern.size()) { // 如果模式串的所有元素都匹配了
                return i;
            }
        }
        if (i + pattern.size() == v.size()) { // 如果模式串的最后一个元素在主串中不存在
            return -1;                        // not found
        }
        v_hash += v[i + pattern.size()] - v[i]; // 将模式串向后移动
        ++i;
    }
    return -1; // not found
}
