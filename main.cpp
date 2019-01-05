// **********************************************************************************************************************************************
//  VERSION     NUMBER    PROGRAMMER          DATE:BEGIN-END              DESCRIPTION(完成的功能)
//  version      1.0      Hamilton Chang      2017/03/15 - 2017/03/19     Scanner class, Parser class(without error)
//  version      1.1      Hamilton Chang      2017/03/23 - 2017/03/25     Add Error Message, and object(proj1全部完成)
//  version      2.0      Hamilton Chang      2017/05/02 - 2017/05/17     Evaluation class(without error)
//  version      2.1      Hamilton Chang      2017/05/19 - 2017/05/20     Add Error Message, and object(proj2全部完成)
//  version      3.0      Hamilton Chang      2017/05/   - 2017/06/       Extend Function 'define', Add Function 'let', 'lambda' (without error)
// **********************************************************************************************************************************************
# include <stdio.h>
# include <stdlib.h>
# include <iostream>
# include <string.h>
# include <string>
# include <vector>
# include <sstream>

# include <exception>
# include <stdexcept>

# include <map> // PROJ2:使用map這個function

// 有三種Syntax ERROR(文法錯誤)
# define UNEXPECTED_TOKEN_ONE 1
# define UNEXPECTED_TOKEN_TWO 2
# define NO_CLOSING_QUOTE     3

// Proj2 ERROR
// 總共有9種ERROR
# define NON_LIST                        4
# define INCORRECT_NUMBER_OF_ARGUMENT    5
# define INCORRECT_ARGUMENT_TYPE         6
# define ATTEMPT_TO_APPLY_NON_FUNCTION   7
# define NO_RETURN_VALUE                 8
# define UNBOUND_SYMBOL                  9
# define DIVISION_BY_ZERO                10
# define FORMAT_ERROR                    11
# define LEVEL_ERROR                     12
# define ATTEMPT_TO_APPLY_NON_FUNCTION_2 13

using namespace std;

// 所有的屬性編號
enum Attribution{ INTEGER = 1, FLOAT, STRING, LEFT_PAREN, RIGHT_PAREN, DOT, T, NIL, QUOTE, SYMBOL } ;

