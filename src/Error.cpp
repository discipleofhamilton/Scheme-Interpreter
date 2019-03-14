//
// Created by Hamilton Chang MSI on 2019/1/29.
//

# include "../header/Error.h"

string Error::ErrorMessage(int synErrType) {

  string errMesg = "\0" ;

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

}

void Error::ReadLeftStuff() {

  char ch = '\0' ;

  ch = getchar() ;

  while ( ch != '\n' && ch != -1 ) {
    ch = getchar() ;
  } // end while

  // 如果在讀取stuff的過程中，發生eof
  if ( ch == -1 )
    uEndOfFileOcurred = true ;

}