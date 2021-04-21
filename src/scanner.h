#ifndef SCANNER_H
#define SCANNER_H
#include "token.h"
#include <map>
#include <fstream>

class Scanner{
public:
    Scanner(std::string filename);
    ~Scanner();
    token_t getToken();
    int getRow();
private:
    std::ifstream fp;
    int row_num;
    std::map<std::string,int> m; 
    int m_index;
    int last_token_type;
    void create_reserved_table();
    void Init(std::string filename);
    void close_file();
    int lookup_reserved(std::string& s);
    void add_symtab(std::string& s);
    void skipComment(int &nextch);
    void increaseColumn(int ch);
    token_t ScanOneToken();
};
#endif