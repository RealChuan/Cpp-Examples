#pragma once

#include <iostream>
#include <stack>
#include <vector>

template<typename T>
class LinkedList
{
public:
    LinkedList()
        : m_head(nullptr)
    {}
    ~LinkedList()
    {
        while (m_head) {
            pop_front();
        }
    }

    void push_front(const T &data)
    {
        Node *const new_node = new Node(data);
        new_node->next = m_head;
        m_head = new_node;
    }

    void pop_front()
    {
        Node *const old_head = m_head;
        m_head = m_head->next;
        delete old_head;
    }

    auto front() -> T & { return m_head->data; }

    auto empty() -> bool { return m_head == nullptr; }

    auto size() -> size_t
    {
        size_t size = 0;
        Node *node = m_head;
        while (node) {
            ++size;
            node = node->next;
        }
        return size;
    }

    void reverse()
    {
        Node *prev = nullptr; // 前一个节点
        Node *curr = m_head;  // 当前节点
        while (curr) {
            Node *next = curr->next; // 下一个节点
            curr->next = prev;       // 当前节点指向前一个节点
            prev = curr;             // 前一个节点指向当前节点
            curr = next;             // 当前节点指向下一个节点
        }
        m_head = prev; // 头节点指向前一个节点
    }

    void print()
    {
        Node *node = m_head;
        while (node) {
            std::cout << node->data << " ";
            node = node->next;
        }
        std::cout << '\n';
    }

private:
    struct Node
    {
        explicit Node(const T &data)
            : data(data)
            , next(nullptr)
        {}

        T data;
        Node *next;
    };

    Node *m_head;
};
