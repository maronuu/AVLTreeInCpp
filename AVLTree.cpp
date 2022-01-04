#include <cassert>
#include <cmath>
#include <iostream>
#include <random>
#include <vector>

template <typename T> T max(const T a, const T b) {
    if (a < b)
        return b;
    else
        return a;
}

template <typename K, typename V> class Node {
  private:
  public:
    int height;
    K key;
    V val;
    Node *left;
    Node *right;

    Node(int height_, K key_, V val_) : height(height_), key(key_), val(val_) {
        left = nullptr;
        right = nullptr;
    }
};

template <typename K, typename V> class AVLTree {
  private:
  public:
    Node<K, V> *root = nullptr;
    bool change;
    K left_max_key;
    V left_max_val;

    // get height of subtree t
    int get_height(const Node<K, V> *t) {
        if (t == nullptr)
            return 0;
        return t->height;
    }
    // |left_sub_tree| - |right_sub_tree|
    int get_bias(const Node<K, V> *t) { return get_height(t->left) - get_height(t->right); }
    // update height of subtree t
    void update_height(Node<K, V> *t) { t->height = 1 + max(get_height(t->left), get_height(t->right)); }

    /////////////////////////////////////////////
    //// rotate
    /////////////////////////////////////////////

    // RotateL(v)
    Node<K, V> *rotateL(Node<K, V> *v) {
        Node<K, V> *u = v->right;
        Node<K, V> *t2 = u->left;
        u->left = v;
        v->right = t2;
        update_height(u->left);
        update_height(u);
        return u;
    }
    // RotateR(u)
    Node<K, V> *rotateR(Node<K, V> *u) {
        Node<K, V> *v = u->left;
        Node<K, V> *t2 = v->right;
        v->right = u;
        u->left = t2;
        update_height(v->right);
        update_height(v);
        return v;
    }
    // RotateRL(u)
    Node<K, V> *rotateRL(Node<K, V> *u) {
        u->right = rotateR(u->right);
        return rotateL(u);
    }
    // RotateLR(u)
    Node<K, V> *rotateLR(Node<K, V> *u) {
        u->left = rotateL(u->left);
        return rotateR(u);
    }

    /////////////////////////////////////////////
    //// Balancing
    /////////////////////////////////////////////

    // insertion to left subtree & deletion to right subtree
    Node<K, V> *balanceL(Node<K, V> *t) {
        if (!change)
            return t; // no change
        const int h = get_height(t);
        if (get_bias(t) == 2) {
            if (get_bias(t->left) >= 0) {
                t = rotateR(t);
            } else {
                t = rotateLR(t);
            }
        } else {
            update_height(t);
        }
        change = (h != get_height(t)); // need change?
        return t;
    }
    // insertion to right subtree & deletion to left subtree
    Node<K, V> *balanceR(Node<K, V> *t) {
        if (!change)
            return t; // no change
        const int h = get_height(t);
        if (get_bias(t) == -2) {
            if (get_bias(t->right) <= 0) {
                t = rotateL(t);
            } else {
                t = rotateRL(t);
            }
        } else {
            update_height(t);
        }
        change = (h != get_height(t));
        return t;
    }

    /////////////////////////////////////////////
    //// Insertion
    /////////////////////////////////////////////

    Node<K, V> *balanceLi(Node<K, V> *t) { return balanceL(t); }
    Node<K, V> *balanceRi(Node<K, V> *t) { return balanceR(t); }
    // insert (key, val) pair
    void insert_node(const K key, const V val) { root = insert_node(root, key, val); }
    Node<K, V> *insert_node(Node<K, V> *t, const K key, const V val) {
        if (t == nullptr) {
            // was empty
            change = true;
            Node<K, V> *ret = new Node<K, V>(1, key, val);
            return ret;
        } else if (key < t->key) {
            // go left
            t->left = insert_node(t->left, key, val);
            return balanceLi(t);
        } else if (t->key < key) {
            // go right
            t->right = insert_node(t->right, key, val);
            return balanceRi(t);
        } else {
            // here
            change = false;
            t->val = val;
            return t;
        }
    }

    /////////////////////////////////////////////
    // Delete
    /////////////////////////////////////////////

    Node<K, V> *balanceLd(Node<K, V> *t) { return balanceR(t); }
    Node<K, V> *balanceRd(Node<K, V> *t) { return balanceL(t); }
    // delete max node in subtree t
    Node<K, V> *delete_max(Node<K, V> *t) {
        if (t->right != nullptr) {
            t->right = delete_max(t->right);
            return balanceRd(t);
        } else {
            // here
            change = true;
            left_max_key = t->key;
            left_max_val = t->val;
            return t->left;
        }
    }
    // delete node by key
    void delete_node(const K key) { root = delete_node(root, key); }
    Node<K, V> *delete_node(Node<K, V> *t, const K key) {
        if (t == nullptr) {
            change = false;
            return nullptr;
        } else if (key < t->key) {
            t->left = delete_node(t->left, key);
            return balanceLd(t);
        } else if (t->key < key) {
            t->right = delete_node(t->right, key);
            return balanceRd(t);
        } else {
            if (t->left == nullptr) {
                change = true;
                return t->right; // promote
            } else {
                // delete max key in left-subtree
                t->left = delete_max(t->left);
                // left_max_key, value are already updated by delete_max func
                t->key = left_max_key;
                t->val = left_max_val;
                return balanceLd(t);
            }
        }
    }

    /////////////////////////////////////////////
    //// Search
    /////////////////////////////////////////////

    // check if node(key, x) is a member
    bool is_member(const K key) {
        Node<K, V> *cur = root;
        while (cur != nullptr) {
            if (key < cur->key) {
                cur = cur->left;
            } else if (key > cur->key) {
                cur = cur->right;
            } else {
                return true;
            }
        }
        return false;
    }

    // lookup table
    V lookup(const K key) {
        Node<K, V> *cur = root;
        while (cur != nullptr) {
            if (key < cur->key) {
                cur = cur->left;
            } else if (key > cur->key) {
                cur = cur->right;
            } else {
                return cur->val;
            }
        }
        return std::numeric_limits<int>::min();
    }

    // is AVL tree empty?
    bool is_empty() { return root == nullptr; }
    // clear the tree
    void clear() { root = nullptr; }
    // get max key
    K get_max_key() {
        Node<K, V> *cur = root;
        if (cur == nullptr)
            return nullptr;
        while (cur->right != nullptr)
            cur = cur->right;
        return cur->key;
    }
    // get min key
    K get_min_key() {
        Node<K, V> *cur = root;
        if (cur == nullptr)
            return nullptr;
        while (cur->left != nullptr)
            cur = cur->left;
        return cur->key;
    }
    // get all keys
    std::vector<K> keys() {
        std::vector<K> ret;
        keys(root, ret);
        return ret;
    }
    void keys(Node<K, V> *t, std::vector<K> &vec) {
        if (t == nullptr)
            return;
        keys(t->left, vec);
        vec.emplace_back(t->key);
        keys(t->right, vec);
    }
    // get all values
    std::vector<V> values() {
        std::vector<V> ret;
        values(root, ret);
        return ret;
    }
    void values(Node<K, V> *t, std::vector<K> &vec) {
        if (t == nullptr)
            return;
        values(t->left, vec);
        vec.emplace_back(t->val);
        values(t->right, vec);
    }

    /////////////////////////////////////////////
    //// debug
    /////////////////////////////////////////////
    bool is_balanced(Node<K, V> *t) {
        if (t == nullptr)
            return true;
        return abs(get_bias(t)) <= 1 && is_balanced(t->left) && is_balanced(t->right);
    }
    bool is_balanced() { return is_balanced(root); }
    bool is_small(const K key, Node<K, V> *t) {
        if (t == nullptr)
            return true;
        return ((key > t->key) && is_small(key, t->left) && is_small(key, t->right));
    }
    bool is_large(const K key, Node<K, V> *t) {
        if (t == nullptr)
            return true;
        return ((key < t->key) && is_large(key, t->left) && is_large(key, t->right));
    }
    bool is_valid_bst(Node<K, V> *t) {
        if (t == nullptr)
            return true;
        return (is_small(t->key, t->left) && is_large(t->key, t->right) && is_valid_bst(t->left) &&
                is_valid_bst(t->right));
    }
    bool is_valid_bst() { return is_valid_bst(root); }
    int size() { return keys().size(); }
};

