#include <iostream>
#include <stdio.h>
#include <vector>

using namespace std;


enum COLOR { BLACK, RED };

class RedBlackTree {
  private:
    typedef struct Node {
      int value;
      COLOR color; // 0
      Node* left; // 1
      Node* right; // 2
      Node* parent; // 3

      bool is_null = true;

      class Mod {
        private:
          int field; 
          Node* node;
          COLOR color;
          int version;

          friend Node;

          Mod(): node(nullptr), color(BLACK) {}

      };

      vector<Mod> mods;

      Node* next;
      Node* return_pointers[3]; // 0:left, 1:right, 2:parent

      void register_old_root(int version)
      {
        roots.emplace_back(make_pair(this, version));
      }

      void copy(int version) {

        Node* node_copy = new Node();
        node_copy->value= this->value;
        node_copy->color= this->color;
        node_copy->left= this->left;
        node_copy->right= this->right;
        node_copy->parent= this->parent;

        for(Mod m: this->mods)
        {
          switch (m.field)
          {
          case 0:
            node_copy->color = m.color;
            break;
          case 1:
            node_copy->left = m.node;
            break;
          case 2:
            node_copy->right = m.node;
            break;
          case 3:
            node_copy->parent = m.node;
          break;
          }
        }

        this->next = node_copy;
        return_pointers[0]->modify(version, 3, this->next);
        return_pointers[1]->modify(version, 3, this->next);
        if(this->is_left_child())
        {
          return_pointers[2]->modify(version, 1, this->next);
        } else if(this->is_right_child())
        {
          return_pointers[2]->modify(version, 2, this->next);
        }
      }

      void modify(int current, int type_field, COLOR color) {
        int size = mods.size();

        if(size == 6) return copy(current);

        int i = size - 1;
        bool new_mod = true;
        while(mods[i].version == current) {
          if(mods[i].field == type_field) 
            mods[i].color = color;
          i--; 
        }
        if(new_mod)
        {
          Mod mod;
          mod.field = type_field;
          mod.node = nullptr;
          mod.color = color; 
          mod.version = current;
          mods.emplace_back(mod);
        }
      }

      void modify(int current, int type_field, Node* modify_node) {
        int size = mods.size();

        if(size == 6) return copy(current);

        int i = size - 1;
        bool new_mod = true;
        while(mods[i].version == current) {
          new_mod = false;
          if(mods[i].field == type_field) 
            mods[i].node = modify_node;
            switch(mods[i].field)
            {
              case 1:
                return_pointers[0] = modify_node;
                break;
              case 2:
                return_pointers[1] = modify_node;
                break;
              case 3:
                return_pointers[2] = modify_node;
                break;
            }
          i--; 
        }

        if(new_mod)
        {
          Mod mod;
          mod.field = type_field;
          mod.node = modify_node;
          mod.color = BLACK; 
          mod.version = current;
          mods.emplace_back(mod);
        }
      }

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
        return this == parent->left;
      }

