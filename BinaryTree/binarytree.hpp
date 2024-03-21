#pragma once

#include <iostream>
#include <vector>

template<typename T>
class BinaryTree
{
public:
    BinaryTree() = default;
    explicit BinaryTree(const std::vector<T> &data);
    BinaryTree(const BinaryTree &other);
    BinaryTree(BinaryTree &&other) noexcept;
    auto operator=(const BinaryTree &other) -> BinaryTree &;
    auto operator=(BinaryTree &&other) noexcept -> BinaryTree &;
    ~BinaryTree();

    auto operator==(const BinaryTree &other) const -> bool;
    auto operator!=(const BinaryTree &other) const -> bool;
    auto operator<(const BinaryTree &other) const -> bool;
    auto operator>(const BinaryTree &other) const -> bool;
    auto operator<=(const BinaryTree &other) const -> bool;
    auto operator>=(const BinaryTree &other) const -> bool;

    void insert(const T &data);
    void insert(T &&data) noexcept;
    void remove(const T &data);
    auto contains(const T &data) const -> bool;
    void clear();
    void preorderTraversal() const;
    void inorderTraversal() const;
    void postorderTraversal() const;
    void levelorderTraversal() const;

private:
    struct Node
    {
        T data;
        Node *left;
        Node *right;

        explicit Node(const T &data);
        explicit Node(T &&data) noexcept;
    };

    Node *root = nullptr;
};

template<typename T>
BinaryTree<T>::BinaryTree(const std::vector<T> &data)
{
    for (const auto &item : data) {
        insert(item);
    }
}

template<typename T>
BinaryTree<T>::BinaryTree(const BinaryTree &other)
{
    if (other.root) {
        root = new Node(other.root->data);
        root->left = other.root->left;
        root->right = other.root->right;
    }
}

template<typename T>
BinaryTree<T>::BinaryTree(BinaryTree &&other) noexcept
{
    root = other.root;
    other.root = nullptr;
}

template<typename T>
auto BinaryTree<T>::operator=(const BinaryTree &other) -> BinaryTree<T> &
{
    if (this != &other) {
        clear();
        if (other.root) {
            root = new Node(other.root->data);
            root->left = other.root->left;
            root->right = other.root->right;
        }
    }
    return *this;
}

template<typename T>
auto BinaryTree<T>::operator=(BinaryTree &&other) noexcept -> BinaryTree<T> &
{
    if (this != &other) {
        clear();
        root = other.root;
        other.root = nullptr;
    }
    return *this;
}

template<typename T>
BinaryTree<T>::~BinaryTree()
{
    clear();
}

template<typename T>
auto BinaryTree<T>::operator==(const BinaryTree &other) const -> bool
{
    if (root == nullptr && other.root == nullptr) {
        return true;
    }
    if (root == nullptr || other.root == nullptr) {
        return false;
    }
    std::vector<typename BinaryTree<T>::Node *> nodes1;
    std::vector<typename BinaryTree<T>::Node *> nodes2;
    nodes1.push_back(root);
    nodes2.push_back(other.root);
    while (!nodes1.empty() && !nodes2.empty()) {
        typename BinaryTree<T>::Node *current1 = nodes1.back();
        nodes1.pop_back();
        typename BinaryTree<T>::Node *current2 = nodes2.back();
        nodes2.pop_back();
        if (current1->data != current2->data) { // 比较当前节点的值
            return false;
        }
        if (current1->left && current2->left) { // 比较左子节点的值
            nodes1.push_back(current1->left);
            nodes2.push_back(current2->left);
        } else if (current1->left || current2->left) {
            // 如果一个有左子节点，一个没有左子节点，就返回false
            return false;
        }
        if (current1->right && current2->right) { // 比较右子节点的值
            nodes1.push_back(current1->right);
            nodes2.push_back(current2->right);
        } else if (current1->right || current2->right) {
            // 如果一个有右子节点，一个没有右子节点，就返回false
            return false;
        }
    }

    return true;
}

template<typename T>
auto BinaryTree<T>::operator!=(const BinaryTree &other) const -> bool
{
    return !(*this == other);
}

