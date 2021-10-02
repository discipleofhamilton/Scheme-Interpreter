//
// Created by Hamilton Chang MSI on 2019/1/16.
//

#include "Scanner.h"

/**
 * ***************************** START private method ****************************
 */

/**
 * string isssue
 * WHAT: There are three situations:
 *    1. COMMON: meet one ", find the second one, make a pair of it
 *    2. ERROR: The second " does not show up but the buffer is empty
 *    3. SPECIAL: The delimeter '\' needs to be deal with in specail
 *       e.g. \+n = ENTER, \+t = TAB, \+" = ", *** \+\ = \ ***
 **/
void ObjScanner::DealWithString() {

  char first = '\0', second = '\0' ;

  first = getchar() ; // peek the first character of the left characters in the buffer
  uGlobal::uErrorColumn = uGlobal::uErrorColumn + 1 ;

  // Set the double quote as the start point,
  // and check does another quote exist before the newline character
  while ( first != '\n' && first != '\"' && first != -1 ) {

    // First, if the first character is '\' AND it is NOT the last character of in the buffer 
    if ( first == '\\' ) {

      second = getchar() ; // peek the second character of the left characters in the buffer
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

    } // end if

    // the other characters: None special cases
    else
      mCurrentToken.strToken = mCurrentToken.strToken + first ; // add the character at the tail of the token

    first = getchar() ;
    uGlobal::uErrorColumn = uGlobal::uErrorColumn + 1 ;
  } // end while

  if ( first == '\"' )
    mCurrentToken.strToken = mCurrentToken.strToken + first ; // add the character at the tail of the token

  else if ( first == -1 )
    uGlobal::uEndOfFileOcurred = true ;

  else // first == '\n'
    throw NO_CLOSING_QUOTE ; // throw error

}


/**
 * @return: A boolean of the character is a integer
 * WHAT: Check if the character is the integer
 * HOW: 1. Utilize the character index in the ASCII CODE table 
 *      2. Check if the index is in the integer indice range of the tables
 */
bool ObjScanner::IsInteger(char ch) {

  int num = ch - '0' ;  // get the index distance between the input character and '0' in the ASCII CODE table

  // Check if the number is the integer range (0 ~ 9)
  if ( ( num >= 0 && num <= 9 ) )
    return true ;
  else
    return false ;

}


/**
 * dot issue in the buffer
 * WHAT: Deal with "DOT" existed is the buffer.
 *       There three attributes and processes of the dot, which depends on the status of the current token
 *       1. Attribution = DOT
 *       2. Attribution = FLOAT
 *       3. Attribution = SYMBOL
 * @arg 
 *  ch (char) : the "NEXT" character in the buffer. P.S. The dot is the current character in the buffer,
 *                                                       and it will be add to the tail of the token outside.
 */
void ObjScanner::DealWithchDot(char next_ch) {

  // 1. Check if the token is empty, which means the DOT is the first character.
  //    AND the NEXT character in the buffer is the white space or the end character (such as '(', ')', ';') or the last character.
  // 2. If the condition above match, then the Attribution/Attribute of the token will be setted as 'DOT'
  if ( mCurrentToken.strToken.empty() && 
        ( next_ch == ' ' || next_ch == '\t' ||
          next_ch == '\n' || next_ch == '(' ||
          next_ch == ')' || next_ch == ';'
        ) 
      ) 
  {
    mCurrentToken.attribution = DOT ; // Set Attribute "DOT" to the token

  } // end if

  // 1. Check if the attribution of the current token is integer, or the token is empty and the next character is INTEGER
  // 2. If the condtion above match, then set the Attribution of the token as "FLOAT"
  else if ( mCurrentToken.attribution == INTEGER || 
            ( mCurrentToken.strToken.empty() &&  
              IsInteger( next_ch ) 
            ) 
          )
  {
    mCurrentToken.attribution = FLOAT ;  // Set Attribute "FLOAT" to the token
  }
  
  // Current character is DOT ".", and the token is neither INTEGER nor FLOAT.
  // The un-paired '(' doesn't exist in the token and the token is not empty.
  // ( !INTEGER && !FLOAT && gLeft_Paren_Num <= 0 ) -> SYMBOL
  else
  {
    mCurrentToken.attribution = SYMBOL ; // Set Attribute "SYMBOL" to the token
  }

}


/**
 * Preprocess the special cases in cutted/current token
 * WHAT:
 *  1. token is INTEGER: deal with '+' delimeter 
 *  2. token is FLOAT: if token is "+." or "_.", then reset the attribution as "SYMBOL"
 *  3. token is NIL or False
 *  4. token is True
 *  e.g. +. != 0.000, +. == +., #f, nil, #t, t -> #t
 */