      int is_right_child() { 
        if(parent == nullptr) return -1;
        return this == parent->right;
      }
    } Node;

    Node* root;
    vector<pair<Node*, int>> roots;
    Node* get_root() { 
      if(root == nullptr) 
        return nullptr;
      return root;
    }
    void print_helper(Node* node) {
      if(node->is_null) return;

      cout << "value: " << node->value << " color: " << node->color << ' ';
      if(!node->left->is_null)
       cout << "left child: " << node->left->value << ' ';
      else
       cout << "left child: " << "null" << ' ';
      if(!node->right->is_null)
       cout << "right child: " << node->right->value << endl;
      else
       cout << "right child: " << "null" << endl;
      
      print_helper(node->left);
      print_helper(node->right);
    }

    void right_rotate(Node* node) {
      Node* aux = node->left;
      node->left = aux->right;

      if(!aux->right->is_null) 
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

      if(!aux->left->is_null) 
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

      if(!node->get_uncle()->is_null && node->get_uncle()->color == COLOR::RED) {
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
      if(node->is_null) return nullptr;
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
      while(node->parent != nullptr && node->color == BLACK) {
        if(node->is_left_child()) {
          Node* right_child = node->parent->right;
          if(right_child->color == RED) {
            right_child->color = BLACK;
            node->parent->color = RED;
            left_rotate(node->parent);
            right_child = node->parent->right;
          }
          if(right_child->left->color == BLACK && right_child->right->color == BLACK) {
            right_child->color = RED;
            node = node->parent;
          }
          else {
            if(right_child->right->color == BLACK) {
              right_child->left->color = BLACK;
              right_child->color = RED;
              right_rotate(right_child);
              right_child = node->parent->right;
            }

            right_child->color = node->parent->color;
            node->parent->color = BLACK;
            right_child->right->color = BLACK;
            left_rotate(node->parent);
            node = root;
          }
        } else {
          Node* left_child = node->parent->left;
          if(left_child->color == RED) {
            left_child->color = BLACK;
            node->parent->color = RED;
            right_rotate(node->parent);
            left_child = node->parent->left;
          }
          if(left_child->left->color == BLACK && left_child->right->color == BLACK) {
            left_child->color = RED;
            node = node->parent;
          }
          else {
            if(left_child->left->color == BLACK) {
              left_child->right->color = BLACK;
              left_child->color = RED;
              left_rotate(left_child);
              left_child = node->parent->left;
            }

            left_child->color = node->parent->color;
            node->parent->color = BLACK;
            left_child->left->color = BLACK;
            right_rotate(node->parent);
            node = root;
          }
        }
      }
      node->color = BLACK;
    }

  public:
    Node nil;
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
          if (!n->right->is_null) // Caso n tenha sub-árvore direita, o sucessor é o mínimo dessa árvore
          {
            n = n->right;
            while(!n->left->is_null)
              n = n->left;
            return n;
          }
          else // Caso n não tenha sub-árvore direita, o sucessor é o pai da sub-árvore cujo n é o máximo
          {
            while((n->value != n->parent->left->value) && (n->parent != nullptr))
            {
              n = n->parent;
            }
            return n->parent; // Caso n não tenha sucessor, irá retornar nullptr
          }
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
      node->left = &(this->nil);
      node->right = &(this->nil);
      node->parent = nullptr;
      node->is_null = false;

      node->left->parent = node;
      node->right->parent = node;

      if(get_root() == nullptr) {
        node->color = BLACK;
        root = node;
      }
      else {
        Node* aux = get_root();
        while(aux != nullptr) {
          if(value > aux->value) {
            if(aux->get_right()->is_null) {
              node->parent = aux;
              aux->right = node;
              break;
            }
            aux = aux->get_right();
          } else {
            if(aux->get_left()->is_null) {
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
      cout << node->mods.size() << endl;
      Data d(node);
      return d;
    }

    void remove(Data d) {
      Node* node = d.node; // y
      COLOR original_color = node->color; // original color of y
      Node* aux;

      if(d.node->left->is_null) {
        aux = d.node->right;
        transplant(d.node, d.node->right);
      } else if(d.node->right->is_null) {
        aux = d.node->left;
        transplant(d.node, d.node->left);
      } else {
        node = d.successor(d.node); 
        original_color = node->color;
        aux = node->right;   
        if(node->parent->value == d.node->value)
          aux->parent = node;
        else {
          transplant(node, node->right);
          node->right = d.node->right;
          node->right->parent = node;
        }
        transplant(d.node, node);   
        node->left = d.node->left;
        node->left->parent = node;
        node->color = d.node->color;
      }
      if(original_color == COLOR::BLACK) 
        remove_fix(aux);
    }
};

int main() {
  RedBlackTree rbtree; 
  rbtree.insert(70);
  rbtree.insert(45);
  rbtree.insert(90);
  rbtree.insert(11);
  rbtree.insert(50);
  rbtree.insert(78);
  rbtree.insert(94);
  rbtree.insert(77);
  rbtree.insert(80);
  rbtree.insert(92);
  rbtree.insert(96);
  rbtree.insert(85);
  rbtree.insert(95);
  rbtree.insert(98);
  
  auto s = rbtree.search(70);
  // if(s != rbtree.null()) rbtree.remove(s);

  rbtree.print();

  return 0;
}