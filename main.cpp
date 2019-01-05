// **********************************************************************************************************************************************
//  VERSION     NUMBER    PROGRAMMER          DATE:BEGIN-END              DESCRIPTION(�������\��)
//  version      1.0      Hamilton Chang      2017/03/15 - 2017/03/19     Scanner class, Parser class(without error)
//  version      1.1      Hamilton Chang      2017/03/23 - 2017/03/25     Add Error Message, and object(proj1��������)
//  version      2.0      Hamilton Chang      2017/05/02 - 2017/05/17     Evaluation class(without error)
//  version      2.1      Hamilton Chang      2017/05/19 - 2017/05/20     Add Error Message, and object(proj2��������)
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

# include <map> // PROJ2:�ϥ�map�o��function

// ���T��Syntax ERROR(��k���~)
# define UNEXPECTED_TOKEN_ONE 1
# define UNEXPECTED_TOKEN_TWO 2
# define NO_CLOSING_QUOTE     3

// Proj2 ERROR
// �`�@��9��ERROR
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

// �Ҧ����ݩʽs��
enum Attribution{ INTEGER = 1, FLOAT, STRING, LEFT_PAREN, RIGHT_PAREN, DOT, T, NIL, QUOTE, SYMBOL } ;

// proj2�Ҧ��\�઺����r
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

static map<string,string> uFunctionNamesMap ; // ��keyword�Oatom��Ϊ�map

static int    uProjNumber  = 1 ;  // Project Number
static int    uErrorLine   = 1 ;  // �O�����~�T�����ĴX�C���~
static int    uErrorColumn = 0 ;  // �O�����~�T�����ĴX����~
static string uErrorToken  = "" ; // �O���o�Ϳ��~��token
static string uErrorFuncN  = "" ; // �O���o�Ϳ��~��functin name
static bool   uEndOfFileOcurred = false ;      // END-OF-FILE�����~�O�_�o��
static bool   uDoesThisLineHasOutput = false ; // �ت��O�ˬdoutput���\n�O�_��Ltoken

// �Ҧ�����token����T
struct Token{

  string strToken ;  // �x�stoken�r��
  int    attribution ; // token���ݩ�

} ; // end Token

// �B�z���FEND-OF-FILE�H�~���Ҧ����~�T��
class Error : public exception {

public:

  // Error Message���P�_�P�^��
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

  // Ū���X�{Error��A����ѤU��stuff
  void ReadLeftStuff() {

    char ch = '\0' ;

    ch = getchar() ;

    while ( ch != '\n' && ch != -1 ) {
      ch = getchar() ;
    } // end while

    // �p�G�bŪ��stuff���L�{���A�o��eof
    if ( ch == -1 )
      uEndOfFileOcurred = true ;

  } // end ReadLeftStuff()

};

// �o��class�ΨӳB�z�Ҧ�����token���ܼƸ���
class ObjScanner{

private:
  // �Ҧ�����Τ��ݭn�b�~���Q�I�s�ϥΪ�method�Bfunction�Bvalue

  Token          mCurrentToken ; // �Ȧs�ثe��token
  vector<Token>  mVec_token ;    // �x�stoken

  // �B�z�r����D
  // �@��|���T�ر��p
  // 1. �@�뱡�p:��J�����޸�(")�A���ĤG�����޸� (Attribution = STRING)
  // 2. error���p:Ū���w�İϳ��٥��X�{�ĤG�����޸���
  // 3. �b�H�W��̤��A�ϭn�̭����X�{'\'���r���N�n���S�O�B�z
  //    ex: \+n = ENTER, \+t = TAB, \+" = ", *** \+\ = \ ***
  void DealWithString() {

    char first = '\0', second = '\0' ;

    first = getchar() ;
    uErrorColumn = uErrorColumn + 1 ;

    // �N�_�l�I�]�����޸����U�@��
    // �d�ݦbENTER�e�O�_���t�@�����޸�
    while ( first != '\n' && first != '\"' && first != -1 ) {

      // 1. ��J��ϱ׽u(\)�r����
      // 2. "�B" ��ϱ׽u�r���b�w�İϤ�"�ëD"���̫�@�Ӧr��
      if ( first == '\\' ) {

        second = getchar() ; // �U�@�Ӧr��
        uErrorColumn = uErrorColumn + 1 ;

        // �B�U�Ӧr���On��
        if ( second == 'n' )
          mCurrentToken.strToken = mCurrentToken.strToken + '\n' ; // �N \n �[��token����

        // �B�U�Ӧr���Ot��
        else if ( second == 't' )
          mCurrentToken.strToken = mCurrentToken.strToken + '\t' ; // �N \t �[��token����

        // �B�U�Ӧr���O"��
        else if ( second == '\"' )
          mCurrentToken.strToken = mCurrentToken.strToken + '\"' ; // �N \" �[��token����

        // �B�U�Ӧr���O"��
        else if ( second == '\\' )
          mCurrentToken.strToken = mCurrentToken.strToken + '\\' ; // �N \\ �[��token����

        // �B�U�Ӧr���O'��
        else if ( second == '\'' )
          mCurrentToken.strToken = mCurrentToken.strToken + '\'' ; // �N \\ �[��token����

        else
          mCurrentToken.strToken = mCurrentToken.strToken + first + second ;

      } // end if

      // �Ѿl����L���p(��L�D�S��r��)
      else
        mCurrentToken.strToken = mCurrentToken.strToken + first ; // �N��character�[��token����

      first = getchar() ;
      uErrorColumn = uErrorColumn + 1 ;
    } // end while

    if ( first == '\"' )
      mCurrentToken.strToken = mCurrentToken.strToken + first ; // �N��character�[��token���� ;

    else if ( first == -1 )
      uEndOfFileOcurred = true ;

    else // first == '\n'
      throw NO_CLOSING_QUOTE ; // throw error

  } // end DealWithString()

  // 1. �T�{��character + �ثe��token�O�_���Ʀr
  // 2. �ثe�u���P�_�@�Ӧr��
  bool IsInteger( char ch ) {

    int num = ch - '0' ;  // �Q��ASCII CODE��X�t�Z

    // �A�Q�αo�X���Ʀr�P�_���r���O�_���Ʀr
    // �P�_�O�_���Ʀr(0~9)
    if ( ( num >= 0 && num <= 9 ) )
      return true ;
    else
      return false ;

  } // end IsInteger()

