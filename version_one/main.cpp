// ************************************************************************************************************************
//  VERSION     NUMBER    PROGRAMMER          DATE:BEGIN-END              DESCRIPTION(完成的功能)
//  version      1.0      Hamilton Chang      2017/03/15 - 2017/03/19     Scanner class, Parser class(without error)
//  version      2.0      Hamilton Chang      2017/03/23 - 2017/03/25     Add Error Message, and object(全部完成)
// ************************************************************************************************************************
# include <stdio.h>
# include <stdlib.h>
# include <iostream>
# include <string.h>
# include <string>
# include <vector>
# include <sstream>

# include <exception>
# include <stdexcept>

// 有三種Syntax ERROR(文法錯誤)
# define UNEXPECTED_TOKEN_ONE 1
# define UNEXPECTED_TOKEN_TWO 2
# define NO_CLOSING_QUOTE     3

using namespace std;

// 所有的屬性編號
enum Attribution{ INTEGER = 1, FLOAT, STRING, LEFT_PAREN, RIGHT_PAREN, DOT, T, NIL, QUOTE, SYMBOL } ;

static int uLeft_Paren_Num = 0 ; // 儲存左括弧(的數量
                                 // 1. 遇到左括弧就+1
                                 // 2. 遇到右括弧就-1

static int    uErrorLine   = 1 ;  // 記錄錯誤訊息中第幾列錯誤
static int    uErrorColumn = 0 ;  // 記錄錯誤訊息中第幾行錯誤
static string uErrorToken  = "" ; // 記錄發生錯誤的token
static bool   uEndOfFileOcurred = false ; // END-OF-FILE的錯誤是否發生
static bool   uDoesThisLineHasOutput = false ; // 目的是檢查output後到\n是否其他token

// 所有有關token的資訊
struct Token{

  string strToken ;  // 儲存token字串
  int    attribution ; // token的屬性

} ; // end Token

// 處理除了END-OF-FILE以外的所有錯誤訊息
class SyntaxError : public exception {

public:

  string ErrorMessage( int synErrType ) {

    string errMesg = "" ;

    if ( synErrType == UNEXPECTED_TOKEN_ONE )
      errMesg = "ERROR (unexpected token) : atom or '(' expected when token at Line " ;

    else if ( synErrType == UNEXPECTED_TOKEN_TWO )
      errMesg = "ERROR (unexpected token) : ')' expected when token at Line " ;

    else if ( synErrType == NO_CLOSING_QUOTE )
      errMesg = "ERROR (no closing quote) : END-OF-LINE encountered at Line ";

    return errMesg ;

  } // end ErrorMessage()

  void ReadLeftStuff() {

    char ch = '\0' ;

    ch = getchar() ;
    // cout << ch ;

    while ( ch != '\n' && ch != -1 ) {
      ch = getchar() ;
      // cout << ch ;
    } // end while

    if ( ch == -1 )
      uEndOfFileOcurred = true ;

    // cout << endl ;

  } // end ReadLeftStuff()

};

// 這個class用來處理所有有關token的變數跟函數
class ObjScanner{

private:
  // 所有不能或不需要在外面被呼叫使用的method、function、value

  Token          mCurrentToken ; // 暫存目前的token
  vector<Token>  mVec_token ;    // 儲存token

