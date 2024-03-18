#include <iostream>
#include <stdio.h>

using namespace std;

enum COLOR { BLACK, RED };

class RedBlackTree {
  private:
    typedef struct Node {
        int value;
        COLOR color;
        Node* left;
        Node* right;
        Node* parent;
        int height;

        Node* get_left() { return left; }
        Node* get_right() { return right; }
        Node* get_parent() { return parent; }
        Node* get_grandparent() { 
          if(parent == nullptr || parent->parent == nullptr) return nullptr;
          return parent->parent;
        }
        Node* get_uncle() { 
          if(get_grandparent() == nullptr) return nullptr;
          if(parent->is_left_child()) return get_grandparent()->right;
          return get_grandparent()->left;
        }

        int is_left_child() { 
          if(parent == nullptr) return -1;
          return value < parent->value;
        }

        int is_right_child() { 
          if(parent == nullptr) return -1;
          return value > parent->value;
        }

        int get_height() { return height; }
    } Node;

    Node* get_root() { 
      if(root == nullptr) 
        return nullptr;
      return root;
    }

    Node* root;

  public:
    RedBlackTree(): root(nullptr) {};
    class Iterator {
      private:
        Node* n;

      friend RedBlackTree;

      public:
        bool operator == (Iterator j) { return n == j.n; }
        bool operator != (Iterator j) { return n != j.n; }
        Iterator(Node *node) : n(node) {}
        int value() { return n->value; }
    };

    Iterator end() { 
      Iterator i(nullptr); 
      return i; 
    }

    void right_rotate(Node* node){}
    void left_rotate(Node* node){}

    void balance(Node* node) {
      if(node == nullptr) return;
      if(node->parent != nullptr && node->parent->color == COLOR::BLACK) return;

      if(node->parent == nullptr && node->color == COLOR::RED) {
        node->color = BLACK;
        return;
      }

      if(node->get_uncle() != nullptr && node->get_uncle()->color == COLOR::RED) {
        node->get_uncle()->color = BLACK;
        node->parent->color = BLACK;
        node->get_grandparent()->color = RED;
        return balance(node->get_grandparent());
      }

      if(node->parent->is_left_child() != -1 && node->parent->is_left_child()) {
        if(node->is_left_child()) {
          node->parent->color = BLACK;
          node->get_grandparent()->color = RED;
        } else {
          left_rotate(node->parent);
          node = node->left;
          node->parent->color = BLACK;
          node->get_grandparent()->color = RED;
        }
        return right_rotate(node->get_grandparent());
      } 

      return balance(node->parent);
    }

    Iterator insert(int value) {
        Node* node = new(nothrow) Node;
        if(node == nullptr) return end();

        node->color = RED;
        node->value = value;
        node->height = 1;
        node->left = nullptr;
        node->right = nullptr;
        node->parent = nullptr;

        if(get_root() == nullptr) {
          node->color = BLACK;
          root = node;
        }
        else {
          Node* aux = get_root();
          while(aux != nullptr) {
            if(value > aux->value) {
              if(aux->get_right() == nullptr) {
                node->parent = aux;
                aux->right = node;
                break;
              }
              aux = aux->get_right();
            } else {
              if(aux->get_left() == nullptr) {
                node->parent = aux;
                aux->left = node;
                break;
              }
              aux = aux->get_left();
            }
          }

          balance(node);
        }

        Iterator i(node);
        return i;
    }
};

int main() {
  RedBlackTree rbtree; 
  auto v1 = rbtree.insert(2);
  auto v2 = rbtree.insert(20);
  auto v3 = rbtree.insert(1);
  auto v4 = rbtree.insert(10);

  return 0;
}