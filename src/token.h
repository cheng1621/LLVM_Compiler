

#ifndef TOKEN_H
#define TOKEN_H
// define a token table
// #define T_SEMICOLON ';' // use ASCII values for single char tokens
// #define T_LPAREN '('
// #define T_RPAREN ')'
// #define T_ASSIGN '='
// #define T_DIVIDE '/'
// #define T_NOT '!'
// #define T_DQUOTE '"'
// #define T_REMINDER '%'
// #define T_AND '&'
// #define T_QUOTE '\''
// #define T_MUL '*'
// #define T_SUM '+'
// #define T_COMMA ''
// #define T_MINUS '-'
// #define T_PERIOD '.'
// #define T_COLON ':'
// #define T_SMALLER '<'
// #define T_LARGET '>'
// #define T_QUES '?'
// #define T_LBRANKETS '['
// #define T_RBRANKETS ']'
// #define T_CARET '^'
// #define T_LBPAREN '{'
// #define T_RBPAREN '}'
// #define T_OR '|'
#define T_END -1
#define T_AND 257
#define T_FOR 258
#define T_TO 259
#define T_DOWNTO 260
#define T_DO 261
#define T_IF 262
#define T_THEN 263
#define T_ELSE 264
#define T_WHILE 265
#define T_REPEAT 266
#define T_UNTIL 267
#define T_FUNCTION 268
#define T_PROCEDURE 269
#define T_BEGIN 270
// #define T_END 271
#define T_PROGRAM 272
#define T_FORWARD 273
#define T_GOTO 274
#define T_OR 275
#define T_NOT 276
#define T_CASE 277
#define T_OTHERWISE 278
#define T_WITH 279
#define T_IN 280

#define T_WRITE 281
#define T_WRITELN 282
#define T_READ 283
#define T_READLN 284

#define T_TYPE 285
#define T_VAR 286
#define T_PACKED 287
#define T_ARRAY 288
#define T_OF 289
#define T_RECORD 290
#define T_CONST 291
#define T_FILE 292
#define T_SET 293
#define T_STRING 294
#define T_NIL 295
#define T_FLOAT_KEYWORD 296
#define T_FLOAT 297
// #define T_LEFT_PAREN 296        // (
// #define T_RIGHT_PAREN 297       // )
// #define T_LEFT_SQUARE 298       // [
// #define T_RIGHT_SQUARE 299      // ]
// #define T_PLUS 300              // +
// #define T_MINUS 301             // -
// #define T_MULTIPLY 302          // *
// #define T_DIVIDE 303            // /
// #define T_COMMA 304             // ,
// #define T_SEMICOLON 305         // ;
// #define T_COLON 306             // :
#define T_ASSIGN 307            // :=
// #define T_PERIOD 308            // .
#define T_DOT_DOT 309           // ..
// #define T_UPARROW 310           // ^
#define T_DIV 311               // div
#define T_MOD 312               // mod
#define T_XOR 313               // xor
#define T_SHR 314               // shr
#define T_SHL 315               // shl

// comparation symbols
#define T_LESS_OR_EQUAL 316     // <=
#define T_GREATER_OR_EQUAL 318  // >=
#define T_NOT_EQUAL     321    // <>
#define T_DOUBLE_EQUAL 341 // ==

#define T_INTEGER 322  // integer
#define T_IDENTIFIER 323
#define T_INTEGER_KEYWORD 324
#define T_STRING_KEYWORD 325
#define T_END_KEYWORD 326
#define T_GLOBAL 327
#define T_IS 328
#define T_BOOL 329
#define T_RETURN 330
// for builtin functions.
#define T_GETBOOL 331
#define T_GETINTEGER 332
#define T_GETFLOAT 333
#define T_GETSTRING 334
#define T_PUTBOOL 335
#define T_PUTINTEGER 336
#define T_PUTFLOAT 337
#define T_PUTSTRING 338
#define T_SQRT 339
#define T_TRUE 340
#define T_FALSE 341

#define T_UNKNOWN 350 // token was unrecognized by scanner 
#include <string>
enum langType {INTEGER,STRING,BOOL,FLOAT};
struct token_t {
 int type; // one of the token codes from above
 std::string StringValue;
 int tm;
 int intValue; // holds lexeme value if integer
 float doubleValue; // holds lexeme value if double
 int numCount;
 bool isGlobal; // initialized to false;
 token_t(){
     this->numCount = 0;
     this->isGlobal = false;
 }
}; 

#endif