  // 處理字串問題
  // 一般會有三種情況
  // 1. 一般情況:當遇到雙引號(")，找到第二個雙引號 (Attribution = STRING)
  // 2. error情況:讀完緩衝區都還未出現第二個雙引號時
  // 3. 在以上兩者中，使要裡面有出現'\'的字元就要做特別處理
  //    ex: \+n = ENTER, \+t = TAB, \+" = ", *** \+\ = \ ***
  void DealWithString() {

    char first = '\0', second = '\0' ;

    first = getchar() ;
    uErrorColumn = uErrorColumn + 1 ;

    // cout << "Column 1: " << uErrorColumn << ", " << first << endl ;

    // 將起始點設為雙引號的下一個
    // 查看在ENTER前是否有另一個雙引號
    while ( first != '\n' && first != '\"' && first != -1 ) {

      // 1. 當遇到反斜線(\)字元時
      // 2. "且" 當反斜線字元在緩衝區中"並非"為最後一個字元
      if ( first == '\\' ) {

        second = getchar() ; // 下一個字元
        uErrorColumn = uErrorColumn + 1 ;

        // 且下個字元是n時
        if ( second == 'n' )
          mCurrentToken.strToken = mCurrentToken.strToken + '\n' ; // 將 \n 加到token尾端

        // 且下個字元是t時
        else if ( second == 't' )
          mCurrentToken.strToken = mCurrentToken.strToken + '\t' ; // 將 \t 加到token尾端

        // 且下個字元是"時
        else if ( second == '\"' )
          mCurrentToken.strToken = mCurrentToken.strToken + '\"' ; // 將 \" 加到token尾端

        // 且下個字元是"時
        else if ( second == '\\' )
          mCurrentToken.strToken = mCurrentToken.strToken + '\\' ; // 將 \\ 加到token尾端

        // 且下個字元是'時
        else if ( second == '\'' )
          mCurrentToken.strToken = mCurrentToken.strToken + '\'' ; // 將 \\ 加到token尾端

        else
          mCurrentToken.strToken = mCurrentToken.strToken + first + second ;

      } // end if

      // 剩餘的其他情況(其他非特殊字元)
      else
        mCurrentToken.strToken = mCurrentToken.strToken + first ; // 將此character加到token尾端

      first = getchar() ;
      uErrorColumn = uErrorColumn + 1 ;
    } // end while

    // cout << "Column 2: " << uErrorColumn << endl ;

    if ( first == '\"' )
      mCurrentToken.strToken = mCurrentToken.strToken + first ; // 將此character加到token尾端 ;

    else if ( first == -1 )
      uEndOfFileOcurred = true ;

    else // first == '\n'
      throw NO_CLOSING_QUOTE ; // throw error

  } // end DealWithString()

  // 1. 確認此character + 目前的token是否為數字
  // 2. 目前只有判斷一個字元
  bool IsInteger( char ch ) {

    int num = ch - '0' ;  // 利用ASCII CODE算出差距

    // 再利用得出的數字判斷此字元是否為數字
    // 判斷是否為數字(0~9)
    if ( ( num >= 0 && num <= 9 ) )
      return true ;
    else
      return false ;

  } // end IsInteger()

  // 處理在buffer中出現點時做的處理
  // 1. Attribution = DOT
  // 2. Attribution = FLOAT
  // 3. Attribution = SYMBOL
  void DealWithchDot( char ch ) {

    // 1. 當token字串為空(還未存入任何字元) -> 第一個
    //    且 緩衝區下一個字元是white space 或 目前是最後一個字元
    // 2. Attribution == DOT
    if ( mCurrentToken.strToken.empty() && ( ch == ' ' || ch == '\t' ||
                                             ch == '\n' || ch == '(' ||
                                             ch == ')' || ch == ';' ) ) {

      mCurrentToken.attribution = DOT ;    // 設定token的屬性(DOT)
      /*
      if ( uLeft_Paren_Num > 0 )
        mCurrentToken.attribution = DOT ;    // 設定token的屬性(DOT)
      else
        cout << "error" << endl ; // error
      */
    } // end if

    // 1. 當此時的token屬性是INTEGER 或 (token字串為空且下一個字元是INTEGER)
    // 2. Attribution = FLOAT
    else if ( mCurrentToken.attribution == INTEGER ||
              ( mCurrentToken.strToken.empty() &&  IsInteger( ch ) ) )
      mCurrentToken.attribution = FLOAT ;  // 設定token的屬性(FLOAT)

    // 字元是點(.)
    // 切出的token不是INGETER或FLOAT，前面也沒有左括號的出現( !INTEGER && !FLOAT && gLeft_Paren_Num <= 0 ) -> SYMBOL
    else
      mCurrentToken.attribution = SYMBOL ; // 設定token的屬性(SYMBOL)

  } // end DealWithchDot()

