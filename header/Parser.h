//
// Created by Hamilton Chang MSI on 2019/1/16.
//
#ifndef SCHEME_INTERPRETER_PARSER_H
#define SCHEME_INTERPRETER_PARSER_H

#include "Scanner.h"

// ERRORs of the Project 2
// There 9 kinds of them
#define NON_LIST                        4
#define INCORRECT_NUMBER_OF_ARGUMENT    5
#define INCORRECT_ARGUMENT_TYPE         6
#define ATTEMPT_TO_APPLY_NON_FUNCTION   7
#define NO_RETURN_VALUE                 8
#define UNBOUND_SYMBOL                  9
#define DIVISION_BY_ZERO                10
#define FORMAT_ERROR                    11
#define LEVEL_ERROR                     12
#define ATTEMPT_TO_APPLY_NON_FUNCTION_2 13

// Define a Parser class/layer to process the tokens that delivered by the Scanner
// e.g. Build Parser Tree, and Syntax Analysis
class ObjParser{

protected:

  bool       mIsAddNode ;   // the flag which was used to mark if the previous node was added between the nodes
  ObjScanner mScanner ;     // Declare a Scanner instance to get the processed tokens from the raw inputs
  Token      mLastToken ;   // to save the last/previous token
  bool       mIsLP ;        // the flag which was used to mark the token is the left parenthesis '('. It only be used in the function PrettyPrint
  bool       mIsLastNodeLP ;// the flag which was used to mark the previous token is the left parenthesis '('. It only be used in the function PrettyPrint
  int        mdotted_pait ; // the times of the dotted-pair appearing

  // Node of the Parser Tree
  struct Cons_node{

    Cons_node *right ;      // the right pointer: point to the right sub-tree
    Cons_node *left ;       // the left pointer: point to the left sub-tree
    Token     atom ;        // node of the leafs/atoms
    bool      dotted_pair ; // the flag of marking whether dotted pair occurred

  } ; // end Cons_node

  Cons_node *NewNode() ;         // Declare a node which would be added
  bool IsAtom( int tokenatrb ) ; // Check if the current node is the leaf/atom by its attribution of the token
  void PrintSpace( int level ) ;

public:

  Cons_node *mroot ; // the root of the Parser Tree
  ObjParser() ;      // Initializer / Constructor
  bool CheckEXIT( Cons_node *tree ) ;       // the function to Check if the exit conditions of the logic tree meet
  Cons_node *BuildTree( Cons_node *tree ) ; // the function to build the Parser Tree with recursion
  void ClearTree( Cons_node *tree ) ; // Clean the Parser Tree
  void ClearAll( Cons_node *tree ) ;  // Clean the Parser instance
  void PrettyPrint( Cons_node *tree, int level ) ;

};

#endif //SCHEME_INTERPRETER_PARSER_H
