#ifndef NODE_H
#define NODE_H

#include<vector>
#include<string>
#include "token.h"

class Node{
public:
    // debugging tool.
    static void print(Node* node);
    static Node* define(std::string label,int level);
    void add_token(token_t t);
    void add_children(Node* node);
    static void destroy(Node* node);
    std::vector<Node*> children; // Node vector.
    std::vector<token_t> token_arr;  // token vector.
    int level; // hold the level for each variable.
    std::string label;
private:
    static void print_node(Node* node);
    static void print_children(Node* node);
    friend std::ostream& operator<<(std::ostream&, const Node&);
};
#endif