  // 將切出token做前處理
  // 1. INEGER
  // 2. FLOAT
  // 3. NIL (false)
  // 4. T   (true)
  // 例如：0. -> 0.000, +3 -> 3, #f, nil, () -> nil, #t, t -> #t
  Token PreprocessingToken() {

    // token是INTEGER
    if ( mCurrentToken.attribution == INTEGER ) {

      // 此INTEGER的第一個字元是正號(+)
      if ( mCurrentToken.strToken[0] == '+' )
        mCurrentToken.strToken = mCurrentToken.strToken.erase( 0, 1 ) ;

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

  } // end PreprocessingToken()

public:
  // 可令其他function呼叫

  // 初始化
  ObjScanner() {

    mCurrentToken.strToken.clear() ;
    mCurrentToken.attribution = 0 ;
    mVec_token.clear() ;

  } // end ObjScanner()

  // 讀取一個token
  Token GetToken() {

    char ch = '\0' ;  // 宣告一變數令讀取的字元放入

    mCurrentToken.strToken.clear() ;
    mCurrentToken.attribution = 0 ;

    ch = getchar() ;

    if ( ch == '\n' ) {

      if ( uDoesThisLineHasOutput ) {
        uDoesThisLineHasOutput = false ;
        uErrorLine = 1 ;
        uErrorColumn = 0 ;
      } // end if

      else {
        uErrorLine = uErrorLine + 1 ;
        uErrorColumn = 0 ;
      } // end else

    } // end if
    else
      uErrorColumn = uErrorColumn + 1 ;

    while ( ch == ' ' || ch == '\t' || ch == '\n' ) {
      // uErrorColumn = uErrorColumn + 1 ;
      ch = getchar() ;

      if ( ch == '\n' ) {
        if ( uDoesThisLineHasOutput ) {
          uDoesThisLineHasOutput = false ;
          uErrorLine = 1 ;
          uErrorColumn = 0 ;
        } // end if

        else {
          uErrorLine = uErrorLine + 1 ;
          uErrorColumn = 0 ;
        } // end else
      } // end if
      else
        uErrorColumn = uErrorColumn + 1 ;

      // cout << "character: " << ch << ", column: " << uErrorColumn << endl ;

    } // end while

    if ( ch != ';' )
      uDoesThisLineHasOutput = false ;

    if ( ch == '\"' ) {
      // cout << "Line: " << uErrorLine << endl ;
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

        if ( uDoesThisLineHasOutput ) {
          uDoesThisLineHasOutput = false ;
          uErrorLine = 1 ;
          uErrorColumn = 0 ;
        } // end if

        else {
          uErrorLine = uErrorLine + 1 ;
          uErrorColumn = 0 ;
        } // end else

        Token thistoken = GetToken() ;
        return thistoken ;
      } // end if

    } // end else if

    else if ( ch == ')' ) {

      uLeft_Paren_Num = uLeft_Paren_Num - 1 ;
      mCurrentToken.strToken    = ')' ;
      mCurrentToken.attribution = RIGHT_PAREN ;
      return mCurrentToken ;

    } // end else if

    else if ( ch == '(' ) {

      uLeft_Paren_Num = uLeft_Paren_Num + 1 ;
      mCurrentToken.strToken    = '(' ;
      mCurrentToken.attribution = LEFT_PAREN ;
      // cout << "token: " << mCurrentToken.strToken << endl ;
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
             cSecond != ' ' && cSecond != '\t' )
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
      uErrorColumn = uErrorColumn + 1 ;
    } // end while

    // 處理eof錯誤訊息
    if ( ch == -1 ) {
      uEndOfFileOcurred = true ;
      // cout << "END OF FILE ERROR" << endl ;
    } // end if

    // 處理eof以外的錯誤訊息
    else if ( ch == '\n' ) {
      // cout << "enter" << endl ;
      uErrorLine   = uErrorLine + 1 ;
      uErrorColumn = 0 ;
    } // end else if

    else if ( ch == ' ' || ch == '\t' ||
              ch == '(' || ch == ')' || ch == '\"' || ch == '\'' || ch == ';' ) {
      cin.putback( ch ) ;
      uErrorColumn = uErrorColumn - 1 ;
    } // end else if


    return PreprocessingToken() ;

  } // end GetToken()

}; // class ObjScanner

// 所有需要在parser曾所處理的function
// 建樹、文法分析等等
class ObjParser{

private:

