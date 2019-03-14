//
// Created by Hamilton Chang MSI on 2019/1/16.
//
#ifndef SCHEME_INTERPRETER_PARSER_H
#define SCHEME_INTERPRETER_PARSER_H

#include "Scanner.h"

// Proj2 ERROR
// 總共有9種ERROR
# define NON_LIST                        4
# define INCORRECT_NUMBER_OF_ARGUMENT    5
# define INCORRECT_ARGUMENT_TYPE         6
# define ATTEMPT_TO_APPLY_NON_FUNCTION   7
# define NO_RETURN_VALUE                 8
# define UNBOUND_SYMBOL                  9
# define DIVISION_BY_ZERO                10
# define FORMAT_ERROR                    11
# define LEVEL_ERROR                     12
# define ATTEMPT_TO_APPLY_NON_FUNCTION_2 13


// proj2所有功能的關鍵字
static string  uFunctionName[] = { "cons", "list", // Constructors
                                   "exit",         // EXIT
                                   "quote",        // Bypassing the default evaluation
                                   "define",       // The binding of a symbol to an S-expression
                                   "car", "cdr",   // Part accessors
                                   "atom?", "pair?", "list?", "null?",
                                   "integer?", "real?", "number?", // Primitive predicates
                                   "string?", "boolean?", "symbol?", // Primitive predicates
                                   "+", "-", "*", "/", // Basic artithmetic, logical and string operations
                                   "not", "and", "or", // Basic artithmetic, logical and string operations
                                   ">", ">=", "<", "<=", "=", // Basic artithmetic, logical and string operations
                                   "string-append", "string>?",
                                   "string<?", "string=?", // Basic artithmetic, logical and string operations
                                   "eqv?", "equal?", // Eqivalence tester
                                   "begin",      // Sequencing and functional composition
                                   "if", "cond", // Conditionals
                                   "clean-environment" // Clean environment
                                } ;

// 所有需要在parser曾所處理的function
// 建樹、文法分析等等
class ObjParser{

protected:

  bool       mIsAddNode ;   // 訂定現在是否有在token與token間新增一節點這樣的狀態
  ObjScanner mScanner ;     // 所有Parser需要Scanner層support
  Token      mLastToken ;   // 暫存上一個token
  bool       mIsLP ;        // 在pretty print用來判斷是否為左括號
  bool       mIsLastNodeLP ;// 在pretty print用來判斷上一個節點是否為左括號
  int        mdotted_pait ; // dotted-pair出現的次數

  // 中間節點
  struct Cons_node{

      Cons_node *right ;      // 右指標(右子樹)
      Cons_node *left ;       // 左指標(左子樹)
      Token     atom ;        // 樹葉節點
      bool      dotted_pair ; // 判斷dotted pair的情況是否有發生

  } ; // end Cons_node

  Cons_node *NewNode() ;         // 宣告要新增的節點，並回傳
  bool IsAtom( int tokenatrb ) ; // 依目前token的屬性判斷此token是否為atom node
  void PrintSpace( int level ) ;

public:

  Cons_node *mroot ; // 此樹的根節點
  ObjParser() ;      // 初始化
  bool CheckEXIT( Cons_node *tree ) ;       // 確認此樹是否為結束條件
  Cons_node *BuildTree( Cons_node *tree ) ; // 用遞迴建樹
  void ClearTree( Cons_node *tree ) ; // 清空樹
  void ClearAll( Cons_node *tree ) ;  // 清空parser
  void PrettyPrint( Cons_node *tree, int level ) ;

};

#endif //SCHEME_INTERPRETER_PARSER_H
