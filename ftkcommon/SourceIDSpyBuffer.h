#ifndef SOURCEIDSPYBUFFER_H
#define SOURCEIDSPYBUFFER_H

#include <iostream>
#include <string>


namespace daq {
  namespace ftk {

    /**
     * Board type
     */
    typedef enum BoardType { 
      FTK   =  0X0,
      IM    =  0X1,
      DF    =  0X2,
      AUX   =  0X3,
      AMB   =  0X4,
      SSB   =  0X5,
      Flic  =  0X6, // lower case because FLIC namespace is already used
      OTHER =  0Xf
    } BoardType;
    
    /**
     * Input/Output position
     */
    typedef enum Position 
    {
      IN             = 0X0,
      OUT            = 0X1,
      OTHER_POSITION = 0X3 
    } Position;

    /**
     * Spy Buffer/Actual Data
     */
    typedef enum DataType 
    { 
      DATA      = 0X0,  ///< ATLAS data
      SPYBUFFER = 0X1,  ///< SpyBuffer data
    } DataType; 
    
    /**
     * SourceIDSpyBuffer encoding struct:
     *  ___________  ___  _______________  _  _____  _______  _______________
     * | | | | | | || | || | | | | | | | || || | | || | | | || | | | | | | | |
     * |1|0|9|8|7|6||5|4||3|2|1|0|9|8|7|6||5||4|3|2||1|0|9|8||7|6|5|4|3|2|1|0|
     * |_|_|_|_|_|_||_|_||_|_|_|_|_|_|_|_||_||_|_|_||_|_|_|_||_|_|_|_|_|_|_|_|
     *  ___________  ___  _______________  _  _____  _______  _______________ 
     * |           ||   ||               || ||     ||       ||               |
     * |   Board   ||P  ||  Subdetector  ||D|| res || Board || Board Number  |
     * |  Internal ||   ||      ID       || ||     || Type  ||               |
     * |___________||___||_______________||_||_____||_______||_______________|
     * 
     * P   = Input/Output position
     * D   = Data Type
     * res = reserved
     *
     * i.e: 6bit Board Internal ID, 2bit Input/Output position, 8bit FTK SubDetector ID, 
     *      1bit Data Type, 3bit reserved, 4bit Board Type and 8bit Board Number
     *
     * NB: if the byte following 0x7F is > 0x70, the payload is a spybuffer
     *     otherwise a real atlas data
     */
    struct SourceIDSpyBuffer 
    {
      uint32_t boardNumber;   ///< bits  0- 7: Board number
      uint32_t boardType;     ///< bits  8-11: FTK, IM, DF, AUX, AMB, SSB, FLIC 
      uint32_t reserved;      ///< bits 12-14: reserved
      uint32_t dataType;      ///< bit     15: 0 == Real data; 1 == SpyBuffer. 
      uint32_t subDetector;   ///< bits 16-23: FTK == 0x7F
      uint32_t position;      ///< bits 24-25: 0 == IN, 1 == OUT
      uint32_t boardInternal; ///< bits 26-31: Board specific bits
    };
    
    /**
     * Constructs the full 32-bit Source ID number made by assembling:
     *  - boardType     [4 bits]
     *  - boardNumber   [8 bits]
     *  - boardInternal [6 bits]
     *  - position      [2 bits]
     *  - reserved      [3 bits]
     */
    uint32_t encode_SourceIDSpyBuffer(daq::ftk::BoardType boardType, uint8_t boardNumber, uint8_t boardInternal, daq::ftk::Position position, uint8_t res=0x0);

    /// Decodes the full 32-bit Source ID returning a SourceIDSpyBuffer struct
    SourceIDSpyBuffer decode_SourceIDSpyBuffer(uint32_t word);

    /// Human readable decoding
    std::string dump_SourceIDSpyBuffer(uint32_t word);
    
  }
}
#endif // SOURCEIDSPYBUFFER_H