  bool       mIsAddNode ;   // 訂定現在是否有在token與token間新增一節點這樣的狀態
  ObjScanner mScanner ;     // 所有Parser需要Scanner層support
  Token      mLastToken ;   // 暫存上一個token
  bool       mIsLP ;        // 在pretty print用來判斷是否為左括號
  int        mdotted_pait ; // dotted-pair出現的次數

  // 中間節點
  struct Cons_node{

    Cons_node *right ;      // 右指標(右子樹)
    Cons_node *left ;       // 左指標(左子樹)
    Token     atom ;        // 樹葉節點
    bool      dotted_pair ; // 判斷dotted pair的情況是否有發生

  } ; // end Cons_node

  // 宣告要新增的節點，並回傳
  Cons_node *NewNode() {

    Cons_node *node = new Cons_node() ;
    node->atom.strToken.clear() ;
    node->atom.attribution = 0 ;
    node->left             = NULL ;
    node->right            = NULL ;
    node->dotted_pair      = false ;

    return node ;

  } // end NewNode()

  // 依目前token的屬性判斷此token是否為atom node
  bool IsAtom( int tokenatrb ) {

    // IS ATOM
    if ( tokenatrb == INTEGER || tokenatrb == FLOAT || tokenatrb == SYMBOL ||
         tokenatrb == STRING || tokenatrb == NIL || tokenatrb == T )
      return true ;

    else // NOT ATOM
      return false ;

  } // end IsAtom()

  void PrintSpace( int level ) {
    for ( int i = 0 ; i < level*2 ; i++ )
      cout << ' ' ;
  } // end PrintSpace()

public:

  Cons_node *mroot ;   // 此樹的根節點

  // 初始化
  ObjParser() {

    mroot        = NULL ;  // 根節點指向NULL
    mIsAddNode   = false ; // 是否有在token跟token插入一節點
    mIsLP        = false ; // 初始為非左括號
    mdotted_pait = 0 ;     // 初始為零
    mLastToken.attribution = 0 ;
    mLastToken.strToken.clear() ;

  } // end ObjParser()

  // 確認此樹是否為結束條件
  bool CheckEXIT( Cons_node *tree ) {

    if ( tree == NULL ) {
      // cout << "1" << endl ;
      return false ;
    } // end if

    else if ( tree->left == NULL && tree->right == NULL ) {
      // cout << "2" << endl ;
      return false ;
    } // end else if

    else if ( tree->atom.attribution == LEFT_PAREN && tree->left->atom.strToken == "exit" &&
              tree->right->atom.attribution == NIL ) {
      // cout << "3" << endl ;
      return true ;
    } // end else if

    else {
      // cout << "4" << endl ;
      return false ;
    } // end else
  } // end CheckEXIT()

