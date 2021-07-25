#include "uGlobal.h"

int uGlobal::uProjNumber  = 1 ;  // Project Number
int uGlobal::uErrorLine   = 1 ;  // 記錄錯誤訊息中第幾列錯誤
int uGlobal::uErrorColumn = 0 ;  // 記錄錯誤訊息中第幾行錯誤
string uGlobal::uErrorToken  = "" ; // 記錄發生錯誤的token
string uGlobal::uErrorFuncN  = "" ; // 記錄發生錯誤的functin name
bool uGlobal::uEndOfFileOcurred = false ;      // END-OF-FILE的錯誤是否發生
bool uGlobal::uDoesThisLineHasOutput = false ; // 目的是檢查output後到\n是否其他token
string uGlobal::uFunctionName[40] = { 
                                    "cons", "list", // Constructors
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