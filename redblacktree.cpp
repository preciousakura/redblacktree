#include <iostream>
#include <stdio.h>
#include <vector>

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

      bool is_null = true;

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

      typedef struct Mod {
        Node* left;
        Node* right;
        Node* parent;
        COLOR color;
        int version;

        int type_field;
        bool is_same_field(COLOR value, int field) {
          return value == color;
        }

        bool is_same_field(Node* value, int field) {
          switch (field) {
            case 1:
              return value == left;
              break;

            case 2:
              return value == right;
              break;

            case 3:
              return value == parent;
              break;
          }
        }

      } Mod;

      vector<Mod> mods;

      Node() {} 
      Node(int value, COLOR color, Node* left, Node* right, Node* parent): 
        value(value), color(color), left(left), right(right), parent(parent) {} 
      
      Node* return_left;
      Node* return_right;
      Node* return_parent;
      
      Node* next; // prox mods;

      Mod create_mod(int version, int field, COLOR color, Node* pointer) {
        Mod mod = { this->left, this->right, this->parent, this->color, version, field };

        if(field == 0) { mod.color = color; return mod; }

        switch (field) {
          case 1:
            mod.left = pointer;
            this->return_left = pointer;
            break;

          case 2:
            mod.right = pointer;
            this->return_right = pointer;
            break;

          case 3:
            mod.parent = pointer;
            this->return_parent = pointer;
            break;
        }
         
        return mod;
      }

      void modify(int version, int field_type, COLOR color, Node* pointer) {
        int size = this->mods.size();
        if(size == 6) { 
          Node* node_copy = new Node(this->value, this->color, this->left, this->right, this->parent);
          for(Mod m : this->mods) {
            switch (m.type_field) {
              case 0:
                node_copy->color = m.color;
                break;
              case 1:
                node_copy->left = m.left;
                break;
              case 2:
                node_copy->right = m.right;
                break;
              case 3:
                node_copy->parent = m.parent;
              break;
            }
          }

          this->next = node_copy;
          return_left->modify(version, 3, BLACK, this->next);
          return_right->modify(version, 3, BLACK, this->next);
          if(this->is_left_child()) 
            return_parent->modify(version, 1, BLACK, this->next);
          else if(this->is_right_child()) 
            return_parent->modify(version, 2, BLACK, this->next);
        }

        Mod mod = this->create_mod(version, field_type, color, pointer);
        if(size > 0) {
          int i = size - 1;
          while(mods[i].version == mod.version && mods[i].type_field == mod.type_field) {
            mods.pop_back(); 
            i--;
          }
          bool is_same_field = field_type > 0 ? 
                               mods[i].is_same_field(pointer, field_type) : 
                               mods[i].is_same_field(color, field_type);
          if(is_same_field) return;
        } 
        this->mods.emplace_back(mod);
      }
    } Node;

    Node* root;
    vector<pair<Node*, int>> roots;
    int current_version = 0;

    void create_root(Node* node, int version) { 
      roots.emplace_back(make_pair(node, version));
    }

    Node* get_root(int version) {
      Node* root;
      int size = this->roots.size();
      if(size > 1) {
        for(int i = 0; this->roots[i].second <= version && i < size; i++) 
          root =  this->roots[i].first;
      }
      else 
        root = this->root;
      return root;
    }

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

      if(node->parent == nullptr) {
        root = aux;
        create_root(aux, this->current_version);
      }
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

      if(node->parent == nullptr) {
        root = aux;
        create_root(aux, this->current_version);
      }
      else if(node->is_left_child()) 
        node->parent->left = aux;
      else
        node->parent->right = aux;

      aux->left = node;
      node->parent = aux;
    }

    void insert_fix(Node* node, int version) {
      if(node == nullptr) return;
      if(node->parent != nullptr && node->parent->color == COLOR::BLACK) return;

      if(node->parent == nullptr && node->color == COLOR::RED) {
        node->color = BLACK;
        node->modify(version, 0, BLACK, node);
        return;
      }

      if(!node->get_uncle()->is_null && node->get_uncle()->color == COLOR::RED) {
        node->get_uncle()->color = BLACK;
        node->get_uncle()->modify(version, 0, BLACK, node->get_uncle());

        node->parent->color = BLACK;
        node->parent->modify(version, 0, BLACK, node->parent);

        node->get_grandparent()->color = RED;
        node->get_grandparent()->modify(version, 0, RED, node->get_grandparent());

        return insert_fix(node->get_grandparent(), version);
      }

      if(node->parent->is_left_child() != -1 && node->parent->is_left_child()) {
        if(node->is_right_child()) {
          left_rotate(node->parent);
          node = node->left;
        } 
        node->parent->color = BLACK;
        node->parent->modify(version, 0, BLACK, node->parent);

        node->get_grandparent()->color = RED;
        node->get_grandparent()->modify(version, 0, RED, node->get_grandparent());

        return right_rotate(node->get_grandparent());
      }

      if(node->parent->is_right_child() != -1 && node->parent->is_right_child())  {
        if(node->is_left_child()) {
          right_rotate(node->parent);
          node = node->right;
        }
        node->parent->color = BLACK;
        node->parent->modify(version, 0, BLACK, node->parent);

        node->get_grandparent()->color = RED;
        node->get_grandparent()->modify(version, 0, RED, node->get_grandparent());

        return left_rotate(node->get_grandparent());
      }

      return insert_fix(node->parent, version);
    }

    Node* search_helper(Node* node, int value, int version) {
      if(node->is_null) return nullptr;
      if(node->value > value) {
        for(Node::Mod m : node->mods) {
          if(m.version > version) return search_helper(node->left, value);
          else if(m.type_field == 1) node->left = m.left;
        }
      }
      else if(node->value < value) {
        for(Node::Mod m : node->mods) {
          if(m.version > version) return search_helper(node->right, value);
          else if(m.type_field == 2) node->right = m.right;
        }
      }
      
      return node;
    }

    Node* search_helper(Node* node, int value) {
      if(node->is_null) return nullptr;
      if(node->value > value) return search_helper(node->left, value);
      else if(node->value < value) return search_helper(node->right, value);
      return node;
    }

    // u eh o noh que sai e v eh o noh que vai substituir
    void transplant(Node* removed, Node* substitute) {
      if(removed->parent == nullptr) {
        root = substitute;
        create_root(substitute, this->current_version);
      }
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

      this->current_version++;

      node->color = RED;
      node->modify(this->current_version, 0, RED, node);

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
        create_root(node, this->current_version);
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
        insert_fix(node, this->current_version);
      }

      Data d(node);
      return d;
    }

    Data search(int value) {
      Node* node = search_helper(get_root(), value);
      Data d(node);
      return d;
    }

    Data search(int value, int version) {      
      Node* node = search_helper(get_root(version), value, version);
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
  
  auto s = rbtree.search(80, 1);
  if(s != rbtree.null()) rbtree.remove(s);
  rbtree.print();

  return 0;
}