

#include "code_generation.h"
#include "node_table.h"
#include "token.h"
#include <string>
#include <iostream>

#include "llvm/IR/Constants.h"
#include "llvm/IR/IRPrintingPasses.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/IR/Verifier.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/Host.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/TargetRegistry.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Target/TargetMachine.h"


CodeGeneration::CodeGeneration(){
  TheContext = new llvm::LLVMContext();
  Builder = new llvm::IRBuilder<>(*TheContext);

}
CodeGeneration::~CodeGeneration(){
    delete Builder;
    // delete TheModule;
    delete TheContext;
}
void CodeGeneration::error(std::string a){
    std::cout << a << std::endl;
    exit(0);
}
void CodeGeneration::outputAssembly(){
    auto TargetTriple = llvm::sys::getDefaultTargetTriple();
    TheModule->setTargetTriple(TargetTriple);
    // initialize all targets for object code
    llvm::InitializeAllTargetInfos();
    llvm::InitializeAllTargets();
    llvm::InitializeAllTargetMCs();
    llvm::InitializeAllAsmParsers();
    llvm::InitializeAllAsmPrinters();
    std::string Error;
    auto Target = llvm::TargetRegistry::lookupTarget(TargetTriple, Error);

    auto CPU = "generic";
    auto Features = "";
    llvm::TargetOptions opt;
    auto RM = llvm::Optional<llvm::Reloc::Model>();
    auto TargetMachine = Target->createTargetMachine(TargetTriple, CPU, Features, opt, RM);

    TheModule->setDataLayout(TargetMachine->createDataLayout());
    // 生成目标代码
    auto Filename = "output.s";
    std::error_code EC;
    llvm::raw_fd_ostream dest(Filename, EC, llvm::sys::fs::OF_None);

    if (EC) {
        llvm::errs() << "Could not open output file: " << EC.message();
        return ;
    }
    llvm::legacy::PassManager pass;
    auto FileType = llvm::CGFT_AssemblyFile;
    TargetMachine->addPassesToEmitFile(pass, dest, nullptr, FileType);
    
    std::error_code errCode2;
    llvm::raw_fd_ostream dest2("output.ll", errCode2, llvm::sys::fs::OF_None);
    pass.add(llvm::createPrintModulePass(dest2));
    pass.run(*TheModule);
    dest.flush();
    dest2.flush(); // 更新到磁盘.
    
}
// 3 main functions. traverse, traverse_children, traverse_child.
// traverse the parser tree.
// identify the type of each block when node goes its way to children.
void CodeGeneration::traverse(Node* node){
    if (node == NULL) return;
    check_program_block(node);
    // // check_for_block(node);
    check_if_block(node);
    check_procedure_block(node);
    check_vars_block(node);
    check_begin(node);
    check_end(node);
    check_assign(node);
    check_return(node);
    check_for_block(node);
    if (node->label != IF && node->label != FOR && node->label != ASSIGN && \
        node->label != BEGIN && node->label != END)
        traverse_children(node);
}
// traverse the children.
void CodeGeneration::traverse_children(Node* node){
    std::vector<Node*> children = node->children;
    for (unsigned int i = 0; i < children.size(); i++) {
        traverse(children.at(i));
    }
}
// traverse the child.
void CodeGeneration::traverse_child(Node* node)
{
    if (node == NULL) return;
    
}

// procedure
void CodeGeneration::check_procedure_block(Node* node){
    if (node->label == PROCEDURE){
        token_t Procedure_name = node->token_arr[0];
        
        // (start).push var_local into var_stack and clear var_local;
        var_stack.push(var_local);
        var_local.clear();
        // (end).
        func_stack.push(node->token_arr);

    }
}

