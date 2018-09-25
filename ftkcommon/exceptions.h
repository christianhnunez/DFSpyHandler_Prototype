///**
// *  @author Francesco Cervigni
// *  @date Dec 2010
// *
// *  @file Declares FTK exceptions
// *
// */

//#ifndef FTK_EXCEPTIONS_H
//#define FTK_EXCEPTIONS_H

///////////////////////
////
////	In order to catch one of these errors:
////
////	try
////	{
////	...yourcode...
////	}
////	catch ( daq::ftk::PartitionProblem& e)
////  {
////    std::cerr << "- Error during your code :" << e.what() << std::endl;
////  }
////
////////////////////////

//#include <ers/ers.h>
//#include <ipc/exceptions.h>
//#include <is/exceptions.h>
//#include <oh/exceptions.h>

//using namespace std;

//inline std::string name_ftk(){ return "";}

//namespace daq
//{
       			
//  ERS_DECLARE_ISSUE( ftk,
//    PartitionProblem,
//    "The parameter '" << parameter << "' was not found in the configuration file",
//    ((std::string)parameter )
//  )

//  ERS_DECLARE_ISSUE( ftk,
//    IOError,
//    "Found I/O error : " << parameter ,
//    ((std::string)parameter )
//  )

//  ERS_DECLARE_ISSUE( ftk,
//    EmptyStringParameter,
//    "The insterted parameter is empty: " << parameter ,
//    ((std::string)parameter )
//  )
	
//  ERS_DECLARE_ISSUE( ftk,
//    VmeError,
//    "Found error when accessing the vme library: " << parameter ,
//    ((std::string)parameter )
//  )

//  ERS_DECLARE_ISSUE( ftk,
//    CMEMError,
//    "Found error when accessing the cmem library: " << parameter ,
//    ((std::string)parameter )
//  )

//  ERS_DECLARE_ISSUE( ftk,
//    ConversionError,
//    "Found error when converting string to integer: " << parameter ,
//    ((std::string)parameter )
//  )
	
//  ERS_DECLARE_ISSUE( ftk,
//    BadConfigFileException,
//    "'" << parameter << "' was not found in the configuration file",
//    ((std::string)parameter )
//  )
    			
//  ERS_DECLARE_ISSUE( ftk,
//    TypeException,
//    "Error: Type exception" << parameter ,
//    ((std::string)parameter )
//  )
    			
//  ERS_DECLARE_ISSUE( ftk,
//    WrongParameters,
//    "Error: Wrong Parameters"  << parameter ,
//    ((std::string)parameter )
//  )
    			
//  ERS_DECLARE_ISSUE( ftk,
//    CoralException,
//    "Error: Interacting with coral"  << parameter ,
//    ((std::string)parameter )
//  )
    
//  ERS_DECLARE_ISSUE( ftk,
//    ISException,
//    "Error: Publishing on IS: " << parameter ,
//    ((std::string)parameter )
//  )
  
//  ERS_DECLARE_ISSUE( ftk,
//    OHException,
//    "Error: Publishing on OH: " << parameter ,
//    ((std::string)parameter )
//  )

//  ERS_DECLARE_ISSUE( ftk,
//    EnvironmentValueException,
//    "Error: "  << parameter ,
//    ((std::string)parameter )
//  )

//  ERS_DECLARE_ISSUE( ftk,
//    Information,
//    parameter ,
//    ((std::string)parameter )
//  )
	
//  ERS_DECLARE_ISSUE( ftk,
//    StatusRegisterIssue,
//    "Error: operation with an FTK status register: " << parameter ,
//    ((std::string)parameter )
//  )

//  ERS_DECLARE_ISSUE( ftk,
//    IPBusIssue,
//    "Error: IPBus access: " << parameter ,
//    ((std::string)parameter )
//  )

//  ERS_DECLARE_ISSUE( ftk,
//    IPBusRead,
//    "Error: IPBus read: " << parameter ,
//    ((std::string)parameter )
//  )

//  ERS_DECLARE_ISSUE( ftk,
//    i2cIPBusIssue,
//    "Error: i2c+IPBus access: " << parameter ,
//    ((std::string)parameter )
//  )

//  ERS_DECLARE_ISSUE( ftk,
//    i2cIPBusRead,
//    "Error: i2c+IPBus read: " << parameter ,
//    ((std::string)parameter )
//  )

//  ERS_DECLARE_ISSUE( ftk,
//    i2cIPBusWrite,
//    "Error: i2c+IPBus write: " << parameter ,
//    ((std::string)parameter )
//  )

//  ERS_DECLARE_ISSUE( ftk,
//    IMLUTWarning,
//    "Warning: " << par0 << " Failed IM LUT upload: "
//                <<        hex<<setw(4)<<setfill('0')<<par1
//	        <<" != "<<hex<<setw(4)<<setfill('0')<<par2
//	        <<" or "<<hex<<setw(4)<<setfill('0')<<par3
//	        <<" != "<<hex<<setw(4)<<setfill('0')<<par4
//                <<". Will be retried...",
//    ((std::string)par0)((uint32_t)par1)((uint32_t)par2)((uint32_t)par3)((uint32_t)par4)
//  )

//  ERS_DECLARE_ISSUE( ftk,
//    WrongFwVersion,
//    "Wrong FW version of "<<par1<<" FPGA "<<par2
//	        <<": "<<hex<<par3
//	        <<", while expected "
//                <<hex<<par4,
//    ((std::string)par1)((uint32_t)par2)((uint32_t)par3)((uint32_t)par4)
//  )

//  ERS_DECLARE_ISSUE( ftk,
//		     EventFragmentCollectionAddWrongL1ID,
//		     "Adding event fragment with L1ID " << std::hex << badL1ID
//		     << " to a collection containing L1IDs " << std::hex << collL1ID,
//		     ((uint32_t)collL1ID)((uint32_t)badL1ID)
//		     )

//  ERS_DECLARE_ISSUE( ftk,
//		     EventFragmentCollectionMissingIdx,
//		     "Requesting subset with " << idx << " when set only has "  << size << " items",
//		     ((uint32_t)idx)((uint32_t)size)
//		     )

//  ERS_DECLARE_ISSUE( ftk,
//                     FTKInfoNotAcquired,
//                     "FTK SpyBuffer sampling: parameter " << parameter << " not found. Setting it to zero.",
//                     ((std::string)parameter)
//                     )

//  ERS_DECLARE_ISSUE( ftk,
//                     FTKFwVersion,
//                     "FTK FW Version: Board " << board << " FPGA " <<fpga_name << ":\t " <<std::hex<<fw_version,
//                     ((std::string)board) ((std::string)fpga_name) ((uint32_t)fw_version)
//                     )

//	ERS_DECLARE_ISSUE( ftk,
//										 ftkException,
//										 "FTK:"<< name  << ": " << message,
//                     ((std::string)name) ((std::string)message)
//										)

//  ERS_DECLARE_ISSUE( ftk,
//	    							 FTKIssue,
//	    							 "Generic FTK Issue: " << parameter ,
//	    							 ((std::string)parameter )
//	  							 )

//} // namespace daq

//#endif
