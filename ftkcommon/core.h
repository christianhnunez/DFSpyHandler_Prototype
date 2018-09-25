///**
// * @author Francesco Cervigni
// * @date September 2011
// * Common macros
// */


//#ifndef FTK_CORE_H
//#define FTK_CORE_H

//#include "/Users/ChristianHaroldNunez/Desktop/ftkcommon/ftkcommon/exceptions.h"
//// #include "ers/ers.h"
//#include <sys/types.h>
//#include <sys/syscall.h>
//#include <stdlib.h>
//#include <string>

//// Debug, info and error macros
//#ifdef DEBUG_VME
////#define PRINT_DEBUG_VME( message ) { std::stringstream ss; ss<<message; root["debug"].append(ss.str());} std::cout << message << std::endl ; ERS_DEBUG( 1, message )
//  #define PRINT_DEBUG_VME( message ) { std::stringstream ss; ss<<message } std::cout << message << std::endl ; ERS_DEBUG( 1, message )
//#else
//  #define PRINT_DEBUG_VME( message )
//#endif

//#define PRINT_INFO( message ) { std::stringstream ss; ss<<message; daq::ftk::Information myIssue( ERS_HERE, ss.str() ); ers::info( myIssue ); }
//#define PRINT_LOG( message ) { std::stringstream ss; ss<<message;} ERS_LOG( message )

//#define FTK_WARNING( message ) { ERS_REPORT_IMPL ( ers::warning, ers::Message, message, ERS_EMPTY); }
//#define FTK_VME_ERROR( message ) { std::stringstream ss; ss<<message; throw daq::ftk::VmeError(ERS_HERE, ss.str() ); }
//#define FTK_CONVERSION_ERROR( message ) { std::stringstream ss; ss<<message; throw daq::ftk::ConversionError(ERS_HERE, ss.str() ); }
//#define FTK_STRING_PARAM_ERROR( message ) { std::stringstream ss; ss<<message; throw daq::ftk::ConversionError(ERS_HERE, ss.str() ); }

//// The same macros but with a root object that seems to be a JSON class (or a map)
////A #define PRINT_LOG( message ) { std::stringstream ss; ss<<message; root["steps"].append(ss.str());} ERS_LOG( message )
////A #define PRINT_WARNING( message ) { std::stringstream ss; ss<<message; root["warnings"].append(ss.str());} ERS_WARNING( message )
////A #define FTK_VME_ERROR( message ) { std::stringstream ss; ss<<message; root["errors"].append(ss.str()); throw daq::ftk::VmeError(ERS_HERE, ss.str() ); }

////redefinition of the ERS_LOG with the printing of the tid and the name of the application
//#undef  ERS_LOG
//#define ERS_LOG( message ) do { \
//{ \
//   ERS_REPORT_IMPL( ers::log, ers::Message, "[" << name_ftk() << "],{" << daq::ftk::gettid() << "} " << message, ERS_EMPTY ); \
//} } while(0)

////  inline std::string name_ftk(){return "";}

//namespace daq {
//namespace ftk {


//  inline pid_t gettid(){
//    return syscall(SYS_gettid);
//  }

//  /** This function provides string to integer conversion with contex-aware error reporting.
//   *  Input can be in every base, with the standard form 123, 0123, 0x123 for respectively decimal, octal and hexadecimal values
//   *
//   *  @param s The string to parse
//   *  @return The converted integer, throws FTK_VME_ERROR otherwise
//   */
//  inline int string_to_int(std::string s)
//  {
//    char* end = 0; // end will be different that zero in case of error
 
//    int value= strtol( s.c_str() , &end, 0); // strtol recognizes explicit bases
//    if (*end)
//    {   // In case error is found
//      FTK_CONVERSION_ERROR( "ERROR parsing input parameter " << s << " to integer." )
//    }
//    return value;
//  }


//  /** This function asserts that the string is not empty. Otherwise an error is reported.
//   *  @param str The string to check
//   *  @throws An error FTK_STRING_PARAM_ERROR if the string is empty
//   */
//  inline void assert_string_parameter_not_empty( std::string & str , std::string par_name  )
//  {
//    if ( str.empty( ) )
//    {
//      FTK_STRING_PARAM_ERROR( "The parameter " << par_name << " is empty.");
//    }
//  }

//  /** This function checks if the system is running as "configuration partition".
//   *  This operation mode is controlled via the $FTK_LOAD_MODE shell variable
//   *  @return True if the variable exists
//   */
//  inline bool isLoadMode()
//  {
//    if( getenv("FTK_LOAD_MODE") != NULL)
//      return true;
//    else
//      return false;
//  }


//} // Namespace ftk
//} // Namespace daq

//#endif
