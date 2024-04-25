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

      COLOR get_color(int version) {
        COLOR color = this->color;
        if(this->mods.empty()) return color; 
        if(this->mods.back().version > version) {
          for(Mod m: this->mods)
            if (m.type_field == 0 && m.version <= version)
              color = m.color;
          return color;
        }
        else if(this->mods.back().version < version) {
          if (this->next != nullptr)
            return this->next->get_color(version);
          for(Mod m: this->mods)
            if (m.type_field == 0)
              color = m.color;
          return color;
        }
        else {
          for(Mod m: this->mods)
            if (m.type_field == 0 && m.version <= version)
              color = m.color;
          if(this->next != nullptr)
            for(Mod m: this->next->mods)
              if (m.type_field == 0 && m.version <= version)
                color = m.color;
          return color;
        }
      }

      Node* get_left(int version) {
        Node* left = this->left;
        if(this->mods.empty()) return left; 
        if(this->mods.back().version > version) {
          for(Mod m: this->mods)
            if (m.type_field == 1 && m.version <= version)
              left = m.left;
          return left;
        }
        else if(this->mods.back().version < version) {
          if (this->next != nullptr)
            return this->next->get_left(version);
          for(Mod m: this->mods)
            if (m.type_field == 1)
              left = m.left;
          return left;
        }
        else {
          for(Mod m: this->mods)
            if (m.type_field == 1 && m.version <= version)
              left = m.left;
          if(this->next != nullptr)
            for(Mod m: this->next->mods)
              if (m.type_field == 1 && m.version <= version)
                left = m.left;       
          return left;
        }
      }

      Node* get_right(int version) {
        Node* right = this->right;
        if(this->mods.empty()) return right; 
        if(this->mods.back().version > version) {
          for(Mod m: this->mods)
            if (m.type_field == 2 && m.version <= version)
              right = m.right;
          return right;
        }
        else if(this->mods.back().version < version) {
          if (this->next != nullptr)
            return this->next->get_right(version);
          for(Mod m: this->mods)
            if (m.type_field == 2)
              right = m.right;
          return right;
        }
        else {
          for(Mod m: this->mods)
            if (m.type_field == 2 && m.version <= version)
              right = m.right;
          if(this->next != nullptr)
            for(Mod m: this->next->mods)
              if (m.type_field == 2 && m.version <= version)
                right = m.right;
          return right;
        }
      }

      Node* get_parent(int version) {
        Node* parent = this->parent;
        if(this->mods.empty()) return parent; 
        if(this->mods.back().version > version) {
          for(Mod m: this->mods)
            if (m.type_field == 3 && m.version <= version)
              parent = m.parent;
          return parent;
        }
        else if(this->mods.back().version < version) {
          if (this->next != nullptr)
            return this->next->get_parent(version);
          for(Mod m: this->mods)
            if (m.type_field == 3)
              parent = m.parent;
          return parent;
        }
        else {
          for(Mod m: this->mods)
            if (m.type_field == 3 && m.version <= version)
                parent = m.parent;
          if(this->next != nullptr)
            for(Mod m: this->next->mods)
              if (m.type_field == 3 && m.version <= version)
                parent = m.parent;
          return parent;
        }
      }

      Node* get_grandparent(int version) { 
        if(this->get_parent(version) == nullptr || this->get_parent(version)->get_parent(version) == nullptr) return nullptr;
        return this->get_parent(version)->get_parent(version);
      }

      Node* get_uncle(int version) { 
        if(this->get_grandparent(version) == nullptr) return nullptr;
        if(this->get_parent(version)->is_left_child(version)) return this->get_grandparent(version)->get_right(version);
        return this->get_grandparent(version)->get_left(version);
      }

      int is_left_child(int version) { 
        if(this->get_parent(version) == nullptr) return -1;
        return this == this->get_parent(version)->get_left(version);
      }

      int is_right_child(int version) { 
        if(this->get_parent(version) == nullptr) return -1;
        return this == this->get_parent(version)->get_right(version);
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
        if(this == nullptr || this->is_null) return;
        int size = this->mods.size();

        if(size == 6) {
          Node* node_copy = new Node(this->value, this->color, this->left, this->right, this->parent);
          node_copy->is_null = false;
          
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

          switch (field_type) {
            case 0:
              node_copy->color = color;
              break;
            case 1:
              node_copy->left = pointer;
              break;
            case 2:
              node_copy->right = pointer;
              break;
            case 3:
              node_copy->parent = pointer;
              break;
          }

          node_copy->return_left = node_copy->left;
          node_copy->return_right = node_copy->right;
          node_copy->return_parent = node_copy->parent;
          this->next = node_copy;

          if (!this->next->return_left->is_null)
            this->next->return_left->modify(version, 3, BLACK, this->next);
          
          if (!this->next->return_right->is_null)
            this->next->return_right->modify(version, 3, BLACK, this->next);

          if(this->is_left_child(version)) 
            this->next->return_parent->modify(version, 1, BLACK, this->next);

          else if(this->is_right_child(version)) 
            this->next->return_parent->modify(version, 2, BLACK, this->next);
            
          return;
        }
        
        Mod mod = this->create_mod(version, field_type, color, pointer);
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
      if(size > 1) 
        for(int i = 0; this->roots[i].second <= version && i < size; i++) 
          root =  this->roots[i].first;
      else 
        root = this->root;
      return root;
    }

    void print_helper(Node* node, int version) {
      if(node->is_null) return;

      cout << "value: " << node->value << " color: " << node->get_color(version) << ' ';
      if(!node->get_left(version)->is_null)
       cout << "left child: " << node->get_left(version)->value << ' ';
      else
       cout << "left child: " << "null" << ' ';
      if(!node->get_right(version)->is_null)
       cout << "right child: " << node->get_right(version)->value << endl;
      else
       cout << "right child: " << "null" << endl;
      
      print_helper(node->get_left(version), version);
      print_helper(node->get_right(version), version);
    }

    void print_to_file_helper(Node* node, int version, ofstream& file, int depth) {
      if(node->is_null) return;

      char c;
      if(node->color) c = 'R';
      else c = 'B';

      file << node->value << ',' << depth << ',' << c << ' '; 
      
      depth++;
      
      print_to_file_helper(node->get_left(version), version, file, depth);
      print_to_file_helper(node->get_right(version), version, file, depth);
    }

    void right_rotate(Node* node, int version) {
      Node* aux = node->get_left(version);
      node->modify(version, 1, BLACK, aux->get_right(version));

      if(!aux->get_right(version)->is_null) 
        aux->get_right(version)->modify(version, 3, BLACK, node);
      aux->modify(version, 3, BLACK, node->get_parent(version));

      if(node->get_parent(version) == nullptr) 
        create_root(aux, this->current_version);
      else if(node->is_right_child(version)) 
        node->get_parent(version)->modify(version, 2, BLACK, aux);
      else 
        node->get_parent(version)->modify(version, 1, BLACK, aux);

      aux->modify(version, 2, BLACK, node);
      node->modify(version, 3, BLACK, aux);
    }

    void left_rotate(Node* node, int version) {
      Node* aux = node->get_right(version); // y
      node->modify(version, 2, BLACK, aux->get_left(version));

      if(!aux->get_left(version)->is_null) 
        aux->get_left(version)->modify(version, 3, BLACK, node);
      aux->modify(version, 3, BLACK, node->get_parent(version));

      if(node->get_parent(version) == nullptr) {
        create_root(aux, version);
      }
      else if(node->is_left_child(version)) 
        node->get_parent(version)->modify(version, 1, BLACK, aux);
      else
        node->get_parent(version)->modify(version, 2, BLACK, aux);

      aux->modify(version, 1, BLACK, node);
      node->modify(version, 3, BLACK, aux);
    }

    void insert_fix(Node* node, int version) {
      if(node == nullptr) return;
      if(node->get_parent(version) != nullptr && node->get_parent(version)->get_color(version) == COLOR::BLACK) return;

      if(node->get_parent(version) == nullptr && node->get_color(version) == COLOR::RED) {
        node->modify(version, 0, BLACK, node);
        return;
      }

      if(!node->get_uncle(version)->is_null && node->get_uncle(version)->get_color(version) == COLOR::RED) {
        node->get_uncle(version)->modify(version, 0, BLACK, node->get_uncle(version));
        node->get_parent(version)->modify(version, 0, BLACK, node->get_parent(version));
        node->get_grandparent(version)->modify(version, 0, RED, node->get_grandparent(version));
        return insert_fix(node->get_grandparent(version), version);
      }
      if(node->get_parent(version)->is_left_child(version) != -1 && node->get_parent(version)->is_left_child(version)) {
        if(node->is_right_child(version)) {
          left_rotate(node->get_parent(version), version);
          node = node->get_left(version);
        } 

        node->get_parent(version)->modify(version, 0, BLACK, node->get_parent(version));
        node->get_grandparent(version)->modify(version, 0, RED, node->get_grandparent(version));

        return right_rotate(node->get_grandparent(version), version);
      }
      if(node->get_parent(version)->is_right_child(version) != -1 && node->get_parent(version)->is_right_child(version))  {
        if(node->is_left_child(version)) {
          right_rotate(node->get_parent(version), version);
          node = node->get_right(version);
        }

        node->get_parent(version)->modify(version, 0, BLACK, node->get_parent(version));
        node->get_grandparent(version)->modify(version, 0, RED, node->get_grandparent(version));
        return left_rotate(node->get_grandparent(version), version);
      }
      return insert_fix(node->get_parent(version), version);
      
    }

    Node* search_helper(Node* node, int value, int version) {
      if(node->is_null) return nullptr;
      if(node->value > value) return search_helper(node->get_left(version), value, version);
      else if(node->value < value) return search_helper(node->get_right(version), value, version);
      return node;
    }

    // u eh o noh que sai e v eh o noh que vai substituir
    void transplant(Node* removed, Node* substitute, int version) {
      if(removed->get_parent(version) == nullptr) {
        root = substitute;
        create_root(substitute, this->current_version);
      }
      else if(removed->is_left_child(version)) 
        removed->get_parent(version)->modify(this->current_version, 1, BLACK, substitute);
      else
        removed->get_parent(version)->modify(this->current_version, 2, BLACK, substitute);
      substitute->modify(this->current_version, 3, BLACK, removed->get_parent(version));
    }

    void remove_fix(Node* node, int version) {
      while(node->get_parent(version) != nullptr && node->get_color(version) == BLACK) {
        if(node->is_left_child(version)) {
          Node* right_child = node->get_parent(version)->get_right(version);
          if(right_child->get_color(version) == RED) {
            right_child->modify(this->current_version, 0, BLACK, nullptr);
            node->get_parent(version)->modify(this->current_version, 0, RED, nullptr);
            left_rotate(node->get_parent(version), version);
            right_child = node->get_parent(version)->get_right(version);
          }
          if(right_child->get_left(version)->get_color(version) == BLACK && right_child->get_right(version)->get_color(version) == BLACK) {
            right_child->modify(this->current_version, 0, RED, nullptr);
            node = node->get_parent(version); //get_parent
          }
          else {
            if(right_child->get_right(version)->get_color(version) == BLACK) {
              right_child->get_left(version)->modify(this->current_version, 0, BLACK, nullptr);
              right_child->modify(this->current_version, 0, RED, nullptr);
              right_rotate(right_child, version);
              right_child = node->get_parent(version)->get_right(version);
            }

            right_child->modify(this->current_version, 0, node->get_parent(version)->get_color(version), nullptr);
            node->get_parent(version)->modify(this->current_version, 0, BLACK, nullptr);
            right_child->get_right(version)->modify(this->current_version, 0, BLACK, nullptr);
            left_rotate(node->get_parent(version), version);
            node = root; // ????
          }
        } else {
          Node* left_child = node->get_parent(version)->get_left(version);
          if(left_child->get_color(version) == RED) {
            left_child->modify(this->current_version, 0, BLACK, nullptr);
            node->get_parent(version)->modify(this->current_version, 0, RED, nullptr);
            right_rotate(node->get_parent(version), version);
            left_child = node->get_parent(version)->get_left(version);
          }
          if(left_child->get_left(version)->get_color(version) == BLACK && left_child->get_right(version)->get_color(version) == BLACK) {
            left_child->modify(this->current_version, 0, RED, nullptr);
            node = node->get_parent(version); //get_parent
          }
          else {
            if(left_child->get_left(version)->get_color(version) == BLACK) {
              left_child->get_right(version)->modify(this->current_version, 0, BLACK, nullptr);
              left_child->modify(this->current_version, 0, RED, nullptr);
              left_rotate(left_child, version);
              left_child = node->get_parent(version)->get_left(version); //get_parent()->get_left
            }

            left_child->modify(this->current_version, 0, node->get_parent(version)->get_color(version), nullptr);
            node->get_parent(version)->modify(this->current_version, 0, BLACK, nullptr);
            left_child->get_left(version)->modify(this->current_version, 0, BLACK, nullptr);
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
          if (!n->get_right(version)->is_null) { // Caso n tenha sub-árvore direita, o sucessor é o mínimo dessa árvore
            n = n->get_right(version);
            while(!n->get_left(version)->is_null)
              n = n->get_left(version);
            return n;
          }
          else { // Caso n não tenha sub-árvore direita, o sucessor é o pai da sub-árvore cujo n é o máximo
            while((n->value != n->get_parent(version)->get_left(version)->value) && (!n->get_parent(version)->is_null))
              n = n->get_parent(version);
            
            return n->get_parent(version); // Caso n não tenha sucessor, irá retornar nullptr

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
      Node* node = get_root(version);
      print_to_file_helper(node, version, file, 0);
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
            if(aux->get_right(this->current_version)->is_null) {
              node->parent = aux;
              node->return_parent = aux;
              aux->modify(this->current_version, 2, BLACK, node);

              break;
            }
            aux = aux->get_right(this->current_version);
          } else {
            if(aux->get_left(this->current_version)->is_null) {
              node->parent = aux;
              node->return_parent = aux;
              aux->modify(this->current_version, 1, BLACK, node);

              break;
            }
            aux = aux->get_left(this->current_version);
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
      if(node->is_null) return INFINITY;
      return node->value;
    }

    void remove(int value) {
      Data d = search(value, this->current_version);

      Node* node = d.node;
      COLOR original_color = node->color; 
      Node* aux;

      this->current_version++;

      if(d.node->get_left(this->current_version)->is_null) {
        aux = d.node->get_right(this->current_version);
        transplant(d.node, d.node->get_right(this->current_version), this->current_version);
      } else if(d.node->get_right(this->current_version)->is_null) {
        aux = d.node->get_left(this->current_version);
        transplant(d.node, d.node->get_left(this->current_version), this->current_version);
      } else {
        node = d.successor(d.node, this->current_version); 
        original_color = node->get_color(this->current_version);
        aux = node->get_right(this->current_version);

        if(node->get_parent(this->current_version)->value == d.node->value)
          aux->modify(this->current_version, 3, BLACK, node);
        else {
          transplant(node, node->get_right(this->current_version), this->current_version);
          node->modify(this->current_version, 2, BLACK, d.node->get_right(this->current_version));
          node->get_right(this->current_version)->modify(this->current_version, 3, BLACK, node);
        }
        transplant(d.node, node, this->current_version);   
        node->modify(this->current_version, 1, BLACK, d.node->get_left(this->current_version));
        node->get_left(this->current_version)->modify(this->current_version, 3, BLACK, node);
        node->modify(this->current_version, 0, d.node->get_color(this->current_version), nullptr);
      }
      if(original_color == COLOR::BLACK) 
        remove_fix(aux, this->current_version);
    }
};

int main() {
  RedBlackTree rbtree; 
  ifstream file("exemplo3.txt");
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
      }
    }
    file.close();
    output_file.close();
  }

  return 0;
}