  // 用遞迴建樹
  // 1.  先判斷目前是否為樹的起始
  // 1.1 是
  //     a. 確認此token是否為atom-node -> 是，將token放入節點中，然後return
  //     b. 確認此token是否為左括號    -> 是，將此token放入節點，接著遞迴
  // 2. Dotted-Pair的建立
  // 3. QUOTE
  Cons_node *BuildTree( Cons_node *tree ) {

    bool isDottedPairOccurred = false ;

    if ( uEndOfFileOcurred )
      return tree ;

    // 如果此節點是第一個節點
    else if ( tree == NULL ) {

      tree = NewNode() ; // 新增節點
      Token thisToken  = mScanner.GetToken() ;

      // 如果此token是atom-node
      if ( IsAtom( thisToken.attribution ) ) {
        tree->atom = thisToken ;
        return tree ;
      } // end if

      // 如果此token是左括號
      // dotted-pair
      else if ( thisToken.attribution == LEFT_PAREN ) {
        tree->atom = thisToken ;
        tree       = BuildTree( tree ) ;
      } // end else if

      // 如果此token是單引號
      else if ( thisToken.attribution == QUOTE ) {
        tree->atom.strToken    = "(" ;
        tree->atom.attribution = LEFT_PAREN ;

        tree->left = NewNode() ;
        tree->left->atom.strToken    = "quote" ;
        tree->left->atom.attribution = QUOTE ;

        // 原始: tree = BuildTree( tree ) ;
        // 下面沒有

        tree->right = NewNode() ;
        tree->right->atom.strToken    = ".(" ;
        tree->right->atom.attribution = LEFT_PAREN ;

        tree->right->right = NewNode() ;
        tree->right->right->atom.strToken    = "nil" ;
        tree->right->right->atom.attribution = NIL ;
        tree->right->right->dotted_pair = true ;

        tree->right = BuildTree( tree->right ) ;
      } // end else if

      else {
        uErrorToken = thisToken.strToken ;
        throw UNEXPECTED_TOKEN_ONE ;
      } // end else

    } // end else if

    // 非第一個節點
    else {

      // 左節點為空
      if ( tree->left == NULL ) {

        Token thisToken ;
        tree->left = NewNode() ;

        // 如果有發生insert一個節點的情況，則不用get新的token
        // 當之前get的token拿來使用
        if ( mIsAddNode ) {

          mIsAddNode = false ;
          thisToken  = mLastToken ;
          mLastToken.attribution = 0 ;
          mLastToken.strToken.clear() ;

        } // end if

        else {

          thisToken  = mScanner.GetToken() ;

        } // end else

        // 此token是左括號
        if ( thisToken.attribution == LEFT_PAREN ) {

          tree->left->atom = thisToken ;
          tree->left       = BuildTree( tree->left ) ;

        } // end if

        // 此token是右括號
        else if ( thisToken.attribution == RIGHT_PAREN ) {

          // 如果上一個token是quote
          if ( tree->right != NULL && tree->right->atom.attribution == NIL ) {
            uErrorToken = thisToken.strToken ;
            throw UNEXPECTED_TOKEN_ONE ;
          } // end if

          else {
            tree->atom.strToken    = "nil" ;
            tree->atom.attribution = NIL ;
            tree->left             = NULL ;
            return tree ;
          } // end else

        } // end else if

        // 此token是單引號
        else if ( thisToken.attribution == QUOTE ) {

          tree->left->atom.strToken    = "(" ;
          tree->left->atom.attribution = LEFT_PAREN ;

          tree->left->left = NewNode() ;

          tree->left->left->atom.strToken    = "quote" ;
          tree->left->left->atom.attribution = QUOTE ;

          tree->left->right = NewNode() ;
          tree->left->right->atom.strToken    = ".(" ;
          tree->left->right->atom.attribution = LEFT_PAREN ;

          tree->left->right->right = NewNode() ;
          tree->left->right->right->atom.strToken    = "nil" ;
          tree->left->right->right->atom.attribution = NIL ;
          tree->left->right->right->dotted_pair = true ;

          tree->left->right = BuildTree( tree->left->right ) ;

        } // end else if

        // 此token是atom-node
        else if ( IsAtom( thisToken.attribution ) ) {

          tree->left->atom = thisToken ;

        } // end else if

        else {

          uErrorToken = thisToken.strToken ;
          throw UNEXPECTED_TOKEN_ONE ;

        } // end else

      } // end if

      // 右節點為空
      if ( tree->right == NULL ) {

        Token thisToken = mScanner.GetToken() ;
        tree->right     = NewNode() ;

        // 此token是右括號
        if ( thisToken.attribution == RIGHT_PAREN ) {

          tree->right->atom.strToken    = "nil" ;
          tree->right->atom.attribution = NIL ;
          tree->right->dotted_pair      = true ;
          // return tree ;

        } // end if

        // 此token是點
        else if ( thisToken.attribution == DOT ) {

          mdotted_pait = mdotted_pait + 1 ;
          isDottedPairOccurred = true ;

          // 跳過點讀下一個token
          thisToken = mScanner.GetToken() ;

          // 如果下一個token是左括號
          if ( thisToken.attribution == LEFT_PAREN ) {

            // 跳過點讀下一個token
            Token nextToken = mScanner.GetToken() ;

            if ( nextToken.attribution == RIGHT_PAREN ) {
              tree->right->atom.strToken    = "nil" ;
              tree->right->atom.attribution = NIL ;
              tree->right->dotted_pair      = true ;
            } // end if

            else {
              mIsAddNode = true ;
              mLastToken = nextToken ;
              tree->right->atom.strToken    = ".(" ;
              tree->right->atom.attribution = LEFT_PAREN ;
              tree->right = BuildTree( tree->right ) ;
            } // end else

          } // end if

          // 如果下一個token是單引號
          else if ( thisToken.attribution == QUOTE ) {

            tree->right->atom.strToken    = ".(" ;
            tree->right->atom.attribution = LEFT_PAREN ;

            tree->right->left = NewNode() ;

            tree->right->left->atom.strToken    = "quote" ;
            tree->right->left->atom.attribution = QUOTE ;
            // tree->right = BuildTree( tree->right ) ;

            tree->right->right = NewNode() ;
            tree->right->right->atom.strToken    = ".(" ;
            tree->right->right->atom.attribution = LEFT_PAREN ;

            tree->right->right->right = NewNode() ;
            tree->right->right->right->atom.strToken    = "nil" ;
            tree->right->right->right->atom.attribution = NIL ;
            tree->right->right->right->dotted_pair = true ;

            tree->right->right = BuildTree( tree->right->right ) ;

          } // end else if

          // 如果下一個token是atom-node
          else if ( IsAtom( thisToken.attribution ) ) {

            tree->right->atom        = thisToken ;
            tree->right->dotted_pair = true ;

          } // end else if

          // 錯誤訊息
          else {
            uErrorToken = thisToken.strToken ;
            throw UNEXPECTED_TOKEN_ONE ;
          } // end else

        } // end else if

        // 剩餘情況
        else {

          // 將get到的token放入"暫存token"的位置
          mLastToken = thisToken ;
          mIsAddNode = true ;

          // 增加一個節點
          tree->right->atom.strToken    = ".(" ;
          tree->right->atom.attribution = LEFT_PAREN ;
          tree->right = BuildTree( tree->right ) ;

        } // end else
      } // end if
    } // end else

    // 捕捉多出來的右括號，出現在dot出現的時候 mdotted_pait > 0
    if ( isDottedPairOccurred ) {

      Token tokenRP = mScanner.GetToken() ;

      // 如果捕到的token是右括號(正常情形)
      if ( tokenRP.attribution == RIGHT_PAREN )
        mdotted_pait = mdotted_pait - 1 ;

      else {
        uErrorToken = tokenRP.strToken ;
        throw UNEXPECTED_TOKEN_TWO ;
      } // end else

    } // end if

    return tree ;
  } // end BuildTree()