template<typename T>
auto BinaryTree<T>::operator<(const BinaryTree &other) const -> bool
{
    if (root == nullptr && other.root == nullptr) {
        return false;
    }
    if (root == nullptr) {
        return true;
    }
    if (other.root == nullptr) {
        return false;
    }
    std::vector<typename BinaryTree<T>::Node *> nodes1;
    std::vector<typename BinaryTree<T>::Node *> nodes2;
    nodes1.push_back(root);
    nodes2.push_back(other.root);
    while (!nodes1.empty() && !nodes2.empty()) {
        typename BinaryTree<T>::Node *current1 = nodes1.back();
        nodes1.pop_back();
        typename BinaryTree<T>::Node *current2 = nodes2.back();
        nodes2.pop_back();
        if (current1->data < current2->data) { // 比较当前节点的值
            return true;
        }
        if (current1->data > current2->data) { // 比较当前节点的值
            return false;
        }
        if (current1->left && current2->left) { // 比较左子节点的值
            nodes1.push_back(current1->left);
            nodes2.push_back(current2->left);
        } else if (current1->left || current2->left) {
            // 如果一个有左子节点，一个没有左子节点，就返回false
            return false;
        }
        if (current1->right && current2->right) {
            // 比较右子节点的值
            nodes1.push_back(current1->right);
            nodes2.push_back(current2->right);
        } else if (current1->right || current2->right) {
            // 如果一个有右子节点，一个没有右子节点，就返回false
            return false;
        }
    }

    return false;
}

template<typename T>
auto BinaryTree<T>::operator>(const BinaryTree &other) const -> bool
{
    if (root == nullptr && other.root == nullptr) {
        return false;
    }
    if (root == nullptr) {
        return false;
    }
    if (other.root == nullptr) {
        return true;
    }
    std::vector<typename BinaryTree<T>::Node *> nodes1;
    std::vector<typename BinaryTree<T>::Node *> nodes2;
    nodes1.push_back(root);
    nodes2.push_back(other.root);
    while (!nodes1.empty() && !nodes2.empty()) {
        typename BinaryTree<T>::Node *current1 = nodes1.back();
        nodes1.pop_back();
        typename BinaryTree<T>::Node *current2 = nodes2.back();
        nodes2.pop_back();
        if (current1->data > current2->data) { // 比较当前节点的值
            return true;
        }
        if (current1->data < current2->data) { // 比较当前节点的值
            return false;
        }
        if (current1->left && current2->left) { // 比较左子节点的值
            nodes1.push_back(current1->left);
            nodes2.push_back(current2->left);
        } else if (current1->left || current2->left) {
            // 如果一个有左子节点，一个没有左子节点，就返回false
            return false;
        }
        if (current1->right && current2->right) { // 比较右子节点的值
            nodes1.push_back(current1->right);
            nodes2.push_back(current2->right);
        } else if (current1->right || current2->right) {
            // 如果一个有右子节点，一个没有右子节点，就返回false
            return false;
        }
    }

    return false;
}

template<typename T>
auto BinaryTree<T>::operator<=(const BinaryTree &other) const -> bool
{
    return !(*this > other);
}

template<typename T>
auto BinaryTree<T>::operator>=(const BinaryTree &other) const -> bool
{
    return !(*this < other);
}

template<typename T>
void BinaryTree<T>::insert(const T &data)
{
    if (!root) {
        root = new Node(data);
        return;
    }

    Node *current = root;
    while (true) {
        if (data < current->data) {
            if (current->left) {
                current = current->left;
            } else {
                current->left = new Node(data);
                break;
            }
        } else if (data > current->data) {
            if (current->right) {
                current = current->right;
            } else {
                current->right = new Node(data);
                break;
            }
        } else {
            break;
        }
    }
}

template<typename T>
void BinaryTree<T>::insert(T &&data) noexcept
{
    if (!root) {
        root = new Node(std::move(data));
        return;
    }

    Node *current = root;
    while (true) {
        if (data < current->data) {
            if (current->left) {
                current = current->left;
            } else {
                current->left = new Node(std::move(data));
                break;
            }
        } else if (data > current->data) {
            if (current->right) {
                current = current->right;
            } else {
                current->right = new Node(std::move(data));
                break;
            }
        } else {
            break;
        }
    }
}

