//
// Created by iansg on 4/16/2024.
//

#include <iostream>
#include <string>
#include <queue>
#include <stack>
#include <vector>
#include <iomanip>
using namespace std;

struct Record {
    char code[5]{};
    char name[20]{};
    int semester {};
    int left {};
    int right {};
    int height {};
};


template<typename TK>
class AVLFile {
    string filename;
    int pos;
public:
    AVLFile(string filename) : filename(filename) {}
    Record find(TK key) {

    }
    void insert(Record record) {

    }
    vector<Record> inOrder() {

    }
}

template<typename T>
class avl {
    class Node {
        T data {};
        Node* parent {};
        Node* left {};
        Node* right {};
        friend avl;
    public:
        Node() = default;
        explicit Node(T value) : data(value) {}
        Node(T value, Node* parent) : data(value), parent(parent) {}
    };

    class iterator {
        Node* cur {};
    public:
        using difference_type = std::ptrdiff_t;
        using value_type = T;
        using pointer = T*;
        using reference = T&;
        using iterator_category = std::bidirectional_iterator_tag;

        iterator() = default;
        explicit iterator(Node* node) : cur(node) {}
        // prefix increment
        iterator& operator++() {
            if (cur == nullptr) return *this;

            if (cur->right) {
                cur = cur->right;
                while (cur->left) {
                    cur = cur->left;
                }
            }
            else {
                while (cur->parent && cur == cur->parent->right) {
                    cur = cur->parent;
                }
                cur = cur->parent;
            }
            return *this;
        }
        // postfix increment
        iterator operator++(int) {
            if (cur == nullptr) return *this;

            iterator old = *this;
            if (cur->right) {
                cur = cur->right;
                while (cur->left) {
                    cur = cur->left;
                }
            }
            else{
                while (cur->parent && cur == cur->parent->right) {
                    cur = cur->parent;
                }
                cur = cur->parent;
            }
            return old;
        }
        // prefix decrement
        iterator& operator--() {
            if (cur == nullptr) return *this;

            if (cur->left) {
                cur = cur->left;
                while (cur->right) {
                    cur = cur->right;
                }
            }
            else {
                while (cur->parent && cur == cur->parent->left) {
                    cur = cur->parent;
                }
                cur = cur->parent;
            }
            return *this;
        }
        // postfix decrement
        iterator operator--(int) {
            if (cur == nullptr) return *this;
            iterator old = *this;

            if (cur->left) {
                cur = cur->left;
                while (cur->right) {
                    cur = cur->right;
                }
            }
            else {
                while (cur->parent && cur == cur->parent->left) {
                    cur = cur->parent;
                }
                cur = cur->parent;
            }
            return old;
        }
        bool operator==(const iterator& other) { return cur == other.cur; }
        bool operator!=(const iterator& other) { return !(*this == other); }
        T& operator*() { return cur->data; }
        T operator*() const { return cur->data; }
        T* operator->() { return cur; }
    };

    Node* root {};
    size_t sz {};

    Node* _replace_with_left(Node* cur) {
        Node* left = cur->left;
        if (cur->right) {
            left->right = cur->right;
            cur->right->parent = left;
        }

        left->parent = cur->parent;
        if (cur->parent) {
            if (cur->parent->left == cur) cur->parent->left = left;
            else cur->parent->right = left;
        }

        return left;
    }
    Node* _replace_with_right(Node* cur) {
        Node* right = cur->right;
        if (cur->left) {
            right->left = cur->left;
            cur->left->parent = right;
        }

        right->parent = cur->parent;
        if (cur->parent) {
            if (cur->parent->left == cur) cur->parent->left = right;
            else cur->parent->right = right;
        }

        return right;
    }

    int _height(Node* cur) {
        if (cur == nullptr) return -1;

        std::queue<std::pair<Node*, int>> q;
        int depth = 0;
        q.emplace(cur, depth);
        while (!q.empty()) {
            auto [u, d] = q.front();
            depth = std::max(depth, d);
            q.pop();
            if (u->left) q.emplace(u->left, d + 1);
            if (u->right) q.emplace(u->right, d + 1);
        }
        return depth;
    }
    int _balance_factor(Node* cur) {
        if (cur == nullptr) return 0;
        return _height(cur->left) - _height(cur->right);
    }
    bool _is_balanced(Node* cur) {
        if (cur == nullptr) return true;
        return std::abs(_balance_factor(cur)) <= 1 && _is_balanced(cur->left) && _is_balanced(cur->right);
    }

    Node* _minimum(Node* cur) {
        if (cur == nullptr) return cur;
        while (cur->left) {
            cur = cur->left;
        }
        return cur;
    }
    Node* _maximum(Node* cur) {
        if (cur == nullptr) return cur;
        while (cur->right) {
            cur = cur->right;
        }
        return cur;
    }

