#include "linkedlist.hpp"

int main()
{
    vector<int> nums = {1, 2, 3, 4, 5};
    LinkedList list(nums);
    list.print();
    list.reverse();
    list.print();

    // remove node
    ListNode *p = list.getHead()->next;
    list.remove(p);
    list.print();

    // insert node
    ListNode *q = list.getHead()->next;
    list.insert(q, 6);
    list.print();

    return 0;
}