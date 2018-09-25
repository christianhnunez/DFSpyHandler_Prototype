#ifndef UTILS_H_
#define UTILS_H_

#include "/Users/ChristianHaroldNunez/Desktop/ftkcommon/ftkcommon/UtilsMath.h"
#include <vector>
#include <string>

namespace daq {
  namespace ftk {
    /*
     * useful utilities for bit manipulation
     */

    Int_tf readBin(std::string n);

    Int_tf readHex(std::string n);

    Int_tf read(const std::string& n);

    int getBit(unsigned int word, unsigned int ibit);

    Int_tf sgn(Int_tf v);

    Int_tf getMask(unsigned int nbits);

    void shiftRight(Int_tf &val, unsigned int nbits, unsigned int maxwidth, bool &overflow);

    Int_tf getBitRange(Int_tf word, unsigned int begin,unsigned int end);

    void zfill(std::string &s, unsigned int ndigits);

    std::string getBinFromHexDigit(char h);

    std::string getBinFromHex(const std::string& h);

    std::string getSignedBin( const std::string &v , unsigned int nbits );

    std::string getSignedHex(const std::string &v , unsigned int nbits);

    std::string getHexStr(Int_tf v, unsigned int ndigits);

    std::string getBinStr(Int_tf v, unsigned int ndigits);

    /*! \brief Spliting a 32-bit uint into a vector of 4 8-bit words
     */
    std::vector<uint32_t> split32bIn4x8b (const uint32_t word);

    /*! \brief Merging 4 8-bit words into a 32-bit uint. 
     * Returns 0xabcddcba if the array is too short    
     */
    uint32_t merge4x8bIn32b         (const std::vector<uint32_t>& v8b);

    /*! \brief Merging 4 8-bit words into a 32-bit uint. 
     * Returns 0xabcddcba if the array is too short.
     * \param  Four input uint32_t words: from most significant to least significant 8 bits.
     */
    uint32_t merge4x8bIn32b         (const uint32_t v8b3, const uint32_t v8b2, const uint32_t v8b1, const uint32_t v8b0);
    
    /*! \brief Merging 2 16-bit words into a 32-bit uint. 
     * Returns 0xabcddcba if the array is too short.
     * \param  A vector of uint32_t of length >=2 
     */
    uint32_t merge2x16bIn32b        (const std::vector<uint32_t>& v16b);

    /*! \brief Merging 2 16-bit words into a 32-bit uint. 
     * Returns 0xabcddcba if the array is too short.
     * \param  Two input uint32_t words: most significant, least significant 16 bits.
     */
    uint32_t merge2x16bIn32b        (const uint32_t v16b1, const uint32_t v16b0);

    /*! \brief Get name of the OH server for publishing common DataFlow monitoring histograms.
     * It is needed to be independent of what users set as ReadouModule names
     * Returns a string of format "FTK_{BoardName}-{CrateId}-{BoardId}{ExtraString}_common", 
     * where {ExtraString} is "IM", "DF" or "".
     * {BoardName} and {ExtraString} are always in capital letters.
     * \param Name of the ReadoutModule application
     * \param Extra string to be added as {ExtraString
     */
    std::string getDFOHNameString( const std::string& RoMName, 
	const std::string& inBoardName = "", const int& inCrateId = -1, const int& inBoardId = -1, 
	const std::string& extra = "");

  }
}
#endif // UTILS_H_
