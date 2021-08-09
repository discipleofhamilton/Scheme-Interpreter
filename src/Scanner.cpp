//
// Created by Hamilton Chang MSI on 2019/1/16.
//

#include "Scanner.h"

// private
/**
 * string isssue處理字串問題
 * WHAT: There are three situations:
 *    1. COMMON: meet one ", find the second one, make a pair of it
 *    2. ERROR: The second " does not show up but the buffer is empty
 *    3. SPECIAL: The delimeter '\' needs to be deal with in specail
 *       e.g. \+n = ENTER, \+t = TAB, \+" = ", *** \+\ = \ ***
 **/
void ObjScanner::DealWithString() {

  char first = '\0', second = '\0' ;

  first = getchar() ;
  uGlobal::uErrorColumn = uGlobal::uErrorColumn + 1 ;

  // 將起始點設為雙引號的下一個
  // 查看在ENTER前是否有另一個雙引號
  while ( first != '\n' && first != '\"' && first != -1 ) {

    // 1. 當遇到反斜線(\)字元時
    // 2. "且" 當反斜線字元在緩衝區中"並非"為最後一個字元
    if ( first == '\\' ) {

      second = getchar() ; // 下一個字元
      uGlobal::uErrorColumn = uGlobal::uErrorColumn + 1 ;

      switch ( second ) {
        
        case 'n':
          mCurrentToken.strToken = mCurrentToken.strToken + '\n' ; // add newline to the bottom of token
          break ;
        case 't':
          mCurrentToken.strToken = mCurrentToken.strToken + '\t' ; // add tab to the bottom of token
          break ;
        case '\"': case '\\': case '\'':
          mCurrentToken.strToken = mCurrentToken.strToken + second ; // add " or \ or ' to the bottom of token
          break ;
        default:
          mCurrentToken.strToken = mCurrentToken.strToken + first + second ;
          break ;

      }

      // // 且下個字元是n時
      // if ( second == 'n' )
      //   mCurrentToken.strToken = mCurrentToken.strToken + '\n' ; // 將 \n 加到token尾端

      //   // 且下個字元是t時
      // else if ( second == 't' )
      //   mCurrentToken.strToken = mCurrentToken.strToken + '\t' ; // 將 \t 加到token尾端

      //   // 且下個字元是"時
      // else if ( second == '\"' )
      //   mCurrentToken.strToken = mCurrentToken.strToken + '\"' ; // 將 \" 加到token尾端

      //   // 且下個字元是"時
      // else if ( second == '\\' )
      //   mCurrentToken.strToken = mCurrentToken.strToken + '\\' ; // 將 \\ 加到token尾端

      //   // 且下個字元是'時
      // else if ( second == '\'' )
      //   mCurrentToken.strToken = mCurrentToken.strToken + '\'' ; // 將 \\ 加到token尾端

      // else
      //   mCurrentToken.strToken = mCurrentToken.strToken + first + second ;

    } // end if

      // 剩餘的其他情況(其他非特殊字元)
    else
      mCurrentToken.strToken = mCurrentToken.strToken + first ; // 將此character加到token尾端

    first = getchar() ;
    uGlobal::uErrorColumn = uGlobal::uErrorColumn + 1 ;
  } // end while

  if ( first == '\"' )
    mCurrentToken.strToken = mCurrentToken.strToken + first ; // 將此character加到token尾端 ;

  else if ( first == -1 )
    uGlobal::uEndOfFileOcurred = true ;

  else // first == '\n'
    throw NO_CLOSING_QUOTE ; // throw error

}


// 1. 確認此character + 目前的token是否為數字
// 2. 目前只有判斷一個字元
bool ObjScanner::IsInteger(char ch) {

  int num = ch - '0' ;  // 利用ASCII CODE算出差距

  // 再利用得出的數字判斷此字元是否為數字
  // 判斷是否為數字(0~9)
  if ( ( num >= 0 && num <= 9 ) )
    return true ;
  else
    return false ;

}

