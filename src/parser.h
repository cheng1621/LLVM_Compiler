#ifndef PARSER_H
#define PARSER_H

#include "scanner.h"
#include "token.h"
#include "node.h"
#include <vector>
#include <stack>

class Parser{
public:
    // three APIS which are open to public.
    Parser(std::string filename);
    ~Parser();
    Node* Parse();
private:
    Scanner* scanner;
    token_t token;

    // new 
    std::vector<token_t> var_set;
    std::stack<std::vector<token_t> > st;
    std::vector<token_t> global_set;
    bool isglobal;
    token_t Assigncheck; // assign check.
    token_t var;
    std::stack<token_t> Procedurecheck;
    bool is_buildin;
    bool is_assign;
    // end
    Node* parseS(); // <S> -> program <vars> <block>
    Node* parseblock(int level); // <block> -> begin <vars> <stats> end.
    Node* parseprocedure(int level); // <procedure>
    Node* parsevars(int level); // <vars> -> empty | variable Identifier <type>
    Node* parsetype(int level); // <type> boolean | integer | string.
    Node* parseexpr(int level); // <expr> -> + - * / <expr>
    Node* parsenegation(int level); // - negation synbol.
    Node* parseR(int level); // parse integer, float, string.
    Node* parsestats(int level); // <stats> -> <stat> <mStat>
    Node* parsestat(int level); // <stat> -> <if> | <for> .
    Node* parsemstat(int level); // <mStat> -> empty | <stats>
    Node* parseif(int level); // <if> -> if (<expr> <O> <expr>)
    Node* parsefor(int level); // <for> -> for (<expr> <O> <expr>)
    Node* parseo(int level); // <O> -> < | > | <= | >= | <>
    Node* parseassign(int level); // :=
    bool checkstats(token_t token); // check.
    Node* printerror(); 
    Node* parseoneassign(int level);
    Node* parseonevar(int level);
    Node* parsereturn(int level); // parse return.
    bool checkbuildin(token_t token);
    
};
#endif