// parse variables.
void CodeGeneration::check_vars_block(Node* node){
    if (node->label == VARS){
        token_t Variable_name = node->token_arr[0]; // get the var name.
        if (Variable_name.isGlobal){
            llvm::Type *ty = getType(Variable_name.tm);
            if (Variable_name.numCount > 0){
                ty = llvm::ArrayType::get(ty, Variable_name.numCount);
            }
            llvm::Constant *initValue = llvm::Constant::getNullValue(ty);
            llvm::Value *addr = new llvm::GlobalVariable(
                *TheModule,
                ty,
                false,
                llvm::GlobalValue::ExternalLinkage,
                initValue,
                Variable_name.StringValue);
            m_global_addr[Variable_name.StringValue] = addr;
        } else
            var_local.push_back(Variable_name);
    }
}
// program block.
void CodeGeneration::check_program_block(Node* node){
    if (node->label == START){
        // initialization the module, make module name same as the program name.
        TheModule = new llvm::Module(node->token_arr[0].StringValue, *TheContext);
        insert_builtin_function(TheModule,Builder);
        // llvm::FunctionType *ft = llvm::FunctionType::get(Builder->getInt32Ty(),false);
        // llvm::Function *func = llvm::Function::Create(ft, llvm::Function::ExternalLinkage, "main", TheModule);
        // llvm::BasicBlock *entry = llvm::BasicBlock::Create(*TheContext, "entry", func);
        // Builder->SetInsertPoint(entry);
        func_stack.push(node->token_arr);
        // current_func = func;
    }
}
// check return 
void CodeGeneration::check_return(Node* node){
    if (node->label == RETURN){
        llvm::Value* temp = check_expr(node->children[0],true);
        // llvm::Value* temp = Builder->CreateLoad(getType(node->token_arr[0].tm), m_local_addr[node->token_arr[0].StringValue]);
        // llvm::Value* temp = Builder->CreateLoad(t->getType(), );
        if (func_stack.size() > 1){
            if (cur_func.tm == INTEGER){
                temp = Builder->CreateIntCast(
                temp,
                Builder->getInt32Ty(),
                false);
            }
            if (cur_func.tm == BOOL){
                // if (node->token_arr[0].tm == INTEGER){
                    llvm::Value *zeroVal = llvm::ConstantInt::get(
                    *TheContext,
                    llvm::APInt(32, 0, true));
                    temp = Builder->CreateICmpNE(
                    temp, 
                    zeroVal);
                // }
            }
            if (cur_func.tm == FLOAT){
                // if (node->token_arr[0].tm == INTEGER){
                    temp = Builder->CreateSIToFP(
                    temp,
                    Builder->getFloatTy());
                // }
            }
        }
        Builder->CreateRet(temp);
        return;
    }
}
// check begin.
void CodeGeneration::check_begin(Node* node){
    if (node->label == BEGIN){
        // define a procedure.
        std::vector<token_t> pro = func_stack.top();
        // set up current function.
        cur_func = pro[0];
        std::vector<llvm::Type*> params;
        if (pro.size() > 1)
            params.push_back(getType(pro[1].tm));
        llvm::FunctionType *ft = llvm::FunctionType::get(getType(pro[0].tm), params, false);
        if (func_stack.size() == 1) pro[0].StringValue = "main";
        llvm::Function *func = llvm::Function::Create(ft, llvm::Function::ExternalLinkage, pro[0].StringValue, TheModule);
        func_addr[pro[0].StringValue] = func;
        for (auto &p: func->args()) {
            p.setName(pro[1].StringValue);
        }
        llvm::BasicBlock *BB = llvm::BasicBlock::Create(*TheContext, "entry", func);
        Builder->SetInsertPoint(BB);
        
        if (pro.size() > 1){
            llvm::Type *ty = getType(pro[1].tm);
            llvm::Value *addr = Builder->CreateAlloca(
                ty,
                nullptr,
                pro[1].StringValue);
            m_local_addr[pro[1].StringValue] = addr;
            auto arg = func->arg_begin();
            llvm::Value* agrVal = arg++;
            Builder->CreateStore(agrVal,addr);
        }

        for (int i = 0;i < var_local.size();i++){
            llvm::Type *ty = getType(var_local[i].tm);
            if (var_local[i].numCount > 0)
                ty = llvm::ArrayType::get(ty, var_local[i].numCount);
            llvm::Value *addr = Builder->CreateAlloca(
                ty,
                nullptr,
                var_local[i].StringValue);
            m_local_addr[var_local[i].StringValue] = addr;
        }
        if (!var_stack.empty()){
            var_local = var_stack.top();
            var_stack.pop();
        }
        // set up current function,
        current_func = func;
    }
}
void CodeGeneration::check_end(Node* node){
    if (node->label == END){
        func_stack.pop();
        // (todo) check if there is a return statement in current function.
        

        // return 0 at the end of the program.
        if (func_stack.empty()){
            llvm::Value *retVal = llvm::ConstantInt::get(
            *TheContext,
            llvm::APInt(32, 0, true));
            Builder->CreateRet(retVal);
        }
    }
}
// check assign block.
void CodeGeneration::check_assign(Node* node){
    if (node->label == ASSIGN){
        token_t v = node->token_arr[0]; // get the token needs to be assigned.
        llvm::Value* dest;
        llvm::Value* var = check_expr(node->children[0],true);
        if (m_local_addr.find(v.StringValue) != m_local_addr.end())
            dest = m_local_addr[v.StringValue];
        if (m_global_addr.find(v.StringValue) != m_global_addr.end()){
            dest = m_global_addr[v.StringValue];
        }
        // code generation for array.
        if (node->token_arr.size() > 1){
            token_t temp_index = node->token_arr[1];
            llvm::Value *index = llvm::ConstantInt::get(*TheContext,llvm::APInt(32, temp_index.intValue, true));
            llvm::Value *zeroVal = llvm::ConstantInt::get(
                                        *TheContext,
                                        llvm::APInt(32, 0, true));
            dest = Builder->CreateInBoundsGEP(
                dest,
                {zeroVal, index});
        }
        if (v.type == T_BOOL){
            if (dest->getType()->isIntegerTy()){
                llvm::Value *zeroVal = llvm::ConstantInt::get(
                    *TheContext,
                    llvm::APInt(32, 0, true));
                var = Builder->CreateICmpNE(
                    var,
                    zeroVal);
            }
        }
        Builder->CreateStore(var,dest);
    }
}
// check expression.
llvm::Value* CodeGeneration::check_expr(Node* node,bool is_load){
    if (node->label == EXPR){
        std::vector<Node*> children = node->children;
        llvm::Value* left;
        llvm::Value* right;
        left = check_negation(children[0],is_load);
        if (children.size() > 1){
            right = check_expr(children[1],is_load);
            left = check_o(node->token_arr[0].type,left,right);
            return left;
        }
        else
            return left;
    } else {
        // need to print out error.(TODO)
        std::cout << 1 << std::endl;
    }
}
// check negation.
llvm::Value* CodeGeneration::check_negation(Node* node,bool is_load){
    if (node->label == HASH){
        return check_R(node->children[0],node->token_arr.size(),is_load);
    }
}
llvm::Value* CodeGeneration::check_R(Node* node,int sign,bool is_load){
    if (node->label == R){
        token_t r = node->token_arr[0];
        // buildin function or procedure call.
        llvm::Value* var;
        if (r.type == T_IDENTIFIER){
            if (m_local_addr.find(r.StringValue) != m_local_addr.end())
                var = m_local_addr[r.StringValue];
            if (m_global_addr.find(r.StringValue) != m_global_addr.end())
                var = m_global_addr[r.StringValue];
            if (func_addr.find(r.StringValue) != func_addr.end()){
                llvm::Function* func = func_addr[r.StringValue];
                if (node->children.size() > 0){
                    llvm::Value* temp = check_expr(node->children[0],true);
                    return Builder->CreateCall(func,temp);
                } else{
                    return Builder->CreateCall(func);
                }
            }
            if (node->token_arr.size() > 1){
                token_t temp_index = node->token_arr[1];
                llvm::Value *index = llvm::ConstantInt::get(*TheContext,llvm::APInt(32, temp_index.intValue, true));
                llvm::Value *zeroVal = llvm::ConstantInt::get(
                                            *TheContext,
                                            llvm::APInt(32, 0, true));

                var = Builder->CreateInBoundsGEP(
                    var,
                    {zeroVal, index});
            }
            if (is_load){
                llvm::Value* temp = Builder->CreateLoad(getType(r.tm),var);
                return temp;
            }
            return var;
        } else if (r.type >= 331 && r.type <= 339){
            if (func_addr.find(r.StringValue) != func_addr.end()){
                llvm::Function* func = func_addr[r.StringValue];
                if (node->children.size() > 0){
                    llvm::Value* temp = check_expr(node->children[0],true);
                    return Builder->CreateCall(func,temp);
                } else {
                    return Builder->CreateCall(func);
                }
            }
        }
        else {
            llvm::Type *ty = getType(r.tm);
            llvm::Value* addr;
            if (r.type == T_INTEGER){
                if (sign)
                    addr = llvm::ConstantInt::get(*TheContext,llvm::APInt(32, 0 - r.intValue, true));
                else 
                    addr = llvm::ConstantInt::get(*TheContext,llvm::APInt(32, r.intValue, true));
            }
            if (r.type == T_FLOAT){
                if (sign)
                    addr = llvm::ConstantFP::get(*TheContext,llvm::APFloat(0.0 - r.doubleValue));
                else 
                    addr = llvm::ConstantFP::get(*TheContext,llvm::APFloat(r.doubleValue));
            }
            if (r.type == T_TRUE){
                addr = llvm::ConstantInt::getTrue(*TheContext);
            }
            if (r.type == T_FALSE){
                addr = llvm::ConstantInt::getFalse(*TheContext);
            }
            if (r.type == T_STRING){
                addr = Builder->CreateGlobalStringPtr(r.StringValue);
            }
            // todo. String
            return addr;
        }
    }
}
llvm::Value* CodeGeneration::check_o(int sign,llvm::Value* left,llvm::Value* right){
    llvm::Value* result;
    switch (sign){
        case '+':
            if (left->getType()->isFloatTy())
                left = Builder->CreateFAdd(left,right);
            else 
                left = Builder->CreateAdd(left,right);
            return left;
        case '-':
            if (left->getType()->isFloatTy())
                left = Builder->CreateFSub(left,right);
            else
                left = Builder->CreateSub(left,right);
            return left;
        case '*':
            if (left->getType()->isFloatTy())
                left = Builder->CreateFMul(left,right);
            else 
                left = Builder->CreateMul(left,right);
            return left;
        case '/':
            if (left->getType()->isFloatTy())
                left = Builder->CreateFDiv(left,right);
            else 
                left = Builder->CreateSDiv(left,right);
            return left;
        case T_DOUBLE_EQUAL:
            if (left->getType()->isFloatTy())
                left = Builder->CreateFCmpOEQ(left,right);
            else if (left->getType() == getType(STRING))
                left = string_equal(left, right);
            else 
                left = Builder->CreateICmpEQ(left,right);
            return left;
        case T_LESS_OR_EQUAL:
            if (left->getType()->isFloatTy())
                left = Builder->CreateFCmpOLE(left,right);
            else 
                left = Builder->CreateICmpSLE(left,right);
            return left;
        case T_GREATER_OR_EQUAL:
            if (left->getType()->isFloatTy())
                left = Builder->CreateFCmpOGE(left,right);
            else 
                left = Builder->CreateICmpSGE(left,right);
            return left;
        case T_NOT_EQUAL:
            if (left->getType()->isFloatTy())
                left = Builder->CreateFCmpONE(left,right);
            else 
                left = Builder->CreateICmpNE(left,right);
            return left;
        case '<':
            if (left->getType()->isFloatTy())
                left = Builder->CreateFCmpOLT(left,right);
            else 
                left = Builder->CreateICmpSLT(left,right);
            return left;
        case '>':
            if (left->getType()->isFloatTy())
                left = Builder->CreateFCmpOGT(left,right);
            else 
                left = Builder->CreateICmpSGT(left,right);
            return left;
        default:
            // pint out the error.
            return left;
    }
}

