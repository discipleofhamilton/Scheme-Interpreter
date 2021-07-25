//
// Created by Hamilton Chang MSI on 2019/1/16.
//

#include "Parser.h"

// protected

// 宣告要新增的節點，並回傳
ObjParser::Cons_node* ObjParser::NewNode() {

  Cons_node *node = new Cons_node() ;
  node->atom.strToken.clear() ;
  node->atom.attribution = 0 ;
  node->left             = NULL ;
  node->right            = NULL ;
  node->dotted_pair      = false ;

  return node ;

}

// 依目前token的屬性判斷此token是否為atom node
bool ObjParser::IsAtom(int tokenatrb) {

  // IS ATOM
  if ( tokenatrb == INTEGER || tokenatrb == FLOAT || tokenatrb == SYMBOL ||
       tokenatrb == STRING || tokenatrb == NIL || tokenatrb == T )
    return true ;

  else // NOT ATOM
    return false ;

}

void ObjParser::PrintSpace(int level) {

  for ( int i = 0 ; i < level*2 ; i++ )
    cout << ' ' ;

}
// protected

// public

// 初始化
ObjParser::ObjParser() {

  mroot        = NULL ;  // 根節點指向NULL
  mIsAddNode   = false ; // 是否有在token跟token插入一節點
  mIsLP        = false ; // 初始為非左括號
  mIsLastNodeLP = true ;  // 初始值為是左括號
  mdotted_pait = 0 ;     // 初始為零
  mLastToken.attribution = 0 ;
  mLastToken.strToken.clear() ;

}

// 確認此樹是否為結束條件
bool ObjParser::CheckEXIT(ObjParser::Cons_node *tree) {

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

}

// 用遞迴建樹
// 1.  先判斷目前是否為樹的起始
// 1.1 是
//     a. 確認此token是否為atom-node -> 是，將token放入節點中，然後return
//     b. 確認此token是否為左括號    -> 是，將此token放入節點，接著遞迴
// 2. Dotted-Pair的建立
// 3. QUOTE
ObjParser::Cons_node* ObjParser::BuildTree(ObjParser::Cons_node *tree) {

  bool isDottedPairOccurred = false ;

  if ( uGlobal::uEndOfFileOcurred )
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
      uGlobal::uErrorToken = thisToken.strToken ;
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
          uGlobal::uErrorToken = thisToken.strToken ;
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

        uGlobal::uErrorToken = thisToken.strToken ;
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
          uGlobal::uErrorToken = thisToken.strToken ;
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
      uGlobal::uErrorToken = tokenRP.strToken ;
      throw UNEXPECTED_TOKEN_TWO ;
    } // end else

  } // end if

  return tree ;

}

// 清空樹
void ObjParser::ClearTree(ObjParser::Cons_node *tree) {

  if ( tree != NULL ) {
    ClearTree( tree->left ) ;
    ClearTree( tree->right ) ;
    delete tree ;
    tree = NULL ;
  } // end if

}

// 清空parser
void ObjParser::ClearAll(ObjParser::Cons_node *tree) {

  // ClearTree( tree ) ;   // 清空樹
  mroot      = NULL ;  // 根節點指向NULL
  mIsAddNode = false ; // 是否有在token跟token插入一節點
  mIsLP      = false ; // 初始為非左括號
  mIsLastNodeLP = true ;  // 初始值為是左括號
  mLastToken.attribution = 0 ;
  mLastToken.strToken.clear() ;

}

void ObjParser::PrettyPrint(ObjParser::Cons_node *tree, int level) {

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

}
// public