  // 清空樹
  void ClearTree( Cons_node *tree ) {

    if ( tree != NULL ) {
      ClearTree( tree->left ) ;
      ClearTree( tree->right ) ;
      delete tree ;
      tree = NULL ;
    } // end if

  } // end ClearTree()

  // 清空parser
  void ClearAll( Cons_node *tree ) {
    ClearTree( tree ) ;
    mroot       = NULL ;  // 根節點指向NULL
    mIsAddNode = false ; // 是否有在token跟token插入一節點
    mIsLP      = false ; // 初始為非左括號
    mLastToken.attribution = 0 ;
    mLastToken.strToken.clear() ;
  } // end ClearAll()

  void PrettyPrint( Cons_node *tree, int level ) {
    // 當樹節點不為NULL時
    if ( tree != NULL ) {
      // 在走訪子樹時，遇到的第一個左括號
      if ( !mIsLP && tree->atom.strToken == "(" ) {
        mIsLP = true ;                   // 將此boolean射程true，代表對此子樹來說已經出現第一個左括號
        PrintSpace( level ) ;            // 印出空白
        cout << tree->atom.strToken << ' ' ; // 印出左括號，**不換行
        level = level + 1 ;              // 樹的層數加一
      } // end if

      // 在走訪子樹時，遇到的"非"第一個左括號
      else if ( mIsLP && tree->atom.strToken == "(" ) {
        cout << tree->atom.strToken << ' ' ; // 印出左括號，**不換行
        level = level + 1 ;              // 樹的層數加一
      } // end else if

      // 在走訪子樹時，遇到的"非"左括號
      else if ( mIsLP && tree->atom.strToken != "(" ) {

        float fout = 0 ;

        if ( tree->atom.attribution == FLOAT ) {
          sscanf( tree->atom.strToken.c_str(), "%f", &fout ) ;
          printf( "%.3f\n", fout ) ;
        } // end if

        else
          cout << tree->atom.strToken << endl ; // 印出此節點內容與換行
        mIsLP = false ;
      } // end else if
      // 在走訪樹時，遇到的"非"左括號
      else if ( !mIsLP && tree->atom.strToken != "(" ) {
        // 依此節點的內容印出空白與內容
        // 1. 是右括號level-1
        if ( tree->atom.attribution == RIGHT_PAREN ) {

          PrintSpace( level-1 ) ;
          cout << tree->atom.strToken << endl ; // 印出此節點內容與換行

        } // end if

        // 2. 當出現dotted-pair的情況
        else if ( tree->dotted_pair ) {

          // 此樹的右節點結尾是nil
          if ( tree->atom.attribution == NIL ) {
            PrintSpace( level-1 ) ;
            cout << ')' << endl ;
          } // end else if

          // 此樹的右節點結尾"不"是nil
          else {

            PrintSpace( level ) ;
            cout << '.' << endl ; // 印出點
            PrintSpace( level ) ;
            float fout = 0 ;

            if ( tree->atom.attribution == FLOAT ) {
              sscanf( tree->atom.strToken.c_str(), "%f", &fout ) ;
              printf( "%.3f\n", fout ) ;
            } // end if

            else
              cout << tree->atom.strToken << endl ; // 印出此節點內容與換行
            PrintSpace( level-1 ) ;
            cout << ')' << endl ;   // 補印右括號
          } // end else

        } // end else if
        // 3. 其餘照舊
        else if ( tree->atom.strToken != ".(" ) {

          PrintSpace( level ) ;
          float fout = 0 ;

          if ( tree->atom.attribution == FLOAT ) {
            sscanf( tree->atom.strToken.c_str(), "%f", &fout ) ;
            printf( "%.3f\n", fout ) ;
          } // end if

          else
            cout << tree->atom.strToken << endl ; // 印出此節點內容與換行
        } // end else if
      } // end else if

      PrettyPrint( tree->left, level ) ;
      PrettyPrint( tree->right, level ) ;
    } // end if
  } // end PrettyPrint()

};

