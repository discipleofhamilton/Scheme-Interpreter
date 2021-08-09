//
// Created by Hamilton Chang MSI on 2019/1/16.
//

#include "Evaluator.h"

map<string,string> uFunctionNamesMap ; // 當keyword是atom實用的map

// private

// 判斷此是否為function name
// 如果是，回傳true
// 如果否，回傳false
bool ObjEvaluation::IsEvaluateFunctions(string key, string &funcName) {

  map<string,string>::iterator iter ;   // declaration iterator
  iter = uFunctionNamesMap.find( key ) ;

  if ( iter != uFunctionNamesMap.end() ) {
    funcName = iter->second ;
    return true ;
  } // end if

  return false ;

}

// 判斷此是否為被defined過的symbol
// 如果是，回傳pointer
// 如果否，回傳NULL
ObjParser::Cons_node* ObjEvaluation::IsDefined(string key) {

  map < string, Cons_node* > :: iterator iter ;   // declaration iterator
  iter = mDef.find( key ) ;

  if ( iter != mDef.end() ) {
    return iter->second ;
  } // end if

  return NULL ;

}

// 確認此expression的格式(format)是否正確
// (define symbol S-exp) 只能宣告或設定非primitive 的symbol
// (define (one-or-more-symbols) one-or-more-S-exp)
// (set! symbol S-exp)
// (lambda (zero-or-more-symbols one-or-more-S-exp)
// (let (zero-or-more-PAIRS) one-or-more-S-exp)
// (cond one-or-more-AT-LEAST-DOUNBLETONS)
// PAIR df = (symbol S-exp)
// AT-LEAST-DOUBLETONS df = a list of two or more S-exp
// 是，回傳true
// 否，回傳false
bool ObjEvaluation::CheckFormat(ObjParser::Cons_node *tree) {

  int numofArg    = 0 ;
  string funcName = "" ;
  funcName.clear() ; // 清空function name

  funcName = tree->left->atom.strToken ;

  // 此function是define
  if ( funcName == "#<procedure define>" ) {

    Cons_node* secondArg = tree->right ; // second argument

    // check whether the second argument is symbol
    // yes, it's symbol

    string second = "" ;

    if ( CheckNumberOfVariable( tree, numofArg ) && secondArg->left->atom.attribution == SYMBOL &&
         !IsEvaluateFunctions( secondArg->left->atom.strToken, second ) ) {

      return true ;

    } // end if

    // no, it is "NOT" symbol or number of argument is "NOT" correct
    // else
    //   return false ;

  } // end if

    // 此function是set!
  else if ( funcName == "#<procedure set!>" ) {

    ;

  } // end else if

    // 此function是lambda
  else if ( funcName == "#<procedure lambda>" ) {

    ;

  } // end else if

    // 此function是let
  else if ( funcName == "#<procedure let>" ) {

    ;

  } // end else if

    // 此function是cond
  else if ( funcName == "#<procedure cond>" ) {

    // 確認argument的數目是否符合規則
    // 是，符合
    if ( CheckNumberOfVariable( tree, numofArg ) ) {

      // 確認所有argument是否都符合規則
      // 只要一有不符合，就return false
      for ( tree = tree->right ; tree->atom.attribution != NIL ; tree = tree->right ) {

        // 是atom，不是樹
        if ( IsAtom( tree->left->atom.attribution ) )
          return false ;

          // 是樹，不是atom
        else {

          Cons_node* argTree    = NewNode() ;
          int        countofArg = 0 ;

          for ( argTree = tree->left ; argTree->atom.attribution != NIL ; argTree = argTree->right ) {

            countofArg = countofArg + 1 ;

          } // end for

          if ( countofArg >= 2 )
            ; // Do Nothing

          else
            return false ;

        } // end else

      } // end for

      return true ;

    } // end if

      // 否，不符合
    else {

      ; // Do Nothing

    } // end else

  } // end else if

  return false ;

}

// 判斷此argument是否有binding(internal function, define)
// 有，回傳指向此binding的pointer
// 否，回傳NUL
ObjParser::Cons_node* ObjEvaluation::CheckBindingAndReturnThePointer(string key) {

  string functionName = "" ;
  Cons_node* result = NewNode() ;

  // 先確認此symbol是否為internal function
  // 是，此symbol 是 internal function
  if ( IsEvaluateFunctions( key, functionName ) ) {

    result->atom.strToken    = functionName ;
    result->atom.attribution = SYMBOL ;

    return result ;

  } // end if

  // 否，此symbol "不是" internal function
  // 再確認此symbol是否為被define過

  result = IsDefined( key ) ;

  if ( result != NULL ) {

    // 確認此symbol(被define過的)是否為internal function
    // 是，此symbol 是 internal function
    if ( IsEvaluateFunctions( result->atom.strToken, functionName ) ) {

      result->atom.strToken    = functionName ;
      result->atom.attribution = SYMBOL ;

    } // end if

    return result ;

  } // end if

  return NULL ;

}

