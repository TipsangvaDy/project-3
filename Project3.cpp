// CS 2414 -- Data Structures -- Fall 2026 -- Project 3
// Medical Index (MedIndex)
//
// This starter file implements a hospital emergency-department patient index.
// Admitted patients are stored in an AVL tree keyed by patient ID.
// Patients who have checked in but are not yet admitted wait in a queue.
// A stack records recent successful operations so the last action can be undone.
//
// The AVL tree class is complete. Implement the rest of the program
// according to the project handout.

#include <iostream>
#include <string>
#include <queue>
#include <stack>
#include <deque>
using namespace std;

class Patient {
public:
    int id;
    string name;
    int acuity;

    Patient() : id(0), name(""), acuity(0) {}
    Patient(int i, const string& n, int a) : id(i), name(n), acuity(a) {}

    bool operator<(const Patient& other) const { return id < other.id; }
    bool operator>(const Patient& other) const { return id > other.id; }
    bool operator==(const Patient& other) const { return id == other.id; }
};

template <typename T>
class Node {
public:
    T data;
    Node* left;
    Node* right;
    int height;
    Node(T val) : data(val), left(nullptr), right(nullptr), height(0) {}
};

template <typename T>
class AVLTree {
    Node<T>* root;

    void destroy(Node<T>* node);
    void inorder(Node<T>* node) const;

    int height(Node<T>* node) const;
    void updateHeight(Node<T>* node);
    int balance(Node<T>* node) const;

    Node<T>* rotateRight(Node<T>* z);
    Node<T>* rotateLeft(Node<T>* z);

    Node<T>* insert(Node<T>* node, T key);
    Node<T>* remove(Node<T>* node, T key);
    Node<T>* minNode(Node<T>* node) const;

public:
    AVLTree() : root(nullptr) {}
    ~AVLTree() { destroy(root); }

    void insert(T key);
    void remove(T key);
    Node<T>* search(T key) const;
    void printInorder() const;
    int treeHeight() const { return height(root); }
    bool empty() const { return root == nullptr; }
};

template <typename T>
void AVLTree<T>::destroy(Node<T>* node) {
    if (node == nullptr) return;
    destroy(node->left);
    destroy(node->right);
    delete node;
}

template <typename T>
int AVLTree<T>::height(Node<T>* node) const {
    if (node == nullptr) return -1;
    return node->height;
}

template <typename T>
void AVLTree<T>::updateHeight(Node<T>* node) {
    if (node == nullptr) return;
    int hl = height(node->left);
    int hr = height(node->right);
    node->height = 1 + (hl > hr ? hl : hr);
}

template <typename T>
int AVLTree<T>::balance(Node<T>* node) const {
    if (node == nullptr) return 0;
    return height(node->left) - height(node->right);
}

template <typename T>
Node<T>* AVLTree<T>::rotateRight(Node<T>* z) {
    Node<T>* y = z->left;
    Node<T>* t = y->right;
    y->right = z;
    z->left = t;
    updateHeight(z);
    updateHeight(y);
    return y;
}

template <typename T>
Node<T>* AVLTree<T>::rotateLeft(Node<T>* z) {
    Node<T>* y = z->right;
    Node<T>* t = y->left;
    y->left = z;
    z->right = t;
    updateHeight(z);
    updateHeight(y);
    return y;
}

template <typename T>
Node<T>* AVLTree<T>::insert(Node<T>* node, T key) {
    if (node == nullptr) return new Node<T>(key);
    if (key < node->data)
        node->left = insert(node->left, key);
    else if (key > node->data)
        node->right = insert(node->right, key);
    else
        return node;

    updateHeight(node);
    int bf = balance(node);

    if (bf > 1 && key < node->left->data)
        return rotateRight(node);
    if (bf < -1 && key > node->right->data)
        return rotateLeft(node);
    if (bf > 1 && key > node->left->data) {
        node->left = rotateLeft(node->left);
        return rotateRight(node);
    }
    if (bf < -1 && key < node->right->data) {
        node->right = rotateRight(node->right);
        return rotateLeft(node);
    }
    return node;
}

template <typename T>
void AVLTree<T>::insert(T key) {
    root = insert(root, key);
}

template <typename T>
Node<T>* AVLTree<T>::remove(Node<T>* node, T key) {
    if (node == nullptr) return nullptr;

    if (key < node->data)
        node->left = remove(node->left, key);
    else if (key > node->data)
        node->right = remove(node->right, key);
    else {
        if (node->left == nullptr || node->right == nullptr) {
            Node<T>* child = (node->left != nullptr) ? node->left : node->right;
            delete node;
            node = child;
        } else {
            Node<T>* succ = minNode(node->right);
            node->data = succ->data;
            node->right = remove(node->right, succ->data);
        }
    }

    if (node == nullptr) return nullptr;

    updateHeight(node);
    int bf = balance(node);

    if (bf > 1 && balance(node->left) >= 0)
        return rotateRight(node);
    if (bf > 1 && balance(node->left) < 0) {
        node->left = rotateLeft(node->left);
        return rotateRight(node);
    }
    if (bf < -1 && balance(node->right) <= 0)
        return rotateLeft(node);
    if (bf < -1 && balance(node->right) > 0) {
        node->right = rotateRight(node->right);
        return rotateLeft(node);
    }
    return node;
}

template <typename T>
void AVLTree<T>::remove(T key) {
    root = remove(root, key);
}

template <typename T>
Node<T>* AVLTree<T>::minNode(Node<T>* node) const {
    while (node->left != nullptr) node = node->left;
    return node;
}

template <typename T>
Node<T>* AVLTree<T>::search(T key) const {
    Node<T>* curr = root;
    while (curr != nullptr) {
        if (key == curr->data) return curr;
        if (key < curr->data) curr = curr->left;
        else curr = curr->right;
    }
    return nullptr;
}

template <typename T>
void AVLTree<T>::inorder(Node<T>* node) const {
    if (node == nullptr) return;
    inorder(node->left);
    cout << node->data.id << " " << node->data.name << " " << node->data.acuity << "\n";
    inorder(node->right);
}

template <typename T>
void AVLTree<T>::printInorder() const {
    if (root == nullptr) {
        cout << "(none)\n";
        return;
    }
    inorder(root);
}

// Records one successful mutating command so it can be undone.
// type: ADMIT, UPDATE, or DISCHARGE
// patient: the Patient involved (full record for ADMIT and DISCHARGE)
// oldAcuity / newAcuity: used only for UPDATE
class Action {
public:
    enum Type { ADMIT, UPDATE, DISCHARGE };
    Type type;
    Patient patient;
    int oldAcuity;
    int newAcuity;
};

class MedIndex {
};

int main() {
	MedIndex medIdx;
    string cmd;
    while (cin >> cmd) {
    }
    return 0;
}