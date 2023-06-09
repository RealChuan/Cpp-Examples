#include "binarytree.hpp"

#include <gtest/gtest.h>

TEST(BinaryTree, insert)
{
    BinaryTree<int> tree;
    tree.insert(5);
    tree.insert(3);
    tree.insert(7);
    tree.insert(2);
    tree.insert(4);
    tree.insert(6);
    tree.insert(8);
    EXPECT_EQ(tree, BinaryTree<int>({5, 3, 7, 2, 4, 6, 8}));
}

TEST(BinaryTree, remove)
{
    BinaryTree<int> tree({5, 3, 7, 2, 4, 6, 8});
    tree.remove(5);
    tree.remove(3);
    tree.remove(7);
    tree.remove(2);
    tree.remove(4);
    tree.remove(6);
    tree.remove(8);
    EXPECT_EQ(tree, BinaryTree<int>());
}

TEST(BinaryTree, contains)
{
    BinaryTree<int> tree({5, 3, 7, 2, 4, 6, 8});
    EXPECT_TRUE(tree.contains(5));
    EXPECT_TRUE(tree.contains(3));
    EXPECT_TRUE(tree.contains(7));
    EXPECT_TRUE(tree.contains(2));
    EXPECT_TRUE(tree.contains(4));
    EXPECT_TRUE(tree.contains(6));
    EXPECT_TRUE(tree.contains(8));
    EXPECT_FALSE(tree.contains(1));
    EXPECT_FALSE(tree.contains(9));
}

TEST(BinaryTree, clear)
{
    BinaryTree<int> tree({5, 3, 7, 2, 4, 6, 8});
    tree.clear();
    EXPECT_EQ(tree, BinaryTree<int>());
}

TEST(BinaryTree, preorderTraversal)
{
    BinaryTree<int> tree({5, 3, 7, 2, 4, 6, 8});
    testing::internal::CaptureStdout();
    tree.preorderTraversal();
    EXPECT_EQ(testing::internal::GetCapturedStdout(), "5 3 2 4 7 6 8 \n");
}

TEST(BinaryTree, inorderTraversal)
{
    BinaryTree<int> tree({5, 3, 7, 2, 4, 6, 8});
    testing::internal::CaptureStdout();
    tree.inorderTraversal();
    EXPECT_EQ(testing::internal::GetCapturedStdout(), "2 3 4 5 6 7 8 \n");
}

TEST(BinaryTree, postorderTraversal)
{
    BinaryTree<int> tree({5, 3, 7, 2, 4, 6, 8});
    testing::internal::CaptureStdout();
    tree.postorderTraversal();
    EXPECT_EQ(testing::internal::GetCapturedStdout(), "2 4 3 6 8 7 5 \n");
}

TEST(BinaryTree, levelorderTraversal)
{
    BinaryTree<int> tree({5, 3, 7, 2, 4, 6, 8});
    testing::internal::CaptureStdout();
    tree.levelorderTraversal();
    EXPECT_EQ(testing::internal::GetCapturedStdout(), "5 3 7 2 4 6 8 \n");
}

TEST(BinaryTree, operatorOut)
{
    BinaryTree<int> tree({5, 3, 7, 2, 4, 6, 8});
    testing::internal::CaptureStdout();
    std::cout << tree;
    EXPECT_EQ(testing::internal::GetCapturedStdout(), "5 3 2 4 7 6 8 \n");
}

TEST(BinaryTree, operatorEqual)
{
    BinaryTree<int> tree1({5, 3, 7, 2, 4, 6, 8});
    BinaryTree<int> tree2({5, 3, 7, 2, 4, 6, 8});
    EXPECT_TRUE(tree1 == tree2);
}

TEST(BinaryTree, operatorNotEqual)
{
    BinaryTree<int> tree1({5, 3, 7, 2, 4, 6, 8});
    BinaryTree<int> tree2({5, 3, 7, 2, 4, 6, 8});
    EXPECT_FALSE(tree1 != tree2);
}

TEST(BinaryTree, operatorLess)
{
    BinaryTree<int> tree1({5, 3, 7, 2, 4, 6, 8});
    BinaryTree<int> tree2({5, 3, 7, 2, 4, 6, 8});
    EXPECT_FALSE(tree1 < tree2);
}

TEST(BinaryTree, operatorLessEqual)
{
    BinaryTree<int> tree1({5, 3, 7, 2, 4, 6, 8});
    BinaryTree<int> tree2({5, 3, 7, 2, 4, 6, 8});
    EXPECT_TRUE(tree1 <= tree2);
}

TEST(BinaryTree, operatorGreater)
{
    BinaryTree<int> tree1({5, 3, 7, 2, 4, 6, 8});
    BinaryTree<int> tree2({5, 3, 7, 2, 4, 6, 8});
    EXPECT_FALSE(tree1 > tree2);
}

TEST(BinaryTree, operatorGreaterEqual)
{
    BinaryTree<int> tree1({5, 3, 7, 2, 4, 6, 8});
    BinaryTree<int> tree2({5, 3, 7, 2, 4, 6, 8});
    EXPECT_TRUE(tree1 >= tree2);
}

void testPrint()
{
    BinaryTree<int> tree({5, 3, 7, 2, 4, 6, 8});
    std::cout << "preorderTraversal: ";
    tree.preorderTraversal();
    std::cout << "inorderTraversal: ";
    tree.inorderTraversal();
    std::cout << "postorderTraversal: ";
    tree.postorderTraversal();
    std::cout << "levelorderTraversal: ";
    tree.levelorderTraversal();
    std::cout << "operator<<: ";
    std::cout << tree;
}

int main(int argc, char *argv[])
{
    testPrint();

    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