ObjParser::Cons_node* ObjEvaluation::EvalFunctions(ObjParser::Cons_node *tree, int numofArg) {

  // evaluated result
  Cons_node* result = NULL ;
  result            = NewNode() ;

  string funcStr = tree->left->atom.strToken ;

  if ( funcStr == "#<procedure clean-environment>" ) {

    mDef.clear() ;
    result->atom.attribution = SYMBOL ;
    result->atom.strToken    = "environment cleaned" ;

  } // end if

  else if ( funcStr == "#<procedure exit>" ) {
    result = tree;
  } // end else if

  else if ( funcStr == "#<procedure define>" ) {

    // 建立且初始化要被define的symbol與他的pointer
    string defSymbol         = "" ;
    Cons_node* defSymPointer = NULL ;

    defSymbol.clear() ;
    tree = tree->right ; // second argument
    defSymbol = tree->left->atom.strToken ;

    tree = tree->right ; // third argument
    defSymPointer = Evaluate( tree->left, false ) ;

    mDef[defSymbol] = defSymPointer ;

    result->atom.attribution = SYMBOL ;
    result->atom.strToken    = defSymbol + " defined" ;

    return result ;

  } // end else if

  else if ( funcStr == "#<procedure cons>" ) {

    result->atom.strToken    = "(" ;
    result->atom.attribution = LEFT_PAREN ;

    result->left = Evaluate( tree->right->left, false ) ;
    result->right = Evaluate( tree->right->right->left, false ) ;

  } // end else if

  else if ( funcStr == "#<procedure list>" ) {

    if ( numofArg == 0 ) {

      result->atom.attribution = NIL ;
      result->atom.strToken    = "nil" ;

    } // end if

    else {

      // build tree
      result->atom.strToken    = "(" ;
      result->atom.attribution = LEFT_PAREN ;

      Cons_node* iter = NewNode() ;
      iter            = result ;

      for ( tree = tree->right ; tree->atom.attribution != NIL ; tree = tree->right ) {

        // evalaute second(and so on) argument
        // and put the node on the left hand side of the root( the top of tree)
        iter->left = NewNode() ;
        iter->left = Evaluate( tree->left, false ) ;

        iter->right = NewNode() ;
        iter        = iter->right ;

        // 當下一個節點是右括號
        // 代表即將結束

        if ( tree->right->atom.attribution != NIL ) {

          iter->atom.attribution = LEFT_PAREN ;
          iter->atom.strToken    = "(" ;

        } // end if

        else {

          iter->atom.attribution = NIL ;
          iter->atom.strToken    = "nil" ;

        } // end else

      } // end for

    } // end else

  } // end else if

  else if ( funcStr == "#<procedure car>" ) {

    result = Evaluate( tree->right->left, false ) ;

    // 當只有一個節點時
    if ( result->left == NULL && result->right == NULL ) {
      uGlobal::uErrorFuncN = "car" ;
      mErrorPointer = result ;
      throw INCORRECT_ARGUMENT_TYPE ;
    } // end if

      // 當是一棵樹時
    else {
      // return 左子樹
      result =  result->left ;
    } // end else

  } // end else if

  else if ( funcStr == "#<procedure cdr>" ) {

    result = Evaluate( tree->right->left, false ) ;

    // 當只有一個節點時
    if ( result->left == NULL && result->right == NULL ) {
      uGlobal::uErrorFuncN = "cdr" ;
      mErrorPointer = result ;
      throw INCORRECT_ARGUMENT_TYPE ;
    } // end if

      // 當是一棵樹時
    else {
      // return 右子樹
      result = result->right ;
    } // end else

  } // end else if

  else if ( funcStr == "#<procedure quote>" ) {

    mIsQuote = true ;
    result = tree->right->left;

  } // end else if

  else if ( funcStr == "#<procedure atom?>" ) {

    Cons_node* arg = NewNode() ;
    arg = tree->right->left ;

    arg = Evaluate( arg, false ) ;

    if ( IsAtom( arg->atom.attribution ) ) {
      result->atom.strToken    = "#t" ;
      result->atom.attribution = T ;
    } // end if

    else {
      result->atom.strToken    = "nil" ;
      result->atom.attribution = NIL ;
    } // end else

  } // end else if

  else if ( funcStr == "#<procedure pair?>" ) {

    Cons_node* arg = NewNode() ;
    arg = tree->right->left ;

    arg = Evaluate( arg, false ) ;

    if ( IsAtom( arg->atom.attribution ) ) {
      result->atom.strToken    = "nil" ;
      result->atom.attribution = NIL ;
    } // end if

    else {
      result->atom.strToken    = "#t" ;
      result->atom.attribution = T ;
    } // end else

  } // end else if

  else if ( funcStr == "#<procedure list?>" ) {

    result = NULL ;

    Cons_node* arg = Evaluate( tree->right->left, false ) ;

    if ( arg->atom.attribution == LEFT_PAREN ) {

      for ( ; arg != NULL ; arg = arg->right ) {

        if ( arg->right == NULL ) {

          if ( arg->atom.attribution == NIL ) {

            result = NewNode() ;
            result->atom.strToken    = "#t" ;
            result->atom.attribution = T ;

          } // end if

        } // end if

      } // end for

      if ( result == NULL ) {
        result = NewNode() ;
        result->atom.strToken    = "nil" ;
        result->atom.attribution = NIL ;
      } // end if

    } // end if

    else {
      result = NewNode() ;
      result->atom.strToken    = "nil" ;
      result->atom.attribution = NIL ;
    } // end else

  } // end else if

  else if ( funcStr == "#<procedure null?>" ) {

    Cons_node* arg = NewNode() ;
    arg = tree->right->left ;

    arg = Evaluate( arg, false ) ;

    if ( arg->right == NULL && arg->left == NULL ) {
      result->atom.strToken    = "#t" ;
      result->atom.attribution = T ;
    } // end if

    else {
      result->atom.strToken    = "nil" ;
      result->atom.attribution = NIL ;
    } // end else

  } // end else if

  else if ( funcStr == "#<procedure integer?>" ) {

    Cons_node* arg = NewNode() ;
    arg = tree->right->left ;

    arg = Evaluate( arg, false ) ;

    if ( arg->atom.attribution == INTEGER ) {
      result->atom.strToken    = "#t" ;
      result->atom.attribution = T ;
    } // end if

    else {
      result->atom.strToken    = "nil" ;
      result->atom.attribution = NIL ;
    } // end else

  } // end else if

  else if ( funcStr == "#<procedure real?>" ) {

    Cons_node* arg = NewNode() ;
    arg = tree->right->left ;

    arg = Evaluate( arg, false ) ;

    if ( arg->atom.attribution == INTEGER || arg->atom.attribution == FLOAT ) {
      result->atom.strToken    = "#t" ;
      result->atom.attribution = T ;
    } // end if

    else {
      result->atom.strToken    = "nil" ;
      result->atom.attribution = NIL ;
    } // end else

  } // end else if

  else if ( funcStr == "#<procedure number?>" ) {

    Cons_node* arg = NewNode() ;
    arg = tree->right->left ;

    arg = Evaluate( arg, false ) ;

    if ( arg->atom.attribution == INTEGER || arg->atom.attribution == FLOAT ) {
      result->atom.strToken    = "#t" ;
      result->atom.attribution = T ;
    } // end if

    else {
      result->atom.strToken    = "nil" ;
      result->atom.attribution = NIL ;
    } // end else

  } // end else if

  else if ( funcStr == "#<procedure string?>" ) {

    Cons_node* arg = NewNode() ;
    arg = tree->right->left ;

    arg = Evaluate( arg, false ) ;

    if ( arg->atom.attribution == STRING ) {
      result->atom.strToken    = "#t" ;
      result->atom.attribution = T ;
    } // end if

    else {
      result->atom.strToken    = "nil" ;
      result->atom.attribution = NIL ;
    } // end else

  } // end else if

  else if ( funcStr == "#<procedure boolean?>" ) {

    Cons_node* arg = NewNode() ;
    arg = tree->right->left ;

    // if ( arg->left != NULL && arg->left->atom.attribution == QUOTE )
    //   arg = Evaluate( arg, false ) ;

    arg = Evaluate( arg, false ) ;

    if ( arg->atom.attribution == T || arg->atom.attribution == NIL ) {
      result->atom.strToken    = "#t" ;
      result->atom.attribution = T ;
    } // end if

    else {
      result->atom.strToken    = "nil" ;
      result->atom.attribution = NIL ;
    } // end else

  } // end else if

  else if ( funcStr == "#<procedure symbol?>" ) {

    Cons_node* arg = NewNode() ;
    arg = tree->right->left ;

    arg = Evaluate( arg, false ) ;

    if ( arg->atom.attribution == SYMBOL ) {
      result->atom.strToken    = "#t" ;
      result->atom.attribution = T ;
    } // end if

    else {
      result->atom.strToken    = "nil" ;
      result->atom.attribution = NIL ;
    } // end else

  } // end else if

  else if ( funcStr == "#<procedure +>" ) {

    float fout    = 0.0 ;
    float transf  = 0.0 ;
    bool  isfloat = false ; // 預設是integer

    for ( tree = tree->right ; tree->atom.attribution != NIL ; tree = tree->right ) {

      tree->left = Evaluate( tree->left, false ) ;

      if ( tree->left->atom.attribution == INTEGER || tree->left->atom.attribution == FLOAT ) {

        if ( tree->left->atom.attribution == FLOAT )
          isfloat = true ;

        sscanf( tree->left->atom.strToken.c_str(), "%f", &transf ) ;
        fout = fout + transf ;

      } // end if

      else {
        uGlobal::uErrorFuncN = "+" ;
        mErrorPointer = tree->left ;
        throw INCORRECT_ARGUMENT_TYPE ;
      } // end else

    } // end for

    stringstream ss ;

    if ( isfloat ) {

      ss << fout;
      result->atom.strToken    = ss.str();
      result->atom.attribution = FLOAT ;

    } // end if

    else {

      ss << fout;
      result->atom.strToken    = ss.str() ;
      result->atom.attribution = INTEGER ;

    } // end else

  } // end else if

  else if ( funcStr == "#<procedure ->" ) {

    float fout    = 0.0 ;
    float transf  = 0.0 ;
    bool  isfirst = true ;  // 是第一個argument
    bool  isfloat = false ; // 預設是integer

    for ( tree = tree->right ; tree->atom.attribution != NIL ; tree = tree->right ) {

      tree->left = Evaluate( tree->left, false ) ;

      if ( tree->left->atom.attribution == INTEGER || tree->left->atom.attribution == FLOAT ) {

        if ( tree->left->atom.attribution == FLOAT )
          isfloat = true ;

        sscanf( tree->left->atom.strToken.c_str(), "%f", &transf ) ;

        if ( isfirst ) {
          isfirst = false ;
          fout = transf ;
        } // end if

        else
          fout = fout - transf ;

      } // end if

      else {
        uGlobal::uErrorFuncN = "-" ;
        mErrorPointer = tree->left ;
        throw INCORRECT_ARGUMENT_TYPE ;
      } // end else

    } // end for

    stringstream ss ;

    if ( isfloat ) {

      ss << fout;
      result->atom.strToken    = ss.str();
      result->atom.attribution = FLOAT ;

    } // end if

    else {

      ss << fout;
      result->atom.strToken    = ss.str() ;
      result->atom.attribution = INTEGER ;

    } // end else

  } // end else if

  else if ( funcStr == "#<procedure *>" ) {

    float fout    = 0.0 ;
    float transf  = 0.0 ;
    bool  isfirst = true ;  // 是第一個argument
    bool  isfloat = false ; // 預設是integer

    for ( tree = tree->right ; tree->atom.attribution != NIL ; tree = tree->right ) {

      tree->left = Evaluate( tree->left, false ) ;

      if ( tree->left->atom.attribution == INTEGER || tree->left->atom.attribution == FLOAT ) {

        if ( tree->left->atom.attribution == FLOAT )
          isfloat = true ;

        sscanf( tree->left->atom.strToken.c_str(), "%f", &transf ) ;

        if ( isfirst ) {
          isfirst = false ;
          fout = transf ;
        } // end if

        else
          fout = fout * transf ;

      } // end if

      else {
        uGlobal::uErrorFuncN = "*" ;
        mErrorPointer = tree->left ;
        throw INCORRECT_ARGUMENT_TYPE ;
      } // end else

    } // end for

    stringstream ss ;

    if ( isfloat ) {

      ss << fout;
      result->atom.strToken    = ss.str();
      result->atom.attribution = FLOAT ;

    } // end if

    else {

      ss << fout;
      result->atom.strToken    = ss.str() ;
      result->atom.attribution = INTEGER ;

    } // end else

  } // end else if

  else if ( funcStr == "#<procedure />" ) {

    int   iout    = 0 ;
    float fout    = 0.0 ;
    float transf  = 0.0 ;
    bool  isfirst = true ;  // 是第一個argument
    bool  isfloat = false ; // 預設是integer
    stringstream ss ;

    for ( tree = tree->right ; tree->atom.attribution != NIL ; tree = tree->right ) {

      tree->left = Evaluate( tree->left, false ) ;

      if ( tree->left->atom.attribution == INTEGER || tree->left->atom.attribution == FLOAT ) {

        sscanf( tree->left->atom.strToken.c_str(), "%f", &transf ) ;

        if ( tree->left->atom.attribution == FLOAT ) {

          isfloat = true ;

        } // end if

        if ( isfirst ) {

          isfirst = false ;
          fout    = transf ;

          if ( isfloat ) {

            tree = tree->right ;
            tree->left = Evaluate( tree->left, false ) ;

            if ( tree->left->atom.attribution == INTEGER || tree->left->atom.attribution == FLOAT ) {

              sscanf( tree->left->atom.strToken.c_str(), "%f", &transf ) ;

              if ( tree->left->atom.attribution == FLOAT ) {

                isfloat = true ;

              } // end if

              if ( transf != 0 )
                fout = fout / transf ;

              else
                throw DIVISION_BY_ZERO ;

            } // end if

              // check whether the type of the evaluated result is correct
              // "NOT" correct
            else {

              // throw Error Message
              // ERROR (XXX with incorrect argument type) : the-evaluated-result
              // XXX must bi the name of some primitive function
              uGlobal::uErrorFuncN = "/" ;
              mErrorPointer = tree->left ;
              throw INCORRECT_ARGUMENT_TYPE ;

            } // end else

          } // end if

            // is Integer
          else {

            tree = tree->right ;
            tree->left = Evaluate( tree->left, false ) ;

            if ( tree->left->atom.attribution == INTEGER || tree->left->atom.attribution == FLOAT ) {

              sscanf( tree->left->atom.strToken.c_str(), "%f", &transf ) ;

              if ( tree->left->atom.attribution == FLOAT ) {

                isfloat = true ;

              } // end if

              if ( transf != 0 ) {

                if ( isfloat ) {
                  fout = fout / transf ;
                } // end if

                else {
                  fout = ( int ) fout / ( int ) transf ;
                } // end else

              } // end if

              else {
                throw DIVISION_BY_ZERO ;
              } // end else

            } // end if

          } // end else

        } // end if

        else {

          if ( transf != 0 )
            fout = fout / transf ;

          else {
            throw DIVISION_BY_ZERO ;
          } // end else

        } // end else

      } // end if

        // check whether the type of the evaluated result is correct
        // "NOT" correct
      else {
        // throw Error Message
        // ERROR (XXX with incorrect argument type) : the-evaluated-result
        // XXX must bi the name of some primitive function

        uGlobal::uErrorFuncN = "/" ;
        mErrorPointer = tree->left ;
        throw INCORRECT_ARGUMENT_TYPE ;
      } // end else

    } // end for

    if ( isfloat ) {

      ss << fout;
      result->atom.strToken    = ss.str();
      result->atom.attribution = FLOAT ;

    } // end if

    else {

      iout = ( int ) fout ;
      ss << iout ;
      result->atom.strToken    = ss.str() ;
      result->atom.attribution = INTEGER ;

    } // end else

  } // end else if

  else if ( funcStr == "#<procedure not>" ) {

    Cons_node* arg = NewNode() ;
    arg = tree->right->left ;

    arg = Evaluate( arg, false ) ;

    if ( arg->atom.attribution == NIL ) {
      result->atom.strToken    = "#t" ;
      result->atom.attribution = T ;
    } // end if

    else {
      result->atom.strToken    = "nil" ;
      result->atom.attribution = NIL ;
    } // end else

  } // end else if

  else if ( funcStr == "#<procedure and>" ) {

    Cons_node* arg = NewNode() ;

    for ( tree = tree->right ; tree->atom.attribution != NIL ; tree = tree->right ) {

      arg = tree->left ;
      arg = Evaluate( arg, false ) ;

      if ( arg->atom.attribution == NIL ) {

        result->atom.strToken    = "nil" ;
        result->atom.attribution = NIL ;
        return result ;

      } // end if

      else {
        ; // Do Nothing
      } // end else

      if ( tree->right->atom.attribution == NIL ) {

        result = arg ;

      } // end if

    } // end for

  } // end else if

  else if ( funcStr == "#<procedure or>" ) {

    Cons_node* arg = NewNode() ;

    for ( tree = tree->right ; tree->atom.attribution != NIL ; tree = tree->right ) {

      arg = tree->left ;
      arg = Evaluate( arg, false ) ;

      if ( arg->atom.attribution == NIL ) {
        ; // Do Nothing
      } // end if

      else {

        result = arg ;
        return result ;

      } // end else

    } // end for

    result->atom.strToken    = "nil" ;
    result->atom.attribution = NIL ;

  } // end else if

  else if ( funcStr == "#<procedure >>" ) {

    Cons_node* argOne = NewNode() ;
    Cons_node* argTwo = NewNode() ;

    bool isCorrect = true ; // 用來判斷每次比對是否正確
    // 有錯就設為false
    // 無錯不會動
    // 預設true(無錯)

    for ( tree = tree->right ; tree->right->atom.attribution != NIL ; tree = tree->right ) {

      argOne = Evaluate( tree->left, false ) ;
      argTwo = Evaluate( tree->right->left, false ) ;

      if ( ( argOne->atom.attribution == INTEGER || argOne->atom.attribution == FLOAT ) &&
           ( argTwo->atom.attribution == INTEGER || argTwo->atom.attribution == FLOAT ) ) {

        float one = 0.0 ;
        float two = 0.0 ;

        sscanf( argOne->atom.strToken.c_str(), "%f", &one ) ;
        sscanf( argTwo->atom.strToken.c_str(), "%f", &two ) ;

        if ( one > two )
          ; // DO NOTHING

        else
          isCorrect = false ; // FALSE

      } // end if

        // check whether the type of the evaluated result is correct
        // "NOT" correct
      else {
        // throw Error Message
        // ERROR (XXX with incorrect argument type) : the-evaluated-result
        // XXX must bi the name of some primitive function

        if ( argOne->atom.attribution != INTEGER && argOne->atom.attribution != FLOAT )
          mErrorPointer = argOne ;

        else
          mErrorPointer = argTwo ;

        uGlobal::uErrorFuncN = ">" ;
        throw INCORRECT_ARGUMENT_TYPE ;
      } // end else

    } // end for

    if ( isCorrect ) {

      result->atom.strToken    = "#t" ;
      result->atom.attribution = T ;

    } // end if

    else {

      result->atom.strToken    = "nil" ;
      result->atom.attribution = NIL ;

    } // end else

  } // end else if

  else if ( funcStr == "#<procedure >=>" ) {

    Cons_node* argOne = NewNode() ;
    Cons_node* argTwo = NewNode() ;

    bool isCorrect = true ; // 用來判斷每次比對是否正確
    // 有錯就設為false
    // 無錯不會動
    // 預設true(無錯)

    for ( tree = tree->right ; tree->right->atom.attribution != NIL ; tree = tree->right ) {

      argOne = Evaluate( tree->left, false ) ;
      argTwo = Evaluate( tree->right->left, false ) ;

      if ( ( argOne->atom.attribution == INTEGER || argOne->atom.attribution == FLOAT ) &&
           ( argTwo->atom.attribution == INTEGER || argTwo->atom.attribution == FLOAT ) ) {

        float one = 0.0 ;
        float two = 0.0 ;

        sscanf( argOne->atom.strToken.c_str(), "%f", &one ) ;
        sscanf( argTwo->atom.strToken.c_str(), "%f", &two ) ;

        if ( one >= two )
          ; // DO NOTHING

        else
          isCorrect = false ; // FALSE

      } // end if

        // check whether the type of the evaluated result is correct
        // "NOT" correct
      else {
        // throw Error Message
        // ERROR (XXX with incorrect argument type) : the-evaluated-result
        // XXX must bi the name of some primitive function

        if ( argOne->atom.attribution != INTEGER && argOne->atom.attribution != FLOAT )
          mErrorPointer = argOne ;

        else
          mErrorPointer = argTwo ;

        uGlobal::uErrorFuncN = ">=" ;
        throw INCORRECT_ARGUMENT_TYPE ;
      } // end else

    } // end for

    if ( isCorrect ) {

      result->atom.strToken    = "#t" ;
      result->atom.attribution = T ;

    } // end if

    else {

      result->atom.strToken    = "nil" ;
      result->atom.attribution = NIL ;

    } // end else

  } // end else if

  else if ( funcStr == "#<procedure <>" ) {

    Cons_node* argOne = NewNode() ;
    Cons_node* argTwo = NewNode() ;

    bool isCorrect = true ; // 用來判斷每次比對是否正確
    // 有錯就設為false
    // 無錯不會動
    // 預設true(無錯)

    for ( tree = tree->right ; tree->right->atom.attribution != NIL ; tree = tree->right ) {

      argOne = Evaluate( tree->left, false ) ;
      argTwo = Evaluate( tree->right->left, false ) ;

      if ( ( argOne->atom.attribution == INTEGER || argOne->atom.attribution == FLOAT ) &&
           ( argTwo->atom.attribution == INTEGER || argTwo->atom.attribution == FLOAT ) ) {

        float one = 0.0 ;
        float two = 0.0 ;

        sscanf( argOne->atom.strToken.c_str(), "%f", &one ) ;
        sscanf( argTwo->atom.strToken.c_str(), "%f", &two ) ;

        if ( one < two )
          ; // DO NOTHING

        else
          isCorrect = false ; // FALSE

      } // end if

        // check whether the type of the evaluated result is correct
        // "NOT" correct
      else {
        // throw Error Message
        // ERROR (XXX with incorrect argument type) : the-evaluated-result
        // XXX must bi the name of some primitive function

        if ( argOne->atom.attribution != INTEGER && argOne->atom.attribution != FLOAT )
          mErrorPointer = argOne ;

        else
          mErrorPointer = argTwo ;

        uGlobal::uErrorFuncN = "<" ;
        throw INCORRECT_ARGUMENT_TYPE ;
      } // end else

    } // end for

    if ( isCorrect ) {

      result->atom.strToken    = "#t" ;
      result->atom.attribution = T ;

    } // end if

    else {

      result->atom.strToken    = "nil" ;
      result->atom.attribution = NIL ;

    } // end else

  } // end else if

  else if ( funcStr == "#<procedure <=>" ) {

    Cons_node* argOne = NewNode() ;
    Cons_node* argTwo = NewNode() ;

    bool isCorrect = true ; // 用來判斷每次比對是否正確
    // 有錯就設為false
    // 無錯不會動
    // 預設true(無錯)

    for ( tree = tree->right ; tree->right->atom.attribution != NIL ; tree = tree->right ) {

      argOne = Evaluate( tree->left, false ) ;
      argTwo = Evaluate( tree->right->left, false ) ;

      if ( ( argOne->atom.attribution == INTEGER || argOne->atom.attribution == FLOAT ) &&
           ( argTwo->atom.attribution == INTEGER || argTwo->atom.attribution == FLOAT ) ) {

        float one = 0.0 ;
        float two = 0.0 ;

        sscanf( argOne->atom.strToken.c_str(), "%f", &one ) ;
        sscanf( argTwo->atom.strToken.c_str(), "%f", &two ) ;

        if ( one <= two )
          ; // DO NOTHING

        else
          isCorrect = false ; // FALSE

      } // end if

        // check whether the type of the evaluated result is correct
        // "NOT" correct
      else {
        // throw Error Message
        // ERROR (XXX with incorrect argument type) : the-evaluated-result
        // XXX must bi the name of some primitive function

        if ( argOne->atom.attribution != INTEGER && argOne->atom.attribution != FLOAT )
          mErrorPointer = argOne ;

        else
          mErrorPointer = argTwo ;

        uGlobal::uErrorFuncN = "<=" ;
        throw INCORRECT_ARGUMENT_TYPE ;
      } // end else

    } // end for

    if ( isCorrect ) {

      result->atom.strToken    = "#t" ;
      result->atom.attribution = T ;

    } // end if

    else {

      result->atom.strToken    = "nil" ;
      result->atom.attribution = NIL ;

    } // end else

  } // end else if

  else if ( funcStr == "#<procedure =>" ) {

    Cons_node* argOne = NewNode() ;
    Cons_node* argTwo = NewNode() ;

    bool isCorrect = true ; // 用來判斷每次比對是否正確
    // 有錯就設為false
    // 無錯不會動
    // 預設true(無錯)

    for ( tree = tree->right ; tree->right->atom.attribution != NIL ; tree = tree->right ) {

      argOne = Evaluate( tree->left, false ) ;
      argTwo = Evaluate( tree->right->left, false ) ;

      if ( ( argOne->atom.attribution == INTEGER || argOne->atom.attribution == FLOAT ) &&
           ( argTwo->atom.attribution == INTEGER || argTwo->atom.attribution == FLOAT ) ) {

        float one = 0.0 ;
        float two = 0.0 ;

        sscanf( argOne->atom.strToken.c_str(), "%f", &one ) ;
        sscanf( argTwo->atom.strToken.c_str(), "%f", &two ) ;

        if ( one == two )
          ; // DO NOTHING

        else
          isCorrect = false ; // FALSE

      } // end if

        // check whether the type of the evaluated result is correct
        // "NOT" correct
      else {
        // throw Error Message
        // ERROR (XXX with incorrect argument type) : the-evaluated-result
        // XXX must bi the name of some primitive function

        if ( argOne->atom.attribution != INTEGER && argOne->atom.attribution != FLOAT )
          mErrorPointer = argOne ;

        else
          mErrorPointer = argTwo ;

        uGlobal::uErrorFuncN = "=" ;
        throw INCORRECT_ARGUMENT_TYPE ;
      } // end else

    } // end for

    if ( isCorrect ) {

      result->atom.strToken    = "#t" ;
      result->atom.attribution = T ;

    } // end if

    else {

      result->atom.strToken    = "nil" ;
      result->atom.attribution = NIL ;

    } // end else

  } // end else if

  else if ( funcStr == "#<procedure string-append>" ) {

    Cons_node* arg = NewNode() ;

    string resultStr = "" ;

    for ( tree = tree->right ; tree->atom.attribution != NIL ; tree = tree->right ) {

      arg = Evaluate( tree->left, false ) ;

      if ( arg->atom.attribution == STRING ) {

        resultStr = resultStr + arg->atom.strToken.substr( 1, arg->atom.strToken.length()-2 ) ;

      } // end if

        // check whether the type of the evaluated result is correct
        // "NOT" correct
      else {
        // throw Error Message
        // ERROR (XXX with incorrect argument type) : the-evaluated-result
        // XXX must bi the name of some primitive function

        uGlobal::uErrorFuncN = "string-append" ;
        mErrorPointer = arg ;
        throw INCORRECT_ARGUMENT_TYPE ;
      } // end else

    } // end for

    resultStr = "\"" + resultStr + "\"" ;

    result->atom.strToken    = resultStr ;
    result->atom.attribution = STRING ;

  } // end else if

  else if ( funcStr == "#<procedure string>?>" ) {

    Cons_node* argOne = NewNode() ;
    Cons_node* argTwo = NewNode() ;

    bool isCorrect = true ; // 用來判斷每次比對是否正確
    // 有錯就設為false
    // 無錯不會動
    // 預設true(無錯)

    tree = tree->right ;
    argOne = Evaluate( tree->left, false ) ;

    for ( ; tree->right->atom.attribution != NIL ; tree = tree->right ) {

      argTwo = Evaluate( tree->right->left, false ) ;

      if ( argOne->atom.attribution == STRING && argTwo->atom.attribution == STRING ) {

        if ( argOne->atom.strToken > argTwo->atom.strToken )
          ; // DO NOTHING

        else
          isCorrect = false ; // FALSE

      } // end if

        // check whether the type of the evaluated result is correct
        // "NOT" correct
      else {
        // throw Error Message
        // ERROR (XXX with incorrect argument type) : the-evaluated-result
        // XXX must bi the name of some primitive function

        if ( argOne->atom.attribution != STRING )
          mErrorPointer = argOne ;

        else
          mErrorPointer = argTwo ;

        uGlobal::uErrorFuncN = "string>?" ;
        throw INCORRECT_ARGUMENT_TYPE ;

      } // end else

      argOne = argTwo ;

    } // end for

    if ( isCorrect ) {

      result->atom.strToken    = "#t" ;
      result->atom.attribution = T ;

    } // end if

    else {

      result->atom.strToken    = "nil" ;
      result->atom.attribution = NIL ;

    } // end else

  } // end else if

  else if ( funcStr == "#<procedure string<?>" ) {

    Cons_node* argOne = NewNode() ;
    Cons_node* argTwo = NewNode() ;

    bool isCorrect = true ; // 用來判斷每次比對是否正確
    // 有錯就設為false
    // 無錯不會動
    // 預設true(無錯)

    tree = tree->right ;
    argOne = Evaluate( tree->left, false ) ;

    for ( ; tree->right->atom.attribution != NIL ; tree = tree->right ) {

      argTwo = Evaluate( tree->right->left, false ) ;

      if ( argOne->atom.attribution == STRING && argTwo->atom.attribution == STRING ) {

        if ( argOne->atom.strToken < argTwo->atom.strToken )
          ; // DO NOTHING

        else
          isCorrect = false ; // FALSE

      } // end if

        // check whether the type of the evaluated result is correct
        // "NOT" correct
      else {
        // throw Error Message
        // ERROR (XXX with incorrect argument type) : the-evaluated-result
        // XXX must bi the name of some primitive function

        if ( argOne->atom.attribution != STRING )
          mErrorPointer = argOne ;

        else
          mErrorPointer = argTwo ;

        uGlobal::uErrorFuncN = "string<?" ;
        throw INCORRECT_ARGUMENT_TYPE ;

      } // end else

      argOne = argTwo ;

    } // end for

    if ( isCorrect ) {

      result->atom.strToken    = "#t" ;
      result->atom.attribution = T ;

    } // end if

    else {

      result->atom.strToken    = "nil" ;
      result->atom.attribution = NIL ;

    } // end else

  } // end else if

  else if ( funcStr == "#<procedure string=?>" ) {

    Cons_node* argOne = NewNode() ;
    Cons_node* argTwo = NewNode() ;

    bool isCorrect = true ; // 用來判斷每次比對是否正確
    // 有錯就設為false
    // 無錯不會動
    // 預設true(無錯)

    tree = tree->right ;
    argOne = Evaluate( tree->left, false ) ;

    for ( ; tree->right->atom.attribution != NIL ; tree = tree->right ) {

      argTwo = Evaluate( tree->right->left, false ) ;

      if ( argOne->atom.attribution == STRING && argTwo->atom.attribution == STRING ) {

        if ( argOne->atom.strToken == argTwo->atom.strToken )
          ; // DO NOTHING

        else
          isCorrect = false ; // FALSE

      } // end if

        // check whether the type of the evaluated result is correct
        // "NOT" correct
      else {
        // throw Error Message
        // ERROR (XXX with incorrect argument type) : the-evaluated-result
        // XXX must bi the name of some primitive function

        if ( argOne->atom.attribution != STRING )
          mErrorPointer = argOne ;

        else
          mErrorPointer = argTwo ;

        uGlobal::uErrorFuncN = "string=?" ;
        throw INCORRECT_ARGUMENT_TYPE ;

      } // end else

      argOne = argTwo ;

    } // end for

    if ( isCorrect ) {

      result->atom.strToken    = "#t" ;
      result->atom.attribution = T ;

    } // end if

    else {

      result->atom.strToken    = "nil" ;
      result->atom.attribution = NIL ;

    } // end else

  } // end else if

  else if ( funcStr == "#<procedure eqv?>" ) {

    Cons_node* argOne = NewNode() ;
    Cons_node* argTwo = NewNode() ;

    bool isSame = true ;  // 比對兩者是否有不一樣的地方
    // 預設都一樣(true)

    argOne = tree->right->left ;
    argTwo = tree->right->right->left ;

    argOne = Evaluate( argOne, false ) ;
    argTwo = Evaluate( argTwo, false ) ;

    if ( argOne == argTwo ) {
      result->atom.strToken    = "#t" ;
      result->atom.attribution = T ;
    } // end if

    else {
      // 只有一個節點(看argOne)
      if ( argOne->left == NULL && argOne->right == NULL ) {

        if ( argOne->atom.attribution == argTwo->atom.attribution ) {

          // FLOAT
          if ( argOne->atom.attribution == FLOAT ) {

            float fout1 = 0.0 ;
            float fout2 = 0.0 ;

            sscanf( argOne->atom.strToken.c_str(), "%f", &fout1 ) ;
            sscanf( argTwo->atom.strToken.c_str(), "%f", &fout2 ) ;

            if ( fout1 == fout2 )
              ; // Do Nothing

            else
              isSame = false ;

          } // end if

            // INTEGER
          else if ( argOne->atom.attribution == INTEGER ) {

            int fout1 = 0 ;
            int fout2 = 0 ;

            sscanf( argOne->atom.strToken.c_str(), "%d", &fout1 ) ;
            sscanf( argTwo->atom.strToken.c_str(), "%d", &fout2 ) ;

            if ( fout1 == fout2 )
              ; // Do Nothing

            else
              isSame = false ;

          } // end else if

            // BOOLEAN
          else if ( argOne->atom.attribution == T || argOne->atom.attribution == NIL ) {

            ; // Do Nothing

          } // end else if

            // SYMBOL
          else if ( argOne->atom.attribution == SYMBOL ) {

            bool hasData = false ;

            for ( int i = 0 ; i < 39 ; i++ ) {

              string procedure = "" ;
              procedure.clear() ;
              procedure        = "#<procedure " + uGlobal::uFunctionName[i] + ">" ;

              if ( argOne->atom.strToken == procedure && argTwo->atom.strToken == procedure &&
                   procedure != "#<procedure quote>" )
                hasData = true ; // Do Nothing

            } // end for

            if ( hasData )
              isSame = true ;

            else
              isSame = false ;


          } // end else if

          else
            isSame = false ;

        } // end if

        else
          isSame = false ;

      } // end if

      else
        isSame = false ;

      if ( isSame ) {
        result->atom.strToken    = "#t" ;
        result->atom.attribution = T ;
      } // end if

      else {
        result->atom.strToken    = "nil" ;
        result->atom.attribution = NIL ;
      } // end else

    } // end else

  } // end else if

  else if ( funcStr == "#<procedure equal?>" ) {

    Cons_node* argOne = NewNode() ;
    Cons_node* argTwo = NewNode() ;

    bool isSame = true ;  // 比對兩者是否有不一樣的地方
    // 預設都一樣(true)

    argOne = tree->right->left ;
    argTwo = tree->right->right->left ;

    argOne = Evaluate( argOne, false ) ;
    argTwo = Evaluate( argTwo, false ) ;

    // 有兩種case
    // 1. 只有一個節點(看argOne)
    // 2. 是棵樹(看argOne)

    // 只有一個節點(看argOne)
    if ( argOne->left == NULL && argOne->right == NULL ) {

      if ( argOne->atom.attribution == argTwo->atom.attribution ) {

        // FLOAT
        if ( argOne->atom.attribution == FLOAT ) {

          float fout1 = 0.0 ;
          float fout2 = 0.0 ;

          sscanf( argOne->atom.strToken.c_str(), "%f", &fout1 ) ;
          sscanf( argTwo->atom.strToken.c_str(), "%f", &fout2 ) ;

          if ( fout1 == fout2 )
            ; // Do Nothing

          else
            isSame = false ;

        } // end if

          // INTEGER
        else if ( argOne->atom.attribution == INTEGER ) {

          int fout1 = 0 ;
          int fout2 = 0 ;

          sscanf( argOne->atom.strToken.c_str(), "%d", &fout1 ) ;
          sscanf( argTwo->atom.strToken.c_str(), "%d", &fout2 ) ;

          if ( fout1 == fout2 )
            ; // Do Nothing

          else
            isSame = false ;

        } // end else if

          // BOOLEAN
        else if ( argOne->atom.attribution == T || argOne->atom.attribution == NIL ) {

          ; // Do Nothing

        } // end else if

          // SYMBOL
        else if ( argOne->atom.attribution == SYMBOL ) {

          if ( argOne->atom.strToken == argTwo->atom.strToken )
            ; // Do Nothing
          else
            isSame = false ;

        } // end else if

        else if ( argOne->atom.attribution == STRING ) {

          if ( argOne->atom.strToken == argTwo->atom.strToken )
            ; // Do Nothing
          else
            isSame = false ;

        } // end else if

        else
          isSame = false ;

      } // end if

      else
        isSame = false ;

    } // end if

      // 是棵樹(看argOne)
    else {
      isSame = CompareTree( argOne, argTwo ) ;
      // isSame = CompareTree( argOne, argTwo ) ;
      // 兩棵樹(argOne,argTwo)長  一樣  時，return true
      //                         不一樣   ，return false
    } // end else

    if ( isSame ) {
      result->atom.strToken    = "#t" ;
      result->atom.attribution = T ;
    } // end if

    else {
      result->atom.strToken    = "nil" ;
      result->atom.attribution = NIL ;
    } // end else

  } // end else if

  else if ( funcStr == "#<procedure begin>" ) {

    Cons_node* arg = NewNode() ;

    for ( tree = tree->right ; tree->atom.attribution != NIL ; tree = tree->right ) {

      arg = tree->left ;
      arg = Evaluate( arg, false ) ;

      if ( tree->right->atom.attribution == NIL ) {

        result = arg ;

      } // end if

    } // end for

  } // end else if

  else if ( funcStr == "#<procedure if>" ) {

    Cons_node* argOne = NewNode() ;
    Cons_node* argTwo = NewNode() ;
    Cons_node* argThree = NewNode() ;

    argOne   = tree->right->left ;
    argTwo   = tree->right->right->left ;

    argOne   = Evaluate( argOne, false ) ;

    if ( numofArg == 2 ) {

      if ( argOne->atom.attribution == NIL ) {

        result = NULL ;

      } // end if

      else {

        argTwo   = Evaluate( argTwo, false ) ;
        result = argTwo ;

      } // end else

    } // end if

    else if ( numofArg == 3 ) {

      argThree = tree->right->right->right->left ;

      if ( argOne->atom.attribution == NIL ) {

        argThree = Evaluate( argThree, false ) ;
        result = argThree ;

      } // end if

      else {

        argTwo   = Evaluate( argTwo, false ) ;
        result = argTwo ;

      } // end else

    } // end else if

  } // end else if

  else if ( funcStr == "#<procedure cond>" ) {

    Cons_node* arg    = NewNode() ;
    Cons_node* argOne = NewNode() ;
    bool isFirstArgNIL = true ;

    // 跑cond層的龍骨
    for ( tree = tree->right ; tree->atom.attribution != NIL ; tree = tree->right ) {

      // 目前argument樹中的第一個argument
      argOne = tree->left->left ;

      // 確認目前argument樹中的第一個argument是否為"else"
      // 是，目前argument樹中的第一個argument是"else"
      if ( argOne->atom.strToken == "else" ) {

        // 確認目前是否為argument樹(跟cond同一層)中，龍骨的倒數第二個節點(最後一個nil)
        // 是，龍骨的倒數第二個節點
        if ( tree->right->atom.attribution == NIL ) {

          isFirstArgNIL = false ;

        } // end if

          // 否，不是龍骨的倒數第二個節點
        else {

          arg = Evaluate( argOne, false ) ;

          if ( arg->atom.attribution == NIL )
            isFirstArgNIL = true ;

          else
            isFirstArgNIL = false ;

        } // end else

      } // end if

        // 否，目前argument樹中的第一個argument不是"else"
      else {

        arg = Evaluate( argOne, false ) ;

        if ( arg->atom.attribution == NIL )
          isFirstArgNIL = true ;

        else
          isFirstArgNIL = false ;

      } // end else

      if ( isFirstArgNIL ) {
        ; // Do Nothing
      } // end if

      else {

        Cons_node* argTree = NewNode() ;

        for ( argTree = tree->left->right ; argTree->atom.attribution != NIL ; argTree = argTree->right ) {

          result = Evaluate( argTree->left, false ) ;

          if ( argTree->right->atom.attribution == NIL ) {

            return result ;

          } // end if

        } // end for

      } // end else

    } // end for

    result = NULL ;

  } // end else if

  return result ;

}

