#ifndef NODE_TABLE_H
#define NODE_TABLE_H

// #include <string>
const std::string ROOT = "root";
const std::string START = "S"; // program <vars> <block>
const std::string BLOCK = "block"; // begin <vars> <stat> end.
const std::string VARS = "vars"; // variable Intifier.
const std::string PROCEDURE = "procedure";
const std::string EXPR = "expr"; // + - * /
// the expe needs to be detailed.
const std::string STATS = "stats";  // <stat> <mStat>
const std::string MSTAT = "mstat"; // empty | <stats>
const std::string STAT = "stat"; // <in> | <out> | <block> | <if> | <for>.
const std::string IF = "if"; // if (<expr> <O> <expr>) <stat>
const std::string FOR = "for"; // for (<expr> <O> <expr>) <stat>
const std::string ASSIGN = "assign"; // :.
const std::string HASH = "hash"; // - symbol.
const std::string R = "r";
const std::string OPERATER = "operator";
const std::string RETURN = "return"; // <return>.
const std::string BEGIN = "begin"; // begin for procedure.
const std::string END = "end";

// for `if` statement.
const std::string ELSE = "else";
#endif