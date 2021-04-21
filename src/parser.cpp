#include<iostream>
#include "parser.h"
// #include "scanner.h"
// #include "token.h"
#include "node_table.h"
#include <string>
using namespace std;

Parser::Parser(string filename){
    scanner = new Scanner(filename);
    isglobal = false;
    is_buildin = false;
    is_assign = false;
}
Parser::~Parser(){
    scanner->~Scanner();
    var_set.clear();
    global_set.clear();
}
Node* Parser::Parse(){
    int level = 1;
    Node* root = Node::define(ROOT,level);
    root->add_children(parseS());
    if (token.type == T_END_KEYWORD){
        root->add_children(Node::define(END,level));
        token = scanner->getToken();
        if (token.type == T_PROGRAM){
            token = scanner->getToken();
            if (token.type == '.')
                return root;
        }
    }
    printerror();
}
// program Math is. parse it with `START` label.
Node* Parser::parseS(){
    int level = 1;
    token = scanner->getToken();
    if (token.type == T_PROGRAM){
        token = scanner->getToken();
        if (token.type == T_IDENTIFIER){
            token_t tt = token;
            token = scanner->getToken();
            if (token.type == T_IS){
                Node* root = Node::define(START,level);
                token = scanner->getToken(); // get the new token.
                root->add_token(tt);
                root->add_children(parsestats(level));
                return root;
            }
        }
    }
    printerror();
}
// variable a : Integer[3]. at this time, I don't know
// what does global do.
Node* Parser::parsevars(int level){
    if (token.type == T_GLOBAL){
        token = scanner->getToken();
        isglobal = true;
    }
    if (token.type == T_VAR){
        level = level + 1;
        Node* node = Node::define(VARS,level);
        token = scanner->getToken();
        if (token.type == T_IDENTIFIER){
            // new 
            if (isglobal){
                int j = 0;
                for (;j < global_set.size();j++){
                    if (global_set[j].StringValue == token.StringValue) break;
                }
                if (j < global_set.size()) printerror();
            }
            token_t tt = scanner->getToken(); // ':'
            if (tt.type != ':') printerror();
            tt = scanner->getToken();
            if (tt.type == T_INTEGER_KEYWORD || tt.type == T_STRING_KEYWORD || tt.type == T_BOOL || tt.type == T_FLOAT_KEYWORD){
                if (tt.type == T_INTEGER_KEYWORD) token.tm = INTEGER;
                if (tt.type == T_STRING_KEYWORD) token.tm = STRING;
                if (tt.type == T_BOOL) token.tm = BOOL;
                if (tt.type == T_FLOAT_KEYWORD) token.tm = FLOAT;
                tt = scanner->getToken(); // []
                if (tt.type == '['){
                    tt = scanner->getToken();
                    token.numCount = tt.intValue;
                    tt = scanner->getToken();
                    if (tt.type != ']') printerror();
                    tt = scanner->getToken(); // ';'
                    if (tt.type == ';'){
                        if (isglobal) token.isGlobal = true;
                        node->add_token(token);
                        if (isglobal){
                            global_set.push_back(token);
                            isglobal = false;
                        } else {
                            var_set.push_back(token);
                        }
                        token = scanner->getToken();
                        // node->add_children(parsevars(level));
                        return node;
                    }
                } else if (tt.type == ';'){
                    if (isglobal) token.isGlobal = true;
                    node->add_token(token);
                    if (isglobal){
                        global_set.push_back(token);
                        isglobal = false;
                    } else {
                        var_set.push_back(token);
                    }
                    token = scanner->getToken();
                    return node;
                }
            }
        }
    } else {
        return NULL;
    }
}
// parse block. <begin> <end>.
Node* Parser::parseblock(int level){
    if (token.type == T_BEGIN){
        level = level + 1;
        Node* node = Node::define(BLOCK,level);
        // need to be detailed.(start)
        token = scanner->getToken();
        // node->add_children(parsevars(level));
        node->add_children(parsestats(level));
        return node;
    }
    printerror();
}
// procedure Fib : integer(variable val : integer)
Node* Parser::parseprocedure(int level){
    if (token.type == T_PROCEDURE){
        Node* node = Node::define(PROCEDURE,level);

        level = level + 1;
        token = scanner->getToken();
        if (token.type == T_IDENTIFIER){
            token_t tt = scanner->getToken();
            if (tt.type != ':') printerror();
            tt = scanner->getToken();
            if (tt.type == T_INTEGER_KEYWORD) token.tm = INTEGER;
            if (tt.type == T_STRING_KEYWORD) token.tm = STRING;
            if (tt.type == T_BOOL) token.tm = BOOL;
            if (tt.type == T_FLOAT_KEYWORD) token.tm = FLOAT;
            // new
            Procedurecheck.push(token);
            var_set.push_back(token);
            // new here.
            st.push(var_set);
            var_set.clear();
            var_set.push_back(token);
            // end
            // end
            node->add_token(token);
            token = scanner->getToken();
            if (token.type != '(') printerror();
            token = scanner->getToken();
            if (token.type != ')' && token.type == T_VAR){
                token = scanner->getToken();
                if (token.type == T_IDENTIFIER){
                    token_t tt = scanner->getToken();
                    if (tt.type != ':') printerror();
                    tt = scanner->getToken();
                    if (tt.type == T_INTEGER_KEYWORD) token.tm = INTEGER;
                    if (tt.type == T_STRING_KEYWORD) token.tm = STRING;
                    if (tt.type == T_BOOL) token.tm = BOOL;
                    if (tt.type == T_FLOAT_KEYWORD) token.tm = FLOAT;
                    node->add_token(token);
                    // new.
                    var_set.push_back(token);
                    // end.
                    token = scanner->getToken();
                    if (token.type != ')') printerror();
                    token = scanner->getToken();
                    // node->add_children(parseprocedure(level));
                    // node->add_children(parsevars(level));
                    // node->add_children(parseblock(level));
                    node->add_children(parsestats(level));
                    if (token.type == T_END_KEYWORD){
                        node->add_children(Node::define(END,level));
                        token = scanner->getToken();
                        if (token.type == T_PROCEDURE){
                            token = scanner->getToken();
                            if (token.type == ';'){
                                // new
                                var_set = st.top();
                                st.pop();
                                // end
                                token = scanner->getToken();
                                return node;
                            }
                        }
                    }
                }
            } else if (token.type == ')'){
                token = scanner->getToken();
                node->add_children(parsestats(level));
                if (token.type == T_END_KEYWORD){
                    node->add_children(Node::define(END,level));
                    token = scanner->getToken();
                    if (token.type == T_PROCEDURE){
                        token = scanner->getToken();
                        if (token.type == ';'){
                            // new
                            var_set = st.top();
                            st.pop();
                            // end
                            token = scanner->getToken();
                            return node;
                        }
                    }
                }
                return node;
            }
        }
    } else {
        return NULL;
    }
}
// parse assign once.
Node* Parser::parseoneassign(int level){
    if (token.type == T_IDENTIFIER){
        // start. look through.
        int i = 0;
        for (;i < var_set.size();i++){
            if (var_set[i].StringValue == token.StringValue) break;
        }
        
        int j = 0;
        for (;j < global_set.size();j++){
            if (global_set[j].StringValue == token.StringValue) break;
        }
        if (i == var_set.size() && j == global_set.size()) printerror();
        if (j < global_set.size()) var = global_set[j];
        if (i < var_set.size()) var = var_set[i];
        // end
        level = level + 1;
        Node* node = Node::define(ASSIGN,level);
        node->add_token(token);
        Assigncheck = token;
        token = scanner->getToken();
        if (token.type == T_ASSIGN){
            token = scanner->getToken();
            node->add_children(parseexpr(level));
            if (token.type == ';'){
                token = scanner->getToken();
                return node;
            }
        }
    }
    printerror();
}
// parse assign symbol. ':='
Node* Parser::parseassign(int level){
    if (token.type == T_IDENTIFIER){
        is_assign = true;
        // start. look through.
        int i = 0;
        for (;i < var_set.size();i++){
            if (var_set[i].StringValue == token.StringValue) break;
        }
        
        int j = 0;
        for (;j < global_set.size();j++){
            if (global_set[j].StringValue == token.StringValue) break;
        }
        if (i == var_set.size() && j == global_set.size()) printerror();
        if (j < global_set.size()) var = global_set[j];
        if (i < var_set.size()) var = var_set[i];
        // end
        // level = level + 1;
        Node* node = Node::define(ASSIGN,level);
        node->add_token(token);
        // new 
        Assigncheck = var;
        // end;
        token = scanner->getToken();
        if (token.type == '[') {
            token = scanner->getToken();
            if (token.type == T_INTEGER){
                node->add_token(token);
                token = scanner->getToken();
                if (token.type != ']') printerror(); 
                token = scanner->getToken();
            }
        }
        if (token.type == T_ASSIGN){
            level++;
            token = scanner->getToken();
            node->add_children(parseexpr(level));
            is_assign = false;
            if (token.type == ';'){
                token = scanner->getToken();
                // node->add_children(parseassign(level));
            }
            return node;
        }
    } else {
        return NULL;
    }
}
// parse expressions. <expr>.
Node* Parser::parseexpr(int level){
    level++;
    Node* node = Node::define(EXPR,level);
    // if (token.type == '('){
    //     token = scanner->getToken();
    // }
    node->add_children(parsenegation(level));
    if (token.type == '+' || token.type == '-' || token.type == '*' || token.type == '/' \
    || token.type == T_DOUBLE_EQUAL || token.type == T_LESS_OR_EQUAL \
    || token.type == T_GREATER_OR_EQUAL || token.type == T_NOT_EQUAL \
    || token.type == '<' || token.type == '>'){
        node->add_token(token);
        token = scanner->getToken();
        node->add_children(parseexpr(level));
        return node;
    }
    // if (token.type == ')') {
    //     token = scanner->getToken();
    // }
    return node;
}
// check the symbol is - or not.
Node* Parser::parsenegation(int level){
    level = level + 1;
    Node* node = Node::define(HASH,level);
    if (token.type == '-'){
        node->add_token(token);
        token = scanner->getToken();
        node->add_children(parseR(level));
        return node;
    } else {
        node->add_children(parseR(level));
        return node;
    }
}
// parse interger expression, bool expression. atomic.
Node* Parser::parseR(int level){
    level = level + 1;
    Node* node = Node::define(R,level);
    if (token.type == T_IDENTIFIER){
        // start. look through.
        if (!is_buildin){
            int i = 0;
            for (;i < var_set.size();i++){
                if (var_set[i].StringValue == token.StringValue) break;
            }
            int j = 0;
            for (;j < global_set.size();j++){
                if (global_set[j].StringValue == token.StringValue) break;
            }
            
            if (i == var_set.size() && j == global_set.size()) printerror();
            if (j < global_set.size()) {
                var = global_set[j];
                token = var;
            }
            if (i < var_set.size()) {
                var = var_set[i];
                token = var;
            }
            if (is_assign && Assigncheck.tm != var.tm) printerror();
            // end.
        } else {
            int i = 0;
            for (;i < var_set.size();i++){
                if (var_set[i].StringValue == token.StringValue) {
                    token = var_set[i];
                    break;
                }
            }
            int j = 0;
            for (;j < global_set.size();j++){
                if (global_set[j].StringValue == token.StringValue) {
                    token = global_set[j];
                    break;
                }
            }
        }
        node->add_token(token);
        token = scanner->getToken();
        if (token.type == '['){
            token = scanner->getToken();
            if (token.type == T_INTEGER){
                node->add_token(token);
                // new
                if (var.numCount <= token.intValue) printerror();
                // end.
                token = scanner->getToken();
                if (token.type == ']'){
                    token = scanner->getToken();
                    return node;
                }
            }
        } else if (token.type == '('){
            token = scanner->getToken();
            if (token.type != ')'){
                Assigncheck = var;
                node->add_children(parseexpr(level));
            }
            if (token.type == ')'){
                token = scanner->getToken();
                return node;
            }
        } else {
            return node;
        }
    } else if (token.type == T_INTEGER){
        if (!is_buildin){
            if (is_assign && Assigncheck.tm != INTEGER) printerror();
        }
        node->add_token(token);
        token = scanner->getToken();
        return node;
    } else if (token.type == T_TRUE || token.type == T_FALSE){
        if (!is_buildin){
            if (is_assign && Assigncheck.tm != BOOL) printerror();
        }
        node->add_token(token);
        token = scanner->getToken();
        return node;
    } else if (token.type == T_STRING){
        if (!is_buildin){
            if (is_assign && Assigncheck.tm != STRING) printerror();
        }
        node->add_token(token);
        token = scanner->getToken();
        return node;
    } else if (token.type == T_FLOAT){
        if (!is_buildin){
            if (is_assign && Assigncheck.tm != FLOAT) printerror();
        }
        node->add_token(token);
        token = scanner->getToken();
        return node;
    }
    else if (checkbuildin(token)){
        is_buildin = true;
        node->add_token(token);
        token = scanner->getToken();
        if (token.type == '('){
            token = scanner->getToken();
            // if (token.type == T_IDENTIFIER || token.type == T_INTEGER || \
            //     token.type == T_STRING){
            //     node->add_token(token);
            //     token = scanner->getToken();
            // }
            if (token.type != ')'){
                node->add_children(parseexpr(level));
                is_buildin = false;
            }
            if (token.type == ')'){
                token = scanner->getToken();
                return node;
            }
        }
    }
    printerror();
}
bool Parser::checkbuildin(token_t token){
    if ((token.type >= 331) && (token.type <= 339)) return true;
    return false;
}
Node* Parser::parsereturn(int level){
    if (token.type == T_RETURN){
        level = level + 1;
        Node* node = Node::define(RETURN,level);
        token = scanner->getToken();
        if (token.type == '(') token = scanner->getToken();
        node->add_children(parseexpr(level));
        if (token.type == ')') token = scanner->getToken();
        if (token.type == ';'){
            token = scanner->getToken();
            return node;
        }
        // if (token.type == T_IDENTIFIER){
            
        //     // new look through the whole set.
        //     int i = 0;
        //     for (;i < var_set.size();i++){
        //         if (var_set[i].StringValue == token.StringValue) break;
        //     }
            
        //     int j = 0;
        //     for (;j < global_set.size();j++){
        //         if (global_set[j].StringValue == token.StringValue) break;
        //     }
        //     if (i == var_set.size() && j == global_set.size()) printerror();
        //     if (j < global_set.size()) {
        //         var = global_set[j];
        //         token = var;
        //     }
        //     if (i < var_set.size()) {
        //         var = var_set[i];
        //         token = var;
        //     }
        //     node->add_token(token);
        //     if (Procedurecheck.top().tm == T_INTEGER && (var.tm != T_BOOL || var.tm != T_INTEGER)) printerror();
        //     else if (Procedurecheck.top().tm == T_BOOL && (var.tm != T_BOOL || var.tm != T_INTEGER)) printerror();
        //     // if (Procedurecheck.top().tm != var.tm) printerror();
        //     token = scanner->getToken();
        //     if (token.type == ';'){
        //         token = scanner->getToken();
        //         return node;
        //     }
        // }
    } 
    printerror();
}
// parse stats.<stats> -> <stat> <mStat>.
Node* Parser::parsestats(int level){
    level = level + 1;
    Node* node = Node::define(STATS,level);
    node->add_children(parsestat(level));
    node->add_children(parsemstat(level));
    return node;
}
// parse mstat. <mStat> -> empty | <stats>
Node* Parser::parsemstat(int level){
    if (checkstats(token)){
        level = level + 1;
        Node* node = Node::define(MSTAT,level);
        node->add_children(parsestats(level));
        return node;
    } else {
        return NULL;
    }
}
// check
bool Parser::checkstats(token_t token){
    if (token.type == T_IF || token.type == T_BEGIN || token.type == T_FOR || \
    token.type == T_PROCEDURE || token.type == T_RETURN || \
    token.type == T_IDENTIFIER || token.type == T_GLOBAL || \
    token.type == T_VAR){
        return true;
    }
    return false;
}
// parse stat. <stat> -> <if> | <for> | <procedure>
Node* Parser::parsestat(int level){
    level = level + 1;
    Node* node = Node::define(STAT,level);
    if (token.type == T_BEGIN){
        node->add_children(Node::define(BEGIN,level));
        node->add_children(parseblock(level));
        return node;
    }
    if (token.type == T_IF){
        node->add_children(parseif(level));
        return node;
    }
    if (token.type == T_FOR){
        node->add_children(parsefor(level));
        return node;
    }
    if (token.type == T_PROCEDURE){
        node->add_children(parseprocedure(level));
        return node;
    }
    if (token.type == T_IDENTIFIER){
        node->add_children(parseassign(level));
        return node;
    }
    if (token.type == T_RETURN){
        node->add_children(parsereturn(level));
        return node;
    }
    if (token.type == T_GLOBAL || token.type == T_VAR){
        node->add_children(parsevars(level));
        return node;
    }
    printerror();
}
// parse if
Node* Parser::parseif(int level){
    if (token.type == T_IF){
        level = level + 1;
        Node* node = Node::define(IF,level);
        token = scanner->getToken();
        if (token.type == '('){ // next token must be (.
            token = scanner->getToken();
            node->add_children(parseexpr(level));
            // if (token.type != ')'){
            //     node->add_children(parseo(level));
            //     node->add_children(parseexpr(level));
            // }
            if (token.type == ')'){
                token = scanner->getToken();
                if (token.type == T_THEN){
                    token = scanner->getToken();
                    node->add_children(parsestats(level));
                    if (token.type == T_ELSE){
                        Node* node1 = Node::define(ELSE,level);
                        node->add_children(node1);
                        // node->add_children(Node::define(ELSE,level));
                        token = scanner->getToken();
                        node1->add_children(parsestats(level));
                        if (token.type == T_END_KEYWORD){
                            token = scanner->getToken();
                            if (token.type == T_IF){
                                token = scanner->getToken();
                                if (token.type == ';'){
                                    token = scanner->getToken();
                                    return node;
                                }
                            }
                        }
                    } else if (token.type == T_END_KEYWORD){
                        token = scanner->getToken();
                        if (token.type == T_IF){
                            token = scanner->getToken();
                            if (token.type == ';'){
                                token = scanner->getToken();
                                return node;
                            }
                        }
                    }
                }
            }
        }
    }
    printerror();
}
// parse operation.
Node* Parser::parseo(int level){
    level = level + 1;
    Node* node = Node::define(OPERATER,level);
    if (token.type == T_DOUBLE_EQUAL || token.type == T_LESS_OR_EQUAL \
    || token.type == T_GREATER_OR_EQUAL || token.type == T_NOT_EQUAL \
    || token.type == '<' || token.type == '>'){
        node->add_token(token);
        token = scanner->getToken();
        return node;
    }
    printerror();
}
// parse for loop.
Node* Parser::parsefor(int level){
    if (token.type == T_FOR){
        level = level + 1;
        Node* node = Node::define(FOR,level);
        token = scanner->getToken();
        if (token.type == '('){
            token = scanner->getToken();
            // replace with one assign.
            node->add_children(parseoneassign(level));
            node->add_children(parseexpr(level));
            // node->add_children(parseo(level));
            // node->add_children(parseexpr(level));
            if (token.type == ')'){
                token = scanner->getToken();
                node->add_children(parsestats(level));
                if (token.type == T_END_KEYWORD){
                    token = scanner->getToken();
                    if (token.type == T_FOR){
                        token = scanner->getToken();
                        if (token.type == ';'){
                            token = scanner->getToken();
                            return node;
                        }
                    }
                }
            }
        }
    }
    printerror();
}
// print error.
Node* Parser::printerror(){
    cout << token.type << " " << token.StringValue << endl;
    cerr << "Parser print error -> line: " << scanner->getRow() << endl;
    exit(1);
}