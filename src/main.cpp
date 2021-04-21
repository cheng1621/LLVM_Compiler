
#include<iostream>
#include "parser.h"
#include "code_generation.h"

using namespace std;
int main(int argc, char* argv[]){
    token_t token;
    Parser parser(argv[1]);
    Node* root = parser.Parse();
    // root->print(root);
    CodeGeneration cg;
    cg.traverse_children(root);
    // cg.outputIR();
	cg.outputAssembly();
    return 0;
}