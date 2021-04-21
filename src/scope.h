

#ifndef SCOPE_H
#define SCOPE_H
#include<vector>
#include "token.h"
#include <map>
#include <string>
#include "llvm/IR/Value.h"
class scope{
public:
    std::vector<token_t> v; // local variable
    std::map<std::string,llvm::Value*> m_var; // local variable and its address.
    std::map<std::string,llvm::Function*> m_func; // the function in it.
}