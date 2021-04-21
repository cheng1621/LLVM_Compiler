
#include<iostream>
#include<fstream>
#include<string>
#include<algorithm>
#include "scanner.h"
#include "error.h"
using namespace std;
Scanner::Scanner(std::string filename){
    Init(filename);
}
Scanner::~Scanner(){
    close_file();
    m.clear();
}
token_t Scanner::getToken(){
    token_t token = ScanOneToken();
    while (token.type == T_UNKNOWN) token = ScanOneToken();
    return token;
}
int Scanner::getRow(){
    return row_num;
}
void Scanner::Init(std::string filename){
    fp.open(filename,ifstream::in);
    if (!fp.good()){
        cout << "the file is not existed" << endl;
        exit(1);
    }
    create_reserved_table();
    row_num = 1;
    last_token_type = T_UNKNOWN;
}
void Scanner::create_reserved_table(){
    m["and"] = T_AND;
    m["for"] = T_FOR;
    m["to"] = T_TO;
    m["downto"] = T_DOWNTO;
    m["do"] = T_DO;
    m["if"] = T_IF;
    m["then"] = T_THEN;
    m["else"] = T_ELSE;
    m["while"] = T_WHILE;
    m["repeat"] = T_REPEAT;
    m["until"] = T_UNTIL;
    m["function"] = T_FUNCTION;
    m["procedure"] = T_PROCEDURE;
    m["begin"] = T_BEGIN;
    m["end"] = T_END_KEYWORD;
    m["program"] = T_PROGRAM;
    m["Program"] = T_PROGRAM;
    m["PROGRAM"] = T_PROGRAM;
    m["forward"] = T_FORWARD;
    m["goto"] = T_GOTO;
    m["or"] = T_OR;
    m["not"] = T_NOT;
    m["case"] = T_CASE;
    m["otherwise"] = T_OTHERWISE;
    m["with"] = T_WITH;
    m["in"] = T_IN;

    m["write"] = T_WRITE;
    m["writeln"] = T_WRITELN;
    m["read"] = T_READ;
    m["readln"] = T_READLN;

    m["type"] = T_TYPE;
    m["variable"] = T_VAR;
    m["packed"] = T_PACKED;
    m["array"] = T_ARRAY;
    m["of"] = T_OF;
    m["record"] = T_RECORD;
    m["const"] = T_CONST;
    m["file"] = T_FILE;
    m["set"] = T_SET;
    m["string"] = T_STRING_KEYWORD;
    m["nil"] = T_NIL;
    m["integer"] = T_INTEGER_KEYWORD;
    m["float"] = T_FLOAT_KEYWORD;
    m["global"] = T_GLOBAL;
    m["is"] = T_IS;
    m["IS"] = T_IS;
    m["bool"] = T_BOOL;
    m["return"] = T_RETURN;
    m["true"] = T_TRUE;
    m["false"] = T_FALSE;
    // for builtin function.
    m["getinteger"] = T_GETINTEGER;
    m["getbool"] = T_GETBOOL;
    m["getfloat"] = T_GETFLOAT;
    m["getstring"] = T_GETSTRING;
    m["putbool"] = T_PUTBOOL;
    m["putinteger"] = T_PUTINTEGER;
    m["putfloat"] = T_PUTFLOAT;
    m["putstring"] = T_PUTSTRING;
    m["sqrt"] = T_SQRT;
}
void Scanner::close_file(){
    fp.close();
}
int Scanner::lookup_reserved(string& s){
    if (m.find(s) != m.end()){
        return m[s];
    } else {
        return -1;
    }
}
void Scanner::add_symtab(string& s){
    // int temp = m_index;
    m[s] = T_IDENTIFIER;
    // m_index = temp;
}
void Scanner::skipComment(int &nextch){
    int ch;
    if (nextch == '/'){
        ch = fp.get();
        increaseColumn(ch);
        while (ch != '\n' && ch != T_END && fp.good()){
            ch = fp.get();
        }
        increaseColumn(ch);
    } else {
        int num_comment = 1;
        ch = fp.get();
        increaseColumn(ch);
        while (ch != fp.eof() && fp.good()){
            ch = fp.get();
            if (ch == T_END && num_comment != 0) error(ERR_ENDOFCOMMENT,row_num);
            increaseColumn(ch);
            if (ch == '*'){
                ch = fp.get();
                if (ch == '/') num_comment--;
                if (num_comment == 0) break;
                if (ch == T_END && num_comment != 0) error(ERR_ENDOFCOMMENT,row_num);
                increaseColumn(ch);
            } else if (ch == '/'){
                ch = fp.get();
                if (ch == '*') num_comment++;
                if (ch == T_END && num_comment != 0) error(ERR_ENDOFCOMMENT,row_num);
                increaseColumn(ch);
            }
        }
    }
}
void Scanner::increaseColumn(int ch){
    if (ch == '\n'){
        row_num = row_num + 1;
    }
}
token_t Scanner::ScanOneToken(){
    int i, ch, nextch;
    token_t* token = new token_t();
    ch = fp.get();
    increaseColumn(ch);
    while (isspace(ch)){
        ch = fp.get();
        increaseColumn(ch);
    }
    // scanner
    switch (ch){
        case '/':   // check if it is the comment line.
            nextch = fp.get();
            increaseColumn(nextch);
            if (nextch == '/' || nextch == '*'){
                skipComment(nextch);
                token->type = T_UNKNOWN;
                last_token_type = token->type;
                return *token;
            } else {
                fp.unget();
                token->type = ch;
                last_token_type = token->type;
                return *token;
            }
        case ':':
            nextch = fp.get();
            increaseColumn(nextch);
            if (nextch == '='){
                token->type = T_ASSIGN;
                last_token_type = token->type;
                return *token;
            } else {
                fp.unget();
                token->type = ch;
                last_token_type = token->type;
                return *token;
            }
        case '.':
            nextch = fp.get();
            increaseColumn(nextch);
            if (nextch == '.'){
                token->type = T_DOT_DOT;
                last_token_type = token->type;
                return *token;
            } else {
                fp.unget();
                token->type = ch;
                last_token_type = token->type;
                return *token;
            }
        case '<':
            nextch = fp.get();
            increaseColumn(nextch);
            if (nextch == '='){
                token->type = T_LESS_OR_EQUAL;
                last_token_type = token->type;
                return *token;
            } else if (nextch == '>'){
                token->type = T_NOT_EQUAL;
                last_token_type = token->type;
                return *token;
            } else {
                fp.unget();
                token->type = ch;
                last_token_type = token->type;
                return *token;
            }
        case '>':
            nextch = fp.get();
            increaseColumn(nextch);
            if (nextch == '='){
                token->type = T_GREATER_OR_EQUAL;
                last_token_type = token->type;
                return *token;
            } else {
                fp.unget();
                token->type = ch;
                last_token_type = token->type;
                return *token;
            }
        case '=':
            nextch = fp.get();
            increaseColumn(nextch);
            if (nextch == '='){
                token->type = T_DOUBLE_EQUAL;
                last_token_type = token->type;
                return *token;
            } else {
                fp.unget();
                token->type = ch;
                last_token_type = token->type;
                return *token;
            }
        case '\"':
            token->type = T_STRING;
            token->StringValue = "";
            while (true){
                ch = fp.get();
                increaseColumn(nextch);
                if (ch == '\"') break;
                token->StringValue += (char)ch;
            }
            if (ch == '\"') return *token;
            else 
                error(ERR_INVALIDSYMBOL,row_num);
        case ';': case ',':
        case '(': case ')': case '[':
        case ']': case '{': case '}':
        case '!': case '\'':
        case '*': case '+': case '-':
        case '|': case '&': case '\?':
        case '^': case '%': case '#':
            token->type = ch;
            last_token_type = token->type;
            return *token;
        case 'A': case 'B': case 'C':
        case 'D': case 'E': case 'F':
        case 'G': case 'H': case 'I':
        case 'J': case 'K': case 'L':
        case 'M': case 'N': case 'O':
        case 'P': case 'Q': case 'R':
        case 'S': case 'T': case 'U':
        case 'V': case 'W': case 'X':
        case 'Y': case 'Z':
        case 'a': case 'b': case 'c':
        case 'd': case 'e': case 'f':
        case 'g': case 'h': case 'i':
        case 'j': case 'k': case 'l':
        case 'm': case 'n': case 'o':
        case 'p': case 'q': case 'r':
        case 's': case 't': case 'u':
        case 'v': case 'w': case 'x':
        case 'y': case 'z':
            if (last_token_type == T_VAR || last_token_type == T_PROCEDURE || last_token_type == T_PROGRAM)
                token->type = T_IDENTIFIER;
            else
                token->type = T_UNKNOWN;
            token->StringValue = "";
            token->StringValue += (char)ch;
            i = 1;
            while (true){
                ch = fp.get();
                increaseColumn(nextch);
                if (!isupper(ch) && !islower(ch) && !isdigit(ch) && ch != '_') break;
                token->StringValue += (char)ch;
                i++;
            }
            fp.unget();
            // if (last_token_type == T_VAR)
                transform(token->StringValue.begin(),token->StringValue.end(),token->StringValue.begin(),::tolower);
            if (lookup_reserved(token->StringValue) != -1){
                token->type = lookup_reserved(token->StringValue);
            } else {
                if (token->type == T_IDENTIFIER){
                    add_symtab(token->StringValue);
                } else {
                    error(ERR_INVALIDSYMBOL,row_num);
                }
            }
            last_token_type = token->type;
            return *token;
        case '0': case '1': case '2': case '3': case '4':
        case '5': case '6': case '7': case '8': case '9':
            token->type = T_INTEGER;
            token->intValue = ch - '0';
            while (isdigit(ch = fp.get()))
                token->intValue = token->intValue * 10 + ch - '0';
            if (ch == '.'){
                token->type = T_FLOAT;
                token->doubleValue = token->intValue;
                token->intValue = 0;
                float temp;
                temp = 0.1;
                while (isdigit(ch = fp.get())){
                    token->doubleValue = token->doubleValue + temp * (ch - '0');
                    temp = temp * 0.1;
                }
            }
            
            fp.unget();
            last_token_type = token->type;
            return *token;
        case EOF:
            token->type = T_END;
            last_token_type = token->type;
            return *token;
        default:
            token->intValue = ch;
            token->type = T_UNKNOWN;
            last_token_type = token->type;
            return *token;
    }
}