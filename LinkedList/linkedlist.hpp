#pragma once

#include <iostream>
#include <stack>
#include <vector>

using namespace std;

// 链表节点
struct ListNode
{
    int val;
    ListNode *next;
    ListNode(int x)
        : val(x)
        , next(NULL)
    {}
};

// 链表类
class LinkedList
{
public:
    // 构造函数
    LinkedList()
        : head(NULL)
    {}
    LinkedList(vector<int> &nums)
    {
        head = NULL;
        ListNode *p = NULL;
        for (int i = 0; i < nums.size(); i++) {
            if (head == NULL) {
                head = new ListNode(nums[i]);
                p = head;
            } else {
                p->next = new ListNode(nums[i]);
                p = p->next;
            }
        }
    }
    // 析构函数
    ~LinkedList()
    {
        ListNode *p = head;
        while (p != NULL) {
            ListNode *q = p->next;
            delete p;
            p = q;
        }
    }
    // 打印链表
    void print()
    {
        ListNode *p = head;
        while (p != NULL) {
            cout << p->val << " ";
            p = p->next;
        }
        cout << endl;
    }
    // 反转链表
    void reverse()
    {
        ListNode *p = head;
        ListNode *q = NULL;
        while (p != NULL) {
            ListNode *r = p->next;
            p->next = q;
            q = p;
            p = r;
        }
        head = q;
    }
    // 删除链表中的某个节点
    void remove(ListNode *node)
    {
        if (node == NULL)
            return;
        if (node->next == NULL) {
            delete node;
            node = NULL;
            return;
        }
        ListNode *p = node->next;
        node->val = p->val;
        node->next = p->next;
        delete p;
        p = NULL;
    }
    // 插入节点
    void insert(ListNode *node, int val)
    {
        if (node == NULL)
            return;
        ListNode *p = new ListNode(val);
        p->next = node->next;
        node->next = p;
    }
    // 获取链表头节点
    ListNode *getHead() { return head; }
    // 获取链表长度
    int getLength()
    {
        int len = 0;
        ListNode *p = head;
        while (p != NULL) {
            len++;
            p = p->next;
        }
        return len;
    }
    // 获取链表中
    ListNode *getMid()
    {
        ListNode *p = head;
        ListNode *q = head;
        while (q != NULL && q->next != NULL) {
            p = p->next;
            q = q->next->next;
        }
        return p;
    }
    // 从尾到头打印链表
    void printReverse()
    {
        stack<int> s;
        ListNode *p = head;
        while (p != NULL) {
            s.push(p->val);
            p = p->next;
        }
        while (!s.empty()) {
            cout << s.top() << " ";
            s.pop();
        }
        cout << endl;
    }

private:
    ListNode *head;
};