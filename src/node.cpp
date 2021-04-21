
#include<iostream>
#include "node.h"
#include <string>
#include <vector>
using namespace std;

Node* Node::define(string label,int level){
    Node* node = new Node();
    node->label = label;
    node->level = level;
    return node;
}
void Node::print(Node* node)
{
    if (node == NULL) {
        return;
    }
    print_node(node);
    print_children(node);
}
void Node::print_node(Node* node)
{
    printf("%*c%d:%-9s ", node->level, ' ', node->level, node->label.c_str());
    for (auto i = node->token_arr.begin(); i != node->token_arr.end(); ++i){
        if ((*i).type == T_IDENTIFIER) cout << (*i).StringValue << ' ';
        else if ((*i).type == T_INTEGER) cout << (*i).intValue << ' ';
        else if ((*i).type == T_FLOAT) cout << (*i).doubleValue << ' ';
        else if ((*i).type <= 255) cout << (char)(*i).type << ' ';
        else cout << (*i).type << ' ';
    }
    printf("\n");
}

void Node::print_children(Node* node)
{
    std::vector<Node*> children = node->children;
    for (unsigned int i = 0; i < children.size(); i++) {
        print(children.at(i));
    }
}
void Node::add_token(token_t t){
    this->token_arr.push_back(t);
}
void Node::add_children(Node* node){
    this->children.push_back(node);
}
void Node::destroy(Node* node){
    if (node == NULL) {
        return;
    }
    vector<Node*> children = node->children;
    for_each(children.begin(), children.end(), &destroy);
    delete node;
}
std::ostream& operator<<(std::ostream &stream, const Node &node)
{
    return stream << node.label;
}