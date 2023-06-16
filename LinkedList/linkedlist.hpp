#pragma once

#include <iostream>
#include <stack>
#include <vector>

template<typename T>
class LinkedList
{
public:
    LinkedList()
        : head_(nullptr)
    {}
    ~LinkedList()
    {
        while (head_) {
            pop_front();
        }
    }

    void push_front(const T &data)
    {
        Node *const new_node = new Node(data);
        new_node->next = head_;
        head_ = new_node;
    }

    void pop_front()
    {
        Node *const old_head = head_;
        head_ = head_->next;
        delete old_head;
    }

    auto front() -> T & { return head_->data; }

    auto empty() -> bool { return head_ == nullptr; }

    size_t size()
    {
        size_t size = 0;
        Node *node = head_;
        while (node) {
            ++size;
            node = node->next;
        }
        return size;
    }

    void reverse()
    {
        Node *prev = nullptr;        // 前一个节点
        Node *curr = head_;          // 当前节点
        while (curr) {
            Node *next = curr->next; // 下一个节点
            curr->next = prev;       // 当前节点指向前一个节点
            prev = curr;             // 前一个节点指向当前节点
            curr = next;             // 当前节点指向下一个节点
        }
        head_ = prev;                // 头节点指向前一个节点
    }

    void print()
    {
        Node *node = head_;
        while (node) {
            std::cout << node->data << " ";
            node = node->next;
        }
        std::cout << std::endl;
    }

private:
    struct Node
    {
        Node(const T &data)
            : data(data)
            , next(nullptr)
        {}

        T data;
        Node *next;
    };

    Node *head_;
};