  // �B�z�bbuffer���X�{�I�ɰ����B�z
  // 1. Attribution = DOT
  // 2. Attribution = FLOAT
  // 3. Attribution = SYMBOL
  void DealWithchDot( char ch ) {

    // 1. ��token�r�ꬰ��(�٥��s�J����r��) -> �Ĥ@��
    //    �B �w�İϤU�@�Ӧr���Owhite space �� �ثe�O�̫�@�Ӧr��
    // 2. Attribution == DOT
    if ( mCurrentToken.strToken.empty() && ( ch == ' ' || ch == '\t' ||
                                             ch == '\n' || ch == '(' ||
                                             ch == ')' || ch == ';' ) ) {

      mCurrentToken.attribution = DOT ;    // �]�wtoken���ݩ�(DOT)

    } // end if

    // 1. ���ɪ�token�ݩʬOINTEGER �� (token�r�ꬰ�ťB�U�@�Ӧr���OINTEGER)
    // 2. Attribution = FLOAT
    else if ( mCurrentToken.attribution == INTEGER ||
              ( mCurrentToken.strToken.empty() &&  IsInteger( ch ) ) )
      mCurrentToken.attribution = FLOAT ;  // �]�wtoken���ݩ�(FLOAT)

    // �r���O�I(.)
    // ���X��token���OINGETER��FLOAT�A�e���]�S�����A�����X�{( !INTEGER && !FLOAT && gLeft_Paren_Num <= 0 ) -> SYMBOL
    else
      mCurrentToken.attribution = SYMBOL ; // �]�wtoken���ݩ�(SYMBOL)

  } // end DealWithchDot()

  // �N���Xtoken���e�B�z
  // 1. INEGER
  // 2. FLOAT
  // 3. NIL (false)
  // 4. T   (true)
  // �Ҧp�G+. != 0.000, +. == +., #f, nil, #t, t -> #t
  Token PreprocessingToken() {

    // token�OINTEGER
    if ( mCurrentToken.attribution == INTEGER ) {

      // ��INTEGER���Ĥ@�Ӧr���O����(+)
      if ( mCurrentToken.strToken[0] == '+' )
        mCurrentToken.strToken = mCurrentToken.strToken.erase( 0, 1 ) ;

    } // end if

    // token�OFLOAT
    else if ( mCurrentToken.attribution == FLOAT ) {

      if ( mCurrentToken.strToken == "+." || mCurrentToken.strToken == "-." )
        mCurrentToken.attribution = SYMBOL ;

    } // end else if

    // �P�_�ثe��token�O�_�����|�ONIL
    else if ( mCurrentToken.strToken == "#f" || mCurrentToken.strToken == "nil" ) {

      mCurrentToken.strToken  = "nil" ; // �N�ثe��token�]��nil
      mCurrentToken.attribution = NIL ; // �]�wtoken���ݩ�(NIL)

    } // end else if

    // �P�_�ثe��token�O�_�����|�OT
    else if ( mCurrentToken.strToken == "#t" || mCurrentToken.strToken == "t" ) {

      mCurrentToken.strToken  = "#t" ;  // �N�ثe��token�]��#t
      mCurrentToken.attribution = T ;   // �]�wtoken���ݩ�(T)

    } // end else if

    return mCurrentToken ; // �^��Token

  } // end PreprocessingToken()

public:
  // �i�O��Lfunction�I�s

  // ��l��
  ObjScanner() {

    mCurrentToken.strToken.clear() ;
    mCurrentToken.attribution = 0 ;
    mVec_token.clear() ;

  } // end ObjScanner()

  // Ū���@��token
  Token GetToken() {

    char ch = '\0' ;  // �ŧi�@�ܼƥOŪ�����r����J

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

      // �P�_��character�O�_���Ʀr
      if ( IsInteger( ch ) ) {
      // �O
      // �P�_current token�O�_���Ŧr��
        if ( mCurrentToken.strToken.empty() )
        // �O
          mCurrentToken.attribution = INTEGER ; // �]�wtoken���ݩ�(INTEGER)
      } // end if

      // �P�_��character�O�_�����t��
      else if ( ch == '+' || ch == '-' ) {
      // 1. ��token�r�ꬰ��(�٥��s�J����r��) -> �Ĥ@��
      // 2. �B����singal(���r���Y��token)

        char cSecond = '\0' ;
        cSecond = getchar() ; // ���ݤU�@�Ӧr��

        if ( mCurrentToken.strToken.empty() &&
             cSecond != ' ' && cSecond != '\t' && cSecond != '\n' &&
             cSecond != '(' && cSecond != ')' )
          mCurrentToken.attribution = INTEGER ; // �]�wtoken���ݩ�(INTEGER)

        // 1. !temp.StrToken.empty()
        // 2. �D�Ĥ@�Ӧr��
        else
          mCurrentToken.attribution = SYMBOL ;  // �]�wtoken���ݩ�(SYMBOL)

        cin.putback( cSecond ) ; // ��^���ݪ��r��
      } // end else if

      // �P�_��character�O�_���I
      else if ( ch == '.' ) {
      // �O
        char cSecond = '\0' ;
        cSecond = getchar() ;      // ���ݤU�@�Ӧr��
        DealWithchDot( cSecond ) ; // �B�z�bbuffer���X�{�I�ɰ����B�z
        cin.putback( cSecond ) ;   // ��^���ݪ��r��

        // �p�Gdot�O��W�X�{�Nreturn
        if ( mCurrentToken.attribution == DOT ) {
          mCurrentToken.strToken = "." ;
          return mCurrentToken ;
        } // end if

      } // end else if
      // SYMBOL
      else
        mCurrentToken.attribution = SYMBOL ; // �]�wtoken���ݩ�(SYMBOL)

      mCurrentToken.strToken = mCurrentToken.strToken + ch ;
      ch     = getchar() ;
      uErrorColumn = uErrorColumn + 1 ;
    } // end while

    // �B�zeof���~�T��
    if ( ch == -1 ) {
      uEndOfFileOcurred = true ;
    } // end if

