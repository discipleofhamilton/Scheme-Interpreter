//
// Created by Hamilton Chang MSI on 2019/1/16.
//

#ifndef SCHEME_INTERPRETER_SCANNER_H
#define SCHEME_INTERPRETER_SCANNER_H

// Define three Syntax ERROR
#define UNEXPECTED_TOKEN_ONE 1
#define UNEXPECTED_TOKEN_TWO 2
#define NO_CLOSING_QUOTE     3

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string.h>
#include <string>
#include <vector>
#include <sstream>
#include "uGlobal.h"

using namespace std ;

// 所有的屬性編號
enum Attribution{ INTEGER = 1, FLOAT, STRING, LEFT_PAREN, RIGHT_PAREN, DOT, T, NIL, QUOTE, SYMBOL } ;

// static int    uProjNumber  = 1 ;  // Project Number
// static int    uErrorLine   = 1 ;  // Record the line where error occured at the Scheme script
// static int    uErrorColumn = 0 ;  // Record the column where error occured at the Scheme script line
// static string uErrorToken  = "" ; // Record the error token
// static string uErrorFuncN  = "" ; // Record the error functin name
// static bool   uEndOfFileOcurred = false ;      // the flag to mark if the error of END-OF-FILE
// static bool   uDoesThisLineHasOutput = false ; // the flag to mark if the other tokens exists before the newline charactor after outputing

// The information of token
struct Token{

  string strToken ; 
  int    attribution ; 

} ; // end Token

class ObjScanner{

private:

  Token          mCurrentToken ; // the current token which is processing
  vector<Token>  mVec_token ;    // saved the processed tokens

  void DealWithString() ;
  bool IsInteger( char ch ) ;      // Check if the input charactor is digit or not
  void DealWithchDot( char next_ch ) ;  // Process the dot is in the tail of the token, the input charactor is next charactor of the left charactors
  Token PreprocessingToken() ;     // Pre-processing the token

public:

  ObjScanner() ;     // Initialize/Constructor
  Token GetToken() ; // Get a token and deal with it.

};

#endif //SCHEME_INTERPRETER_SCANNER_H
