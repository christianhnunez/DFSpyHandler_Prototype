//#include "ftkcommon/SourceIDSpyBuffer.h"
//#include "ftkcommon/Utils.h"
//#include "ftkcommon/core.h"
//#include "eformat/SourceIdentifier.h"
//#include <sstream>

//namespace daq
//{
//  namespace ftk
//  {

//    /***********************************************************/
//    uint32_t encode_SourceIDSpyBuffer(daq::ftk::BoardType boardtype, uint8_t boardnumber, uint8_t boardinternal, daq::ftk::Position position, uint8_t res)
//    /***********************************************************/
//    {
//      uint32_t sourceID   = 0X0;
//      uint16_t moduleID   = 0X0;
//      uint8_t  reservedID = 0X0;

//      // encode 16 bit word used as module ID
//      moduleID |= (0X1) & SPYBUFFER;          // 1bit Spy Buffer/ Actual Data
//      moduleID <<= 1;
//      moduleID |= (0x1) & res;                // reserved 1bits
//      moduleID <<= 2;
//      moduleID |= (0X3) & position;           // 2bits for position
//      moduleID <<= 4;
//      moduleID |= (0Xf) & boardtype;        // 4bit board type
//      moduleID <<= 8;
//      moduleID |= (0XFF) & boardnumber;     // 8bit board number

//      // encode 8bit word used as reserved ID
//      reservedID |= (0XFF) & boardinternal; // 8bit board internal ID
      
//      sourceID = eformat::helper::SourceIdentifier(eformat::TDAQ_FTK, moduleID, reservedID).code();

//      return sourceID;
//    }

//    /*********************************************************/
//    SourceIDSpyBuffer decode_SourceIDSpyBuffer(uint32_t word)
//    /*********************************************************/
//    {
//      SourceIDSpyBuffer sourceID;

//      sourceID.boardInternal = daq::ftk::getBitRange(word, 24, 31);
//      sourceID.subDetector   = daq::ftk::getBitRange(word, 16, 23);
//      sourceID.dataType      = daq::ftk::getBit(word, 15);
//      sourceID.reserved      = daq::ftk::getBit(word, 14);
//      sourceID.position      = daq::ftk::getBitRange(word, 12, 13);
//      sourceID.boardType     = daq::ftk::getBitRange(word,  8, 11);
//      sourceID.boardNumber   = daq::ftk::getBitRange(word,  0,  7);

//      if(sourceID.subDetector != eformat::TDAQ_FTK)
//        FTK_WARNING("No FTK source ID word, the Sub Detector ID = " << sourceID.subDetector);
      
//      return sourceID;
//    }


//    /*********************************************************/
//    std::string dump_SourceIDSpyBuffer(uint32_t word)
//    /*********************************************************/
//    {
//      std::ostringstream o;
//      SourceIDSpyBuffer sourceID = decode_SourceIDSpyBuffer(word);
//      o << std::hex
//        << "\nInternal = 0x" << sourceID.boardInternal
//        << "\nPosition = 0x" << sourceID.position
//        << "\nsubDet   = 0x" << sourceID.subDetector
//        << "\ndataType = 0x" << sourceID.dataType
//        << "\nboardType= 0x" << sourceID.boardType
//        << "\nboardNbr = 0x" << sourceID.boardNumber
//        << "\nreserved = 0x" << sourceID.reserved
//        << std::dec;
//      return o.str();
//    }

//  }

//}