template<typename T>
void BinaryTree<T>::remove(const T &data)
{
    if (!root) {
        return;
    }

    Node *current = root;
    Node *parent = nullptr;
    while (true) {
        if (data < current->data) {
            if (current->left) {
                parent = current;
                current = current->left;
            } else {
                return;
            }
        } else if (data > current->data) {
            if (current->right) {
                parent = current;
                current = current->right;
            } else {
                return;
            }
        } else {
            break;
        }
    }

    if (current->left && current->right) {
        Node *min = current->right;
        Node *minParent = current;
        while (min->left) {
            minParent = min;
            min = min->left;
        }
        current->data = std::move(min->data);
        current = min;
        parent = minParent;
    }

    Node *child = nullptr;
    if (current->left) {
        child = current->left;
    } else if (current->right) {
        child = current->right;
    }

    if (!parent) {
        root = child;
    } else if (parent->left == current) {
        parent->left = child;
    } else {
        parent->right = child;
    }

    delete current;
}

template<typename T>
auto BinaryTree<T>::contains(const T &data) const -> bool
{
    Node *current = root;
    while (current) {
        if (data < current->data) {
            current = current->left;
        } else if (data > current->data) {
            current = current->right;
        } else {
            return true;
        }
    }
    return false;
}

template<typename T>
void BinaryTree<T>::clear()
{
    if (!root) {
        return;
    }

    std::vector<Node *> stack;
    stack.push_back(root);
    while (!stack.empty()) {
        Node *current = stack.back();
        stack.pop_back();
        if (current->left) {
            stack.push_back(current->left);
        }
        if (current->right) {
            stack.push_back(current->right);
        }
        delete current;
    }
    root = nullptr;
}

template<typename T>
void BinaryTree<T>::preorderTraversal() const
{
    if (!root) {
        return;
    }

    std::vector<Node *> stack;
    stack.push_back(root);
    while (!stack.empty()) {
        Node *current = stack.back();
        stack.pop_back();
        std::cout << current->data << " ";
        if (current->right) {
            stack.push_back(current->right);
        }
        if (current->left) {
            stack.push_back(current->left);
        }
    }
    std::cout << '\n';
}

template<typename T>
void BinaryTree<T>::inorderTraversal() const
{
    if (!root) {
        return;
    }

    std::vector<Node *> stack;
    Node *current = root;
    while (true) {
        while (current) {
            stack.push_back(current);
            current = current->left;
        }
        if (stack.empty()) {
            break;
        }
        current = stack.back();
        stack.pop_back();
        std::cout << current->data << " ";
        current = current->right;
    }
    std::cout << '\n';
}

template<typename T>
void BinaryTree<T>::postorderTraversal() const
{
    if (!root) {
        return;
    }

    std::vector<Node *> stack;
    Node *current = root;
    Node *lastVisited = nullptr;
    while (true) {
        while (current) {
            stack.push_back(current);
            current = current->left;
        }
        if (stack.empty()) {
            break;
        }
        current = stack.back();
        if (current->right && current->right != lastVisited) {
            current = current->right;
        } else {
            std::cout << current->data << " ";
            stack.pop_back();
            lastVisited = current;
            current = nullptr;
        }
    }
    std::cout << '\n';
}

template<typename T>
void BinaryTree<T>::levelorderTraversal() const
{
    if (!root) {
        return;
    }

    std::vector<Node *> queue;
    queue.push_back(root);
    while (!queue.empty()) {
        Node *current = queue.front();
        queue.erase(queue.begin());
        std::cout << current->data << " ";
        if (current->left) {
            queue.push_back(current->left);
        }
        if (current->right) {
            queue.push_back(current->right);
        }
    }
    std::cout << '\n';
}

template<typename T>
BinaryTree<T>::Node::Node(const T &data)
{
    this->data = data;
    left = nullptr;
    right = nullptr;
}

template<typename T>
BinaryTree<T>::Node::Node(T &&data) noexcept
{
    this->data = std::move(data);
    left = nullptr;
    right = nullptr;
}

template<typename T>
auto operator<<(std::ostream &os, const BinaryTree<T> &tree) -> std::ostream &
{
    tree.preorderTraversal();
    return os;
}