int main() {

  ObjScanner  scanner ;      // 宣告Scanner class
  ObjParser   parser ;       // 宣告Parser class
  SyntaxError errMesg ;      // 宣告error
  int        testNum = 0 ;   // 測試數據題號

  cin  >> testNum ;
  getchar() ;

  cout << "Welcome to OurScheme!" << endl ;
  // cout << endl << "> " ;

  do {

    cout << endl << "> " ;

    try {

      parser.mroot = parser.BuildTree( parser.mroot ) ;

      if ( !parser.CheckEXIT( parser.mroot ) && uLeft_Paren_Num <= 0 ) {

        parser.PrettyPrint( parser.mroot, 0 ) ;
        parser.ClearAll( parser.mroot ) ; // 清空樹

        // ERROR line and column initialize
        uErrorLine   = 1 ;
        uErrorColumn = 0 ;
        uDoesThisLineHasOutput = true ;

        // cout << endl << "> " ;

      } // end if

    } // end try

    catch ( int err ) {

      if ( !uEndOfFileOcurred ) {

        if ( err == NO_CLOSING_QUOTE )
          cout << errMesg.ErrorMessage( err ) << uErrorLine << " Column " << uErrorColumn << endl ;

        else {
          // 讀掉剩下的token
          errMesg.ReadLeftStuff() ;
          cout << errMesg.ErrorMessage( err ) << uErrorLine << " Column " <<
                  uErrorColumn-uErrorToken.length()+1 << " is >>" << uErrorToken << "<<" << endl ;
        } // end else

        // ERROR line and column initialize
        uErrorLine   = 1 ;
        uErrorColumn = 0 ;
        uErrorToken.clear() ;
        uDoesThisLineHasOutput = false ;
        uLeft_Paren_Num = 0 ;
        // cout << endl << "> " ;

      } // end if

    } // end catch

  } while ( !parser.CheckEXIT( parser.mroot ) && !uEndOfFileOcurred ) ; // end while

  if ( uEndOfFileOcurred )
    cout << "ERROR (no more input) : END-OF-FILE encountered" ;

  cout << endl << "Thanks for using OurScheme!" ;

  return 0;
} // end main()
