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

    Node* get_root() { 
      if(root == nullptr) 
        return nullptr;
      return root;
    }

    void print_helper(Node* node) {
      if(node == nullptr) return;

      cout << "value: " << node->value << " color: " << node->color << ' ';
      if(node->left != nullptr)
       cout << "left child: " << node->left->value << ' ';
      else
       cout << "left child: " << "null" << ' ';
      if(node->right != nullptr)
       cout << "right child: " << node->right->value << endl;
      else
       cout << "right child: " << "null" << endl;
      
      print_helper(node->left);
      print_helper(node->right);
    }

    void right_rotate(Node* node) {
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

    void left_rotate(Node* node) {
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

    void insert_fix(Node* node) {
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
        return insert_fix(node->get_grandparent());
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

      return insert_fix(node->parent);
    }

    Node* search_helper(Node* node, int value) {
      if(node == nullptr) return node;
      if(node->value > value) return search_helper(node->left, value);
      else if(node->value < value) return search_helper(node->right, value);
      return node;
    }

    // u eh o noh que sai e v eh o noh que vai substituir
    void transplant(Node* removed, Node* substitute) {
      if(removed->parent == nullptr) 
        root = substitute;
      else if(removed->is_left_child()) 
        removed->parent->left = substitute;
      else
        removed->parent->right = substitute;
      substitute->parent = removed->parent;
    }

    void remove_fix(Node* node) {
      if(node->parent == nullptr) return;
      if(node->is_left_child()) {
        Node* right_child = node->parent->right;
        if(right_child->color == COLOR::RED) {
          right_child->color = BLACK;
          node->parent->color = RED;
          left_rotate(node->parent);
          right_child = node->parent->right;
        } 
        if(right_child->color == COLOR::BLACK && right_child->left->color == COLOR::BLACK && right_child->right->color == COLOR::BLACK) {
          right_child->color = RED;
          node = node->parent;
          if(node->color == RED) node->color = BLACK;
          else remove_fix(node);
        }
        if(right_child->color == COLOR::BLACK && right_child->right->color == COLOR::BLACK) {
          right_child->left->color = BLACK;
          right_child->color = RED;
          right_rotate(right_child);
          right_child = node->parent->right;
        } 
        if(right_child->color == COLOR::BLACK && right_child->right->color == COLOR::RED) {
          right_child->color = right_child->parent->color;
          right_child->parent->color = BLACK;
          right_child->right->color = BLACK;
          left_rotate(right_child->parent);
          if(root->color == COLOR::RED)
            root->color = COLOR::BLACK;
        }
      }
    }

  public:
    RedBlackTree(): root(nullptr) {};

    class Data {
      private:
        Node* node;
      friend RedBlackTree;
      public:
        bool operator == (Data d) { return node == d.node; }
        bool operator != (Data d) { return node != d.node; }
        void operator ++ () { node = successor(node); }

        Node* successor(Node* n) {
          n = n->right;
          while(n->left != nullptr)
            n = n->left;
          return n;
        }

        Data(Node* node): node(node) {}
        int value() { return node->value; }
    };

    Data null() { Data d(nullptr); return d; }

    void print() {
      Node* node = get_root();
      print_helper(node);
    }
  
    Data insert(int value) {
      Node* node = new(nothrow) Node;
      if(node == nullptr) return null(); 

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
        insert_fix(node);
      }

      Data d(node);
      return d;
    }

    Data search(int value) {
      Node* node = search_helper(get_root(), value);
      Data d(node);
      return d;
    }

    void remove(Data d) {
      Node* node = d.node; // y
      COLOR original_color = node->color; // original color of y
      Node* aux;

      if(d.node->left == nullptr) {
        aux = d.node->right;
        transplant(d.node, aux);
      } else if(d.node->right == nullptr) {
        aux = d.node->left;
        transplant(d.node, aux);
      } else {
        node = d.successor(d.node); // y
        aux = node->right;         
        original_color = node->color;
        transplant(node, aux);        

        node->left = d.node->left;
        d.node->left->parent = node;
        node->right = d.node->right;
        d.node->right->parent = node;
        transplant(d.node, node);
        
        node->color = d.node->color;
      }

      if(original_color == COLOR::BLACK) 
        remove_fix(aux);
    }
};

int main() {
  RedBlackTree rbtree; 
  rbtree.insert(75);
  rbtree.insert(11);
  rbtree.insert(90);
  rbtree.insert(4);
  rbtree.insert(50);
  rbtree.insert(78);
  rbtree.insert(94);
  rbtree.insert(45);
  rbtree.insert(70);
  rbtree.insert(77);
  rbtree.insert(80);
  rbtree.insert(92);
  rbtree.insert(96);
  rbtree.insert(85);
  rbtree.insert(95);
  rbtree.insert(98);
  
  // auto s = rbtree.search(4);
  // if(s != rbtree.null()) rbtree.remove(s);

  rbtree.print();
  return 0;
}