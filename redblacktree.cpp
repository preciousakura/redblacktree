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

    } Node;

    Node* root;

  public:
    RedBlackTree(): root(nullptr) {};
    
    Node* get_root() { 
      if(root == nullptr) 
        return nullptr;
      return root;
    }

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

    void right_rotate(Node* node){
      Node* aux = node->left;
      node->left = aux->right;

      if(aux->right != nullptr) 
        aux->right->parent = node;
      aux->parent = node->parent;

      if(node->parent == nullptr) 
        root = aux;
      else if(node->is_right_child()) 
        node->parent->right = aux;
      else 
        node->parent->left = aux;

      aux->right = node;
      node->parent = aux;
    }

    void left_rotate(Node* node){
      Node* aux = node->right; // y
      node->right = aux->left;

      if(aux->left != nullptr) 
        aux->left->parent = node;
      aux->parent = node->parent;

      if(node->parent == nullptr) 
        root = aux;
      else if(node->is_left_child()) 
        node->parent->left = aux;
      else
        node->parent->right = aux;

      aux->left = node;
      node->parent = aux;
    }

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
        if(node->is_right_child()) {
          left_rotate(node->parent);
          node = node->left;
        } 
        node->parent->color = BLACK;
        node->get_grandparent()->color = RED;
        return right_rotate(node->get_grandparent());
      }


      if(node->parent->is_right_child() != -1 && node->parent->is_right_child())  {
        if(node->is_left_child()) {
          right_rotate(node->parent);
          node = node->right;
        }
        node->parent->color = BLACK;
        node->get_grandparent()->color = RED;
        return left_rotate(node->get_grandparent());
      }

      return balance(node->parent);
    }

    Iterator insert(int value) {
        Node* node = new(nothrow) Node;
        if(node == nullptr) return end();

        node->color = RED;
        node->value = value;
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

    void print(Node* node) {
      if(node == nullptr) return;

      cout << "value: " << node->value << ' ';
      if(node->left != nullptr)
       cout << "left child: " << node->left->value << ' ';
      else
       cout << "left child: " << "null" << ' ';
      if(node->right != nullptr)
       cout << "right child: " << node->right->value << endl;
      else
       cout << "right child: " << "null" << endl;
      
      print(node->left);
      print(node->right);
    }
};

int main() {
  RedBlackTree rbtree; 
  auto v1 = rbtree.insert(11);
  auto v2 = rbtree.insert(2);
  auto v3 = rbtree.insert(14);
  auto v4 = rbtree.insert(7);
  auto v5 = rbtree.insert(1);
  auto v6 = rbtree.insert(15);
  auto v7 = rbtree.insert(5);
  auto v8 = rbtree.insert(8);
  auto v9 = rbtree.insert(4);
  
  rbtree.print(rbtree.get_root());
  return 0;
}