// proj2所有功能的關鍵字
static string  uFunctionName[] = { "cons", "list", // Constructors
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

static map<string,string> uFunctionNamesMap ; // 當keyword是atom實用的map

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

// 處理除了END-OF-FILE以外的所有錯誤訊息
class Error : public exception {

public:

  // Error Message的判斷與回傳
  string ErrorMessage( int synErrType ) {

    string errMesg = "" ;

    if ( synErrType == UNEXPECTED_TOKEN_ONE )
      errMesg = "ERROR (unexpected token) : atom or '(' expected when token at Line " ;

    else if ( synErrType == UNEXPECTED_TOKEN_TWO )
      errMesg = "ERROR (unexpected token) : ')' expected when token at Line " ;

    else if ( synErrType == NO_CLOSING_QUOTE )
      errMesg = "ERROR (no closing quote) : END-OF-LINE encountered at Line ";

    else if ( synErrType == INCORRECT_NUMBER_OF_ARGUMENT )
      errMesg = "ERROR (incorrect number of arguments) : " ;

    else if ( synErrType == ATTEMPT_TO_APPLY_NON_FUNCTION )
      errMesg = "ERROR (attempt to apply non-function) : " ;

    else if ( synErrType == ATTEMPT_TO_APPLY_NON_FUNCTION_2 )
      errMesg = "ERROR (attempt to apply non-function) : " ;

    else if ( synErrType == UNBOUND_SYMBOL )
      errMesg = "ERROR (unbound symbol) : " ;

    else if ( synErrType == INCORRECT_ARGUMENT_TYPE )
      errMesg = "ERROR (" + uErrorFuncN + " with incorrect argument type) : " ;

    else if ( synErrType == NON_LIST )
      errMesg = "ERROR (non-list) : " ;

    else if ( synErrType == DIVISION_BY_ZERO )
      errMesg = "ERROR (division by zero) : /" ;

    else if ( synErrType == NO_RETURN_VALUE )
      errMesg = "ERROR (no return value) : " ;

    else if ( synErrType == FORMAT_ERROR ) {

      if ( uErrorToken == "#<procedure define>" )
        errMesg = "ERROR (DEFINE format) : " ;

      else if ( uErrorToken == "#<procedure set!>" )
        errMesg = "ERROR (SET! format) : " ;

      else if ( uErrorToken == "#<procedure let>" )
        errMesg = "ERROR (LET format) : " ;

      else if ( uErrorToken == "#<procedure cond>" )
        errMesg = "ERROR (COND format) : " ;

      else if ( uErrorToken == "#<procedure lambda>" )
        errMesg = "ERROR (LAMBDA format) : " ;

    } // end else if

    else if ( synErrType == LEVEL_ERROR ) {

      if ( uErrorToken == "#<procedure clean-environment>" )
        errMesg = "ERROR (level of CLEAN-ENVIRONMENT)" ;

      else if ( uErrorToken == "#<procedure define>" )
        errMesg = "ERROR (level of DEFINE)" ;

      else if ( uErrorToken == "#<procedure exit>" )
        errMesg = "ERROR (level of EXIT)" ;

    } // end else if

    return errMesg ;

  } // end ErrorMessage()

  // 讀掉出現Error後，此行剩下的stuff
  void ReadLeftStuff() {

    char ch = '\0' ;

    ch = getchar() ;

    while ( ch != '\n' && ch != -1 ) {
      ch = getchar() ;
    } // end while

    // 如果在讀取stuff的過程中，發生eof
    if ( ch == -1 )
      uEndOfFileOcurred = true ;

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
  // 例如：+. != 0.000, +. == +., #f, nil, #t, t -> #t
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

    } // end while

    if ( ch != ';' )
      uDoesThisLineHasOutput = false ;

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
      uErrorColumn = uErrorColumn + 1 ;
    } // end while

    // 處理eof錯誤訊息
    if ( ch == -1 ) {
      uEndOfFileOcurred = true ;
    } // end if

    // 處理eof以外的錯誤訊息
    else if ( ch == '\n' ) {
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

protected:

  bool       mIsAddNode ;   // 訂定現在是否有在token與token間新增一節點這樣的狀態
  ObjScanner mScanner ;     // 所有Parser需要Scanner層support
  Token      mLastToken ;   // 暫存上一個token
  bool       mIsLP ;        // 在pretty print用來判斷是否為左括號
  bool       mIsLastNodeLP ;// 在pretty print用來判斷上一個節點是否為左括號
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
    mIsLastNodeLP = true ;  // 初始值為是左括號
    mdotted_pait = 0 ;     // 初始為零
    mLastToken.attribution = 0 ;
    mLastToken.strToken.clear() ;

  } // end ObjParser()

  // 確認此樹是否為結束條件
  bool CheckEXIT( Cons_node *tree ) {

    if ( tree == NULL ) {
      return false ;
    } // end if

    else if ( tree->left == NULL && tree->right == NULL ) {
      return false ;
    } // end else if

    else if ( tree->atom.attribution == LEFT_PAREN && tree->left->atom.strToken == "exit" &&
              tree->right->atom.attribution == NIL ) {
      return true ;
    } // end else if

    else {
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
        // tree->right->atom.strToken    = ".(" ;
        tree->right->atom.strToken    = "(" ;
        tree->right->dotted_pair      = true ;
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
          // tree->left->right->atom.strToken    = ".(" ;
          tree->left->right->atom.strToken    = "(" ;
          tree->left->right->dotted_pair       = true ;
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
              // tree->right->atom.strToken    = ".(" ;
              tree->right->atom.strToken    = "(" ;
              tree->right->dotted_pair      = true ;
              tree->right->atom.attribution = LEFT_PAREN ;
              tree->right = BuildTree( tree->right ) ;
            } // end else

          } // end if

          // 如果下一個token是單引號
          else if ( thisToken.attribution == QUOTE ) {

            // tree->right->atom.strToken    = ".(" ;
            tree->right->atom.strToken    = "(" ;
            tree->right->dotted_pair      = true ;
            tree->right->atom.attribution = LEFT_PAREN ;

            tree->right->left = NewNode() ;

            tree->right->left->atom.strToken    = "quote" ;
            tree->right->left->atom.attribution = QUOTE ;
            // tree->right = BuildTree( tree->right ) ;

            tree->right->right = NewNode() ;
            // tree->right->right->atom.strToken    = ".(" ;
            tree->right->right->atom.strToken    = "(" ;
            tree->right->right->dotted_pair      = true ;
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
          // tree->right->atom.strToken    = ".(" ;
          tree->right->atom.strToken    = "(" ;
          tree->right->dotted_pair      = true ;
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
    // ClearTree( tree ) ;   // 清空樹
    mroot      = NULL ;  // 根節點指向NULL
    mIsAddNode = false ; // 是否有在token跟token插入一節點
    mIsLP      = false ; // 初始為非左括號
    mIsLastNodeLP = true ;  // 初始值為是左括號
    mLastToken.attribution = 0 ;
    mLastToken.strToken.clear() ;
  } // end ClearAll()

  void PrettyPrint( Cons_node *tree, int level ) {

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
            PrettyPrint( tree->left, level ) ;

          } // end if

          // 此龍骨中"非"第一個左括弧
          else if ( tree->atom.attribution == LEFT_PAREN  && !first ) {

            if ( tree->left != NULL )
              PrettyPrint( tree->left, level ) ;

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

                else
                  cout << tree->atom.strToken << endl ; // 印出此節點內容與換行

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

              else
                cout << tree->atom.strToken << endl ; // 印出此節點內容與換行

              // right = false ;

            } // end else

            mIsLastNodeLP = false ;

          } // end else if

          tree = tree->right ;
          right = true ;

        } // end while

      } // end else

    } // end if

  } // end PrettyPrint()

};

