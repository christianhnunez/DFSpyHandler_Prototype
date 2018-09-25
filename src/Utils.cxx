#include "ftkcommon/Utils.h"
#include "ftkcommon/UtilsString.h"
#include <sstream>
#include <iomanip> // std::setfill, std::setw
#include <algorithm>
 


namespace daq
{
  namespace ftk
  {
    int getBit(unsigned int word, unsigned int ibit)
    { return (word >> ibit) % 2; }
 
    Int_tf readBin(std::string n)
    {
      if( n.find("0b")==0 ) n.replace(0,2,"");
      return BaseConvertLL(n,2);
    }
 
    Int_tf readHex(std::string n)
    {
      if( n.find("0x")==0 ) n.replace(0,2,"");
      if( n.find("0X")==0 ) n.replace(0,2,"");
      return BaseConvertLL(n,16);
    }
 
    Int_tf read(const std::string& n)
    {
      if( n.find("0b") == 0 ) return readBin(n);
      if( n.find("0x") == 0 ) return readHex(n);
      return BaseConvertLL(n,10);
    }
 
    Int_tf sgn(Int_tf v) { return (v < 0 ? -1 : 1 ); }
 
    Int_tf getMask(unsigned int nbits)
    {
      static Int_tf one_tf=1;
      Int_tf mask = (one_tf<<nbits);
      mask -= one_tf;
      return mask;
    }
 
    void shiftRight(Int_tf &val, unsigned int nbits, unsigned int maxwidth, bool &overflow)
    {
      Int_tf val_sgn = sgn(val);
      Int_tf pos_val = (val >> nbits) * val_sgn;
      Int_tf pos_val_masked = pos_val & getMask(maxwidth-1); // -1 because of sign bit
      if( pos_val_masked != pos_val ) overflow = true;
      val = pos_val_masked * val_sgn;
    }
 
    Int_tf getBitRange(Int_tf word, unsigned int begin,unsigned int end)
    {
      Int_tf result = word >> begin;
      return result & getMask( 1 + end - begin );
    }
 
    void zfill( std::string &s , unsigned int ndigits )
    {
      while( s.length() < ndigits ) s = "0"+s;
    }
 
    std::string getBinFromHexDigit(char h)
    {
      if( h=='0') return "0000";
      if( h=='1') return "0001";
      if( h=='2') return "0010";
      if( h=='3') return "0011";
      if( h=='4') return "0100";
      if( h=='5') return "0101";
      if( h=='6') return "0110";
      if( h=='7') return "0111";
      if( h=='8') return "1000";
      if( h=='9') return "1001";
      if( h=='a') return "1010";
      if( h=='b') return "1011";
      if( h=='c') return "1100";
      if( h=='d') return "1101";
      if( h=='e') return "1110";
      if( h=='f') return "1111";
      assert( false );
    }
 
    std::string getBinFromHex(const std::string& h)
    {
      std::string result = "";
      for(unsigned int ichar = 0 ; ichar < h.length() ; ichar++ )
	result = result + getBinFromHexDigit(h[ichar]);
 
      return result;
    }
 
    std::string getSignedBin( const std::string &v , unsigned int nbits )
    {
      // TODO simplifty
      Int_tf n = std::stoll(v,0,10);
      std::ostringstream oss;
      oss << std::hex << n;
      std::string hexstr = oss.str();
      std::string binstr = getBinFromHex( hexstr );
      if( binstr.length() > nbits ) binstr.erase( 0 , binstr.length() - nbits );
      zfill( binstr , nbits );
      return binstr;
    }
 
    std::string getSignedHex(const std::string &v , unsigned int nbits)
    {
      std::string binstr = getSignedBin( v , nbits );
      Int_tf n = std::stoll(binstr,0,2);
      std::ostringstream oss;
      oss << std::hex << n;
      return oss.str();
    }
 
    std::string getHexStr(Int_tf v, unsigned int ndigits)
    {
      std::ostringstream oss;
      oss << std::hex << v;
      std::string hexstr = oss.str();
      zfill( hexstr , ndigits );
      return hexstr;
    }
 