// 兩棵樹(argOne,argTwo)長  一樣  時，return true
//                         不一樣   ，return false
bool ObjEvaluation::CompareTree(ObjParser::Cons_node *one, ObjParser::Cons_node *two) {

  bool returnvalue = true ;

  if ( one != NULL && two != NULL ) {

    if ( one->atom.strToken != two->atom.strToken )
      return false ;

    returnvalue = CompareTree( one->left, two->left ) ;

    if ( !returnvalue )
      return false ;

    returnvalue = CompareTree( one->right, two->right ) ;

    if ( !returnvalue )
      return false ;

  } // end if

  if ( !returnvalue )
    return false ;
  else
    return true ;

}

// HOW : 如何使用此function
// 傳入 1. 一棵tree 2. 一個integer，(用來記錄變數的數量 call by reference)
// 依function name來確認要evaluate的變數數量是否符合規則
// 是，變數數量符合規則 return true
// 否，變數數量不符合規則 return false
//
// WHAT : 如何implement此function
// 先計算變數數量
// 再依function name與其規則判定是否符合
bool ObjEvaluation::CheckNumberOfVariable(ObjParser::Cons_node *tree, int &numofArg) {

  int numberofvariable = -1 ; // 如果第一個節點是LP
  // 則第二個節點就是function
  // 剩下到nil(RP)前就是要evaluate的變數
  Cons_node* firstArg = tree->left ; // first argument

  // 計算此S-exp的變數數量
  for ( Cons_node *iter = tree ; iter->atom.attribution != NIL ; iter = iter->right )
    numberofvariable = numberofvariable + 1 ;

  numofArg = numberofvariable ;

  // 此function是 'cons'
  if ( firstArg->atom.strToken == "#<procedure cons>" ) {

    // check whether the number of arguments is correct
    if ( numberofvariable == 2 )
      return true ;

  } // end if

    // 此function是 'list'
  else if ( firstArg->atom.strToken == "#<procedure list>" ) {

    // check whether the number of arguments is correct
    if ( numberofvariable >= 0 )
      return true ;

  } // end else if

    // 此function是 'quote'
  else if ( firstArg->atom.strToken == "#<procedure quote>" ) {

    // check whether the number of arguments is correct
    if ( numberofvariable == 1 )
      return true ;

  } // end else if

    // 此function是 'define'
  else if ( firstArg->atom.strToken == "#<procedure define>" ) {

    // check whether the number of arguments is correct
    if ( numberofvariable == 2 )
      return true ;

  } // end else if

    // 此function是 'car'
  else if ( firstArg->atom.strToken == "#<procedure car>" ) {

    // check whether the number of arguments is correct
    if ( numberofvariable == 1 )
      return true ;

  } // end else if

    // 此function是 'cdr'
  else if ( firstArg->atom.strToken == "#<procedure cdr>" ) {

    // check whether the number of arguments is correct
    if ( numberofvariable == 1 )
      return true ;

  } // end else if

    // 此function是 'atom?', 'pair?', 'list?', 'null?', 'integer?'
    //              'real?', 'number?', 'string?', 'boolean?', 'symbol?'
  else if ( firstArg->atom.strToken == "#<procedure atom?>" ||
            firstArg->atom.strToken == "#<procedure pair?>" ||
            firstArg->atom.strToken == "#<procedure list?>" ||
            firstArg->atom.strToken == "#<procedure null?>" ||
            firstArg->atom.strToken == "#<procedure integer?>" ||
            firstArg->atom.strToken == "#<procedure real?>" ||
            firstArg->atom.strToken == "#<procedure number?>" ||
            firstArg->atom.strToken == "#<procedure string?>" ||
            firstArg->atom.strToken == "#<procedure boolean?>" ||
            firstArg->atom.strToken == "#<procedure symbol?>" ) {

    // check whether the number of arguments is correct
    if ( numberofvariable == 1 )
      return true ;

  } // end else if

    // 此function是 '+', '-', '*', '/'
  else if ( firstArg->atom.strToken == "#<procedure +>" || firstArg->atom.strToken == "#<procedure ->" ||
            firstArg->atom.strToken == "#<procedure *>" || firstArg->atom.strToken == "#<procedure />" ) {

    // check whether the number of arguments is correct
    if ( numberofvariable >= 2 )
      return true ;

  } // end else if

    // 此function是 'not'
  else if ( firstArg->atom.strToken == "#<procedure not>" ) {

    // check whether the number of arguments is correct
    if ( numberofvariable == 1 )
      return true ;

  } // end else if

    // 此function是 'and', 'or'
  else if ( firstArg->atom.strToken == "#<procedure and>" ||
            firstArg->atom.strToken == "#<procedure or>" ) {

    // check whether the number of arguments is correct
    if ( numberofvariable >= 2 )
      return true ;

  } // end else if

    // 此function是 '>', '>=', '<', '<=', '='
    //              'string-append', 'string>?', 'string<?', 'string=?'
  else if ( firstArg->atom.strToken == "#<procedure >>" ||
            firstArg->atom.strToken == "#<procedure >=>" ||
            firstArg->atom.strToken == "#<procedure <>" ||
            firstArg->atom.strToken == "#<procedure <=>" ||
            firstArg->atom.strToken == "#<procedure =>" ||
            firstArg->atom.strToken == "#<procedure string-append>" ||
            firstArg->atom.strToken == "#<procedure string>?>" ||
            firstArg->atom.strToken == "#<procedure string<?>" ||
            firstArg->atom.strToken == "#<procedure string=?>" ) {

    // check whether the number of arguments is correct
    if ( numberofvariable >= 2 )
      return true ;

  } // end else if

    // 此function是 'eqv?', 'equal?'
  else if ( firstArg->atom.strToken == "#<procedure eqv?>" ||
            firstArg->atom.strToken == "#<procedure equal?>" ) {

    // check whether the number of arguments is correct
    if ( numberofvariable == 2 )
      return true ;

  } // end else if

    // 此function是 'begin'
  else if ( firstArg->atom.strToken == "#<procedure begin>" ) {

    // check whether the number of arguments is correct
    if ( numberofvariable >= 1 )
      return true ;

  } // end else if

    // 此function是 'if'
  else if ( firstArg->atom.strToken == "#<procedure if>" ) {

    // check whether the number of arguments is correct
    if ( numberofvariable == 2 || numberofvariable == 3 )
      return true ;

  } // end else if

    // 此function是 'cond'
  else if ( firstArg->atom.strToken == "#<procedure cond>" ) {

    // check whether the number of arguments is correct
    if ( numberofvariable >= 1 )
      return true ;

  } // end else if

    // 此function是 'clean-environment', 'exit'
  else if ( firstArg->atom.strToken == "#<procedure clean-environment>" ||
            firstArg->atom.strToken == "#<procedure exit>" ) {

    // check whether the number of arguments is correct
    if ( numberofvariable == 0 )
      return true ;

  } // end else if

  return false ;

}