    // left child is now root
    // shift tree to the right
    Node* _ll(Node* cur) {
        Node* left = cur->left;

        cur->left = left->right;
        if (left->right) left->right->parent = cur;

        left->parent = cur->parent;
        if (left->parent) {
            if (cur == cur->parent->left) cur->parent->left = left;
            else cur->parent->right = left;
        }

        cur->parent = left;
        left->right = cur;

        return left;
    }
    // right child is now root
    // shift tree to the left
    Node* _rr(Node* cur) {
        Node* right = cur->right;

        cur->right = right->left;
        if (right->left) right->left->parent = cur;

        right->parent = cur->parent;
        if (right->parent) {
            if (cur == cur->parent->left) cur->parent->left = right;
            else cur->parent->right = right;
        }

        cur->parent = right;
        right->left = cur;

        return right;
    }
    // right child of left child is now root
    // lr subtree is the unbalance problem cause
    Node* _lr(Node* cur) {
        Node* new_cur = cur->left->right;

        cur->left->right = new_cur->left;
        if (new_cur->left) new_cur->left->parent = cur->left;

        new_cur->left = cur->left;
        cur->left->parent = new_cur;

        cur->left = new_cur->right;
        if (new_cur->right) new_cur->right->parent = cur;

        new_cur->parent = cur->parent;
        if (new_cur->parent) {
            if (cur == cur->parent->left) cur->parent->left = new_cur;
            else cur->parent->right = new_cur;
        }

        new_cur->right = cur;
        cur->parent = new_cur;

        return new_cur;
    }
    // left child or right child is now root
    // rl subtree is the unbalance problem cause
    Node* _rl(Node* cur) {
        Node* new_cur = cur->right->left;

        cur->right->left = new_cur->right;
        if (new_cur->right) new_cur->right->parent = cur->right;

        new_cur->right = cur->right;
        cur->right->parent = new_cur;

        cur->right = new_cur->left;
        if (new_cur->left) new_cur->left->parent = cur;

        new_cur->parent = cur->parent;
        if (cur->parent) {
            if (cur == cur->parent->left) cur->parent->left = new_cur;
            else cur->parent->right = new_cur;
        }

        new_cur->left = cur;
        cur->parent = new_cur;

        return new_cur;
    }

    Node* _balance(Node* cur) {
        auto bf = _balance_factor(cur);
        if (bf > 1) {
            if (_height(cur->left->right) + 1 == _height(cur->left)) return _lr(cur);
            return _ll(cur);
        }
        else if (bf < -1) {
            if (_height(cur->right->left) + 1 == _height(cur->right)) return _lr(_rr(cur));
            return _rr(cur);
        }
        return cur;
    }

    void _clear(Node* cur) {
        if (cur == nullptr) return;

        if (cur->parent) {
            if (cur == cur->parent->left) cur->parent->left = nullptr;
            else cur->parent->right = nullptr;
        }
        else root = nullptr;

        std::stack<Node*> s;
        s.push(cur);
        while (!s.empty()) {
            auto u = s.top();
            s.pop();
            if (u->left) s.push(u->left);
            if (u->right) s.push(u->right);
            --sz;
            delete u;
        }
    }

    void _pre_order(std::vector<T>& v, Node* cur) {
        if (cur == nullptr) return;
        v.push_back(cur->data);
        _in_order(v, cur->left);
        _in_order(v, cur->right);
    }

    void _in_order(std::vector<T>& v, Node* cur) {
        if (cur == nullptr) return;
        _in_order(v, cur->left);
        v.push_back(cur->data);
        _in_order(v, cur->right);
    }

    void _post_order(std::vector<T>& v, Node* cur) {
        if (cur == nullptr) return;
        _in_order(v, cur->left);
        _in_order(v, cur->right);
        v.push_back(cur->data);
    }

    int max_value_length(Node* node) {
        if (node != nullptr) {
            int val = std::to_string(node->data).length();
            int val2 = std::max(val, max_value_length(node->left));
            return std::max(val2, max_value_length(node->right));
        }
        else {
            return 0;
        }
    }

public:
    avl() = default;
    ~avl() {
        _clear(root);
    }

    bool empty() { return sz > 0; }
    size_t size() { return sz; }
    int height() { return _height(root); }

    iterator minimum() { return iterator(_minimum(root)); }
    iterator maximum() { return iterator(_maximum(root)); }
    iterator begin() { return minimum(); }
    iterator end() { return iterator(); }


    bool is_balanced() {
        return _is_balanced(root);
    }
    void balance(Node* cur = nullptr) {
        if (cur == nullptr) cur = root;
        while (std::abs(_balance_factor(cur)) > 1) {
            if (_balance_factor(cur) > 1) {
                if (_height(cur->left->right) + 1 == _height(cur->left)) cur = _lr(cur);
                else cur = _ll(cur);
            }
            else {
                if (_height(cur->right->left) + 1 == _height(cur->right)) cur = _lr(_rr(cur));
                else cur = _rr(cur);
            }
        }
        if (cur->left) balance(cur->left);
        if (cur->right) balance(cur->right);
    }