// 處理在buffer中出現點時做的處理
// 1. Attribution = DOT
// 2. Attribution = FLOAT
// 3. Attribution = SYMBOL
void ObjScanner::DealWithchDot(char ch) {

  // 1. 當token字串為空(還未存入任何字元) -> 第一個
  //    且 緩衝區下一個字元是white space 或 目前是最後一個字元
  // 2. Attribution == DOT
  if ( mCurrentToken.strToken.empty() && ( ch == ' ' || ch == '\t' ||
                                           ch == '\n' || ch == '(' ||
                                           ch == ')' || ch == ';'
                                         ) 
      ) {

    mCurrentToken.attribution = DOT ;    // 設定token的屬性(DOT)

  } // end if

    // 1. 當此時的token屬性是INTEGER 或 (token字串為空且下一個字元是INTEGER)
    // 2. Attribution = FLOAT
  else if ( mCurrentToken.attribution == INTEGER || 
            ( mCurrentToken.strToken.empty() &&  
              IsInteger( ch ) 
            ) 
          )
    mCurrentToken.attribution = FLOAT ;  // 設定token的屬性(FLOAT)

    // 字元是點(.)
    // 切出的token不是INGETER或FLOAT，前面也沒有左括號的出現( !INTEGER && !FLOAT && gLeft_Paren_Num <= 0 ) -> SYMBOL
  else
    mCurrentToken.attribution = SYMBOL ; // 設定token的屬性(SYMBOL)

}

// 將切出token做前處理
// 1. INEGER
// 2. FLOAT
// 3. NIL (false)
// 4. T   (true)
// 例如：+. != 0.000, +. == +., #f, nil, #t, t -> #t
Token ObjScanner::PreprocessingToken() {

  // token是INTEGER
  if ( mCurrentToken.attribution == INTEGER ) {

    // 此INTEGER的第一個字元是正號(+)
    if ( mCurrentToken.strToken[0] == '+' )
      mCurrentToken.strToken = mCurrentToken.strToken.erase(0, 1) ;

  } // end if

    // token是FLOAT
  else if ( mCurrentToken.attribution == FLOAT ) {

    if ( mCurrentToken.strToken == "+." || mCurrentToken.strToken == "-." )
      mCurrentToken.attribution = SYMBOL ;

  } // end else if

    // 判斷目前的token是否有機會是NIL
  else if ( mCurrentToken.strToken == "#f" || mCurrentToken.strToken == "nil" ) {

    mCurrentToken.strToken  = "nil" ; // 將目前的token設成nil
    mCurrentToken.attribution = NIL ; // 設定token的屬性(NIL)

  } // end else if

    // 判斷目前的token是否有機會是T
  else if ( mCurrentToken.strToken == "#t" || mCurrentToken.strToken == "t" ) {

    mCurrentToken.strToken  = "#t" ;  // 將目前的token設成#t
    mCurrentToken.attribution = T ;   // 設定token的屬性(T)

  } // end else if

  return mCurrentToken ; // 回傳Token

}
// private

// public

// 初始化
ObjScanner::ObjScanner() {

  mCurrentToken.strToken.clear() ;
  mCurrentToken.attribution = 0 ;
  mVec_token.clear() ;

}

