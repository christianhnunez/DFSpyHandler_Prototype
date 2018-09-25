#ifndef PATTERNBANK_LIB_H
#define PATTERNBANK_LIB_H

#include <string>
#include <vector>
/** \brief The FTKPatternBank class represents a pattern bank for the AM board.
 * An object should be instantiated, then the setup completed, finally a
 * pattern bank file can be loaded.
 *
 * The FTKPatternBank class represents an interface to load a bank from the
 * offline world into the boards. Some parameters are indeed related to the
 * bank to be loaded and others to the configuration of the board where
 * the bank will be used.
 *
 * An object using this class should be created, only an empty constructor
 * is currently provided, and then setup.
 *
 * The first parameter to be set is the number of layers to be used, with
 * setNLayers(). This set the default values for other internal parameters:
 * number of DC bits in the loaded bank, number of dc bits in the chip,
 * SS description offset.
 *
 * When the setup is completed the pattern bank can be loaded. At this stage
 * the number of patterns is set and the OK flag is set. Multiple bank formats
 * can be loaded and some parameters can change. The specific method to load
 * the bank can eventually interfere with the internal parameters, overriding
 * some. Check the documentation for each format.
 *
 * The number of DC bits for the bank should be set before the bank is loaded,
 * while the DC bits in the AM chip, or other boards, can changed at any time.
 *
 */
class FTKPatternBank {
private:

  std::string m_bankname; //!< Assign a name to identify the bank, no specific use

  /** number of layers in the bank */
  unsigned int m_nlayers;

  /** array with the number of DC bits in the pattern bank,
   * the size is set when the number of layers is set, default 0 everywhere */
  std::vector<uint32_t> m_bankdcbits;
  /** array with the number of bits in the chip, the size is set
   * when the number of layers is set, default is 2 everywhere */
  std::vector<uint32_t> m_chipdcbits;
  /** it is possible to assign an offset to all the SS to prevent the SS=0 in the pattern */
  std::vector<uint32_t> m_ssoffset;

  /** number of patterns, this will be set while the patterns are loaded */
  unsigned int m_npatterns;
  /** the pattern bank content */
  std::vector<uint32_t> m_patterns;
  /** the fit constants ID */
  std::vector<uint32_t> m_sectorsID;

  /** checksum for the current bank, to use only if the m_isOK flag is ok */
  unsigned int m_checksum;

  /** control variable: true if the object is ready to used */
  bool m_isOK;
  /** check variable that records the maximum SS */
  unsigned int m_maxSS;

  /** internal inline method to return the position of the SS of a given pattern in
   * a speicific layer
   */
  inline unsigned int _SSPos(unsigned int ipatt, unsigned int ilayer) const { return ipatt*m_nlayers+ilayer; }

  /** specific function to load pattern from an offline ascii file */
  int loadPatternBankAscii(std::string, unsigned int);
  /** specific function to load pattern from an offline cache */
  int loadPatternBankROOTCache(std::string, unsigned int, bool);
  /** specific function to load DCconfig-only from an offline cache */
  int loadPatternBankDCconfigROOTCache(std::string, bool);

public:
  static const std::size_t NLAYER=8;
  /** default constuctor */
  FTKPatternBank() :
    m_bankname("PBank"),
      m_nlayers(NLAYER),
      m_bankdcbits(NLAYER,0),
      m_chipdcbits(NLAYER,3),
      m_ssoffset(NLAYER,0),
      m_npatterns(0), m_patterns(0x0), m_sectorsID(0x0),
      m_checksum(0), m_isOK(false)
	{}
  
  ~FTKPatternBank();
    
  void printConfiguration() const;

	void clearAll();

  /** set and get the pattern bank name */
  void setName(const char *name) { m_bankname = name; }
  const std::string& getName() const { return m_bankname; }

  /** set the number of layers and initialize internal parameters */
  void setNLayers(unsigned int n);
  unsigned int getNLayers() const { return m_nlayers; }

  void setDCConfig(const std::vector<uint32_t> &bankdc); // allocate the DC bit arrays
  void setDCConfig(unsigned int layer, unsigned int ndc) { m_bankdcbits[layer] = ndc; }
  const unsigned& getDCConfig(unsigned int layer) const { return m_bankdcbits[layer]; }

  /** return the number of pattern that exist in the bank */
  unsigned int getNPatterns() const { return m_npatterns; }

  /**< overwrite the SSs of 1 pattern every pattstep */
  void overrideSpecialPattern(unsigned int ss, unsigned int pattstep, unsigned int firstpatt=0);

  /** boolean flag that identifies if the bank is ok */
  bool getIsOK() const { return m_isOK; }

  /** as getIsOK() */
  bool operator!() const { return m_isOK; }

  const unsigned int& getChecksum() const { return m_checksum; }

  int loadPatternBank(const char *, unsigned int, unsigned int, bool debug = false); // load a pattern bank, specify the file type, the number of patterns has to be passed

  int loadPatternBankDCconfig(const char *, unsigned int, bool debug = false); // load a pattern bank for DCbit configuration

  int createRandomBank(unsigned int, unsigned int mode=0); // create a random pattern bank of N patterns according a specific mode

  int createSequentialBank(unsigned int, unsigned int); // create a bank with equal SS and progressive SS in each patterns

  void clearBank(); // allow to clean the memory allocated to store the patterns

  void dumpPatternBank(const char *); // save the current bank in a commonly used format

  int getPattern(unsigned int, std::vector<uint32_t> &) const;
  int getPatternAndDC(unsigned int, std::vector<uint32_t>&,std::vector<uint32_t> &) const;
  int getPatternAUX(unsigned int, std::vector<uint32_t>&,std::vector<uint32_t>&, std::vector<uint32_t>&, unsigned int&, bool loadAllSectors = false, unsigned int lamb = 4) const;

  const std::vector<uint32_t> &getPatterns() const { return m_patterns; }

  void setChipDCBits(unsigned il, unsigned val) { m_chipdcbits[il] = val; }
  unsigned int getChipDCBits(unsigned il) { return m_chipdcbits[il]; }

  void setSSOffset(unsigned il, unsigned val) { m_ssoffset[il] = val;}

  void calculateChecksum();
  void updateChecksum(const unsigned int , const std::vector<uint32_t> &);

  /** rencode the SS in a representation with a different number of DC bits */
  static unsigned int convertSSAdjustTernaryBits(unsigned int, unsigned int, unsigned int);
  /** Extract a mask showing where the DC bits are used in a SS */
  static unsigned int extractDCBits(unsigned int, unsigned int);

  /** Helper function that converts a pattern number into a road ID */
  static unsigned int getHWRoadID(unsigned int id, unsigned int nPatternsPerChip, unsigned int offset=1);
};
#endif // PATTERNBANK_LIB_H