class ObjEvalutaion : protected ObjParser{

private:

  map < string, Cons_node* > mDef ; // 儲存被defined過的樹
  bool mIsQuote ;

  // 判斷此是否為function name
  // 如果是，回傳true
  // 如果否，回傳false
  bool IsEvaluateFunctions( string key, string &funcName ) {

    map<string,string>::iterator iter ;   // declaration iterator
    iter = uFunctionNamesMap.find( key ) ;

    if ( iter != uFunctionNamesMap.end() ) {
      funcName = iter->second ;
      return true ;
    } // end if

    return false ;

  } // end IsEvaluateFunctions()

  // 判斷此是否為被defined過的symbol
  // 如果是，回傳pointer
  // 如果否，回傳NULL
  Cons_node* IsDefined( string key ) {

    map < string, Cons_node* > :: iterator iter ;   // declaration iterator
    iter = mDef.find( key ) ;

    if ( iter != mDef.end() ) {
      return iter->second ;
    } // end if

    return NULL ;

  } // end IsDefined()

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
  bool CheckFormat( Cons_node* tree ) {

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

  } // end CheckFormat()

  // 判斷此argument是否有binding(internal function, define)
  // 有，回傳指向此binding的pointer
  // 否，回傳NULL
  Cons_node* CheckBindingAndReturnThePointer( string key ) {

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

  } // end CheckBindingAndReturnThePointer()

  Cons_node* EvalFunctions( Cons_node *tree, int numofArg ) {

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
        uErrorFuncN = "car" ;
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
        uErrorFuncN = "cdr" ;
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
          uErrorFuncN = "+" ;
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
          uErrorFuncN = "-" ;
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
          uErrorFuncN = "*" ;
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
                uErrorFuncN = "/" ;
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

          uErrorFuncN = "/" ;
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

          uErrorFuncN = ">" ;
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

          uErrorFuncN = ">=" ;
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

          uErrorFuncN = "<" ;
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

          uErrorFuncN = "<=" ;
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

          uErrorFuncN = "=" ;
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

          uErrorFuncN = "string-append" ;
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

          uErrorFuncN = "string>?" ;
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

          uErrorFuncN = "string<?" ;
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

          uErrorFuncN = "string=?" ;
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
                procedure        = "#<procedure " + uFunctionName[i] + ">" ;

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

  } // end EvalFunctions()

  // 兩棵樹(argOne,argTwo)長  一樣  時，return true
  //                         不一樣   ，return false
  bool CompareTree( Cons_node* one, Cons_node* two ) {

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

  } // end CompareTree()

  // HOW : 如何使用此function
  // 傳入 1. 一棵tree 2. 一個integer，(用來記錄變數的數量 call by reference)
  // 依function name來確認要evaluate的變數數量是否符合規則
  // 是，變數數量符合規則 return true
  // 否，變數數量不符合規則 return false
  //
  // WHAT : 如何implement此function
  // 先計算變數數量
  // 再依function name與其規則判定是否符合
  bool CheckNumberOfVariable( Cons_node *tree, int& numofArg ) {

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

  } // CheckNumberOfVariable()

  string RepairNameOfInternalFunction( string symbol ) {

    string internalFunction = "" ;
    internalFunction.clear() ;

    for ( int i = 0 ; i < 39 ; i++ ) {

      string procedure = "" ;
      procedure.clear() ;
      procedure        = "#<procedure " + uFunctionName[i] + ">" ;

      if ( symbol == procedure )
        internalFunction = uFunctionName[i] ;

    } // end for

    return internalFunction ;

  } // RepairNameOfInternalFunction()

