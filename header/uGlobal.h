//
// Created by Hamilton Chang MSI on 2019/1/29.
//

#ifndef SCHEME_INTERPRETER_UGLOBAL_H
#define SCHEME_INTERPRETER_UGLOBAL_H

#include <string>
//#include <string.h>

using namespace std ;

class uGlobal {

  static int    uProjNumber  ;  // Project Number
  static int    uErrorLine   ;  // 記錄錯誤訊息中第幾列錯誤
  static int    uErrorColumn ;  // 記錄錯誤訊息中第幾行錯誤
  static string uErrorToken  ;  // 記錄發生錯誤的token
  static string uErrorFuncN  ;  // 記錄發生錯誤的functin name
  static bool   uEndOfFileOcurred ;      // END-OF-FILE的錯誤是否發生
  static bool   uDoesThisLineHasOutput ; // 目的是檢查output後到\n是否其他token

  uGlobal(){

    uProjNumber  = 1 ;  // Project Number
    uErrorLine   = 1 ;  // 記錄錯誤訊息中第幾列錯誤
    uErrorColumn = 0 ;  // 記錄錯誤訊息中第幾行錯誤
    uErrorToken  = "" ; // 記錄發生錯誤的token
    uErrorFuncN  = "" ; // 記錄發生錯誤的functin name
    uEndOfFileOcurred = false ;      // END-OF-FILE的錯誤是否發生
    uDoesThisLineHasOutput = false ; // 目的是檢查output後到\n是否其他token

  }

};


#endif //SCHEME_INTERPRETER_UGLOBAL_H
