#include <iostream>
#include <stdio.h>
#include <vector>
#include <fstream>
#include <sstream>
#include <string.h>
#define INFINITY 999999999;

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

      COLOR get_color(int version, COLOR color) {
        if(this == nullptr) return color; 
        if(this->is_null) return color;

        color = this->color;

        if(this->mods.empty()) return color;

        for(Mod m: this->mods)
          if (m.type_field == 0 && m.version <= version)
            color = m.color;

        if(this->mods.back().version > version) return color;
        if(this->next != nullptr) 
          return this->next->get_color(version, color);
        return color;
      }

      Node* get_left(int version, Node* left) {
        if(this == nullptr) return nullptr; 
        if(this->is_null) return nullptr;

        left = left == nullptr ? this->left : left;
        
        if(this->mods.empty()) return left;

        for(Mod m: this->mods)
          if (m.type_field == 1 && m.version <= version)
            left = m.left;

        if(this->mods.back().version > version) return left;
        if(this->next != nullptr) 
          return this->next->get_left(version, left);
        return left;
      }

      Node* get_right(int version, Node* right) {
        if(this == nullptr) return nullptr; 
        if(this->is_null) return nullptr;

        right = right == nullptr ? this->right : right;

        if(this->mods.empty()) return right;

        for(Mod m: this->mods)
          if (m.type_field == 2 && m.version <= version)
            right = m.right;

        if(this->mods.back().version > version) return right;
        if(this->next != nullptr) 
          return this->next->get_right(version, right);
        return right;
      }

      Node* get_parent(int version, Node* parent) {
        if(this == nullptr) return nullptr; 
        if(this->is_null) return nullptr;

        parent = parent == nullptr ? this->parent : parent;
        
        if(this->mods.empty()) return parent;

        for(Mod m: this->mods)
          if (m.type_field == 3 && m.version <= version)
            parent = m.parent;

        if(this->mods.back().version > version) return parent;
        if(this->next != nullptr) 
          return this->next->get_parent(version, parent);
        return parent;
      }

      Node* get_grandparent(int version) { 
        if(this->get_parent(version, nullptr) == nullptr) return nullptr;
        return this->get_parent(version, nullptr)->get_parent(version, nullptr);
      }

      Node* get_uncle(int version) { 
        if(this->get_grandparent(version) == nullptr) return nullptr;
        if(this->get_parent(version, nullptr)->is_left_child(version)) return this->get_grandparent(version)->get_right(version, nullptr);
        return this->get_grandparent(version)->get_left(version, nullptr);
      }

      int is_left_child(int version) { 
        if(this->get_parent(version, nullptr) == nullptr) return -1;
        return this->value == this->get_parent(version, nullptr)->get_left(version, nullptr)->value;
      }

      int is_right_child(int version) { 
        if(this->get_parent(version, nullptr) == nullptr) return -1;
        return this->value == this->get_parent(version, nullptr)->get_right(version, nullptr)->value;
      }

      typedef struct Mod {
        Node* left;
        Node* right;
        Node* parent;
        COLOR color;
        int version;

        int type_field;
      } Mod;

      vector<Mod> mods;

      Node() {} 
      Node(int value, COLOR color, Node* left, Node* right, Node* parent): 
        value(value), color(color), left(left), right(right), parent(parent) {} 
      
      Node* return_left;
      Node* return_right;
      Node* return_parent;
      
      Node* next;

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
        if(this == nullptr) return;
        if(this->is_null) return;
        Node* node = this;

        while(node->next != nullptr)
          node = node->next;

        int size = node->mods.size();
        if(size == 6) {
          Node* node_copy = new Node(node->value, node->color, node->left, node->right, node->parent);
          node_copy->is_null = false;

          for(Mod m : node->mods) {
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

          node_copy->return_left = node_copy->left;
          node_copy->return_right = node_copy->right;
          node_copy->return_parent = node_copy->parent;
          node->next = node_copy;

          if(node_copy->return_left != nullptr && !node_copy->return_left->is_null) {
            if(node_copy->return_left->get_parent(version, nullptr)->value == node_copy->value)
              node_copy->return_left->modify(version, 3, BLACK, node_copy);
          }

          if(node_copy->return_right != nullptr && !node_copy->return_right->is_null) {
            if(node_copy->return_right->get_parent(version, nullptr)->value == node_copy->value)
              node_copy->return_right->modify(version, 3, BLACK, node_copy);
          }

          if(node_copy->return_parent != nullptr) {
            if(!node_copy->return_parent->get_left(version, nullptr)->value == node_copy->value)
              node_copy->return_parent->modify(version, 1, BLACK, node_copy);
          }

          if(node_copy->return_parent != nullptr) {
            if(!node_copy->return_parent->get_right(version, nullptr)->value == node_copy->value)
              node_copy->return_parent->modify(version, 2, BLACK, node_copy);
          }

          node = node_copy;
        }

        Mod mod = node->create_mod(version, field_type, color, pointer);
        node->mods.emplace_back(mod);
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
      if(size > 1) 
        for(int i = 0; this->roots[i].second <= version && i < size; i++) 
          root =  this->roots[i].first;
      else 
        root = this->root;
      return root;
    }

    void print_helper(Node* node, int version) {
      if(node->is_null) return;

      cout << "value: " << node->value << " color: " << node->get_color(version, BLACK) << ' ';
      if(!node->get_left(version, nullptr)->is_null)
       cout << "left child: " << node->get_left(version, nullptr)->value << ' ';
      else
       cout << "left child: " << "null" << ' ';
      if(!node->get_right(version, nullptr)->is_null)
       cout << "right child: " << node->get_right(version, nullptr)->value << ' ';
      else
       cout << "right child: " << "null" << ' ';
      if(node->get_parent(version, nullptr) != nullptr)
       cout << "parent child: " << node->get_parent(version, nullptr)->value << endl;
      else
       cout << "parent child: " << "null" << endl;
      
      print_helper(node->get_left(version, nullptr), version);
      print_helper(node->get_right(version, nullptr), version);
    }

    void right_rotate(Node* node, int version) {
      Node* aux = node->get_left(version, nullptr);
      node->modify(version, 1, BLACK, aux->get_right(version, nullptr));

      if(!aux->get_right(version, nullptr)->is_null) 
        aux->get_right(version, nullptr)->modify(version, 3, BLACK, node);
      aux->modify(version, 3, BLACK, node->get_parent(version, nullptr)); // aqui

      if(node->get_parent(version, nullptr) == nullptr) 
        create_root(aux, this->current_version);
      else if(node->is_right_child(version)) 
        node->get_parent(version, nullptr)->modify(version, 2, BLACK, aux);
      else 
        node->get_parent(version, nullptr)->modify(version, 1, BLACK, aux);

      aux->modify(version, 2, BLACK, node);
      node->modify(version, 3, BLACK, aux);
    }

    void left_rotate(Node* node, int version) {
      Node* aux = node->get_right(version, nullptr); // y
      node->modify(version, 2, BLACK, aux->get_left(version, nullptr));

      if(!aux->get_left(version, nullptr)->is_null) 
        aux->get_left(version, nullptr)->modify(version, 3, BLACK, node);
      aux->modify(version, 3, BLACK, node->get_parent(version, nullptr));

      if(node->get_parent(version, nullptr) == nullptr) 
        create_root(aux, version);
      else if(node->is_left_child(version)) 
        node->get_parent(version, nullptr)->modify(version, 1, BLACK, aux);
      else
        node->get_parent(version, nullptr)->modify(version, 2, BLACK, aux);

      aux->modify(version, 1, BLACK, node);
      node->modify(version, 3, BLACK, aux);
    }

    void insert_fix(Node* node, int version) {
      if(node == nullptr) return;
      if(node->get_parent(version, nullptr) != nullptr && node->get_parent(version, nullptr)->get_color(version, BLACK) == COLOR::BLACK) return;

      if(node->get_parent(version, nullptr) == nullptr && node->get_color(version, BLACK) == COLOR::RED) {
        node->modify(version, 0, BLACK, node);
        return;
      }

      if(!node->get_uncle(version)->is_null && node->get_uncle(version)->get_color(version, BLACK) == COLOR::RED) {
        node->get_uncle(version)->modify(version, 0, BLACK, node->get_uncle(version));
        node->get_parent(version, nullptr)->modify(version, 0, BLACK, node->get_parent(version, nullptr));
        node->get_grandparent(version)->modify(version, 0, RED, node->get_grandparent(version));
        return insert_fix(node->get_grandparent(version), version);
      }
      if(node->get_parent(version, nullptr)->is_left_child(version) != -1 && node->get_parent(version, nullptr)->is_left_child(version)) {
        if(node->is_right_child(version)) {
          left_rotate(node->get_parent(version, nullptr), version);
          node = node->get_left(version, nullptr);
        } 

        node->get_parent(version, nullptr)->modify(version, 0, BLACK, node->get_parent(version, nullptr));
        node->get_grandparent(version)->modify(version, 0, RED, node->get_grandparent(version));

        return right_rotate(node->get_grandparent(version), version);
      }
      if(node->get_parent(version, nullptr)->is_right_child(version) != -1 && node->get_parent(version, nullptr)->is_right_child(version))  {
        if(node->is_left_child(version)) {
          right_rotate(node->get_parent(version, nullptr), version);
          node = node->get_right(version, nullptr);
        }

        node->get_parent(version, nullptr)->modify(version, 0, BLACK, node->get_parent(version, nullptr));
        node->get_grandparent(version)->modify(version, 0, RED, node->get_grandparent(version));
        return left_rotate(node->get_grandparent(version), version);
      }
      return insert_fix(node->get_parent(version, nullptr), version);
      
    }

    Node* search_helper(Node* node, int value, int version) {
      if(node->is_null) return nullptr;
      if(node->value > value) return search_helper(node->get_left(version, nullptr), value, version);
      else if(node->value < value) return search_helper(node->get_right(version, nullptr), value, version);
      return node;
    }

    void transplant(Node* removed, Node* substitute, int version) {
      if(removed->get_parent(version, nullptr) == nullptr) {
        root = substitute;
        create_root(substitute, this->current_version);
      }
      else if(removed->is_left_child(version)) 
        removed->get_parent(version, nullptr)->modify(this->current_version, 1, BLACK, substitute);
      else
        removed->get_parent(version, nullptr)->modify(this->current_version, 2, BLACK, substitute);
      substitute->modify(this->current_version, 3, BLACK, removed->get_parent(version, nullptr));
    }

    void remove_fix(Node* node, int version) {
      while(node->get_parent(version, nullptr) != nullptr && node->get_color(version, BLACK) == BLACK) {
        if(node->is_left_child(version)) {
          Node* right_child = node->get_parent(version, nullptr)->get_right(version, nullptr);
          if(right_child->get_color(version, BLACK) == RED) {
            right_child->modify(this->current_version, 0, BLACK, nullptr);
            node->get_parent(version, nullptr)->modify(this->current_version, 0, RED, nullptr);
            left_rotate(node->get_parent(version, nullptr), version);
            right_child = node->get_parent(version, nullptr)->get_right(version, nullptr);
          }
          if(right_child->get_left(version, nullptr)->get_color(version, BLACK) == BLACK && right_child->get_right(version, nullptr)->get_color(version, BLACK) == BLACK) {
            right_child->modify(this->current_version, 0, RED, nullptr);
            node = node->get_parent(version, nullptr); //get_parent
          }
          else {
            if(right_child->get_right(version, nullptr)->get_color(version, BLACK) == BLACK) {
              right_child->get_left(version, nullptr)->modify(this->current_version, 0, BLACK, nullptr);
              right_child->modify(this->current_version, 0, RED, nullptr);
              right_rotate(right_child, version);
              right_child = node->get_parent(version, nullptr)->get_right(version, nullptr);
            }

            right_child->modify(this->current_version, 0, node->get_parent(version, nullptr)->get_color(version, BLACK), nullptr);
            node->get_parent(version, nullptr)->modify(this->current_version, 0, BLACK, nullptr);
            right_child->get_right(version, nullptr)->modify(this->current_version, 0, BLACK, nullptr);
            left_rotate(node->get_parent(version, nullptr), version);
            node = root; // ????
          }
        } else {
          Node* left_child = node->get_parent(version, nullptr)->get_left(version, nullptr);
          if(left_child->get_color(version, BLACK) == RED) {
            left_child->modify(this->current_version, 0, BLACK, nullptr);
            node->get_parent(version, nullptr)->modify(this->current_version, 0, RED, nullptr);
            right_rotate(node->get_parent(version, nullptr), version);
            left_child = node->get_parent(version, nullptr)->get_left(version, nullptr);
          }
          if(left_child->get_left(version, nullptr)->get_color(version, BLACK) == BLACK && left_child->get_right(version, nullptr)->get_color(version, BLACK) == BLACK) {
            left_child->modify(this->current_version, 0, RED, nullptr);
            node = node->get_parent(version, nullptr); //get_parent
          }
          else {
            if(left_child->get_left(version, nullptr)->get_color(version, BLACK) == BLACK) {
              left_child->get_right(version, nullptr)->modify(this->current_version, 0, BLACK, nullptr);
              left_child->modify(this->current_version, 0, RED, nullptr);
              left_rotate(left_child, version);
              left_child = node->get_parent(version, nullptr)->get_left(version, nullptr); //get_parent()->get_left
            }

            left_child->modify(this->current_version, 0, node->get_parent(version, nullptr)->get_color(version, BLACK), nullptr);
            node->get_parent(version, nullptr)->modify(this->current_version, 0, BLACK, nullptr);
            left_child->get_left(version, nullptr)->modify(this->current_version, 0, BLACK, nullptr);
            right_rotate(node->parent, version); //get_parent
            node = root;
          }
        }
      }
      node->modify(this->current_version, 0, BLACK, nullptr);
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
        Node* successor(Node* n, int version) {
          if(n == nullptr) return nullptr;
          if (!n->get_right(version, nullptr)->is_null) { // Caso n tenha sub-árvore direita, o sucessor é o mínimo dessa árvore
            n = n->get_right(version, nullptr);
            while(!n->get_left(version, nullptr)->is_null)
              n = n->get_left(version, nullptr);
            return n;
          }
          else { // Caso n não tenha sub-árvore direita, o sucessor é o pai da sub-árvore cujo n é o máximo
            while((n->get_parent(version, nullptr) != nullptr) && (n->value != n->get_parent(version, nullptr)->get_left(version, nullptr)->value))
              n = n->get_parent(version, nullptr);
            return n->get_parent(version, nullptr); // Caso n não tenha sucessor, irá retornar nullptr
          }
        }

        Data(Node* node): node(node) {}
        int value() { return node->value; }
    };

    int get_last_version() {
      return this->current_version;
    }

    Data null() { Data d(nullptr); return d; }

    void print() {
      Node* node = get_root(this->current_version);
      print_helper(node, this->current_version);
    }

    void print(int version) {
      Node* node = get_root(version);
      print_helper(node, version);
    }

    void print(Data d, int version) {
      print_helper(d.node, version);
    }

    void print_to_file(int version, ofstream& file) {
      version = this->current_version < version ? this->current_version : version;
      int depth = 0;
      Node* node = get_root(version);
      while(!node->get_left(version, nullptr)->is_null) {
        node = node->get_left(version, nullptr);
        depth++;
      }
      
      while(true) {
        char c;
        if(node->get_color(version, BLACK)) c = 'R';
        else c = 'B';
        file << node->value << ',' << depth << ',' << c << ' ';
        
        if (!node->get_right(version, nullptr)->is_null) { // Caso n tenha sub-árvore direita, o sucessor é o mínimo dessa árvore
            node = node->get_right(version, nullptr);
            depth++;
            while(!node->get_left(version, nullptr)->is_null) {
              depth++;
              node = node->get_left(version, nullptr);
            }
        }
        else { // Caso n não tenha sub-árvore direita, o sucessor é o pai da sub-árvore cujo n é o máximo
          while((node->get_parent(version, nullptr) != nullptr) && (node->value != node->get_parent(version, nullptr)->get_left(version, nullptr)->value)) {  
            node = node->get_parent(version, nullptr);
            depth--;
          }  
          node = node->get_parent(version, nullptr);
          depth--;
          if (node == nullptr) break;
        }
      }
    }
  
    Data insert(int value) {
      Node* node = new(nothrow) Node;
      if(node == nullptr) return null(); 

      this->current_version++;

      node->is_null = false;
      node->value = value;

      node->color = RED;
      node->left = &(this->nil);
      node->right = &(this->nil);
      node->parent = nullptr;

      node->left->parent = node;
      node->right->parent = node;

      node->return_left = &(this->nil);
      node->return_right = &(this->nil);
      node->return_parent = &(this->nil);

      if(get_root(this->current_version) == nullptr) {
        node->color = BLACK;
        root = node;
        create_root(node, this->current_version);
      }
      else {
        Node* aux = get_root(this->current_version);
        while(aux != &(this->nil)) {
          if(value > aux->value) {
            if(aux->get_right(this->current_version, nullptr)->is_null) {
              node->parent = aux;
              node->return_parent = aux;
              aux->modify(this->current_version, 2, BLACK, node);
              break;
            }
            aux = aux->get_right(this->current_version, nullptr);
          } else {
            if(aux->get_left(this->current_version, nullptr)->is_null) {
              node->parent = aux;
              node->return_parent = aux;
              aux->modify(this->current_version, 1, BLACK, node);

              break;
            }
            aux = aux->get_left(this->current_version, nullptr);
          }
        }
        insert_fix(node, this->current_version);
      }

      Data d(node);
      return d;
    }

    Data search(int value, int version) {      
      Node* node = search_helper(get_root(version), value, version);
      Data d(node);
      return d;
    }

    int get_successor(int value, int version) {
      Data d = search(value, version);
      Node* node = d.successor(d.node, version);
      version = this->current_version < version ? this->current_version : version;
      if(node == nullptr) return INFINITY;
      return node->value;
    }

    void remove(int value) {
      Data d = search(value, this->current_version);
      Node* node = d.node;
      if(d.node == nullptr) return;
      COLOR original_color = node->color; 
      Node* aux;

      this->current_version++;

      if(d.node->get_left(this->current_version, nullptr)->is_null) {
        aux = d.node->get_right(this->current_version, nullptr);
        transplant(d.node, d.node->get_right(this->current_version, nullptr), this->current_version);
      } else if(d.node->get_right(this->current_version, nullptr)->is_null) {
        aux = d.node->get_left(this->current_version, nullptr);
        transplant(d.node, d.node->get_left(this->current_version, nullptr), this->current_version);
      } else {
        node = d.successor(d.node, this->current_version); 
        original_color = node->get_color(this->current_version, BLACK);
        aux = node->get_right(this->current_version, nullptr);

        if(node->get_parent(this->current_version, nullptr)->value == d.node->value)
          aux->modify(this->current_version, 3, BLACK, node);
        else {
          transplant(node, node->get_right(this->current_version, nullptr), this->current_version);
          node->modify(this->current_version, 2, BLACK, d.node->get_right(this->current_version, nullptr));
          node->get_right(this->current_version, nullptr)->modify(this->current_version, 3, BLACK, node);
        }
        transplant(d.node, node, this->current_version);   
        node->modify(this->current_version, 1, BLACK, d.node->get_left(this->current_version, nullptr));
        node->get_left(this->current_version, nullptr)->modify(this->current_version, 3, BLACK, node);
        node->modify(this->current_version, 0, d.node->get_color(this->current_version, BLACK), nullptr);
      }
      if(original_color == COLOR::BLACK) 
        remove_fix(aux, this->current_version);
    }
};

int main() {
  RedBlackTree rbtree; 
  ifstream file("./test/1.txt");
  ofstream output_file("out.txt");

  if (file.is_open() && output_file.is_open()) {    
    string line;
    while (getline(file, line)) {
      istringstream iss(line); 
      string command; 
      iss >> command; 
      if (command == "INC") {
        int number;
        iss >> number;
        cout << "INC " << number << endl;
        rbtree.insert(number);
      }
      else if(command == "REM"){
        int number;
        iss >> number;
        cout << "REM "<<number<<endl;
        rbtree.remove(number);
      }
      else if (command == "SUC"){
        int number;
        int version;
        iss >> number >> version;
        cout << "SUC " << number << " " << version << endl;
        output_file << "SUC " << number << " " << version << endl << rbtree.get_successor(number, version) << endl;
      }
      else if(command == "IMP") {
        int version;
        iss >> version;
        cout << "IMP " << version << endl;
        output_file << "IMP " << version << endl;

        rbtree.print_to_file(version, output_file);
        output_file << endl;
      }
    }
    file.close();
    output_file.close();
  }

  return 0;
}