string ObjEvaluation::RepairNameOfInternalFunction(string symbol) {

  string internalFunction = "" ;
  internalFunction.clear() ;

  for ( int i = 0 ; i < 39 ; i++ ) {

    string procedure = "" ;
    procedure.clear() ;
    procedure        = "#<procedure " + uGlobal::uFunctionName[i] + ">" ;

    if ( symbol == procedure )
      internalFunction = uGlobal::uFunctionName[i] ;

  } // end for

  return internalFunction ;

}

bool ObjEvaluation::CheckPureList(ObjParser::Cons_node *tree) {

  for ( ; tree != NULL ; tree = tree->right ) {

    if ( tree->right == NULL && tree->atom.attribution == NIL )
      return true ;

  } // end for

  return false ;

}
// private

// public

// Constructor
ObjEvaluation::ObjEvaluation() {
  mReturnPointer = NULL ;
  mErrorPointer  = NULL ;
  mIsQuote = false ;
}

// 重新初始化
void ObjEvaluation::ReObjEvalutaion() {
  mReturnPointer = NULL ;
  mErrorPointer  = NULL ;
  mIsQuote = false ;
}

ObjParser::Cons_node* ObjEvaluation::Evaluate(ObjParser::Cons_node *tree, bool topLevel) {

  Cons_node* result ; // evaluated result
  int numofArg = 0 ;  // 紀錄除了first argument以外的argument數量

  // 不是tree，只有一個節點
  if ( tree->left == NULL && tree->right == NULL ) {

    // 是atom，不是symbol
    if ( IsAtom( tree->atom.attribution ) && tree->atom.attribution != SYMBOL ) {
      result = tree ; // return the atom
    } // end if

      // 是symbol(不是atom)
    else if ( tree->atom.attribution == SYMBOL ) {

      Cons_node* node = NewNode() ;
      // 確認此symbol是否有binding到S-exp或internal function

      // 沒有binding(unbound)

      node = CheckBindingAndReturnThePointer( tree->atom.strToken ) ;

      if ( node == NULL ) {

        uGlobal::uErrorToken = tree->atom.strToken ;
        throw UNBOUND_SYMBOL ;

        // throw Error Message
        // ERROR (unbound symbol) : SYMBOL
      } // end if

        // 有binding到
      else {
        // return the binding of the S-exp or internal function
        result = node ;
      } // end else

    } // end else if

  } // end if

    // 是棵tree
  else {

    // 先確認此tree是" pure list "
    // 如果不是，就ERROR

    // 此tree "不是" pure list
    if ( !CheckPureList( tree ) ) {
      mErrorPointer = tree ;
      throw NON_LIST ;
      // throw Error Message
      // ERROR (non-list) : PrettyPrint(tree)
    } // end if

    // 當第一個元素(element、argument)是atom，非symbol
    if ( IsAtom( tree->left->atom.attribution ) && tree->left->atom.attribution != SYMBOL &&
         tree->left->atom.attribution != QUOTE ) {

      uGlobal::uErrorToken = tree->left->atom.strToken ;
      throw ATTEMPT_TO_APPLY_NON_FUNCTION ;

      // throw Error Message
      // ERROR (attempt to apply non-function) : First-Argument
    } // end else if

      // 當第一個元素(element、argument)是symbol，非atom
    else if ( tree->left->atom.attribution == SYMBOL || tree->left->atom.attribution == QUOTE ) {

      string errToken = "" ;
      errToken = tree->left->atom.strToken ;
      // 確認此symbol是否為已知function(the name of function)
      // (symbol has a binding, and that binding is an internal function)
      // 是
      tree->left = CheckBindingAndReturnThePointer( tree->left->atom.strToken ) ;

      if ( tree->left != NULL ) {

        bool isInternalFunction = false ;

        for ( int i = 0 ; i < 39 ; i++ ) {

          string procedure = "" ;
          procedure.clear() ;
          procedure        = "#<procedure " + uGlobal::uFunctionName[i] + ">" ;

          if ( tree->left->atom.strToken == procedure )
            isInternalFunction = true ;

        } // end for

        if ( isInternalFunction )
          ; // Do Nothing

        else {
          uGlobal::uErrorToken = tree->left->atom.strToken ;
          throw ATTEMPT_TO_APPLY_NON_FUNCTION ;
        } // end else

        // 如果 the current level is "NOT" the top level，且symbol為 'clean-environment'、'define'、'exit'其中之一
        if ( !topLevel && ( tree->left->atom.strToken == "#<procedure clean-environment>" ||
                            tree->left->atom.strToken == "#<procedure define>" ||
                            tree->left->atom.strToken == "#<procedure exit>" ) ) {

          uGlobal::uErrorToken = tree->left->atom.strToken ;
          throw LEVEL_ERROR ;

          // throw Error Message
          // ERROR (level of CLEAN-ENVIRONMENT / DEFINE / EXIT)
        } // end if

        // 如果此symbol是'define'、'set!'、'let'、'cond'、'lambda'其中之一
        else if ( tree->left->atom.strToken == "#<procedure define>" ||
                  tree->left->atom.strToken == "#<procedure set!>" ||
                  tree->left->atom.strToken == "#<procedure let>" ||
                  tree->left->atom.strToken == "#<procedure cond>" ||
                  tree->left->atom.strToken == "#<procedure lambda>" ) {

          // 確認此expression的格式(format)是否正確
          // (define symbol S-exp) 只能宣告或設定非primitive 的symbol
          // (define (one-or-more-symbols) one-or-more-S-exp)
          // (set! symbol S-exp)
          // (lambda (zero-or-more-symbols one-or-more-S-exp)
          // (let (zero-or-more-PAIRS) one-or-more-S-exp)
          // (cond one-or-more-AT-LEAST-DOUNBLETONS)
          // PAIR df = (symbol S-exp)
          // AT-LEAST-DOUBLETONS df = a list of two or more S-exp
          // 否。格式錯誤
          if ( !CheckFormat( tree ) ) {

            mErrorPointer = tree ;
            uGlobal::uErrorToken   = tree->left->atom.strToken ;
            throw FORMAT_ERROR ;

            // throw Error Message
            // ERROR (COND format) : <the main S-exp>
            // ERROR (DEFINE format) : <the main S-exp>  // 有可能是因為redefining primitive之故
            // ERROR (SET! format) : <the main S-exp>  // 有可能是因為redefining primitive之故
            // ERROR (LET format) : <the main S-exp>  // 有可能是因為redefining primitive之故
            // ERROR (LAMBDA format) : <the main S-exp>  // 有可能是因為redefining primitive之故
          } // end if

          // Project 3
          // 是，格是正確
          // evaluate this tree
          // return the result of the evaluate(and exit this call to Evaluate)

        } // end else if

          // 如果此symbol是'if'、'and'、'or'其中之一
        else if ( tree->left->atom.strToken == "#<procedure if>" ||
                  tree->left->atom.strToken == "#<procedure and>" ||
                  tree->left->atom.strToken == "#<procedure or>" ) {

          // check whether the number of arguments is correct
          // "NOT" correct
          if ( !CheckNumberOfVariable( tree, numofArg ) ) {

            uGlobal::uErrorToken = RepairNameOfInternalFunction( tree->left->atom.strToken ) ;
            throw INCORRECT_NUMBER_OF_ARGUMENT ;

            // throw Error Message
            // ERROR (incorrect number of arguments) : if / and / or

          } // end if

          // correct
          // evaluate this tree
          // return the evaluated result( and exit this call to Evaluate() )

        } // end else if

          // 此symbol是一個已知的function，但不是'define'、'let'、'cond'、'lambda'其中之一
        else {

          // check whether the number of arguments is correct
          // "NOT" correct
          if ( !CheckNumberOfVariable( tree, numofArg ) ) {

            uGlobal::uErrorToken = RepairNameOfInternalFunction( tree->left->atom.strToken ) ;
            throw INCORRECT_NUMBER_OF_ARGUMENT ;

            // throw Error Message
            // ERROR (incorrect number of arguments) : SYMBOL
          } // end if

        } // end else

      } // end if

        // 此symbol並非任何已知的function
      else {

        // throw Error Message
        // ERROR (unbound symbol) : SYMBOL
        // ERROR (attempt to apply non-function) : ATOM // ATOM is the binding of Symbol
        // cout << "ERROR (unbound symbol)" << endl ;

        uGlobal::uErrorToken = errToken ;
        throw UNBOUND_SYMBOL ;

      } // end else

    } // end else if

      // 當第一個元素(element、argument)是一棵樹 -> (...) tree A
    else {

      Cons_node* errToken = NewNode() ;

      // evaluate tree A
      tree->left = Evaluate( tree->left, false ) ;
      errToken = tree->left ;

      // 如果沒有任何ERROR發生在evaluate tree A
      // 則動作繼續

      // 確認evaluate tree A的結果是否是一個internal function
      // 是，it is an internal function
      if ( mIsQuote ) {
        mIsQuote = false ;
        uGlobal::uErrorToken = errToken->atom.strToken ;
        throw ATTEMPT_TO_APPLY_NON_FUNCTION ;
        // throw Error Message
        // ERROR (attempt to apply non-function) : First-Argument
      } // end if

      else {

        bool isInternalFunction = false ;

        for ( int i = 0 ; i < 39 ; i++ ) {

          string procedure = "" ;
          procedure.clear() ;
          procedure        = "#<procedure " + uGlobal::uFunctionName[i] + ">" ;

          if ( tree->left->atom.strToken == procedure )
            isInternalFunction = true ;

        } // end for

        if ( isInternalFunction )
          ; // Do Nothing

        else
          tree->left = CheckBindingAndReturnThePointer( tree->left->atom.strToken ) ;

      } // end else

      if ( tree->left != NULL ) {

        // check whether the number of arguments is correct
        // "NOT" correct
        if ( !CheckNumberOfVariable( tree, numofArg ) ) {

          uGlobal::uErrorToken = RepairNameOfInternalFunction( tree->left->atom.strToken ) ;
          throw INCORRECT_NUMBER_OF_ARGUMENT ;

          // throw Error Message
          // ERROR (incorrect number of arguments) : name-of-the-function
          // ERROR (incorrect number of arguments) : lambda expression // in the case of nameless functions

        } // end if

      } // end if

        // 否，it is "NOT" an internal function
      else {

        mErrorPointer = errToken ;
        throw ATTEMPT_TO_APPLY_NON_FUNCTION_2 ;

        // throw Error Message
        // ERROR (attempt to apply non-function) : * // * is the evaluated result

      } // end else

    } // end else

    result = EvalFunctions( tree, numofArg ) ; // evaluate Function

  } // end else

  // 如果"沒有"evaluate的結果要被return
  if ( result == NULL ) {
    // throw Error Message
    // ERROR (no return result) : name-of-this-function
    // ERROR (no return result) : lambda expression // if there is such a case ...

    mErrorPointer = tree ;
    throw NO_RETURN_VALUE ;

  } // end if

  return result ;

}