// bool
// void 
// // check stats block.
// void check_stats(Node* node){
    
// }

// check if block.
void CodeGeneration::check_if_block(Node* node){
    if (node->label == IF){
        std::vector<Node*> children = node->children;
        llvm::Value* exp = check_expr(children[0],true);
        // zero val.
        llvm::Value* zero_val = llvm::ConstantInt::get(
            *TheContext,
            llvm::APInt(32, 0, true));
        
        // // compare it with (zero val). 
        
        // exp = Builder->CreateICmpNE(
        //     exp, 
        //     zero_val);

        // code generation for if statement.
        llvm::Value* zero = llvm::ConstantInt::get(
        *TheContext,
        llvm::APInt(1, 0, true));
        llvm::Value* if_cond = Builder->CreateICmpNE(exp, zero);
        exp = if_cond;
        llvm::BasicBlock *ifThenBB = llvm::BasicBlock::Create(*TheContext, "ifThen", current_func);
        llvm::BasicBlock *elseBB = llvm::BasicBlock::Create(*TheContext, "ifElse", current_func);
        llvm::BasicBlock *endBB = llvm::BasicBlock::Create(*TheContext, "ifend", current_func);
        Builder->CreateCondBr(if_cond, ifThenBB, elseBB);
        Builder->SetInsertPoint(ifThenBB);
        // traverse the child.
        for (unsigned int i = 1; i < children.size(); i++) {
            statement(children.at(i));
            if (Builder->GetInsertBlock()->getTerminator() == nullptr)
                Builder->CreateBr(endBB);
            if (i <= children.size() - 1)
                Builder->SetInsertPoint(elseBB);
        }
        // set up the else block.
        if (Builder->GetInsertBlock()->getTerminator() == nullptr) {
            Builder->CreateBr(endBB);
        }
        Builder->SetInsertPoint(endBB);
    }
}
// check for statement.
void CodeGeneration::check_for_block(Node* node){
    if (node->label == FOR){
        std::vector<Node*> children = node->children;
        check_assign(children[0]);
        llvm::BasicBlock *loopHeaderBB = llvm::BasicBlock::Create(*TheContext, "loopHead", current_func);
        llvm::BasicBlock *loopBodyBB = llvm::BasicBlock::Create(*TheContext, "loopBody", current_func);
        llvm::BasicBlock *loopEndBB = llvm::BasicBlock::Create(*TheContext, "loopEnd", current_func);
        Builder->CreateBr(loopHeaderBB);
        Builder->SetInsertPoint(loopHeaderBB);


        // expression
        
        llvm::Value* exp = check_expr(children[1],true);
        // zero val.
        // llvm::Value* zero_val = llvm::ConstantInt::get(
        //     *TheContext,
        //     llvm::APInt(32, 0, true));
        

        // // compare it with (zero val). 
        // exp = Builder->CreateICmpNE(
        //     exp,
        //     zero_val);

        // code generation for if statement.
        llvm::Value* zero = llvm::ConstantInt::get(
        *TheContext,
        llvm::APInt(1, 0, true));
        llvm::Value* loop_cond = Builder->CreateICmpNE(exp, zero);

        exp = loop_cond;
        Builder->CreateCondBr(loop_cond, loopBodyBB, loopEndBB);

        // loop body.
        Builder->SetInsertPoint(loopBodyBB);

        // traverse the body.
        for (unsigned int i = 2; i < children.size(); i++) {
            statement(children.at(i));
            if (Builder->GetInsertBlock()->getTerminator() == nullptr)
                Builder->CreateBr(loopHeaderBB);
        }
        Builder->SetInsertPoint(loopEndBB);

    }
}
void CodeGeneration::statement(Node* node){
    if (node == NULL) return;
    if (node->label == IF){
        check_if_block(node);
    }
    if (node->label == ASSIGN){
        check_assign(node);
    }
    if (node->label == RETURN){
        check_return(node);
    }
    if (node->label == FOR){
        check_for_block(node);
    }
    std::vector<Node*> children = node->children;
    for (unsigned int i = 0; i < children.size(); i++) { 
        statement(children.at(i));
    }
    // (todo) loop.
}
// builtin function.
void CodeGeneration::insert_builtin_function(llvm::Module* mod,llvm::IRBuilder<>* builder){
    std::string str;
    llvm::FunctionType *ft;
    llvm::Function *func;
    str = "getbool";
    
    ft = llvm::FunctionType::get(builder->getInt1Ty(), {}, false);
    func = llvm::Function::Create(ft, llvm::Function::ExternalLinkage, str, *mod);
    func_addr[str] = func;

    str = "getinteger";
    ft = llvm::FunctionType::get(builder->getInt32Ty(), {}, false);
    func = llvm::Function::Create(ft, llvm::Function::ExternalLinkage, str, *mod);
    func_addr[str] = func;

    str = "getfloat";
    ft = llvm::FunctionType::get(builder->getFloatTy(), {}, false);
    func = llvm::Function::Create(ft, llvm::Function::ExternalLinkage, str, *mod);
    func_addr[str] = func;

    str = "getstring";
    ft = llvm::FunctionType::get(builder->getInt8PtrTy(), {}, false);
    func = llvm::Function::Create(ft, llvm::Function::ExternalLinkage, str, *mod);
    func_addr[str] = func;

    str = "putbool";
    ft = llvm::FunctionType::get(builder->getInt1Ty(), {builder->getInt1Ty()}, false);
    func = llvm::Function::Create(ft, llvm::Function::ExternalLinkage, str, *mod);
    func_addr[str] = func;

    str = "putinteger";
    ft = llvm::FunctionType::get(builder->getInt1Ty(), {builder->getInt32Ty()}, false);
    func = llvm::Function::Create(ft, llvm::Function::ExternalLinkage, str, *mod);
    func_addr[str] = func;

    str = "putfloat";
    ft = llvm::FunctionType::get(builder->getInt1Ty(), {builder->getFloatTy()}, false);
    func = llvm::Function::Create(ft, llvm::Function::ExternalLinkage, str, *mod);
    func_addr[str] = func;

    str = "putstring";
    ft = llvm::FunctionType::get(builder->getInt1Ty(), {builder->getInt8PtrTy()}, false);
    func = llvm::Function::Create(ft, llvm::Function::ExternalLinkage, str, *mod);
    func_addr[str] = func;

    str = "sqrt";
    ft = llvm::FunctionType::get(builder->getInt1Ty(), {builder->getInt32Ty()}, false);
    func = llvm::Function::Create(ft, llvm::Function::ExternalLinkage, str, *mod);
    func_addr[str] = func;
}
// get llvm type.
llvm::Type* CodeGeneration::getType(int x){
    switch (x){
        case INTEGER:
            return Builder->getInt32Ty();
        case FLOAT:
            return Builder->getFloatTy();
        case STRING:
            return Builder->getInt8PtrTy();
        case BOOL:
            return Builder->getInt1Ty();
        default:
            // error("Invalid type");
            return nullptr;
    }
}
llvm::Value* CodeGeneration::string_equal(llvm::Value* a,llvm::Value* b){
    // reference from github.
    llvm::Function *func = current_func;
    llvm::BasicBlock *CmpBB = llvm::BasicBlock::Create(*TheContext, "Cmp", func);
    llvm::BasicBlock *CmpendBB = llvm::BasicBlock::Create(*TheContext, "CmpEnd", func);
    llvm::Value *indAddr = Builder->CreateAlloca(
        getType(0),
        nullptr,
        "strCmpInd");
    llvm::Value *index = llvm::ConstantInt::get(
        *TheContext,
        llvm::APInt(32, 0, true));
    Builder->CreateStore(index, indAddr);

    Builder->CreateBr(CmpBB);
    Builder->SetInsertPoint(CmpBB);

    index = Builder->CreateLoad(getType(0), indAddr);

    llvm::Value *lhsCharAddr = Builder->CreateInBoundsGEP(
        a,
        index);
    llvm::Value *rhsCharAddr = Builder->CreateInBoundsGEP(
        b,
        index);
    llvm::Value *lhsCharVal = Builder->CreateLoad(
        Builder->getInt8Ty(), 
        lhsCharAddr);
    llvm::Value *rhsCharVal = Builder->CreateLoad(
        Builder->getInt8Ty(), 
        rhsCharAddr);
    llvm::Value *cmp = Builder->CreateICmpEQ(lhsCharVal, rhsCharVal);

    llvm::Value *zeroVal8 = llvm::ConstantInt::get(
        *TheContext,
        llvm::APInt(8, 0, true));
    
    llvm::Value *notNullTerm = Builder->CreateICmpNE(lhsCharVal, zeroVal8);

    llvm::Value *increment = llvm::ConstantInt::get(
        *TheContext,
        llvm::APInt(32, 1, true));
    index = Builder->CreateAdd(index, increment);
    Builder->CreateStore(index, indAddr);
    llvm::Value *andCond = Builder->CreateAnd(cmp, notNullTerm);
    Builder->CreateCondBr(andCond, CmpBB, CmpendBB);

    Builder->SetInsertPoint(CmpendBB);
    return cmp;
    
}
void CodeGeneration::outputIR(){
    TheModule->print(llvm::outs(),nullptr);
    return;
}
