//
// Created by Hamilton Chang MSI on 2019/1/16.
//

#ifndef SCHEME_INTERPRETER_EVALUATOR_H
#define SCHEME_INTERPRETER_EVALUATOR_H

# include <map>     // PROJ2:使用map這個function
# include "Parser.h"

extern map<string,string> uFunctionNamesMap ; // 當keyword是atom實用的map

class ObjEvaluation : protected ObjParser {

private:

  map < string, Cons_node* > mDef ; // 儲存被defined過的樹
  bool mIsQuote ;

  bool IsEvaluateFunctions( string key, string &funcName ) ; // 判斷此是否為function name
  Cons_node* IsDefined( string key ) ;  // 判斷此是否為被defined過的symbol
  bool CheckFormat( Cons_node* tree ) ; // 確認此expression的格式(format)是否正確
  Cons_node* CheckBindingAndReturnThePointer( string key ) ; // 判斷此argument是否有binding(internal function, define)
  Cons_node* EvalFunctions( Cons_node *tree, int numofArg ) ;
  bool CompareTree( Cons_node* one, Cons_node* two ) ; // 判斷兩棵樹是否一樣
  bool CheckNumberOfVariable( Cons_node *tree, int& numofArg ) ; // 判斷此function是否符合規則
  string RepairNameOfInternalFunction( string symbol ) ;
  bool CheckPureList( Cons_node *tree ) ;

public:

  Cons_node* mReturnPointer ;
  Cons_node* mErrorPointer ;

  ObjEvaluation() ; // Constructor
  void ReObjEvalutaion() ; // 重新初始化
  Cons_node* Evaluate( Cons_node *tree, bool topLevel ) ;
  void ErrorPrettyPrint( Cons_node *tree, int level ) ;

};

#endif //SCHEME_INTERPRETER_EVALUATOR_H
