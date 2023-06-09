#include "linkedlist.hpp"

int main()
{
    LinkedList<int> linked_list;
    linked_list.push_front(1);
    linked_list.push_front(2);
    linked_list.push_front(3);
    linked_list.push_front(4);
    linked_list.push_front(5);
    linked_list.print();
    linked_list.reverse();
    linked_list.print();
    return 0;
}