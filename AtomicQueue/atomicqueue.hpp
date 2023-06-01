#pragma once

#include <atomic>
#include <iostream>
#include <thread>
#include <vector>

using namespace std;

class Node
{
public:
    int value;                //结点存储的值
    std::atomic<Node *> next; //结点指向的下一个结点，使用原子指针
    Node(int v)
        : value(v)
        , next(nullptr)
    {} //构造函数
};

class AtomicQueue
{
public:
    AtomicQueue()
        : head_(new Node(0))
        , tail_(head_.load())
    {} //构造函数

    ~AtomicQueue()
    {
        int data;
        while (pop(data))
            ;
        delete head_.load();
    } //析构函数

    void push(int data)
    {
        Node *node = new Node(data);        //创建新结点
        Node *tail = tail_.load();          //获取尾结点
        while (true) {
            Node *next = tail->next.load(); //获取尾结点的下一个结点
            if (next == nullptr) {          //如果尾结点的下一个结点为空
                if (tail->next.compare_exchange_weak(next,
                                                     node)) { //尝试将尾结点的下一个结点设置为新结点
                    break;                                    //设置成功，退出循环
                }
            } else { //如果尾结点的下一个结点不为空
                tail_.compare_exchange_weak(tail, next); //尝试将尾结点设置为尾结点的下一个结点
            }
            tail = tail_.load();                         //获取新的尾结点
        }
        tail_.compare_exchange_weak(tail, node); //尝试将尾结点设置为新结点
    }

    bool pop(int &data)
    {
        Node *head = head_.load();                       //获取头结点
        while (true) {
            Node *tail = tail_.load();                   //获取尾结点
            Node *next = head->next.load();              //获取头结点的下一个结点
            if (head == tail) {                          //如果头结点和尾结点相同
                if (next == nullptr) {                   //如果头结点的下一个结点为空
                    return false;                        //队列为空，返回false
                }
                tail_.compare_exchange_weak(tail, next); //尝试将尾结点设置为尾结点的下一个结点
            } else {                                     //如果头结点和尾结点不相同
                data = next->value;                      //获取头结点的下一个结点的值
                if (head_.compare_exchange_weak(head, next)) { //尝试将头结点设置为头结点的下一个结点
                    break;                                     //设置成功，退出循环
                }
            }
            head = head_.load(); //获取新的头结点
        }
        delete head;             //删除头结点
        return true;             //返回true
    }

private:
    std::atomic<Node *> head_; //头结点，使用原子指针
    std::atomic<Node *> tail_; //尾结点，使用原子指针
};