  bool CheckPureList( Cons_node *tree ) {

    for ( ; tree != NULL ; tree = tree->right ) {

      if ( tree->right == NULL && tree->atom.attribution == NIL )
        return true ;

    } // end for

    return false ;

  } // end CheckPureList()

public:

  Cons_node* mReturnPointer ;
  Cons_node* mErrorPointer ;

  // Constructor
  ObjEvalutaion() {
    mReturnPointer = NULL ;
    mErrorPointer  = NULL ;
    mIsQuote = false ;
  } // ObjEvalutaion()

  // 重新初始化
  void ReObjEvalutaion() {
    mReturnPointer = NULL ;
    mErrorPointer  = NULL ;
    mIsQuote = false ;
  } // ReObjEvalutaion()

  Cons_node* Evaluate( Cons_node *tree, bool topLevel ) {

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

          uErrorToken = tree->atom.strToken ;
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

        uErrorToken = tree->left->atom.strToken ;
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
            procedure        = "#<procedure " + uFunctionName[i] + ">" ;

            if ( tree->left->atom.strToken == procedure )
              isInternalFunction = true ;

          } // end for

          if ( isInternalFunction )
            ; // Do Nothing

          else {
            uErrorToken = tree->left->atom.strToken ;
            throw ATTEMPT_TO_APPLY_NON_FUNCTION ;
          } // end else

          // 如果 the current level is "NOT" the top level，且symbol為 'clean-environment'、'define'、'exit'其中之一
          if ( !topLevel && ( tree->left->atom.strToken == "#<procedure clean-environment>" ||
                              tree->left->atom.strToken == "#<procedure define>" ||
                              tree->left->atom.strToken == "#<procedure exit>" ) ) {

            uErrorToken = tree->left->atom.strToken ;
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
              uErrorToken   = tree->left->atom.strToken ;
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

              uErrorToken = RepairNameOfInternalFunction( tree->left->atom.strToken ) ;
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

              uErrorToken = RepairNameOfInternalFunction( tree->left->atom.strToken ) ;
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

          uErrorToken = errToken ;
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
          uErrorToken = errToken->atom.strToken ;
          throw ATTEMPT_TO_APPLY_NON_FUNCTION ;
          // throw Error Message
          // ERROR (attempt to apply non-function) : First-Argument
        } // end if

