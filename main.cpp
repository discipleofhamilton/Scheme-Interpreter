// **********************************************************************************************************************************************
//  VERSION         PROGRAMMER          DATE:BEGIN-END              DESCRIPTION(完成的功能)
//  version1.0      Hamilton Chang      2017/03/15 - 2017/03/19     Scanner class, Parser class(without error)
//  version1.1      Hamilton Chang      2017/03/23 - 2017/03/25     Add Error Message, and object(proj1全部完成)
//  version2.0      Hamilton Chang      2017/05/02 - 2017/05/17     Evaluation class(without error)
//  version2.1      Hamilton Chang      2017/05/19 - 2017/05/20     Add Error Message, and object(proj2全部完成)
//  version2.2      Hamilton Chang      2019/01/16 - 2019/01/23     Re-struct the code
//  version3.0      Hamilton Chang      2019/00/   - 2019/00/       Extend Function 'define', Add Function 'let', 'lambda' (without error)
// **********************************************************************************************************************************************

#include "header/Evaluator.h" // Evaluator class
#include "header/Error.h"     // Error class

int main() {

  ObjScanner    scanner ;       // 宣告Scanner class
  ObjParser     parser ;        // 宣告Parser class
  ObjEvaluation evaluate ;      // 宣告Evaluater class
  Error         errMesg ;       // 宣告error
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

  cout << "Welcome to OurScheme!" << endl ;
  cout << "\n> " ;

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

        cout << "\n> " ;

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