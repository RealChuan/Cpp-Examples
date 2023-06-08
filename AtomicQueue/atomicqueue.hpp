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
        Node *const new_node = new Node(data);
        Node *const old_tail = tail_.exchange(new_node);
        old_tail->next = new_node;
    }

    void pop()
    {
        Node *const old_head = head_.load();
        head_.store(old_head->next);
        delete old_head;
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