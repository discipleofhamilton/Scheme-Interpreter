//
// Created by Hamilton Chang MSI on 2019/1/16.
//

#ifndef SCHEME_INTERPRETER_SCANNER_H
#define SCHEME_INTERPRETER_SCANNER_H

// 有三種Syntax ERROR(文法錯誤)
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

using namespace std ;

// 所有的屬性編號
enum Attribution{ INTEGER = 1, FLOAT, STRING, LEFT_PAREN, RIGHT_PAREN, DOT, T, NIL, QUOTE, SYMBOL } ;

static int    uProjNumber  = 1 ;  // Project Number
static int    uErrorLine   = 1 ;  // 記錄錯誤訊息中第幾列錯誤
static int    uErrorColumn = 0 ;  // 記錄錯誤訊息中第幾行錯誤
static string uErrorToken  = "" ; // 記錄發生錯誤的token
static string uErrorFuncN  = "" ; // 記錄發生錯誤的functin name
static bool   uEndOfFileOcurred = false ;      // END-OF-FILE的錯誤是否發生
static bool   uDoesThisLineHasOutput = false ; // 目的是檢查output後到\n是否其他token

// 所有有關token的資訊
struct Token{

  string strToken ;  // 儲存token字串
  int    attribution ; // token的屬性

} ; // end Token

class ObjScanner{

private:
// 所有不能或不需要在外面被呼叫使用的method、function、value

  Token          mCurrentToken ; // 暫存目前的token
  vector<Token>  mVec_token ;    // 儲存token

  void DealWithString() ;
  bool IsInteger( char ch ) ;      // 確認此character + 目前的token是否為數字
  void DealWithchDot( char ch ) ;  // 處理在buffer中出現點時做的處理
  Token PreprocessingToken() ;     // 將切出token做前處理

public:

  ObjScanner() ;     // 初始化
  Token GetToken() ; // 讀取一個token

};

#endif //SCHEME_INTERPRETER_SCANNER_H
