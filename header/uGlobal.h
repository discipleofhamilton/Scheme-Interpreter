//
// Created by Hamilton Chang MSI on 2019/1/29.
//

#ifndef SCHEME_INTERPRETER_UGLOBAL_H
#define SCHEME_INTERPRETER_UGLOBAL_H

#include <string>
//#include <string.h>

using namespace std ;
class uGlobal {

public:

  static int    uProjNumber  ;  // Project Number
  static int    uErrorLine   ;  // to record the row no where the ERROR occurred
  static int    uErrorColumn ;  // to record the column no where the ERROR occurred
  static string uErrorToken  ;  // to record the ERROR token
  static string uErrorFuncN  ;  // to record the ERROR function name
  static bool   uEndOfFileOcurred ;      // the flag of whether the END-OF-FILE occurred
  static bool   uDoesThisLineHasOutput ; // the flag of wether the other tokens exists before the newline charactor after outputing
  static string uFunctionName[40];       // all function keyword in project 2

};

#endif //SCHEME_INTERPRETER_UGLOBAL_H
