// **********************************************************************************************************************************************
//  VERSION         PROGRAMMER          DATE:BEGIN-END              DESCRIPTION)
//  version1.0      Hamilton Chang      2017/03/15 - 2017/03/19     Scanner class, Parser class(without error)
//  version1.1      Hamilton Chang      2017/03/23 - 2017/03/25     Add Error Message, and object(proj1 Completed)
//  version2.0      Hamilton Chang      2017/05/02 - 2017/05/17     Evaluation class(without error)
//  version2.1      Hamilton Chang      2017/05/19 - 2017/05/20     Add Error Message, and object(proj2 Completed)
//  version2.2      Hamilton Chang      2019/01/16 - 2019/01/23     Re-struct the code
//  version3.0      Hamilton Chang      2019/00/   - 2019/00/       Extend Function 'define', Add Function 'let', 'lambda' (without error)
// **********************************************************************************************************************************************

#include "Evaluator.h"    
#include "header/Error.h"  

int main() {

  ObjScanner    scanner ;      
  ObjParser     parser ;        
  ObjEvaluation evaluate ;      
  Error         errMesg ;       
  int           numofFunc = 0 ; // Numbers of internal Function(primitive function)

  numofFunc = sizeof( uGlobal::uFunctionName ) / sizeof( uGlobal::uFunctionName[0] ) ;

  // Take name of the internal functions/primitive funtions as keyword
  // Make a title for it when it considered as an "atom"
  for ( int i = 0 ; i < numofFunc ; i++ ) {

    string procedure = "" ;
    procedure.clear() ;
    procedure        = "#<procedure " + uGlobal::uFunctionName[i] + ">" ;
    uFunctionNamesMap[uGlobal::uFunctionName[i]] = procedure ;

  } // end for

  map<string,string>::iterator iter ;   // declaration iterator

  cout << "Welcome to OurScheme!" << endl ;
  cout << "\n> " ;

  do {

    try {

      parser.mroot = parser.BuildTree( parser.mroot ) ; // build logical tree basee on scheme logic
                                                        // ,and return a pointer which is point at the root of tree

      // Chech if the exit condition "(exit)" doesn't meet
      // , and the number of the left and right parenthesis(括弧) are match  
      // s如果不是結束條件:(exit)，且左右括弧數目相同
      if ( !parser.CheckEXIT( parser.mroot ) ) {

        // parser.PrettyPrint( parser.mroot, 0 ) ; // pretty print, 第一個參數:指向樹根節點的pointer、第二個參數:level
        uGlobal::uProjNumber = 2 ;
        evaluate.mReturnPointer = evaluate.Evaluate( parser.mroot, true ) ;
        parser.PrettyPrint( evaluate.mReturnPointer, 0 ) ; // pretty print, 第一個參數:指向樹根節點的pointer、第二個參數:level
        // parser.ClearAll( parser.mroot ) ;       // 清空樹
        parser.mroot = NULL ; // 將pointer指向NULL

        // ERROR line and column initialize
        uGlobal::uProjNumber  = 1 ;
        uGlobal::uErrorLine   = 1 ;
        uGlobal::uErrorColumn = 0 ;
        uGlobal::uDoesThisLineHasOutput = true ; // 此行一有輸出，此bool就設為true
        evaluate.ReObjEvalutaion() ;

        cout << "\n> " ;

      } // end if

    } // end try

    catch ( int err ) {

      if ( !uGlobal::uEndOfFileOcurred ) {

        if ( uGlobal::uProjNumber == 1 ) {

          if ( uGlobal::uErrorLine == 0 )
            uGlobal::uErrorLine = 1 ;

          // 字串的Error
          if ( err == NO_CLOSING_QUOTE )
            cout << errMesg.ErrorMessage( err ) << uGlobal::uErrorLine << " Column " << uGlobal::uErrorColumn << endl ;

            // 除了字串與EOF以外的Error
          else {
            // 讀掉剩下的token
            errMesg.ReadLeftStuff() ;
            cout << errMesg.ErrorMessage( err ) << uGlobal::uErrorLine << " Column " <<
                 uGlobal::uErrorColumn-uGlobal::uErrorToken.length()+1 << " is >>" << 
                 uGlobal::uErrorToken << "<<" << endl ;
          } // end else

          uGlobal::uErrorLine   = 1 ;

        } // end if

        else if ( uGlobal::uProjNumber == 2 ) {

          if ( err == INCORRECT_NUMBER_OF_ARGUMENT )
            cout << errMesg.ErrorMessage( err ) << uGlobal::uErrorToken << endl ;

          else if ( err == ATTEMPT_TO_APPLY_NON_FUNCTION )
            cout << errMesg.ErrorMessage( err ) << uGlobal::uErrorToken << endl ;

          else if ( err == UNBOUND_SYMBOL )
            cout << errMesg.ErrorMessage( err ) << uGlobal::uErrorToken << endl ;

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

          uGlobal::uErrorLine = 0 ;

        } // end else if

        // ERROR line and column initialize
        parser.mroot = NULL ; 
        uGlobal::uProjNumber  = 1 ;
        // uErrorLine   = 1 ;
        uGlobal::uErrorColumn = 0 ;
        uGlobal::uErrorToken.clear() ;
        uGlobal::uDoesThisLineHasOutput = false ;
        cout << endl << "> " ;

      } // end if

    } // end catch

  } while ( !parser.CheckEXIT( parser.mroot ) && !uGlobal::uEndOfFileOcurred ) ; // end while

  if ( uGlobal::uEndOfFileOcurred )
    cout << "ERROR (no more input) : END-OF-FILE encountered" ;

  cout << endl << "Thanks for using OurScheme!" ;

  return 0;
} // end main()