

#include<iostream>
#include "error.h"
using namespace std;

void error(ErrorCode err, int lineNo){
    switch (err) {
  case ERR_ENDOFCOMMENT:
    cout << lineNo << ": " << ERM_ENDOFCOMMENT << endl;
    break;
  case ERR_IDENTTOOLONG:
    cout << lineNo << ": " << ERM_IDENTTOOLONG << endl;
    break;
  case ERR_INVALIDCHARCONSTANT:
    cout << lineNo << ": " << ERM_INVALIDCHARCONSTANT << endl;
    break;
  case ERR_INVALIDSYMBOL:
    cout << lineNo << ": " << ERM_INVALIDSYMBOL << endl;
    break;
  case ERR_INVALIDCONSTANT:
    cout << lineNo << ": " << ERM_INVALIDCONSTANT << endl;
    break;
  case ERR_INVALIDTYPE:
    cout << lineNo << ": " << ERM_INVALIDTYPE << endl;
    break;
  case ERR_INVALIDBASICTYPE:
    cout << lineNo << ": " << ERM_INVALIDBASICTYPE << endl;
    break;
  case ERR_INVALIDPARAM:
    cout << lineNo << ": " << ERM_INVALIDPARAM << endl;
    break;
  case ERR_INVALIDSTATEMENT:
    cout << lineNo << ": " << ERM_INVALIDSTATEMENT << endl;
    break;
  case ERR_INVALIDARGUMENTS:
    cout << lineNo << ": " << ERM_INVALIDARGUMENTS << endl;
    break;
  case ERR_INVALIDCOMPARATOR:
    cout << lineNo << ": " << ERM_INVALIDCOMPARATOR << endl;
    break;
  case ERR_INVALIDEXPRESSION:
    cout << lineNo << ": " << ERM_INVALIDEXPRESSION << endl;
    break;
  case ERR_INVALIDTERM:
    cout << lineNo << ": " << ERM_INVALIDTERM << endl;
    break;
  case ERR_INVALIDFACTOR:
    cout << lineNo << ": " << ERM_INVALIDFACTOR << endl;
    break;
  }
  exit(0);
}