    std::pair<iterator,bool> insert(T value) {
        ++sz;
        Node* temp = root;
        while (temp) {
            if (value == temp->data) {
                --sz;
                return {iterator(temp), false};
            }
            else if (value < temp->data) {
                if (temp->left) temp = temp->left;
                else {
                    temp->left = new Node(value, temp);

                    Node* aux = temp->left;
                    while (temp->parent) {
                        temp = _balance(temp);
                        temp = temp->parent;
                    }
                    root = _balance(root);

                    return {iterator(aux), true};
                }
            }
            else if (temp->right) temp = temp->right;
            else {
                temp->right = new Node(value, temp);

                Node* aux = temp->right;
                while (temp->parent) {
                    temp = _balance(temp);
                    temp = temp->parent;
                }
                root = _balance(root);

                return {iterator(aux), true};
            }
        }

        root = new Node(value);
        return {iterator(root), true};
    }

    bool erase(T value) {
        Node* temp = root;
        while (temp) {
            if (value < temp->data) {
                if (temp->left) temp = temp->left;
                else return false;
            }
            else if (value > temp->data) {
                if (temp->right) temp = temp->right;
                else return false;
            }
            else {
                --sz;

                Node* aux = temp;
                if (temp->left) {
                    if (!temp->left->right) temp = _replace_with_left(temp);
                    else {
                        T el = _maximum(temp->left)->data;
                        erase(el);
                        aux->data = el;
                        return true;
                    }
                }
                else if (temp->right) {
                    if (!temp->right->left) temp = _replace_with_right(temp);
                    else {
                        T el = _minimum(temp->right)->data;
                        erase(el);
                        aux->data = el;
                        return true;
                    }
                }
                else if (temp->parent) {
                    if (temp->parent->left == temp) temp->parent->left = nullptr;
                    else temp->parent->right = nullptr;
                    temp = temp->parent;
                }
                else root = temp = nullptr;

                if (aux == root) root = temp;
                delete aux;
                if (temp) {
                    while (temp->parent) {
                        temp = _balance(temp);
                        temp = temp->parent;
                    }
                    root = _balance(root);
                }

                return true;
            }
        }

        return false;
    }

    iterator search(T value) {
        Node* temp = root;
        while (temp) {
            if (value == temp->data) return iterator(temp);
            else if (value < temp->data) {
                if (temp->left) temp = temp->left;
                else return iterator();
            }
            else if (temp->right) temp = temp->right;
            else return iterator();
        }
        return iterator();
    }
    bool contains(T value) {
        return search(value) != end();
    }

    void clear() {
        _clear(root);
    }

    std::vector<T> pre_order() {
        std::vector<T> v;
        _pre_order(v, root);
        return v;
    }
    std::vector<T> in_order() {
        std::vector<T> v;
        _in_order(v, root);
        return v;
    }
    std::vector<T> post_order() {
        std::vector<T> v;
        _post_order(v, root);
        return v;
    }

    void pretty_print(int setwValue = 5, char nulo = '.') {
        if (root == nullptr) {
            return;
        }
        int h = height();
        int indent = 0;
        for (int temp = 0; temp < h; temp++) {
            indent = 2 * indent + 1;
        }
        std::queue<Node*> q;
        int pos = 0, cant = 0;

        for (int temp = 0; temp < indent; ++temp) {
            std::cout << std::setw(setwValue) << ' ';
        }

        q.push(root);
        while (pos <= h) {
            auto* nodo = q.front();
            q.pop();
            if (nodo == nullptr) {
                std::cout << std::setw(setwValue) << nulo;
            }
            else {
                std::cout << std::setw(setwValue) << nodo->data;
            }
            for (int temp = 0; temp < indent*2+1; ++temp) {
                std::cout << std::setw(setwValue) << ' ';
            }
            cant++;
            if (cant == pow(2, pos)) {
                cant = 0;
                pos++;
                std::cout << '\n';

                indent = (indent - 1) / 2;
                for (int temp = 0; temp < indent; ++temp) {
                    std::cout << std::setw(setwValue) << ' ';
                }
            }
            if (nodo == nullptr) {
                q.push(nullptr);
                q.push(nullptr);
            }
            else if (nodo->left && nodo->right) {
                q.push(nodo->left);
                q.push(nodo->right);
            }
            else if (nodo->left) {
                q.push(nodo->left);
                q.push(nullptr);
            }
            else if (nodo->right) {
                q.push(nullptr);
                q.push(nodo->right);
            }
            else {
                q.push(nullptr);
                q.push(nullptr);
            }
        }
    }

    void auto_pretty_print() {
        int max = 1;
        // go to every node and find max lenght of string(value)
        max = max_value_length(root);
        pretty_print(max);
    }
};