Token ObjScanner::PreprocessingToken() {

  // Check if the Attirbution of the token is INTEGER
  if ( mCurrentToken.attribution == INTEGER ) {

    // Check if the first character of the integer string is positive mark/"+"
    // then remove the character
    if ( mCurrentToken.strToken[0] == '+' )
      mCurrentToken.strToken = mCurrentToken.strToken.erase(0, 1) ;

  } // end if

  // Check if the Attirbution of the token is FLOAT
  else if ( mCurrentToken.attribution == FLOAT ) {

    // Check if the token is "+." or "_.", then reset the attribution as "SYMBOL"
    if ( mCurrentToken.strToken == "+." || mCurrentToken.strToken == "-." )
      mCurrentToken.attribution = SYMBOL ;

  } // end else if

  // Check if the token is NIL/False sign 
  // Condition: token is "#f" or "nil"
  else if ( mCurrentToken.strToken == "#f" || mCurrentToken.strToken == "nil" ) {

    mCurrentToken.strToken  = "nil" ; // Assign "nil" to the current token 
    mCurrentToken.attribution = NIL ; // Set Attribution NIL to the token

  } // end else if

  // Check if the token is True sign.
  // Condition: token is "#t" or "t"
  else if ( mCurrentToken.strToken == "#t" || mCurrentToken.strToken == "t" ) {

    mCurrentToken.strToken  = "#t" ;  // Assign "#t" to the current token
    mCurrentToken.attribution = T ;   // Set Attribution T to the token

  } // end else if

  return mCurrentToken ; 

}
/**
 * ***************************** END private method ****************************
 */


/**
 * ***************************** START public method ****************************
 */

// Initialize: Constructor of the Scanner 
ObjScanner::ObjScanner() {

  mCurrentToken.strToken.clear() ;
  mCurrentToken.attribution = 0 ;
  mVec_token.clear() ;

}

/**
 * Get the token which has been processed
 * WHAT: Get the token from the buffer and the process it.
 * HOW:
 *  1. 
 */
Token ObjScanner::GetToken() {

  char ch = '\0' ;  // declare a variable to save the read character

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

    // Check if the character is digit
    if ( IsInteger( ch ) ) {

      // Check the current token is empty
      if ( mCurrentToken.strToken.empty() ) 
      {
        mCurrentToken.attribution = INTEGER ; // Set Attribution INTEGER to the token
      }
        
    } // end if

    // Check if the character is positive or negative sign
    else if ( ch == '+' || ch == '-' ) {
      // 1. if the token is empty, which means the sign the first character of the token
      // 2. and the character is not the only character in the token, which means the token is not the signal

      char cSecond = '\0' ;
      cSecond = getchar() ; // peek the next character

      if ( mCurrentToken.strToken.empty() &&
           cSecond != ' ' && cSecond != '\t' && cSecond != '\n' &&
           cSecond != '(' && cSecond != ')' )
        mCurrentToken.attribution = INTEGER ; // Set Attribution INTEGER to the tokens

      // the temperory token is not empty 
      else
        mCurrentToken.attribution = SYMBOL ;  // Set Attribution SYMBOL to the token

      cin.putback( cSecond ) ; // put the peeked character back to the buffer
    } // end else if

    // Check if the character is DOT
    else if ( ch == '.' ) {
      
      char cSecond = '\0' ;
      cSecond = getchar() ;      // peek the next character
      DealWithchDot( cSecond ) ; // deal with the current token when the current character is Dot and depends on the second character
      cin.putback( cSecond ) ;   // put the peeked character back to the buffer

      // if DOT is showed singly, then return the token as DOT
      if ( mCurrentToken.attribution == DOT ) {
        mCurrentToken.strToken = "." ;
        return mCurrentToken ;
      } // end if

    } // end else if
      // SYMBOL
    else
      mCurrentToken.attribution = SYMBOL ; // Set Attribution SYMBOL to the token

    mCurrentToken.strToken = mCurrentToken.strToken + ch ;
    ch     = getchar() ;
    uGlobal::uErrorColumn = uGlobal::uErrorColumn + 1 ;
  } // end while

  // Check if the EOF ERROR is occured
  if ( ch == -1 ) {
    uGlobal::uEndOfFileOcurred = true ;
  } // end if

  // Check if the other ERRORS are occured
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
/**
 * ***************************** START public method ****************************
 */