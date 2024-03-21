#include "linkedlist.hpp"

#include <gtest/gtest.h>

TEST(LinkedList, push_front)
{
    LinkedList<int> linked_list;
    linked_list.push_front(1);
    linked_list.push_front(2);
    linked_list.push_front(3);
    linked_list.push_front(4);
    linked_list.push_front(5);
    linked_list.print();
}

TEST(LinkedList, pop_front)
{
    LinkedList<int> linked_list;
    linked_list.push_front(1);
    linked_list.push_front(2);
    linked_list.push_front(3);
    linked_list.push_front(4);
    linked_list.push_front(5);
    linked_list.print();
    linked_list.pop_front();
    linked_list.print();
}

TEST(LinkedList, front)
{
    LinkedList<int> linked_list;
    linked_list.push_front(1);
    linked_list.push_front(2);
    linked_list.push_front(3);
    linked_list.push_front(4);
    linked_list.push_front(5);
    linked_list.print();
    std::cout << linked_list.front() << '\n';
}

TEST(LinkedList, empty)
{
    LinkedList<int> linked_list;
    EXPECT_TRUE(linked_list.empty());
    linked_list.push_front(1);
    EXPECT_FALSE(linked_list.empty());
}

TEST(LinkedList, size)
{
    LinkedList<int> linked_list;
    EXPECT_EQ(linked_list.size(), 0);
    linked_list.push_front(1);
    EXPECT_EQ(linked_list.size(), 1);
    linked_list.push_front(2);
    EXPECT_EQ(linked_list.size(), 2);
    linked_list.push_front(3);
    EXPECT_EQ(linked_list.size(), 3);
    linked_list.push_front(4);
    EXPECT_EQ(linked_list.size(), 4);
    linked_list.push_front(5);
    EXPECT_EQ(linked_list.size(), 5);
}

TEST(LinkedList, reverse)
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
}

TEST(LinkedList, print)
{
    LinkedList<int> linked_list;
    linked_list.push_front(1);
    linked_list.push_front(2);
    linked_list.push_front(3);
    linked_list.push_front(4);
    linked_list.push_front(5);
    linked_list.print();
}

auto main(int argc, char *argv[]) -> int
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
