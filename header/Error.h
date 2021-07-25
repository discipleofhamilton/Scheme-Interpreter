//
// Created by Hamilton Chang MSI on 2019/1/29.
//
#ifndef SCHEME_INTERPRETER_ERROR_H
#define SCHEME_INTERPRETER_ERROR_H

#include <exception>
#include <stdexcept>
#include "Evaluator.h"
#include "uGlobal.h"

// 處理除了END-OF-FILE以外的所有錯誤訊息
class Error : public exception {

public:

  string ErrorMessage( int synErrType ) ; // Error Message的判斷與回傳
  void ReadLeftStuff() ; // 讀掉出現Error後，此行剩下的stuff

};

#endif //SCHEME_INTERPRETER_ERROR_H