    std::string getBinStr(Int_tf v, unsigned int ndigits)
    {
      std::ostringstream oss;
      oss << std::hex << v;
      std::string hexstr = oss.str();
      std::string binstr = getBinFromHex(hexstr);
      zfill( binstr , ndigits );
      return binstr;
    }

    std::vector<uint32_t> split32bIn4x8b(const uint32_t word)
    {
      std::vector<uint32_t> v8b;
      v8b.push_back (word & 0Xff);
      v8b.push_back((word & 0Xff00)     >> 8);
      v8b.push_back((word & 0Xff0000)   >> 16);
      v8b.push_back((word & 0Xff000000) >> 24);
      return v8b;
    }
    
    uint32_t merge4x8bIn32b(const std::vector<uint32_t>& v8b)
    {
      uint32_t word;
      if (v8b.size() < 4)
        return 0xabcddbca;
      word = ((v8b.at(3) << 24) | (v8b.at(2) << 16) | (v8b.at(1) << 8) | v8b.at(0));
      return word;
    }

    uint32_t merge4x8bIn32b(const uint32_t v8b3, const uint32_t v8b2, const uint32_t v8b1, const uint32_t v8b0)
    {
      std::vector<uint32_t> v8b;
      v8b.push_back(v8b0);
      v8b.push_back(v8b1);
      v8b.push_back(v8b2);
      v8b.push_back(v8b3);
      uint32_t tmp=  merge4x8bIn32b(v8b);
      return tmp;
    }

    uint32_t merge2x16bIn32b(const std::vector<uint32_t>& v16b)
    {
      uint32_t word;
      if (v16b.size() < 2)
        return 0xabcddbca;
      word = ((v16b.at(1) << 16) | v16b.at(0));
      return word;
    }

    uint32_t merge2x16bIn32b(const uint32_t v16b1, const uint32_t v16b0)
    {
      std::vector<uint32_t> v16b;
      v16b.push_back(v16b0);
      v16b.push_back(v16b1);
      uint32_t tmp=  merge2x16bIn32b(v16b);
      return tmp;
    }

    std::string getDFOHNameString( const std::string& RoMName, 
	const std::string& inBoardName, const int& inCrateId, const int& inBoardId,
	const std::string& extra )
    {
      std::ostringstream oss;
      oss << "FTK_";
      //transform the input RoM name into all capitals
      std::string sin = RoMName;      
      std::transform ( sin.begin(), sin.end(), sin.begin(), ::toupper );
      //loop over all board types and try to find them in capitalised RoMName or XXX if none is found
      std::string boardName = "XXX";
      if ( inBoardName.empty() ) {
        std::vector<std::string> boardNameV = {"DF", "AUX", "AMB", "SSB", "FLIC", "DUMMY"};
        for ( auto bN : boardNameV ) {
          if ( sin.find( bN ) == 0 ) {
            boardName = bN;
          }
        }
      } else {
        boardName = inBoardName;
        std::transform ( boardName.begin(), boardName.end(), boardName.begin(), ::toupper );
      }

      //add boardName
      oss << boardName;

      std::vector<std::string> sinDashTokens = Tokenize( sin, '-' );
      //check if RoMName contains a crate index, set it to 1 if not crate index given
      if ( inCrateId == -1 ) {
        if ( sinDashTokens.size() >= 3 ) {
          oss << "-" << sinDashTokens[1] << "-";
        } else 
          oss << "-1-";
      } else
        oss << "-" << inCrateId << "-";
      
      //extract the board id
      oss << std::setfill('0') << std::setw(2);
      if ( inBoardId == -1 ) {
        if ( sinDashTokens.size() == 1 || ( sinDashTokens.size() == 2 && ! isdigit(sinDashTokens[1][0]) ) ) {
          int boardI = std::stoi( sin.substr( boardName.size() ) );
          if ( boardName == "DF" )
	    boardI /= 8;
          oss << boardI;
        } else if ( sinDashTokens.size() == 2 ) {
          oss << std::stoi( sinDashTokens[1] );
        } else {
          oss << std::stoi( sinDashTokens[2] );
        }
      } else
        oss << inBoardId ;

      oss << extra;
      oss << "_common"; 

      return oss.str();
    }

  } //ftk
} //daq