    // �B�zeof�H�~�����~�T��
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

// �Ҧ��ݭn�bparser���ҳB�z��function
// �ؾ�B��k���R����
class ObjParser{

protected:

  bool       mIsAddNode ;   // �q�w�{�b�O�_���btoken�Ptoken���s�W�@�`�I�o�˪����A
  ObjScanner mScanner ;     // �Ҧ�Parser�ݭnScanner�hsupport
  Token      mLastToken ;   // �Ȧs�W�@��token
  bool       mIsLP ;        // �bpretty print�ΨӧP�_�O�_�����A��
  bool       mIsLastNodeLP ;// �bpretty print�ΨӧP�_�W�@�Ӹ`�I�O�_�����A��
  int        mdotted_pait ; // dotted-pair�X�{������

  // �����`�I
  struct Cons_node{

    Cons_node *right ;      // �k����(�k�l��)
    Cons_node *left ;       // ������(���l��)
    Token     atom ;        // �𸭸`�I
    bool      dotted_pair ; // �P�_dotted pair�����p�O�_���o��

  } ; // end Cons_node

  // �ŧi�n�s�W���`�I�A�æ^��
  Cons_node *NewNode() {

    Cons_node *node = new Cons_node() ;
    node->atom.strToken.clear() ;
    node->atom.attribution = 0 ;
    node->left             = NULL ;
    node->right            = NULL ;
    node->dotted_pair      = false ;

    return node ;

  } // end NewNode()

  // �̥ثetoken���ݩʧP�_��token�O�_��atom node
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

  Cons_node *mroot ;   // ���𪺮ڸ`�I

  // ��l��
  ObjParser() {

    mroot        = NULL ;  // �ڸ`�I���VNULL
    mIsAddNode   = false ; // �O�_���btoken��token���J�@�`�I
    mIsLP        = false ; // ��l���D���A��
    mIsLastNodeLP = true ;  // ��l�Ȭ��O���A��
    mdotted_pait = 0 ;     // ��l���s
    mLastToken.attribution = 0 ;
    mLastToken.strToken.clear() ;

  } // end ObjParser()

  // �T�{����O�_����������
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