// 讀取一個token
Token ObjScanner::GetToken() {

  char ch = '\0' ;  // 宣告一變數令讀取的字元放入

  mCurrentToken.strToken.clear() ;
  mCurrentToken.attribution = 0 ;

  ch = getchar() ;

  if ( ch == '\n' ) {

    if ( uGlobal::uDoesThisLineHasOutput ) {
      uGlobal::uDoesThisLineHasOutput = false ;
      uGlobal::uErrorLine = 1 ;
      uGlobal::uErrorColumn = 0 ;
    } // end if

    else {
      uGlobal::uErrorLine = uGlobal::uErrorLine + 1 ;
      uGlobal::uErrorColumn = 0 ;
    } // end else

  } // end if
  else
    uGlobal::uErrorColumn = uGlobal::uErrorColumn + 1 ;

  while ( ch == ' ' || ch == '\t' || ch == '\n' ) {

    ch = getchar() ;

    if ( ch == '\n' ) {
      if ( uGlobal::uDoesThisLineHasOutput ) {
        uGlobal::uDoesThisLineHasOutput = false ;
        uGlobal::uErrorLine = 1 ;
        uGlobal::uErrorColumn = 0 ;
      } // end if

      else {
        uGlobal::uErrorLine = uGlobal::uErrorLine + 1 ;
        uGlobal::uErrorColumn = 0 ;
      } // end else
    } // end if
    else
      uGlobal::uErrorColumn = uGlobal::uErrorColumn + 1 ;

  } // end while

  if ( ch != ';' )
    uGlobal::uDoesThisLineHasOutput = false ;

  if ( ch == '\"' ) {

    mCurrentToken.strToken    = '\"' ;
    mCurrentToken.attribution = STRING ;
    DealWithString() ;
    return mCurrentToken ;

  } // end if

  else if ( ch == ';' ) {

    ch = getchar() ;

    while ( ch != '\n' && ch != -1 )
      ch = getchar() ;

    if ( ch == '\n' ) {

      if ( uGlobal::uDoesThisLineHasOutput ) {
        uGlobal::uDoesThisLineHasOutput = false ;
        uGlobal::uErrorLine = 1 ;
        uGlobal::uErrorColumn = 0 ;
      } // end if

      else {
        uGlobal::uErrorLine = uGlobal::uErrorLine + 1 ;
        uGlobal::uErrorColumn = 0 ;
      } // end else

      Token thistoken = GetToken() ;
      return thistoken ;
    } // end if

  } // end else if

  else if ( ch == ')' ) {

    mCurrentToken.strToken    = ')' ;
    mCurrentToken.attribution = RIGHT_PAREN ;
    return mCurrentToken ;

  } // end else if

  else if ( ch == '(' ) {

    mCurrentToken.strToken    = '(' ;
    mCurrentToken.attribution = LEFT_PAREN ;
    return mCurrentToken ;

  } // end else if

  else if ( ch == '\'' ) {

    mCurrentToken.strToken    = '\'' ;
    mCurrentToken.attribution = QUOTE ;
    return mCurrentToken ;

  } // end else if

  while ( ch != '\n' &&
          ch != ' ' && ch != '\t' &&
          ch != '(' && ch != ')' &&
          ch != '\"' && ch != '\'' && ch != ';' &&
          ch != -1 ) {

    // 判斷此character是否為數字
    if ( IsInteger( ch ) ) {
      // 是
      // 判斷current token是否為空字串
      if ( mCurrentToken.strToken.empty() )
        // 是
        mCurrentToken.attribution = INTEGER ; // 設定token的屬性(INTEGER)
    } // end if

      // 判斷此character是否為正負號
    else if ( ch == '+' || ch == '-' ) {
      // 1. 當token字串為空(還未存入任何字元) -> 第一個
      // 2. 且不為singal(此字元即為token)

      char cSecond = '\0' ;
      cSecond = getchar() ; // 偷看下一個字元

      if ( mCurrentToken.strToken.empty() &&
           cSecond != ' ' && cSecond != '\t' && cSecond != '\n' &&
           cSecond != '(' && cSecond != ')' )
        mCurrentToken.attribution = INTEGER ; // 設定token的屬性(INTEGER)

        // 1. !temp.StrToken.empty()
        // 2. 非第一個字元
      else
        mCurrentToken.attribution = SYMBOL ;  // 設定token的屬性(SYMBOL)

      cin.putback( cSecond ) ; // 放回偷看的字元
    } // end else if

      // 判斷此character是否為點
    else if ( ch == '.' ) {
      // 是
      char cSecond = '\0' ;
      cSecond = getchar() ;      // 偷看下一個字元
      DealWithchDot( cSecond ) ; // 處理在buffer中出現點時做的處理
      cin.putback( cSecond ) ;   // 放回偷看的字元

      // 如果dot是單獨出現就return
      if ( mCurrentToken.attribution == DOT ) {
        mCurrentToken.strToken = "." ;
        return mCurrentToken ;
      } // end if

    } // end else if
      // SYMBOL
    else
      mCurrentToken.attribution = SYMBOL ; // 設定token的屬性(SYMBOL)

    mCurrentToken.strToken = mCurrentToken.strToken + ch ;
    ch     = getchar() ;
    uGlobal::uErrorColumn = uGlobal::uErrorColumn + 1 ;
  } // end while

  // 處理eof錯誤訊息
  if ( ch == -1 ) {
    uGlobal::uEndOfFileOcurred = true ;
  } // end if

    // 處理eof以外的錯誤訊息
  else if ( ch == '\n' ) {
    uGlobal::uErrorLine   = uGlobal::uErrorLine + 1 ;
    uGlobal::uErrorColumn = 0 ;
  } // end else if

  else if ( ch == ' ' || ch == '\t' || ch == '(' || ch == ')' || ch == '\"' || ch == '\'' || ch == ';' ) {
    cin.putback( ch ) ;
    uGlobal::uErrorColumn = uGlobal::uErrorColumn - 1 ;
  } // end else if

  return PreprocessingToken() ;

}
// public