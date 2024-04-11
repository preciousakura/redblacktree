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

      Node* get_left(int version)
      {
        Node* left = this->left;
        if(this->mods.back().version > version)
        {
          for(Mod m: this->mods)
          {
              if (m.type_field == 1 && m.version <= version)
              {
                left = m.left;
              }
          }
          return left;
        }
        else if(this->mods.back().version < version)
        {
          return this->next->get_left(version);
        }
        else
        {
          for(Mod m: this->mods)
          {
              if (m.type_field == 1 && m.version <= version)
              {
                left = m.left;
              }
          }
          if(this->next != nullptr)
          {
            for(Mod m: this->next->mods)
            {
                if (m.type_field == 1 && m.version <= version)
                {
                  left = m.left;
                }
            }
          }
          return left;
        }
      }

      Node* get_right(int version)
      {
        Node* right = this->right;
        if(this->mods.back().version > version)
        {
          for(Mod m: this->mods)
          {
              if (m.type_field == 2 && m.version <= version)
              {
                right = m.right;
              }
          }
          return right;
        }
        else if(this->mods.back().version < version)
        {
          return this->next->get_right(version);
        }
        else
        {
          for(Mod m: this->mods)
          {
              if (m.type_field == 2 && m.version <= version)
              {
                right = m.right;
              }
          }
          if(this->next != nullptr)
          {
            for(Mod m: this->next->mods)
            {
                if (m.type_field == 2 && m.version <= version)
                {
                  right = m.right;
                }
            }
          }
          return right;
        }
      }
      
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
          return field == type_field && value == color;
        }

        bool is_same_field(Node* value, int field) {
          switch (field) {
            case 1:
              return field == type_field && value == left;
              break;

            case 2:
              return field == type_field && value == right;
              break;

            case 3:
              return field == type_field && value == parent;
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
      COLOR get_color()
      {

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

    void right_rotate(Node* node, int version) {
      Node* aux = node->left;
      node->modify(version, 1, BLACK, aux->get_right());


      if(!aux->right->is_null) 
        aux->get_right()->modify(version, 3, BLACK, node);
      aux->modify(version, 3, BLACK, node->get_parent());

      if(node->parent == nullptr) 
        create_root(aux, this->current_version);
      else if(node->is_right_child()) 
        node->get_parent()->modify(version, 2, BLACK, aux);
      else 
        node->get_parent()->modify(version, 1, BLACK, aux);

      aux->modify(version, 2, BLACK, node);
      node->modify(version, 3, BLACK, aux);
    }

    void left_rotate(Node* node, int version) {
      Node* aux = node->right; // y
      node->modify(version, 2, BLACK, aux->get_left());

      if(!aux->left->is_null) 
        aux->get_left()->modify(version, 3, BLACK, node);
      aux->modify(version, 3, BLACK, node->get_parent());

      if(node->parent == nullptr) {
        create_root(aux, this->current_version);
      }
      else if(node->is_left_child()) 
        node->get_parent()->modify(version, 1, BLACK, aux);
      else
        node->get_parent()->modify(version, 2, BLACK, aux);

      aux->modify(version, 1, BLACK, node);
      node->modify(version, 3, BLACK, aux);
    }

    void insert_fix(Node* node, int version) {
      if(node == nullptr) return;
      if(node->parent != nullptr && node->parent->color == COLOR::BLACK) return;

      if(node->parent == nullptr && node->color == COLOR::RED) {
        node->modify(version, 0, BLACK, node);
        return;
      }

      if(!node->get_uncle()->is_null && node->get_uncle()->color == COLOR::RED) {
        node->get_uncle()->modify(version, 0, BLACK, node->get_uncle());
        node->parent->modify(version, 0, BLACK, node->parent);
        node->get_grandparent()->modify(version, 0, RED, node->get_grandparent());
        return insert_fix(node->get_grandparent(), version);
      }

      if(node->parent->is_left_child() != -1 && node->parent->is_left_child()) {
        if(node->is_right_child()) {
          left_rotate(node->parent, version);
          node = node->get_left();
        } 

        node->parent->modify(version, 0, BLACK, node->parent);
        node->get_grandparent()->modify(version, 0, RED, node->get_grandparent());

        return right_rotate(node->get_grandparent(), version);
      }

      if(node->parent->is_right_child() != -1 && node->parent->is_right_child())  {
        if(node->is_left_child()) {
          right_rotate(node->parent, version);
          node = node->right;
        }
        node->parent->modify(version, 0, BLACK, node->parent);
        node->get_grandparent()->modify(version, 0, RED, node->get_grandparent());
        return left_rotate(node->get_grandparent(), version);
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
      if(removed->parent == nullptr) { //get_parent
        root = substitute;
        create_root(substitute, this->current_version);
      }
      else if(removed->is_left_child()) 
        //removed->parent->left = substitute;
        removed->get_parent()->modify(this->current_version, 1, BLACK, substitute);
      else
        //removed->parent->right = substitute;
        removed->get_parent()->modify(this->current_version, 2, BLACK, substitute);
      //substitute->parent = removed->parent;
      substitute->modify(this->current_version, 3, BLACK, removed->get_parent());
    }

    void remove_fix(Node* node, int version) {
      while(node->parent != nullptr && node->color == BLACK) { //get_parent && get_color
        if(node->is_left_child()) {
          Node* right_child = node->parent->right; //get_parent()->get_right
          if(right_child->color == RED) { // get_color
            //right_child->color = BLACK;
            right_child->modify(this->current_version, 0, BLACK, nullptr);
            //node->parent->color = RED;
            node->get_parent()->modify(this->current_version, 0, RED, nullptr);
            left_rotate(node->parent, version); //get_parent
            right_child = node->parent->right; // get_parent()->get_right
          }
          if(right_child->left->color == BLACK && right_child->right->color == BLACK) { //get_left()->get_color && get_right()->get_color
            //right_child->color = RED;
            right_child->modify(this->current_version, 0, RED, nullptr);
            node = node->parent; //get_parent
          }
          else {
            if(right_child->right->color == BLACK) { //get_right()->get_color
              //right_child->left->color = BLACK;
              right_child->get_left()->modify(this->current_version, 0, BLACK, nullptr);
              //right_child->color = RED;
              right_child->modify(this->current_version, 0, RED, nullptr);
              right_rotate(right_child, version);
              right_child = node->parent->right; //get_parent()->get_right
            }

            //right_child->color = node->parent->color;
            right_child->modify(this->current_version, 0, node->get_parent()->get_color(), nullptr);
            //node->parent->color = BLACK;
            node->get_parent()->modify(this->current_version, 0, BLACK, nullptr);
            //right_child->right->color = BLACK;
            right_child->get_right()->modify(this->current_version, 0, BLACK, nullptr);
            left_rotate(node->parent, version); //get_parent
            node = root;
          }
        } else {
          Node* left_child = node->parent->left; // get_left
          if(left_child->color == RED) { //get_color
            //left_child->color = BLACK;
            left_child->modify(this->current_version, 0, BLACK, nullptr);
            node->parent->color = RED;
            node->get_parent()->modify(this->current_version, 0, RED, nullptr);
            right_rotate(node->parent, version); //get_parent
            left_child = node->parent->left; //get_parent()->get_left
          }
          if(left_child->left->color == BLACK && left_child->right->color == BLACK) { //get_left()->get_color && get_right()->get_color
            //left_child->color = RED;
            left_child->modify(this->current_version, 0, RED, nullptr);
            node = node->parent; //get_parent
          }
          else {
            if(left_child->left->color == BLACK) {//get_left()->get_color
              //left_child->right->color = BLACK;
              left_child->get_right()->modify(this->current_version, 0, BLACK, nullptr);
              //left_child->color = RED;
              left_child->modify(this->current_version, 0, RED, nullptr);
              left_rotate(left_child, version);
              left_child = node->parent->left; //get_parent()->get_left
            }

            //left_child->color = node->parent->color;
            left_child->modify(this->current_version, 0, node->get_parent()->get_color(), nullptr);
            //node->parent->color = BLACK;
            node->get_parent()->modify(this->current_version, 0, BLACK, nullptr);
            //left_child->left->color = BLACK;
            left_child->get_left()->modify(this->current_version, 0, BLACK, nullptr);
            right_rotate(node->parent, version); //get_parent
            node = root;
          }
        }
      }
      //node->color = BLACK;
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

      node->is_null = false;
      node->value = value;

      node->color = RED;
      node->left = &(this->nil);
      node->right = &(this->nil);
      node->parent = nullptr;

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
              aux->modify(this->current_version, 2, BLACK, node);

              break;
            }
            aux = aux->get_right();
          } else {
            if(aux->get_left()->is_null) {
              node->parent = aux;
              aux->modify(this->current_version, 1, BLACK, node);

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

      this->current_version++;

      if(d.node->left->is_null) { //get_left
        aux = d.node->right; //get_right
        transplant(d.node, d.node->right); //get_right
      } else if(d.node->right->is_null) { //get_right
        aux = d.node->left; //get_left
        transplant(d.node, d.node->left); //get_left
      } else {
        node = d.successor(d.node); 
        original_color = node->color; // get_color
        aux = node->right;   //get_right
        if(node->parent->value == d.node->value) //get_parent
        {
          //aux->parent = node;
          aux->modify(this->current_version, 3, BLACK, node);
        }
        else {
          transplant(node, node->right); //get_right
          //node->right = d.node->right;
          node->modify(this->current_version, 2, BLACK, d.node->get_right());
          node->get_right()->modify(this->current_version, 3, BLACK, node);
          //node->right->parent = node;
        }
        transplant(d.node, node);   
        //node->left = d.node->left;
        node->modify(this->current_version, 1, BLACK, d.node->get_left());
        //node->left->parent = node;
        node->get_left()->modify(this->current_version, 3, BLACK, node);
        //node->color = d.node->color;
        node->modify(this->current_version, 0, d.node->get_color(), nullptr);
      }
      if(original_color == COLOR::BLACK) 
        remove_fix(aux, this->current_version);
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