        else {

          bool isInternalFunction = false ;

          for ( int i = 0 ; i < 39 ; i++ ) {

            string procedure = "" ;
            procedure.clear() ;
            procedure        = "#<procedure " + uFunctionName[i] + ">" ;

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

            uErrorToken = RepairNameOfInternalFunction( tree->left->atom.strToken ) ;
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

  } // end Evaluate()

  void ErrorPrettyPrint( Cons_node *tree, int level ) {

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
            procedure        = "#<procedure " + uFunctionName[i] + ">" ;

            if ( tree->atom.strToken == procedure ) {

              functionName = uFunctionName[i] ;
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
                    procedure        = "#<procedure " + uFunctionName[i] + ">" ;

                    if ( tree->atom.strToken == procedure ) {

                      functionName = uFunctionName[i] ;
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
                  procedure        = "#<procedure " + uFunctionName[i] + ">" ;

                  if ( tree->atom.strToken == procedure ) {
                    functionName = uFunctionName[i] ;
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

  } // end ErrorPrettyPrint()

}; // end ObjEvaluation

int main() {

  ObjScanner    scanner ;       // 宣告Scanner class
  ObjParser     parser ;        // 宣告Parser class
  ObjEvalutaion evaluate ;      // 宣告Evaluater class
  Error         errMesg ;       // 宣告error
  int           testNum = 0 ;   // 測試數據題號
  int           numofFunc = 0 ; // Internal Function的數目

  numofFunc = sizeof( uFunctionName ) / sizeof( uFunctionName[0] ) ;

  // 將要keyword是atom時，所需要輸出的string
  for ( int i = 0 ; i < numofFunc ; i++ ) {

    string procedure = "" ;
    procedure.clear() ;
    procedure        = "#<procedure " + uFunctionName[i] + ">" ;
    uFunctionNamesMap[uFunctionName[i]] = procedure ;

  } // end for

  map<string,string>::iterator iter ;   // declaration iterator

  cin  >> testNum ; // 讀掉測試數據
  getchar() ;       // 讀掉'\n'

  cout << "Welcome to OurScheme!" << endl ;
  cout << endl << "> " ;

  do {

    try {

      parser.mroot = parser.BuildTree( parser.mroot ) ; // 建樹並回傳指向此樹根節點的pointer

      // 如果不是結束條件:(exit)，且左右括弧數目相同
      if ( !parser.CheckEXIT( parser.mroot ) ) {

        // parser.PrettyPrint( parser.mroot, 0 ) ; // pretty print, 第一個參數:指向樹根節點的pointer、第二個參數:level
        uProjNumber = 2 ;
        evaluate.mReturnPointer = evaluate.Evaluate( parser.mroot, true ) ;
        parser.PrettyPrint( evaluate.mReturnPointer, 0 ) ; // pretty print, 第一個參數:指向樹根節點的pointer、第二個參數:level
        // parser.ClearAll( parser.mroot ) ;       // 清空樹
        parser.mroot = NULL ; // 將pointer指向NULL

        // ERROR line and column initialize
        uProjNumber  = 1 ;
        uErrorLine   = 1 ;
        uErrorColumn = 0 ;
        uDoesThisLineHasOutput = true ; // 此行一有輸出，此bool就設為true
        evaluate.ReObjEvalutaion() ;

        cout << endl << "> " ;

      } // end if

    } // end try

    catch ( int err ) {

      if ( !uEndOfFileOcurred ) {

        if ( uProjNumber == 1 ) {

          if ( uErrorLine == 0 )
            uErrorLine = 1 ;

          // 字串的Error
          if ( err == NO_CLOSING_QUOTE )
            cout << errMesg.ErrorMessage( err ) << uErrorLine << " Column " << uErrorColumn << endl ;

          // 除了字串與EOF以外的Error
          else {
            // 讀掉剩下的token
            errMesg.ReadLeftStuff() ;
            cout << errMesg.ErrorMessage( err ) << uErrorLine << " Column " <<
                    uErrorColumn-uErrorToken.length()+1 << " is >>" << uErrorToken << "<<" << endl ;
          } // end else

          uErrorLine   = 1 ;

        } // end if

        else if ( uProjNumber == 2 ) {

          if ( err == INCORRECT_NUMBER_OF_ARGUMENT )
            cout << errMesg.ErrorMessage( err ) << uErrorToken << endl ;

          else if ( err == ATTEMPT_TO_APPLY_NON_FUNCTION )
            cout << errMesg.ErrorMessage( err ) << uErrorToken << endl ;

          else if ( err == UNBOUND_SYMBOL )
            cout << errMesg.ErrorMessage( err ) << uErrorToken << endl ;

          else if ( err == FORMAT_ERROR ) {

            cout << errMesg.ErrorMessage( err ) ;
            evaluate.ErrorPrettyPrint( evaluate.mErrorPointer, 0 ) ;

          } // end else if

          else if ( err == ATTEMPT_TO_APPLY_NON_FUNCTION_2 ) {

            cout << errMesg.ErrorMessage( err ) ;
            parser.PrettyPrint( evaluate.mErrorPointer, 0 ) ;

          } // end else if

          else if ( err == LEVEL_ERROR )
            cout << errMesg.ErrorMessage( err ) << endl ;

          else if ( err == DIVISION_BY_ZERO )
            cout << errMesg.ErrorMessage( err ) << endl ;

          else if ( err == INCORRECT_ARGUMENT_TYPE ) {

            cout << errMesg.ErrorMessage( err ) ;
            evaluate.ErrorPrettyPrint( evaluate.mErrorPointer, 0 ) ;

          } // end else if

          else if ( err == NON_LIST ) {

            cout << errMesg.ErrorMessage( err ) ;
            evaluate.ErrorPrettyPrint( evaluate.mErrorPointer, 0 ) ;

          } // end else if

          else if ( err == NO_RETURN_VALUE ) {

            cout << errMesg.ErrorMessage( err ) ;
            evaluate.ErrorPrettyPrint( evaluate.mErrorPointer, 0 ) ;

          } // end else if

          uErrorLine = 0 ;

        } // end else if

        // ERROR line and column initialize
        parser.mroot = NULL ; // 將pointer指向NULL
        uProjNumber  = 1 ;
        // uErrorLine   = 1 ;
        uErrorColumn = 0 ;
        uErrorToken.clear() ;
        uDoesThisLineHasOutput = false ;
        cout << endl << "> " ;

      } // end if

    } // end catch

  } while ( !parser.CheckEXIT( parser.mroot ) && !uEndOfFileOcurred ) ; // end while

  if ( uEndOfFileOcurred )
    cout << "ERROR (no more input) : END-OF-FILE encountered" ;

  cout << endl << "Thanks for using OurScheme!" ;

  return 0;
} // end main()
