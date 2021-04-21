
#ifndef CODE_GENERATION_H
#define CODE_GENERATION_H
// #include "llvm/Analysis/Passes.h"
// #include "llvm/ExecutionEngine/ExecutionEngine.h"
// #include "llvm/ExecutionEngine/SectionMemoryManager.h"
// #include "llvm/IR/DataLayout.h"
// #include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"


#include "node.h"
#include <string>
#include <stack>
#include <vector>
#include <map>


// context: contain a lot of core LLVM structure.
// Builder: make it easy to generate LLVM instruction. 
// module: contains functions and global variables.


class CodeGeneration{
public:
  CodeGeneration();
  ~CodeGeneration();
  void check_program_block(Node* node); // done.
  // define a procedure block.
  void check_procedure_block(Node* node); // done.
  // Function* procedure_declaration(Node* node);
  
  void check_vars_block(Node* node); // done.
  llvm::Value* check_expr(Node* node,bool is_load = false);
  llvm::Value* check_negation(Node* node,bool is_load);
  llvm::Value* check_R(Node* node,int index,bool is_load);
  void check_stats(Node* node);
  void check_stat(Node* node);
  void check_mstat(Node* node);
  void check_for_block(Node* node);
  void check_if_block(Node* node);
  void statement(Node* node);

  llvm::Value* check_o(int sign,llvm::Value* left,llvm::Value* right);
  void check_assign(Node* node);
  void check_return(Node* node); // done.
  void check_buildin(Node* node);
  void check_begin(Node* node);
  void check_end(Node* node);

  void traverse_children(Node* node);
  void traverse_child(Node* node);
  void traverse(Node* node);
  void check(llvm::Function* main_func);
  llvm::Type* getType(int x);
  void outputIR();
  void outputAssembly();
  void error(std::string a);
  llvm::Value* string_equal(llvm::Value* a,llvm::Value* b);
  // builtin function.
  void insert_builtin_function(llvm::Module* mode,llvm::IRBuilder<>* build);


  bool checkVar(Node* node);
  
public:
  llvm::LLVMContext *TheContext;
  llvm::Module *TheModule;
  llvm::IRBuilder<> *Builder;

  std::vector<token_t> var_local;
  std::map<std::string,llvm::Value*> m_local_addr;
  std::stack<std::vector<token_t>> var_stack;

  std::map<std::string,llvm::Value*> m_global_addr;

  std::stack<std::vector<token_t>> func_stack;
  std::map<std::string,llvm::Function*> func_addr;
  llvm::Function* current_func;
  token_t cur_func;

  // std::stack<llvm::Function*> func_stack;
  // llvm::Function* current_func;
  // std::vector<token_t> v_local;
  // std::stack<std::vector<token_t>> v_stack;
  // std::map<std::string,llvm::Value*> m_local;
  // std::map<std::string,llvm::Value*> m_global;
};
#endif