void ObjEvaluation::ErrorPrettyPrint(ObjParser::Cons_node *tree, int level) {

  float fout = 0.0 ;

  if ( tree != NULL ) {

    // 是一個節點 && 是第一層
    if ( level == 0 && IsAtom( tree->atom.attribution ) ) {

      // 確認此節點是否為float
      if ( tree->atom.attribution == FLOAT ) {

        sscanf( tree->atom.strToken.c_str(), "%f", &fout ) ;
        printf( "%.3f\n", fout ) ;

      } // end if

        // 非float的atom
      else {

        bool isInternalFunction = false ;
        string functionName     = "" ;

        for ( int i = 0 ; i < 39 ; i++ ) {

          string procedure = "" ;
          procedure.clear() ;
          procedure        = "#<procedure " + uGlobal::uFunctionName[i] + ">" ;

          if ( tree->atom.strToken == procedure ) {

            functionName = uGlobal::uFunctionName[i] ;
            isInternalFunction = true ;

          } // end if

        } // end for

        if ( isInternalFunction )
          cout << functionName << endl ;

        else
          cout << tree->atom.strToken << endl ;

      } // end else

    } // end if

      // 一棵樹
    else {

      bool first = true ;  // 第一個node
      bool right = false ; // 預設上一個節點是從左子樹來的
      // 用於印出atom時

      // 右子樹(龍骨)
      while ( tree != NULL ) {

        // 左子樹
        // 此龍骨中第一個左括弧
        if ( tree->atom.attribution == LEFT_PAREN  && first ) {

          first = false ;

          // 上一個印出的節點"不是"左括弧
          if ( !mIsLastNodeLP ) {
            PrintSpace( level ) ;
          } // end if

          cout << tree->atom.strToken << ' ' ;

          mIsLastNodeLP = true ;
          level = level + 1 ;
          ErrorPrettyPrint( tree->left, level ) ;

        } // end if

          // 此龍骨中"非"第一個左括弧
        else if ( tree->atom.attribution == LEFT_PAREN  && !first ) {

          if ( tree->left != NULL )
            ErrorPrettyPrint( tree->left, level ) ;

        } // end else if

          // atom
        else if ( tree->atom.attribution != LEFT_PAREN ) {

          // 上一個節點是右子樹的
          if ( right ) {

            // 此樹的右節點結尾是nil
            if ( tree->atom.attribution == NIL ) {
              ;
            } // end else if

              // 此樹的右節點結尾"不"是nil
            else {

              PrintSpace( level ) ;
              cout << '.' << endl ; // 印出點
              PrintSpace( level ) ;

              if ( tree->atom.attribution == FLOAT ) {
                sscanf( tree->atom.strToken.c_str(), "%f", &fout ) ;
                printf( "%.3f\n", fout ) ;
              } // end if

              else {

                bool isInternalFunction = false ;
                string functionName     = "" ;

                for ( int i = 0 ; i < 39 ; i++ ) {

                  string procedure = "" ;
                  procedure.clear() ;
                  procedure        = "#<procedure " + uGlobal::uFunctionName[i] + ">" ;

                  if ( tree->atom.strToken == procedure ) {

                    functionName = uGlobal::uFunctionName[i] ;
                    isInternalFunction = true ;

                  } // end if

                } // end for

                // 印出此節點內容與換行
                if ( isInternalFunction )
                  cout << functionName << endl ;

                else
                  cout << tree->atom.strToken << endl ;

              } // end else

            } // end else

            PrintSpace( level-1 ) ;
            cout << ')' << endl ;   // 補印右括號
            right = false ;

          } // end if

            // 上一個節點是左子樹的
          else {

            // 上一個印出的節點"不是"左括弧
            if ( !mIsLastNodeLP ) {
              PrintSpace( level ) ;
            } // end if

            if ( tree->atom.attribution == FLOAT ) {
              sscanf( tree->atom.strToken.c_str(), "%f", &fout ) ;
              printf( "%.3f\n", fout ) ;
            } // end if

            else {

              bool isInternalFunction = false ;
              string functionName     = "" ;

              for ( int i = 0 ; i < 39 ; i++ ) {

                string procedure = "" ;
                procedure.clear() ;
                procedure        = "#<procedure " + uGlobal::uFunctionName[i] + ">" ;

                if ( tree->atom.strToken == procedure ) {
                  functionName = uGlobal::uFunctionName[i] ;
                  isInternalFunction = true ;
                } // end if

              } // end for

              // 印出此節點內容與換行
              if ( isInternalFunction )
                cout << functionName << endl ;

              else
                cout << tree->atom.strToken << endl ;

            } // end else

          } // end else

          mIsLastNodeLP = false ;

        } // end else if

        tree = tree->right ;
        right = true ;

      } // end while

    } // end else

  } // end if

}
// public