  // �λ��j�ؾ�
  // 1.  ���P�_�ثe�O�_���𪺰_�l
  // 1.1 �O
  //     a. �T�{��token�O�_��atom-node -> �O�A�Ntoken��J�`�I���A�M��return
  //     b. �T�{��token�O�_�����A��    -> �O�A�N��token��J�`�I�A���ۻ��j
  // 2. Dotted-Pair���إ�
  // 3. QUOTE
  Cons_node *BuildTree( Cons_node *tree ) {

    bool isDottedPairOccurred = false ;

    if ( uEndOfFileOcurred )
      return tree ;

    // �p�G���`�I�O�Ĥ@�Ӹ`�I
    else if ( tree == NULL ) {

      tree = NewNode() ; // �s�W�`�I
      Token thisToken  = mScanner.GetToken() ;

      // �p�G��token�Oatom-node
      if ( IsAtom( thisToken.attribution ) ) {
        tree->atom = thisToken ;
        return tree ;
      } // end if

      // �p�G��token�O���A��
      // dotted-pair
      else if ( thisToken.attribution == LEFT_PAREN ) {
        tree->atom = thisToken ;
        tree       = BuildTree( tree ) ;
      } // end else if

      // �p�G��token�O��޸�
      else if ( thisToken.attribution == QUOTE ) {
        tree->atom.strToken    = "(" ;
        tree->atom.attribution = LEFT_PAREN ;

        tree->left = NewNode() ;
        tree->left->atom.strToken    = "quote" ;
        tree->left->atom.attribution = QUOTE ;

        // ��l: tree = BuildTree( tree ) ;
        // �U���S��

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

    // �D�Ĥ@�Ӹ`�I
    else {

      // ���`�I����
      if ( tree->left == NULL ) {

        Token thisToken ;
        tree->left = NewNode() ;

        // �p�G���o��insert�@�Ӹ`�I�����p�A�h����get�s��token
        // ���eget��token���Өϥ�
        if ( mIsAddNode ) {

          mIsAddNode = false ;
          thisToken  = mLastToken ;
          mLastToken.attribution = 0 ;
          mLastToken.strToken.clear() ;

        } // end if

        else {

          thisToken  = mScanner.GetToken() ;

        } // end else

        // ��token�O���A��
        if ( thisToken.attribution == LEFT_PAREN ) {

          tree->left->atom = thisToken ;
          tree->left       = BuildTree( tree->left ) ;

        } // end if

        // ��token�O�k�A��
        else if ( thisToken.attribution == RIGHT_PAREN ) {

          // �p�G�W�@��token�Oquote
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

        // ��token�O��޸�
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

        // ��token�Oatom-node
        else if ( IsAtom( thisToken.attribution ) ) {

          tree->left->atom = thisToken ;

        } // end else if

        else {

          uErrorToken = thisToken.strToken ;
          throw UNEXPECTED_TOKEN_ONE ;

        } // end else

      } // end if

      // �k�`�I����
      if ( tree->right == NULL ) {

        Token thisToken = mScanner.GetToken() ;
        tree->right     = NewNode() ;

        // ��token�O�k�A��
        if ( thisToken.attribution == RIGHT_PAREN ) {

          tree->right->atom.strToken    = "nil" ;
          tree->right->atom.attribution = NIL ;
          tree->right->dotted_pair      = true ;
          // return tree ;

        } // end if

        // ��token�O�I
        else if ( thisToken.attribution == DOT ) {

          mdotted_pait = mdotted_pait + 1 ;
          isDottedPairOccurred = true ;

          // ���L�IŪ�U�@��token
          thisToken = mScanner.GetToken() ;

          // �p�G�U�@��token�O���A��
          if ( thisToken.attribution == LEFT_PAREN ) {

            // ���L�IŪ�U�@��token
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

          // �p�G�U�@��token�O��޸�
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

          // �p�G�U�@��token�Oatom-node
          else if ( IsAtom( thisToken.attribution ) ) {

            tree->right->atom        = thisToken ;
            tree->right->dotted_pair = true ;

          } // end else if

          // ���~�T��
          else {
            uErrorToken = thisToken.strToken ;
            throw UNEXPECTED_TOKEN_ONE ;
          } // end else

        } // end else if

        // �Ѿl���p
        else {

          // �Nget�쪺token��J"�Ȧstoken"����m
          mLastToken = thisToken ;
          mIsAddNode = true ;

          // �W�[�@�Ӹ`�I
          // tree->right->atom.strToken    = ".(" ;
          tree->right->atom.strToken    = "(" ;
          tree->right->dotted_pair      = true ;
          tree->right->atom.attribution = LEFT_PAREN ;
          tree->right = BuildTree( tree->right ) ;

        } // end else
      } // end if
    } // end else

    // �����h�X�Ӫ��k�A���A�X�{�bdot�X�{���ɭ� mdotted_pait > 0
    if ( isDottedPairOccurred ) {

      Token tokenRP = mScanner.GetToken() ;

      // �p�G���쪺token�O�k�A��(���`����)
      if ( tokenRP.attribution == RIGHT_PAREN )
        mdotted_pait = mdotted_pait - 1 ;

      else {
        uErrorToken = tokenRP.strToken ;
        throw UNEXPECTED_TOKEN_TWO ;
      } // end else

    } // end if

    return tree ;
  } // end BuildTree()

  // �M�ž�
  void ClearTree( Cons_node *tree ) {

    if ( tree != NULL ) {
      ClearTree( tree->left ) ;
      ClearTree( tree->right ) ;
      delete tree ;
      tree = NULL ;
    } // end if

  } // end ClearTree()

  // �M��parser
  void ClearAll( Cons_node *tree ) {
    // ClearTree( tree ) ;   // �M�ž�
    mroot      = NULL ;  // �ڸ`�I���VNULL
    mIsAddNode = false ; // �O�_���btoken��token���J�@�`�I
    mIsLP      = false ; // ��l���D���A��
    mIsLastNodeLP = true ;  // ��l�Ȭ��O���A��
    mLastToken.attribution = 0 ;
    mLastToken.strToken.clear() ;
  } // end ClearAll()

  void PrettyPrint( Cons_node *tree, int level ) {

    float fout = 0.0 ;

    if ( tree != NULL ) {

      // �O�@�Ӹ`�I && �O�Ĥ@�h
      if ( level == 0 && IsAtom( tree->atom.attribution ) ) {

        // �T�{���`�I�O�_��float
        if ( tree->atom.attribution == FLOAT ) {

          sscanf( tree->atom.strToken.c_str(), "%f", &fout ) ;
          printf( "%.3f\n", fout ) ;

        } // end if

        // �Dfloat��atom
        else {

          cout << tree->atom.strToken << endl ;

        } // end else

      } // end if

      // �@�ʾ�
      else {

        bool first = true ;  // �Ĥ@��node
        bool right = false ; // �w�]�W�@�Ӹ`�I�O�q���l��Ӫ�
                             // �Ω�L�Xatom��

        // �k�l��(�s��)
        while ( tree != NULL ) {

          // ���l��
          // ���s�����Ĥ@�ӥ��A��
          if ( tree->atom.attribution == LEFT_PAREN  && first ) {

            first = false ;

            // �W�@�ӦL�X���`�I"���O"���A��
            if ( !mIsLastNodeLP ) {
              PrintSpace( level ) ;
            } // end if

            cout << tree->atom.strToken << ' ' ;

            mIsLastNodeLP = true ;
            level = level + 1 ;
            PrettyPrint( tree->left, level ) ;

          } // end if

          // ���s����"�D"�Ĥ@�ӥ��A��
          else if ( tree->atom.attribution == LEFT_PAREN  && !first ) {

            if ( tree->left != NULL )
              PrettyPrint( tree->left, level ) ;

          } // end else if

          // atom
          else if ( tree->atom.attribution != LEFT_PAREN ) {

            // �W�@�Ӹ`�I�O�k�l��
            if ( right ) {

              // ���𪺥k�`�I�����Onil
              if ( tree->atom.attribution == NIL ) {
                ;
              } // end else if

              // ���𪺥k�`�I����"��"�Onil
              else {

                PrintSpace( level ) ;
                cout << '.' << endl ; // �L�X�I
                PrintSpace( level ) ;

                if ( tree->atom.attribution == FLOAT ) {
                  sscanf( tree->atom.strToken.c_str(), "%f", &fout ) ;
                  printf( "%.3f\n", fout ) ;
                } // end if

                else
                  cout << tree->atom.strToken << endl ; // �L�X���`�I���e�P����

              } // end else

              PrintSpace( level-1 ) ;
              cout << ')' << endl ;   // �ɦL�k�A��
              right = false ;

            } // end if

            // �W�@�Ӹ`�I�O���l��
            else {

              // �W�@�ӦL�X���`�I"���O"���A��
              if ( !mIsLastNodeLP ) {
                PrintSpace( level ) ;
              } // end if

              if ( tree->atom.attribution == FLOAT ) {
                sscanf( tree->atom.strToken.c_str(), "%f", &fout ) ;
                printf( "%.3f\n", fout ) ;
              } // end if

              else
                cout << tree->atom.strToken << endl ; // �L�X���`�I���e�P����

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

  map < string, Cons_node* > mDef ; // �x�s�Qdefined�L����
  bool mIsQuote ;

  // �P�_���O�_��function name
  // �p�G�O�A�^��true
  // �p�G�_�A�^��false
  bool IsEvaluateFunctions( string key, string &funcName ) {

    map<string,string>::iterator iter ;   // declaration iterator
    iter = uFunctionNamesMap.find( key ) ;

    if ( iter != uFunctionNamesMap.end() ) {
      funcName = iter->second ;
      return true ;
    } // end if

    return false ;

  } // end IsEvaluateFunctions()

  // �P�_���O�_���Qdefined�L��symbol
  // �p�G�O�A�^��pointer
  // �p�G�_�A�^��NULL
  Cons_node* IsDefined( string key ) {

    map < string, Cons_node* > :: iterator iter ;   // declaration iterator
    iter = mDef.find( key ) ;

    if ( iter != mDef.end() ) {
      return iter->second ;
    } // end if

    return NULL ;

  } // end IsDefined()

  // �T�{��expression���榡(format)�O�_���T
  // (define symbol S-exp) �u��ŧi�γ]�w�Dprimitive ��symbol
  // (define (one-or-more-symbols) one-or-more-S-exp)
  // (set! symbol S-exp)
  // (lambda (zero-or-more-symbols one-or-more-S-exp)
  // (let (zero-or-more-PAIRS) one-or-more-S-exp)
  // (cond one-or-more-AT-LEAST-DOUNBLETONS)
  // PAIR df = (symbol S-exp)
  // AT-LEAST-DOUBLETONS df = a list of two or more S-exp
  // �O�A�^��true
  // �_�A�^��false
  bool CheckFormat( Cons_node* tree ) {

    int numofArg    = 0 ;
    string funcName = "" ;
    funcName.clear() ; // �M��function name

    funcName = tree->left->atom.strToken ;

    // ��function�Odefine
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

    // ��function�Oset!
    else if ( funcName == "#<procedure set!>" ) {

      ;

    } // end else if

    // ��function�Olambda
    else if ( funcName == "#<procedure lambda>" ) {

      ;

    } // end else if

    // ��function�Olet
    else if ( funcName == "#<procedure let>" ) {

      ;

    } // end else if

    // ��function�Ocond
    else if ( funcName == "#<procedure cond>" ) {

      // �T�{argument���ƥجO�_�ŦX�W�h
      // �O�A�ŦX
      if ( CheckNumberOfVariable( tree, numofArg ) ) {

        // �T�{�Ҧ�argument�O�_���ŦX�W�h
        // �u�n�@�����ŦX�A�Nreturn false
        for ( tree = tree->right ; tree->atom.attribution != NIL ; tree = tree->right ) {

          // �Oatom�A���O��
          if ( IsAtom( tree->left->atom.attribution ) )
            return false ;

          // �O��A���Oatom
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

      // �_�A���ŦX
      else {

        ; // Do Nothing

      } // end else

    } // end else if

    return false ;

  } // end CheckFormat()

  // �P�_��argument�O�_��binding(internal function, define)
  // ���A�^�ǫ��V��binding��pointer
  // �_�A�^��NULL
  Cons_node* CheckBindingAndReturnThePointer( string key ) {

    string functionName = "" ;
    Cons_node* result = NewNode() ;

    // ���T�{��symbol�O�_��internal function
    // �O�A��symbol �O internal function
    if ( IsEvaluateFunctions( key, functionName ) ) {

      result->atom.strToken    = functionName ;
      result->atom.attribution = SYMBOL ;

      return result ;

    } // end if

    // �_�A��symbol "���O" internal function
    // �A�T�{��symbol�O�_���Qdefine�L

    result = IsDefined( key ) ;

    if ( result != NULL ) {

      // �T�{��symbol(�Qdefine�L��)�O�_��internal function
      // �O�A��symbol �O internal function
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

      // �إߥB��l�ƭn�Qdefine��symbol�P�L��pointer
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

          // ��U�@�Ӹ`�I�O�k�A��
          // �N��Y�N����

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

      // ��u���@�Ӹ`�I��
      if ( result->left == NULL && result->right == NULL ) {
        uErrorFuncN = "car" ;
        mErrorPointer = result ;
        throw INCORRECT_ARGUMENT_TYPE ;
      } // end if

      // ��O�@�ʾ��
      else {
        // return ���l��
        result =  result->left ;
      } // end else

    } // end else if

    else if ( funcStr == "#<procedure cdr>" ) {

      result = Evaluate( tree->right->left, false ) ;

      // ��u���@�Ӹ`�I��
      if ( result->left == NULL && result->right == NULL ) {
        uErrorFuncN = "cdr" ;
        mErrorPointer = result ;
        throw INCORRECT_ARGUMENT_TYPE ;
      } // end if

      // ��O�@�ʾ��
      else {
        // return �k�l��
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
      bool  isfloat = false ; // �w�]�Ointeger

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
      bool  isfirst = true ;  // �O�Ĥ@��argument
      bool  isfloat = false ; // �w�]�Ointeger

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
      bool  isfirst = true ;  // �O�Ĥ@��argument
      bool  isfloat = false ; // �w�]�Ointeger

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
      bool  isfirst = true ;  // �O�Ĥ@��argument
      bool  isfloat = false ; // �w�]�Ointeger
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

      bool isCorrect = true ; // �ΨӧP�_�C�����O�_���T
                              // �����N�]��false
                              // �L�����|��
                              // �w�]true(�L��)

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

      bool isCorrect = true ; // �ΨӧP�_�C�����O�_���T
                              // �����N�]��false
                              // �L�����|��
                              // �w�]true(�L��)

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

      bool isCorrect = true ; // �ΨӧP�_�C�����O�_���T
                              // �����N�]��false
                              // �L�����|��
                              // �w�]true(�L��)

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

      bool isCorrect = true ; // �ΨӧP�_�C�����O�_���T
                              // �����N�]��false
                              // �L�����|��
                              // �w�]true(�L��)

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

      bool isCorrect = true ; // �ΨӧP�_�C�����O�_���T
                              // �����N�]��false
                              // �L�����|��
                              // �w�]true(�L��)

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

      bool isCorrect = true ; // �ΨӧP�_�C�����O�_���T
                              // �����N�]��false
                              // �L�����|��
                              // �w�]true(�L��)

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

      bool isCorrect = true ; // �ΨӧP�_�C�����O�_���T
                              // �����N�]��false
                              // �L�����|��
                              // �w�]true(�L��)

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

      bool isCorrect = true ; // �ΨӧP�_�C�����O�_���T
                              // �����N�]��false
                              // �L�����|��
                              // �w�]true(�L��)

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

      bool isSame = true ;  // ����̬O�_�����@�˪��a��
                            // �w�]���@��(true)

      argOne = tree->right->left ;
      argTwo = tree->right->right->left ;

      argOne = Evaluate( argOne, false ) ;
      argTwo = Evaluate( argTwo, false ) ;

      if ( argOne == argTwo ) {
        result->atom.strToken    = "#t" ;
        result->atom.attribution = T ;
      } // end if

      else {
        // �u���@�Ӹ`�I(��argOne)
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

      bool isSame = true ;  // ����̬O�_�����@�˪��a��
                            // �w�]���@��(true)

      argOne = tree->right->left ;
      argTwo = tree->right->right->left ;

      argOne = Evaluate( argOne, false ) ;
      argTwo = Evaluate( argTwo, false ) ;

      // �����case
      // 1. �u���@�Ӹ`�I(��argOne)
      // 2. �O�ʾ�(��argOne)

      // �u���@�Ӹ`�I(��argOne)
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

      // �O�ʾ�(��argOne)
      else {
        isSame = CompareTree( argOne, argTwo ) ;
        // isSame = CompareTree( argOne, argTwo ) ;
        // ��ʾ�(argOne,argTwo)��  �@��  �ɡAreturn true
        //                         ���@��   �Areturn false
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

      // �]cond�h���s��
      for ( tree = tree->right ; tree->atom.attribution != NIL ; tree = tree->right ) {

        // �ثeargument�𤤪��Ĥ@��argument
        argOne = tree->left->left ;

        // �T�{�ثeargument�𤤪��Ĥ@��argument�O�_��"else"
        // �O�A�ثeargument�𤤪��Ĥ@��argument�O"else"
        if ( argOne->atom.strToken == "else" ) {

          // �T�{�ثe�O�_��argument��(��cond�P�@�h)���A�s�����˼ƲĤG�Ӹ`�I(�̫�@��nil)
          // �O�A�s�����˼ƲĤG�Ӹ`�I
          if ( tree->right->atom.attribution == NIL ) {

            isFirstArgNIL = false ;

          } // end if

          // �_�A���O�s�����˼ƲĤG�Ӹ`�I
          else {

            arg = Evaluate( argOne, false ) ;

            if ( arg->atom.attribution == NIL )
              isFirstArgNIL = true ;

            else
              isFirstArgNIL = false ;

          } // end else

        } // end if

        // �_�A�ثeargument�𤤪��Ĥ@��argument���O"else"
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

  // ��ʾ�(argOne,argTwo)��  �@��  �ɡAreturn true
  //                         ���@��   �Areturn false
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

  // HOW : �p��ϥΦ�function
  // �ǤJ 1. �@��tree 2. �@��integer�A(�ΨӰO���ܼƪ��ƶq call by reference)
  // ��function name�ӽT�{�nevaluate���ܼƼƶq�O�_�ŦX�W�h
  // �O�A�ܼƼƶq�ŦX�W�h return true
  // �_�A�ܼƼƶq���ŦX�W�h return false
  //
  // WHAT : �p��implement��function
  // ���p���ܼƼƶq
  // �A��function name�P��W�h�P�w�O�_�ŦX
  bool CheckNumberOfVariable( Cons_node *tree, int& numofArg ) {

    int numberofvariable = -1 ; // �p�G�Ĥ@�Ӹ`�I�OLP
                                // �h�ĤG�Ӹ`�I�N�Ofunction
                                // �ѤU��nil(RP)�e�N�O�nevaluate���ܼ�
    Cons_node* firstArg = tree->left ; // first argument

    // �p�⦹S-exp���ܼƼƶq
    for ( Cons_node *iter = tree ; iter->atom.attribution != NIL ; iter = iter->right )
      numberofvariable = numberofvariable + 1 ;

    numofArg = numberofvariable ;

    // ��function�O 'cons'
    if ( firstArg->atom.strToken == "#<procedure cons>" ) {

      // check whether the number of arguments is correct
      if ( numberofvariable == 2 )
        return true ;

    } // end if

    // ��function�O 'list'
    else if ( firstArg->atom.strToken == "#<procedure list>" ) {

      // check whether the number of arguments is correct
      if ( numberofvariable >= 0 )
        return true ;

    } // end else if

    // ��function�O 'quote'
    else if ( firstArg->atom.strToken == "#<procedure quote>" ) {

      // check whether the number of arguments is correct
      if ( numberofvariable == 1 )
        return true ;

    } // end else if

    // ��function�O 'define'
    else if ( firstArg->atom.strToken == "#<procedure define>" ) {

      // check whether the number of arguments is correct
      if ( numberofvariable == 2 )
        return true ;

    } // end else if

    // ��function�O 'car'
    else if ( firstArg->atom.strToken == "#<procedure car>" ) {

      // check whether the number of arguments is correct
      if ( numberofvariable == 1 )
        return true ;

    } // end else if

    // ��function�O 'cdr'
    else if ( firstArg->atom.strToken == "#<procedure cdr>" ) {

      // check whether the number of arguments is correct
      if ( numberofvariable == 1 )
        return true ;

    } // end else if

    // ��function�O 'atom?', 'pair?', 'list?', 'null?', 'integer?'
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

    // ��function�O '+', '-', '*', '/'
    else if ( firstArg->atom.strToken == "#<procedure +>" || firstArg->atom.strToken == "#<procedure ->" ||
              firstArg->atom.strToken == "#<procedure *>" || firstArg->atom.strToken == "#<procedure />" ) {

      // check whether the number of arguments is correct
      if ( numberofvariable >= 2 )
        return true ;

    } // end else if

    // ��function�O 'not'
    else if ( firstArg->atom.strToken == "#<procedure not>" ) {

      // check whether the number of arguments is correct
      if ( numberofvariable == 1 )
        return true ;

    } // end else if

    // ��function�O 'and', 'or'
    else if ( firstArg->atom.strToken == "#<procedure and>" ||
              firstArg->atom.strToken == "#<procedure or>" ) {

      // check whether the number of arguments is correct
      if ( numberofvariable >= 2 )
        return true ;

    } // end else if

    // ��function�O '>', '>=', '<', '<=', '='
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

    // ��function�O 'eqv?', 'equal?'
    else if ( firstArg->atom.strToken == "#<procedure eqv?>" ||
              firstArg->atom.strToken == "#<procedure equal?>" ) {

      // check whether the number of arguments is correct
      if ( numberofvariable == 2 )
        return true ;

    } // end else if

    // ��function�O 'begin'
    else if ( firstArg->atom.strToken == "#<procedure begin>" ) {

      // check whether the number of arguments is correct
      if ( numberofvariable >= 1 )
        return true ;

    } // end else if

    // ��function�O 'if'
    else if ( firstArg->atom.strToken == "#<procedure if>" ) {

      // check whether the number of arguments is correct
      if ( numberofvariable == 2 || numberofvariable == 3 )
        return true ;

    } // end else if

    // ��function�O 'cond'
    else if ( firstArg->atom.strToken == "#<procedure cond>" ) {

      // check whether the number of arguments is correct
      if ( numberofvariable >= 1 )
        return true ;

    } // end else if

    // ��function�O 'clean-environment', 'exit'
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

  // ���s��l��
  void ReObjEvalutaion() {
    mReturnPointer = NULL ;
    mErrorPointer  = NULL ;
    mIsQuote = false ;
  } // ReObjEvalutaion()

  Cons_node* Evaluate( Cons_node *tree, bool topLevel ) {

    Cons_node* result ; // evaluated result
    int numofArg = 0 ;  // �������Ffirst argument�H�~��argument�ƶq

    // ���Otree�A�u���@�Ӹ`�I
    if ( tree->left == NULL && tree->right == NULL ) {

      // �Oatom�A���Osymbol
      if ( IsAtom( tree->atom.attribution ) && tree->atom.attribution != SYMBOL ) {
        result = tree ; // return the atom
      } // end if

      // �Osymbol(���Oatom)
      else if ( tree->atom.attribution == SYMBOL ) {

        Cons_node* node = NewNode() ;
        // �T�{��symbol�O�_��binding��S-exp��internal function

        // �S��binding(unbound)

        node = CheckBindingAndReturnThePointer( tree->atom.strToken ) ;

        if ( node == NULL ) {

          uErrorToken = tree->atom.strToken ;
          throw UNBOUND_SYMBOL ;

          // throw Error Message
          // ERROR (unbound symbol) : SYMBOL
        } // end if

        // ��binding��
        else {
          // return the binding of the S-exp or internal function
          result = node ;
        } // end else

      } // end else if

    } // end if

    // �O��tree
    else {

      // ���T�{��tree�O" pure list "
      // �p�G���O�A�NERROR

      // ��tree "���O" pure list
      if ( !CheckPureList( tree ) ) {
        mErrorPointer = tree ;
        throw NON_LIST ;
        // throw Error Message
        // ERROR (non-list) : PrettyPrint(tree)
      } // end if

      // ��Ĥ@�Ӥ���(element�Bargument)�Oatom�A�Dsymbol
      if ( IsAtom( tree->left->atom.attribution ) && tree->left->atom.attribution != SYMBOL &&
           tree->left->atom.attribution != QUOTE ) {

        uErrorToken = tree->left->atom.strToken ;
        throw ATTEMPT_TO_APPLY_NON_FUNCTION ;

        // throw Error Message
        // ERROR (attempt to apply non-function) : First-Argument
      } // end else if

      // ��Ĥ@�Ӥ���(element�Bargument)�Osymbol�A�Datom
      else if ( tree->left->atom.attribution == SYMBOL || tree->left->atom.attribution == QUOTE ) {

        string errToken = "" ;
        errToken = tree->left->atom.strToken ;
        // �T�{��symbol�O�_���w��function(the name of function)
        // (symbol has a binding, and that binding is an internal function)
        // �O
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

          // �p�G the current level is "NOT" the top level�A�Bsymbol�� 'clean-environment'�B'define'�B'exit'�䤤���@
          if ( !topLevel && ( tree->left->atom.strToken == "#<procedure clean-environment>" ||
                              tree->left->atom.strToken == "#<procedure define>" ||
                              tree->left->atom.strToken == "#<procedure exit>" ) ) {

            uErrorToken = tree->left->atom.strToken ;
            throw LEVEL_ERROR ;

            // throw Error Message
            // ERROR (level of CLEAN-ENVIRONMENT / DEFINE / EXIT)
          } // end if

          // �p�G��symbol�O'define'�B'set!'�B'let'�B'cond'�B'lambda'�䤤���@
          else if ( tree->left->atom.strToken == "#<procedure define>" ||
                    tree->left->atom.strToken == "#<procedure set!>" ||
                    tree->left->atom.strToken == "#<procedure let>" ||
                    tree->left->atom.strToken == "#<procedure cond>" ||
                    tree->left->atom.strToken == "#<procedure lambda>" ) {

            // �T�{��expression���榡(format)�O�_���T
            // (define symbol S-exp) �u��ŧi�γ]�w�Dprimitive ��symbol
            // (define (one-or-more-symbols) one-or-more-S-exp)
            // (set! symbol S-exp)
            // (lambda (zero-or-more-symbols one-or-more-S-exp)
            // (let (zero-or-more-PAIRS) one-or-more-S-exp)
            // (cond one-or-more-AT-LEAST-DOUNBLETONS)
            // PAIR df = (symbol S-exp)
            // AT-LEAST-DOUBLETONS df = a list of two or more S-exp
            // �_�C�榡���~
            if ( !CheckFormat( tree ) ) {

              mErrorPointer = tree ;
              uErrorToken   = tree->left->atom.strToken ;
              throw FORMAT_ERROR ;

              // throw Error Message
              // ERROR (COND format) : <the main S-exp>
              // ERROR (DEFINE format) : <the main S-exp>  // ���i��O�]��redefining primitive���G
              // ERROR (SET! format) : <the main S-exp>  // ���i��O�]��redefining primitive���G
              // ERROR (LET format) : <the main S-exp>  // ���i��O�]��redefining primitive���G
              // ERROR (LAMBDA format) : <the main S-exp>  // ���i��O�]��redefining primitive���G
            } // end if

            // Project 3
            // �O�A��O���T
            // evaluate this tree
            // return the result of the evaluate(and exit this call to Evaluate)

          } // end else if

          // �p�G��symbol�O'if'�B'and'�B'or'�䤤���@
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

          // ��symbol�O�@�Ӥw����function�A�����O'define'�B'let'�B'cond'�B'lambda'�䤤���@
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

        // ��symbol�ëD����w����function
        else {

          // throw Error Message
          // ERROR (unbound symbol) : SYMBOL
          // ERROR (attempt to apply non-function) : ATOM // ATOM is the binding of Symbol
          // cout << "ERROR (unbound symbol)" << endl ;

          uErrorToken = errToken ;
          throw UNBOUND_SYMBOL ;

        } // end else

      } // end else if

      // ��Ĥ@�Ӥ���(element�Bargument)�O�@�ʾ� -> (...) tree A
      else {

        Cons_node* errToken = NewNode() ;

        // evaluate tree A
        tree->left = Evaluate( tree->left, false ) ;
        errToken = tree->left ;

        // �p�G�S������ERROR�o�ͦbevaluate tree A
        // �h�ʧ@�~��

        // �T�{evaluate tree A�����G�O�_�O�@��internal function
        // �O�Ait is an internal function
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

        // �_�Ait is "NOT" an internal function
        else {

          mErrorPointer = errToken ;
          throw ATTEMPT_TO_APPLY_NON_FUNCTION_2 ;

          // throw Error Message
          // ERROR (attempt to apply non-function) : * // * is the evaluated result

        } // end else

      } // end else

      result = EvalFunctions( tree, numofArg ) ; // evaluate Function

    } // end else

    // �p�G"�S��"evaluate�����G�n�Qreturn
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

      // �O�@�Ӹ`�I && �O�Ĥ@�h
      if ( level == 0 && IsAtom( tree->atom.attribution ) ) {

        // �T�{���`�I�O�_��float
        if ( tree->atom.attribution == FLOAT ) {

          sscanf( tree->atom.strToken.c_str(), "%f", &fout ) ;
          printf( "%.3f\n", fout ) ;

        } // end if

        // �Dfloat��atom
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

      // �@�ʾ�
      else {

        bool first = true ;  // �Ĥ@��node
        bool right = false ; // �w�]�W�@�Ӹ`�I�O�q���l��Ӫ�
                             // �Ω�L�Xatom��

        // �k�l��(�s��)
        while ( tree != NULL ) {

          // ���l��
          // ���s�����Ĥ@�ӥ��A��
          if ( tree->atom.attribution == LEFT_PAREN  && first ) {

            first = false ;

            // �W�@�ӦL�X���`�I"���O"���A��
            if ( !mIsLastNodeLP ) {
              PrintSpace( level ) ;
            } // end if

            cout << tree->atom.strToken << ' ' ;

            mIsLastNodeLP = true ;
            level = level + 1 ;
            ErrorPrettyPrint( tree->left, level ) ;

          } // end if

          // ���s����"�D"�Ĥ@�ӥ��A��
          else if ( tree->atom.attribution == LEFT_PAREN  && !first ) {

            if ( tree->left != NULL )
              ErrorPrettyPrint( tree->left, level ) ;

          } // end else if

          // atom
          else if ( tree->atom.attribution != LEFT_PAREN ) {

            // �W�@�Ӹ`�I�O�k�l��
            if ( right ) {

              // ���𪺥k�`�I�����Onil
              if ( tree->atom.attribution == NIL ) {
                ;
              } // end else if

              // ���𪺥k�`�I����"��"�Onil
              else {

                PrintSpace( level ) ;
                cout << '.' << endl ; // �L�X�I
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

                  // �L�X���`�I���e�P����
                  if ( isInternalFunction )
                    cout << functionName << endl ;

                  else
                    cout << tree->atom.strToken << endl ;

                } // end else

              } // end else

              PrintSpace( level-1 ) ;
              cout << ')' << endl ;   // �ɦL�k�A��
              right = false ;

            } // end if

            // �W�@�Ӹ`�I�O���l��
            else {

              // �W�@�ӦL�X���`�I"���O"���A��
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

                // �L�X���`�I���e�P����
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

  ObjScanner    scanner ;       // �ŧiScanner class
  ObjParser     parser ;        // �ŧiParser class
  ObjEvalutaion evaluate ;      // �ŧiEvaluater class
  Error         errMesg ;       // �ŧierror
  int           testNum = 0 ;   // ���ռƾ��D��
  int           numofFunc = 0 ; // Internal Function���ƥ�

  numofFunc = sizeof( uFunctionName ) / sizeof( uFunctionName[0] ) ;

  // �N�nkeyword�Oatom�ɡA�һݭn��X��string
  for ( int i = 0 ; i < numofFunc ; i++ ) {

    string procedure = "" ;
    procedure.clear() ;
    procedure        = "#<procedure " + uFunctionName[i] + ">" ;
    uFunctionNamesMap[uFunctionName[i]] = procedure ;

  } // end for

  map<string,string>::iterator iter ;   // declaration iterator

  cin  >> testNum ; // Ū�����ռƾ�
  getchar() ;       // Ū��'\n'

  cout << "Welcome to OurScheme!" << endl ;
  cout << endl << "> " ;

  do {

    try {

      parser.mroot = parser.BuildTree( parser.mroot ) ; // �ؾ�æ^�ǫ��V����ڸ`�I��pointer

      // �p�G���O��������:(exit)�A�B���k�A���ƥجۦP
      if ( !parser.CheckEXIT( parser.mroot ) ) {

        // parser.PrettyPrint( parser.mroot, 0 ) ; // pretty print, �Ĥ@�ӰѼ�:���V��ڸ`�I��pointer�B�ĤG�ӰѼ�:level
        uProjNumber = 2 ;
        evaluate.mReturnPointer = evaluate.Evaluate( parser.mroot, true ) ;
        parser.PrettyPrint( evaluate.mReturnPointer, 0 ) ; // pretty print, �Ĥ@�ӰѼ�:���V��ڸ`�I��pointer�B�ĤG�ӰѼ�:level
        // parser.ClearAll( parser.mroot ) ;       // �M�ž�
        parser.mroot = NULL ; // �Npointer���VNULL

        // ERROR line and column initialize
        uProjNumber  = 1 ;
        uErrorLine   = 1 ;
        uErrorColumn = 0 ;
        uDoesThisLineHasOutput = true ; // ����@����X�A��bool�N�]��true
        evaluate.ReObjEvalutaion() ;

        cout << endl << "> " ;

      } // end if

    } // end try

    catch ( int err ) {

      if ( !uEndOfFileOcurred ) {

        if ( uProjNumber == 1 ) {

          if ( uErrorLine == 0 )
            uErrorLine = 1 ;

          // �r�ꪺError
          if ( err == NO_CLOSING_QUOTE )
            cout << errMesg.ErrorMessage( err ) << uErrorLine << " Column " << uErrorColumn << endl ;

          // ���F�r��PEOF�H�~��Error
          else {
            // Ū���ѤU��token
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
        parser.mroot = NULL ; // �Npointer���VNULL
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