int main() {
    constexpr int N = 100'000;
    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_int_distribution<int> dist(-1'000'000, 1'000'000);
    int num_insertion_err = 0;
    int num_delete_err = 0;

    AVLTree<int, int> tree;
    AVLTree<int, int> gt;

    // insert N random elements
    for (int i = 0; i < N; ++i) {
        const int key = dist(mt);
        tree.insert_node(key, i);
        gt.insert_node(key, i);
    }
    // check insert validity
    for (int key : gt.keys()) {
        const int v1 = tree.lookup(key);
        const int v2 = gt.lookup(key);
        assert(v1 == v2);
        if (v1 != v2)
            num_insertion_err++;
    }
    // delete half of them
    int rest = gt.size() / 2;
    for (int key : gt.keys()) {
        if (rest == 0)
            break;
        tree.delete_node(key);
        rest--;
    }
    rest = gt.size() / 2;
    for (int key : gt.keys()) {
        if (rest == 0)
            break;
        rest--;
        assert(!tree.is_member(key));
        if (tree.is_member(key))
            num_delete_err++;
    }

    // test
    std::cout << "===TEST===" << std::endl
              << "Balance: " << (tree.is_balanced() ? "PASSED" : "FAILED") << std::endl
              << "BST Validity: " << (tree.is_valid_bst() ? "PASSED" : "FAILED") << std::endl
              << "Insertion: " << (num_insertion_err == 0 ? "PASSED" : "FAILED") << std::endl
              << "Delete: " << (num_delete_err == 0 ? "PASSED" : "FAILED") << std::endl;

    for (int key : tree.keys()) {
        tree.delete_node(key);
    }
    std::cout << "Clear: " << (tree.is_empty() ? "PASSED" : "FAILED") << std::endl;
}