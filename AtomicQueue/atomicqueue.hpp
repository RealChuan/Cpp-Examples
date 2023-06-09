#pragma once

#include <atomic>
#include <iostream>
#include <thread>
#include <vector>

template<typename T>
class AtomicQueue
{
public:
    AtomicQueue()
        : head_(new Node)
        , tail_(head_.load(std::memory_order_relaxed))
    {}
    ~AtomicQueue()
    {
        while (Node *const old_head = head_.load()) {
            head_.store(old_head->next);
            delete old_head;
        }
    }

    void push(const T &data)
    {
        Node *const new_node = new Node(data);           // 创建新节点
        Node *const old_tail = tail_.exchange(new_node); // 交换尾节点
        old_tail->next = new_node;                       // 原尾节点指向新尾节点
    }

    void pop()
    {
        Node *const old_head = head_.load(); // 获取头节点
        head_.store(old_head->next);         // 交换头节点
        delete old_head;                     // 删除原头节点
    }

    T &front() { return head_.load()->next->data; }

    T &back() { return tail_.load()->data; }

    bool empty() { return head_.load() == tail_.load(); }

    size_t size()
    {
        size_t size = 0;
        Node *node = head_.load();
        while (node != tail_.load()) {
            ++size;
            node = node->next;
        }
        return size;
    }

private:
    struct Node
    {
        Node()
            : next(nullptr)
        {}
        Node(const T &data)
            : data(data)
            , next(nullptr)
        {}
        T data;
        Node *next;
    };

    std::atomic<Node *> head_;
    std::atomic<Node *> tail_;
};