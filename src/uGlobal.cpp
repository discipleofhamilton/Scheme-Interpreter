#include "uGlobal.h"

int uGlobal::uProjNumber  = 1 ;  // Project Number
int uGlobal::uErrorLine   = 1 ;  // to record the row no where the ERROR occurred
int uGlobal::uErrorColumn = 0 ;  // to record the column no where the ERROR occurred
string uGlobal::uErrorToken  = "" ; // to record the ERROR token
string uGlobal::uErrorFuncN  = "" ; // to record the ERROR function name
bool uGlobal::uEndOfFileOcurred = false ;      //  the flag of whether the END-OF-FILE occurred
bool uGlobal::uDoesThisLineHasOutput = false ; //  the flag of wether the other tokens exists before the newline charactor after outputing
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