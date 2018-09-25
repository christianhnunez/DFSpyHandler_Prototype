//#include "ftkcommon/ReadSRFromISVectors.h"
//#include <sstream>      										// std::stringstream, std::stringbuf
//#include "ftkcommon/exceptions.h"						//

//namespace daq
//{
//	namespace ftk
//	{
//		unsigned int ReadSRFromISVectors( unsigned int srAdress, const std::vector< std::vector<unsigned int> > &srList, const std::vector< std::vector<unsigned int> > &srInfoList, unsigned int fpgaNum, unsigned int selectorRegister, unsigned int readerRegister )
//		{
//			int i_collection(-1);
//			uint i_minPrescale(9999);
//			for(uint i=0; i<srInfoList.size(); i++)
//			{
//				if(srInfoList[i][0] <= srAdress && srAdress < srInfoList[i][1]
//					&& fpgaNum == srInfoList[i][3]
//					&& selectorRegister == srInfoList[i][4] && readerRegister == srInfoList[i][5])
//				{
//					// read prescale value from meta-data on IS, assume 1, if not available
//					uint prescale;
//					try {
//						prescale = srInfoList[i].at(6);
//					} catch ( const std::out_of_range& oor ) {
//						prescale = 1;
//					}
//					// pick the collection with the smallest prescale value
//					if( i_minPrescale > prescale ) {
//						i_minPrescale = prescale;
//						i_collection = i;
//					}
//				}
//			}
//			if( i_collection == -1){
//				// Register not available on IS
//				std::stringstream message;
//				message << "Status register addr. 0x" << std::hex << srAdress
//					<< " with selector " << std::hex << selectorRegister
//					<< " and read selector " << std::hex << readerRegister
//					<< " on FPGA # " << fpgaNum
//					<< " is not available in considered IS SR collections. Return 0";
//				daq::ftk::ftkException ex(ERS_HERE, "ReadSRFromISVectors", message.str() );
//				ers::warning(ex);
//				return 0;
//			}
//			else{
//				unsigned int i_sr = (srAdress - srInfoList[i_collection][0] ) /  srInfoList[i_collection][2];
//				//debug printout if needed
//				//ERS_LOG("Address : " << srAdress << "  FPGA = " << fpgaNum
//				//	<< "  Collection = " << i_collection << "  Step = " << i_sr << "    Value = " << srList[i_collection][i_sr]);
//				return srList[i_collection][i_sr];
//			}
//		}
//	} //namespace ftk
//} // namespace daq




