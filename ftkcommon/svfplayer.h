/*
 *
 * Author: G. Volpi <guido.volpi@cern.ch>
 *
 * SVF player, designed to parse SVF file and send the JTAG command
 * sequences through VME interface that controls the AMB and LAMB
 * cards.
 *
 */
#ifndef _SFVPLAYER_H_
#define _SFVPLAYER_H_
#include <sys/time.h>

#include <iostream>

#include <boost/program_options/option.hpp>
#include <boost/program_options/options_description.hpp>
#include <boost/program_options/variables_map.hpp>
#include <boost/program_options/parsers.hpp>

#include "rcc_error/rcc_error.h"
#include "vme_rcc/vme_rcc.h"
#include <exception>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <ctime>
#include <vector>
using namespace std;

/** This class represent a JTAG sequence, handling the
 * buffer that contain the TMS, TDI and TDO signals.
 * The size of the buffers is the same by default and can
 * be dynamically set.
 */
class JTAGCmdBuffer {
private:
	string _name;

  size_t _nbits;
  size_t _npos;

  bool _hastdo; // true if the TDO was set

  bool _moveexit;

  std::vector<uint8_t> _tms;
  std::vector<uint8_t>_tdi;
  std::vector<uint8_t>_smask;
  std::vector<uint8_t>_tdo;
  std::vector<uint8_t>_mask;
  
public:
  /** Construct the buffer */
  JTAGCmdBuffer(const char *name = "") : _name(name), _nbits(0), _npos(0),
  _hastdo(false), _moveexit(false)
  
  {
    // nothing to be done
  }

  /** Destructor */
  ~JTAGCmdBuffer() {
  }
#if 0
  JTAGCmdBuffer(const JTAGCmdBuffer &cpy) :
  	_name(cpy._name), _nbits(cpy._nbits), _npos(cpy._npos),
	  _hastdo(cpy._hastdo), _moveexit(cpy._moveexit),
	  _tms(_npos,0),
	  _tdi(_npos,0),
	  _smask(_npos,0),
	  _tdo(_npos,0),
	  _mask(_npos,0)
  {
  	if (!_nbits) return;

  	_tms = std::vector<uint8_t>(_npos);
  	_tdi = std::vector<uint8_t>(_npos);
  	_smask = std::vector<uint8_t>(_npos);
  	_tdo = std::vector<uint8_t>(_npos);
  	_mask = std::vector<uint8_t>(_npos);

  	for (size_t pos=0; pos!=_npos; ++pos) {
  		_tms[pos] = cpy._tms[pos];
  		_tdi[pos] = cpy._tdi[pos];
  		_smask[pos] = cpy._smask[pos];
  		_tdo[pos] = cpy._tdo[pos];
  		_mask[pos] = cpy._mask[pos];
  	}
  }
#endif

  /** Set the size, as number of bits, resizing
   * all the buffers, resetting all the parameters.
   *
   * If the new size is equal to the previous the fields
   * are reset, excluding MASK and SMASK.
   * Compatible with: http://www.xilinx.com/support/answers/3173.html
   * */
  void setSize(size_t n) {
    _hastdo = false;
    _moveexit = false;

    if (n!=_nbits) {
      // need to resize all, first clearing the memory
      if (_nbits) {
        _tms.clear();
        _tdi.clear();
        _smask.clear();
        _tdo.clear();
        _mask.clear();
      }
      // resize the internal structures, evaluating its size
      _nbits = n;
      _npos = (n+7)/8;

      if (_nbits==0) return; // skip

      _tms = std::vector<uint8_t>(_npos,0x0);
      _tdi = std::vector<uint8_t>(_npos,0x0);
      _smask = std::vector<uint8_t>(_npos,0xff);
      _tdo = std::vector<uint8_t>(_npos,0x0);
      _mask = std::vector<uint8_t>(_npos,0xff);
      // position 0 represents the first digit, sometime is not full
      if (_nbits&0x7) _mask[0] = _smask[0] = ~((~0)<<(_nbits&0x7));
    }
    else {
      // reset all the elements
      std::fill(_tms.begin(),_tms.end(),0x0);
      std::fill(_tdi.begin(),_tdi.end(),0x0);
      std::fill(_tdi.begin(),_tdi.end(),0x0);      
    }
  }

  /** Return the number of bits */
  inline size_t getNBits() const { return _nbits; }
  /** Return the number of words in the buffers */
  inline size_t getBufferSize() const { return _npos; }

  /** Return true if the TDO was set of this buffer */
  bool hasTDO() const { return _hastdo; }
  /** Check that the command has a TDO to be checked */
  void checkTDO() { _hastdo = true; }

  // Control the stream of TMS bits
  void setTMS(size_t pos, uint8_t val) { _tms[pos] = val; }
  uint8_t getTMS(size_t pos) const { return _tms[pos]; }
  uint8_t* getTMS() { return _tms.data(); }

  void setTDI(size_t pos, uint8_t val) { _tdi[pos] = val; }
  uint8_t getTDI(size_t pos) const { return _tdi[pos]; }
  uint8_t* getTDI() { return _tdi.data(); }

  void setSMask(size_t pos, uint8_t val) { _smask[pos] = val; }
  uint8_t getSMask(size_t pos) const { return _smask[pos]; }
  uint8_t* getSMask() { return _smask.data(); }

  void setTDO(size_t pos, uint8_t val) { _tdo[pos] = val;}
  uint8_t getTDO(size_t pos) const { return _tdo[pos]; }
  uint8_t* getTDO() { return _tdo.data(); }

  void setMask(size_t pos, uint8_t val) { _mask[pos] = val; }
  uint8_t getMask(size_t pos) const { return _mask[pos]; }
  uint8_t* getMask() { return _mask.data(); }

  /** This method specify that this represents the final sequence
   * of a stream of commands, this implies that the Exit flag is
   * set and the latest TMS bit to be sent has to be 0
   */
  void setExit() {
    _moveexit = true;
    _tms[0] |= 1<<((_nbits-1)&7); // set the last bit to move, usually used for shift-*r
  }
  bool getExit() const { return _moveexit; }

  /** Print a status summary of all the buffers. The first line represents the
   * number of bits and positions in the buffer (in paranthesis).
   *
   * The following lines are the exact status for TMS, TDI, SMASK, TDO, MASK.
   *
   * A tag parameter, prepended to all the lines can be set.
   *  */
  void printStatus(const char *tag="") {
    cout << tag << _name << " #b (#B): " << _nbits << "(" << _npos << ")" << endl;
    cout << tag << _name << " TMS  :" << hex;
    for (size_t pos=0; pos!=_npos; ++pos) cout << setfill('0') << setw(2) <<(unsigned int) _tms[pos];
    cout << dec << endl;
    cout << tag << _name << " TDI  :" << hex;
    for (size_t pos=0; pos!=_npos; ++pos) cout << setfill('0') << setw(2) << (unsigned int) _tdi[pos];
    cout << dec << endl;
    cout << tag << _name << " SMASK:" << hex;
    for (size_t pos=0; pos!=_npos; ++pos) cout << setfill('0') << setw(2) << (unsigned int) _smask[pos];
    cout << dec << endl;
    if (_hastdo) {
    	cout << tag << _name << " TDO  :" << hex;
    	for (size_t pos=0; pos!=_npos; ++pos) cout << setfill('0') << setw(2) << (unsigned int) _tdo[pos];
    	cout << dec << endl;
    	cout << tag << _name << " MASK :" << hex;
    	for (size_t pos=0; pos!=_npos; ++pos) cout << setfill('0') << setw(2) << (unsigned int) _mask[pos];
    	cout << dec << endl;
    }
  }

  /** The method compares the expected TDO status for the buffer with a buffer
   * collected externally.
   *
   * @param tdores a buffer of received TDOs, it has to be of the same size of the
   *  other buffers within the object.
   * @return true if the two buffers are identical, excluding bits set as "don't care"
   *  by the MASK
   */
  bool compareTDO(const uint8_t *tdores) const {
  	for (size_t pos=0; pos!=_npos; ++pos) {
  		if ( (_tdo[pos]&_mask[pos])!=(tdores[pos]&_mask[pos]) ) return false;
  	}

  	return true;
  }
};

/** The class mirrors the Xilinx tap controller for FPGA programming
 * and provides function to move from a state another, returning the
 * set of JTAG commands to be gave.
 *
 * Objects of this class not directly control the FPGA TAP controller.
 *
 * TODO: the moveToState() method currently doesn't support all possible
 * transition, however looks to be covering most of the ones required
 * by FPGA programming, including flash memories.
 */
class JTAGTapControl {
public:
  // enumerate the possible states for the TAP controller
  enum TapState {Unkown=0, TestLogicReset, RunTestIDLE,
    SelectDRScan=10, CaptureDR, ShiftDR, Exit1DR, PauseDR, Exit2DR, UpdateDR,
    SelectIRScan=20, CaptureIR, ShiftIR, Exit1IR, PauseIR, Exit2IR, UpdateIR
  };

  /** Helper function used to convert a string representin a state in
   * the value according the enum TapState declaration.
   *
   * TODO Currently not all state are mapped, only the most used ones
   *
   * @param state, string representing the state name
   * @return the enum TapState corrspondent value, Unknown if not mapped
   */
  static TapState toTapState(string state) {
    if (state.find("IDLE")!=string::npos) return RunTestIDLE;
    else if (state.find("IRPAUSE")!=string::npos) return PauseIR;
    else if (state.find("DRPAUSE")!=string::npos) return PauseDR;
    else if (state.find("RESET")!=string::npos) return TestLogicReset;
    else return Unkown;
  }

  static const char* toName(TapState state) {
  	switch (state) {
  	case RunTestIDLE:
  		return "IDLE";
  	case TestLogicReset:
  		return "RESET";
  	default:
  		return "Unkown";
  	}
  }
private:

  TapState _curstate;///< Current status for the TAP controller

public:
  JTAGTapControl() : _curstate(JTAGTapControl::Unkown)
  {;}

  /** Return the status of the TAP controller */
  TapState getState() const { return _curstate;  }
  /** Set the current status */
  void setState(TapState state) { _curstate = state; }

  /** Return the sequence of JTAG commands, usually all TMS commands,
   * required to move from the current state to a specific new state.
   *
   * @param newState, the new state
   * @return true if the transition is possible and current state
   *  is updated, if false the commands will allow to move to RESET and
   *  the internal state will reproduce this.
   */
  bool moveToState(TapState newState, JTAGCmdBuffer &buffer) {
    if (newState==TestLogicReset) {
      // five transition always go to TestLogicReset
    	_curstate = TestLogicReset;
      buffer.setSize(5);
      buffer.setTMS(0, 0x1f);
      return true;
    }
    else if (newState==_curstate) {
    	// nothing to be done
    	buffer.setSize(0);
    	return true;
    }

    // prepare the variables to move to the wanted state
    unsigned int tmsseq(0);
    int steps(0);
    bool onTargetState(false);

    if (_curstate==TestLogicReset) {
      // when here move from Reset to any known state
      switch (newState) {
        case RunTestIDLE:
          steps += 1;
          tmsseq = (tmsseq<<1) | 0x0;
          onTargetState = true;
          break;
        case ShiftDR:
          steps += 4;
          tmsseq = (tmsseq<<4) | 0x2;
          onTargetState = true;
          break;
        case ShiftIR:
          steps += 5;
          tmsseq = (tmsseq<<5) | 0x6;
          onTargetState = true;
          break;
        default:
          cerr << "Unknown transition, remain in RESET" << endl;
          newState = TestLogicReset;
          steps += 5;
          tmsseq = (tmsseq<<5) | 0x1f;
          onTargetState = false;
          break;
      }
    }
    else if (_curstate==RunTestIDLE) {
    	switch (newState) {
    	case ShiftDR:
    		steps += 3;
    		tmsseq = (tmsseq<<3) | 0x1;
    		onTargetState = true;
    		break;
    	case ShiftIR:
    		steps += 4;
    		tmsseq = (tmsseq<<4) | 0x3;
    		onTargetState = true;
    		break;
    	default:
    		cerr << "Unknown transition, remain in RESET" << endl;
    		newState = TestLogicReset;
        steps += 5;
        tmsseq = (tmsseq<<5) | 0x1f;
    		onTargetState = false;
    		break;
    	}
    }
  	else if (_curstate==ShiftDR) {
  		switch (newState) {
  		case RunTestIDLE:
  			steps += 3;
  			tmsseq = (tmsseq<<3) | 0x3;
  			onTargetState = true;
  			break;
  		default:
  			cerr << "Unknown transition, remain in RESET" << endl;
  			newState = TestLogicReset;
  			steps += 5;
  			tmsseq = (tmsseq<<5) | 0x1f;
  			onTargetState = false;
  			break;
  		}
  	}
  	else if (_curstate==ShiftIR) {
  		switch (newState) {
  		case RunTestIDLE:
  			steps += 3;
  			tmsseq = (tmsseq<<3) | 0x3;
  			onTargetState = true;
  			break;
  		default:
  			cerr << "Unknown transition, remain in RESET" << endl;
  			newState = TestLogicReset;
  			steps += 5;
  			tmsseq = (tmsseq<<5) | 0x1f;
  			onTargetState = false;
  			break;
  		}
  	}
  	else if ( (_curstate==Exit1DR || _curstate==Exit1IR) && newState==RunTestIDLE ) {
  		steps += 2;
  		tmsseq =(tmsseq<<2) | 0x1;
  		onTargetState = true;
  	}
  	else {
			cerr << "Unknown transition, remain in RESET" << endl;
			newState = TestLogicReset;
			steps += 5;
			tmsseq = (tmsseq<<5) | 0x1f;
			onTargetState = false;
  	}


    /* Update the internal status and build the command buffer used
     * to send the commands used to move the real TAP controller.
     */
    _curstate = newState;
    buffer.setSize(steps);
    for (unsigned int i=0;i!=buffer.getBufferSize();++i) {
      buffer.setTMS(i,(tmsseq>>(i*8))&0xff);
    }

    return onTargetState;
  }

  /** Helper function that takes the newstate as string.
   *
   * */
  bool moveToState(string newState, JTAGCmdBuffer &buffer) {
    return moveToState(toTapState(newState), buffer);
  }

  /** This provides the steps required to run a test, remaining
   * in the current state for n cycles.
   */
  bool runTest(unsigned int nTCK, JTAGCmdBuffer &buffer) {
  	// the buffer will have the size of the number of clocks
  	// this incidentally also imply that nTCK bits in TMS are 0
  	buffer.setSize(nTCK); //
  	return true;
  }
};


class SVFInterpreterError : public exception {
public:
	const char *what() const noexcept { return "Error in SVF file"; }
};

/** The class provide a generic SVF interpreter. The interpreter has a
 * send_command() method that by default just provides debugging information
 * to control that an SVF file has been parsed correctly. To actually
 * send a stream of JTAG command to a real device the class has to be
 * inherited and the send_command() method overload to reflect the
 * needs of the specific implementation.
 *
 */
class SVFInterpreter {
protected:
	int _verbose; // verbosity level
	bool _printComment; // regulate if the comments are reported or not

	float _frequency;

	bool _hasCable; // it is true if a real JTAG cable or similar connection exists

	size_t _totnbits; // total number of bits send to the JTAG chain
	unsigned _nTDOerrors;
	unsigned _nTDOchecks;
	double _TotIDLETime;
	unsigned int _TotIDLETck;

	string _filepath;

	ifstream _svffile;

	streampos _beginpos;
	streampos _endpos;

	JTAGTapControl _tapcontrol;

	// buffers used to store the JTAG commands during the parsing
	JTAGCmdBuffer _movbuffer;
	JTAGCmdBuffer _sirbuffer;
	JTAGCmdBuffer _sdrbuffer;
	JTAGCmdBuffer _hirbuffer;
	JTAGCmdBuffer _hdrbuffer;
	JTAGCmdBuffer _tirbuffer;
	JTAGCmdBuffer _tdrbuffer;
	JTAGCmdBuffer *_curbuffer; // buffer used in the current set of instructions

	JTAGTapControl::TapState _defir;
	JTAGTapControl::TapState _defdr;

	/** small function that converts a single character representing
	 * a hex value as integer.
	 */
	unsigned int _Char2Int(const char &c) {
	  if (c>='0' && c<='9') return c-'0';
	  else if (c>='a' && c<='f') return c-'a'+10;
	  else if (c>='A' && c<='F') return c-'A'+10;
	  else return 0;
	}

	virtual void send_command(const JTAGCmdBuffer &);
	bool checkTDOs(const JTAGCmdBuffer &, const uint8_t *);

	void parse_bitmask(JTAGCmdBuffer *, unsigned int);

	string nextToken();

public:
	SVFInterpreter();
	virtual ~SVFInterpreter();

	void setVerbose(int verb) { _verbose = verb; }
	int getVerbose() const { return _verbose; }

	void setPrintComment(bool doit=true) { _printComment = doit; }
	bool getPrintComment() const { return _printComment; }

	bool hasCable() { return _hasCable; }

	unsigned int getNTDOErrors() const { return _nTDOerrors; }
	unsigned int getNTDOChecks() const { return _nTDOchecks; }

	void openFile(const char*);

	void parse();

	virtual void printSummary();
};

SVFInterpreter::SVFInterpreter() :
		_verbose(0), _printComment(true),
		_frequency(1e6), _hasCable(false),
		_beginpos(0L), _endpos(0),
		_tapcontrol(),
		_movbuffer("MOV"),
		_sirbuffer("SIR"), _sdrbuffer("SDR"),
		_hirbuffer("SIR"), _hdrbuffer("SDR"),
		_tirbuffer("SIR"), _tdrbuffer("SDR"),
		_curbuffer(0x0),
		_defir(JTAGTapControl::RunTestIDLE),
		_defdr(JTAGTapControl::RunTestIDLE)
{;}

SVFInterpreter::~SVFInterpreter()
{	; }

/** Open a file, throw an error in case of problems */
void SVFInterpreter::openFile(const char *fname) {
	_filepath = fname;

	_svffile.open(_filepath);

	if (!_svffile) {
		ios_base::failure("Error opening the file, stopping the procedure");
	}

	// check the file size, moving to the EOF and checking the position
	_beginpos = _svffile.tellg();
	_svffile.seekg( 0, _svffile.end);
	_endpos = _svffile.tellg();
	_svffile.seekg( 0, _svffile.beg);
}

/** Internal function used to read the next word.
 *
 * The function read until a blank or ; symbol, the ; is
 * read only if is the only character before a blank/newline.
 */
string SVFInterpreter::nextToken() {
  string token;

  _svffile >> token;

  if (token==";") return token;
  else if (token.back()==';') {
    token.pop_back();
    _svffile.putback(';');
  }

  return token;
}

/** Asks to parse the current file.
 *
 * The parsing is based on SVF files generated by Impact and may need
 * generaliziotion. */
void SVFInterpreter::parse() {
	// command parsing variables
	bool cmdbegin(true);
	string cmdword;
	string cmdarg;

	unsigned int oldfrac(0);
	while (1) { // SVF file parser loop

	  // check the position in the file to show that the read is not stuck
		streampos curpos = _svffile.tellg();
		unsigned int curfrac =100./(_endpos-_beginpos)*(curpos-_beginpos);
		if (curfrac>oldfrac) {
			cout << "Advance " << curfrac << "%" << " read " << curpos << " of " << _endpos << endl;
			printSummary();
			oldfrac = curfrac;
		}

	  // read the next string value
		string keyword(nextToken());
		//svffile >> keyword;

		if (_svffile.eof()) break; // end of the read loop

		if (cmdbegin) { // check if the line is a comment
			if (keyword.find("//")==0) {
				getline(_svffile, cmdarg);
				/* The comments often help to identify the where the program is during the
				 * procedure and understand where problems happened, they are indeed
				 * usually printed, this can be suppressed.
				 */
				if (_printComment) cout << "Comment: " << cmdarg << endl;
				continue;
			}
			// if not a comment, this is the current command
			cmdword = keyword;
			cmdbegin = false; // the command is now started
		} // end check of the comment



		if (keyword==";") { // end of the command
			if (_verbose) cout << "+++ Execute: " << cmdword << " +++" << endl;

			// for a few commands there are actions that are taken
			if (cmdword=="SIR") { // Send data to the Instruction Register
			  // obtain the sequnce of command to move the ShiftIR position
			  if (!_tapcontrol.moveToState(JTAGTapControl::ShiftIR, _movbuffer)) {
			  	cerr << "Error: impossible to move to the Shift-IR state";
			  }
			  /* The final step that has to be loaded in the IR has to be identified
			   * because responsible of moving to to Exit1IR state, this final
			   * step can the last bit of the TIR or SIR, if TIR size is 0
			   */
			  if (_tirbuffer.getNBits()) _tirbuffer.setExit();
			  else _sirbuffer.setExit();

			  // Move to the ShiftIR position
			  if (_verbose) _movbuffer.printStatus();
			  send_command(_movbuffer);
			  // send the header
			  if (_verbose) _hirbuffer.printStatus();
			  send_command(_hirbuffer);
			  // send the instruction
			  if (_verbose) _sirbuffer.printStatus();
			  send_command(_sirbuffer);
			  // send the trailer
			  if (_verbose) _tirbuffer.printStatus();
			  send_command(_tirbuffer);
			  // comunicate the TAP representation the new state
			  _tapcontrol.setState(JTAGTapControl::Exit1IR);
			  // obtain the command to move to the default state
			  if (!_tapcontrol.moveToState(_defir, _movbuffer)) {
			  	cerr << "Error: impossible to move to the " << JTAGTapControl::toName(_defir) << " state";
			  }
			  if (_verbose) _movbuffer.printStatus();
			  send_command(_movbuffer);
			}
			else if (cmdword=="SDR") {
			  /* Move the TAP controller in the ShiftDR state and
			   * send data, similarly to what was done befor for the SIR
			   */
			  if (!_tapcontrol.moveToState(JTAGTapControl::ShiftDR, _movbuffer)) {
			  	cerr << "Error: impossible to move to the ShiftDR state";
			  }
			  if (_tdrbuffer.getNBits()) _tdrbuffer.setExit();
			  else _sdrbuffer.setExit();
			  if (_verbose) _movbuffer.printStatus();
			  send_command(_movbuffer);
			  if (_verbose) _hdrbuffer.printStatus();
			  send_command(_hdrbuffer);
			  if (_verbose) _sdrbuffer.printStatus();
			  send_command(_sdrbuffer);
			  if (_verbose) _tdrbuffer.printStatus();
			  send_command(_tdrbuffer);
			  _tapcontrol.setState(JTAGTapControl::Exit1DR);
			  if (!_tapcontrol.moveToState(_defdr, _movbuffer)) {
			  	cerr << "Error: impossible to move to the " << JTAGTapControl::toName(_defdr) << " state";
			  }
			  if (_verbose) _movbuffer.printStatus();
			  send_command(_movbuffer);
			}

      cmdbegin = true;
		}
		else if (keyword=="STATE") {
		  /* Move to a specific state in the TAP controller.
		   *
		   */

		  // read the state
		  _svffile >> cmdarg;

		  // move to the state, no success control is performed
		  if (_verbose) cout << "Move to: " << cmdarg << endl;
		  if (!_tapcontrol.moveToState(cmdarg, _movbuffer)) {
		  	cerr << "Error: impossible to move to the " << cmdarg << " state";
		  }
		  send_command(_movbuffer);

		  cmdbegin = true; // reset parser
		  continue;
		}
		else if (keyword=="ENDIR") {
		  // Update the TAP state to go after a SIR command
		  _svffile >> cmdarg;
		  _defir = JTAGTapControl::toTapState(cmdarg);

		  cmdbegin = true; // reset parser
		  continue;
		}
		else if (keyword=="ENDDR") {
		  // Update the TAP state to go after a SDR command
		  _svffile >> cmdarg;
		  _defdr = JTAGTapControl::toTapState(cmdarg);

		  cmdbegin = true; // reset parser
		  continue;
		}
		else if (keyword=="FREQUENCY") {
		  // read the cable frequency
		  string unit;
		  _svffile >> _frequency >> unit; // assumed to be Hz
		  cerr << "FREQUENCY command is read but later ignored, fixed at 1 MHz" << endl;
		  cmdbegin = true; // reset parser
		}
		else if (keyword=="RUNTEST") {
		  /* This command will put the TAP controller in IDLE state
		   * for some time. At the moment the number of clock or the
		   * seconds are interpreted as the amount of time the TAP
		   * controller should be in this state, no JTAG commands are
		   * sent.
		   * Official documentation of Xilinx has been found not particularly
		   * precise on this topic, however this was found to be workign fine
		   * and good to reduce the VME communication.
		   * TODO: The supported command line is probably not the most generic
		   * possible, more checks should performed.
		   */

		  // Prepare some variables and read the next 2 keywords, expected
		  // to be a delay value and time unit:
		  // expected syntax: RUNTEST VAL UNIT
		  string rawvalue;
		  string state("IDLE");
		  string unit;
		  _svffile >> rawvalue >> unit;

		  // check the rawvalue does not represents a valid state,  in
		  // this case the expected syntax is:
		  // RUNTEST STATE VAL UNIT
		  if (JTAGTapControl::toTapState(rawvalue)!=JTAGTapControl::Unkown) {
		    // reshuffle the variable to agree with the expected syntax
		    state = rawvalue;
		    rawvalue = unit;
		    _svffile >> unit; // unit has still to be read, taking from the file
		  }

		  // obtain the sequence of JTAG commands to move to the wanted state
		  // and send the command to the chain
		  if (_verbose) cout << "Move to: " << state << endl;
		  _tapcontrol.moveToState(state, _movbuffer);
		  send_command(_movbuffer);

		  if (unit.find("TCK")!=string::npos) {
		    /* If the unit is TCK, the delay is calculated as function of the
		     * frequency. First convert the rawvalue string in tick numbers.
		     */
		    unsigned int count = stol(rawvalue);

		    unsigned int countthr(1000000);
		    if (count<countthr) {
		    	_TotIDLETck += count;
		    	_tapcontrol.runTest(count, _movbuffer);
		    	send_command(_movbuffer);
		    	count = 0;
		    }
		    else {
		    	_TotIDLETck += countthr;
		    	_tapcontrol.runTest(countthr, _movbuffer);
		    	send_command(_movbuffer);
		    	count -= countthr;
		    }

		    if (count>0) {
		    	// calculate the seconds and nanoseconds to be in this state
		    	unsigned int nanosec = (count%1000000)*1000;
		    	unsigned int sec = count/1000000;
		    	_TotIDLETime += sec+nanosec*1e-9;
		    	timespec deltaT = {sec, nanosec};
		    	if (_verbose)  cout << "Sleep a little: (" << sec << ", " << nanosec << ")" << endl;
		    	// the sleep is ignored if this is in test mode.
		    	if (_hasCable) nanosleep(&deltaT,0x0);
		    }
		  }
		  else if (unit.find("SEC")!=string::npos) {
		    // The time to wait in the given state is expressed in seconds,
		    // given as floating point number
		    float sectime = stof(rawvalue);
		    _TotIDLETime += sectime;

		    // Extract the number of seconds and nanoseconds fir the timespect structure
		    unsigned int nanosec = sectime*1e9;
		    unsigned int sec = sectime;
		    nanosec -= sec*1000000000;

		    timespec deltaT = {sec, nanosec};
		    if (_verbose)  cout << "Sleep a little: (" << sec << ", " << nanosec << ")" << endl;
		    if (_hasCable) nanosleep(&deltaT,0x0); // do nothing if in test mode
		  }
		  else {
		  	cerr << "Units not recognized: " << keyword << " " << rawvalue << " " << unit << endl;
		  }

		  cmdbegin = true; // reset parser
		}
		else if (keyword=="SIR") {
		  /* In this case all the following keywords, until ;
		   * will be referred to a SIR command.
		   */
		  cmdword = keyword;
		  size_t nbits;
		  _svffile >> nbits;
		  _sirbuffer.setSize(nbits);
		  _curbuffer = &_sirbuffer;
		}
		else if (keyword=="SDR") {
      /* In this case all the following keywords, until ;
       * will be referred to a SDR command.
       */
		  cmdword = keyword;
		  size_t nbits;
		  _svffile >> nbits;
		  _sdrbuffer.setSize(nbits);
		  _curbuffer = &_sdrbuffer;
		}
		else if (keyword=="HIR") {
      /* In this case all the following keywords, until ;
       * will be referred to a HIR command.
       */
		  cmdword = keyword;
		  size_t nbits;
		  _svffile >> nbits;
		  _hirbuffer.setSize(nbits);
		  _curbuffer = &_hirbuffer;
		}
		else if (keyword=="TIR") {
      /* In this case all the following keywords, until ;
       * will be referred to a TIR command.
       */
		  cmdword = keyword;
		  size_t nbits;
		  _svffile >> nbits;
		  _tirbuffer.setSize(nbits);
		  _curbuffer = &_tirbuffer;
		}
		else if (keyword=="HDR") {
      /* In this case all the following keywords, until ;
       * will be referred to a HDR command.
       */
		  cmdword = keyword;
		  size_t nbits;
		  _svffile >> nbits;
		  _hdrbuffer.setSize(nbits);
		  _curbuffer = &_hdrbuffer;
		}
		else if (keyword=="TDR") {
		  /* In this case all the following keywords, until ;
		   * will be referred to a TDR command.
		   */
		  cmdword = keyword;
		  size_t nbits;
		  _svffile >> nbits;
		  _tdrbuffer.setSize(nbits);
		  _curbuffer = &_tdrbuffer;
		}
		else if (keyword=="TDI") {
		  /* Set the TDI value for the command */
      parse_bitmask(_curbuffer, 0);
		}
		else if (keyword=="SMASK") {
		  /* Update the SMASK value for the current command */
		  parse_bitmask(_curbuffer, 1);
		}
		else if (keyword=="TDO") {
		  /* Set the TDO message expected as output of this command,
		   * the checkTDO() command is required to enable the check
		   * of the value.
		   */
		  parse_bitmask(_curbuffer, 2);
		  _curbuffer->checkTDO();
		}
		else if (keyword=="MASK") {
		  /* update the MASK status of the current command, usually SDR,
		   * using the value from the file.
		   */
		  parse_bitmask(_curbuffer, 3);
		}
		else if (keyword=="TRST") {
		  _svffile >> cmdarg; // take the value
		  cout << "TRST is ignored, was required " << cmdarg << endl;
		}
		else {
		  // The command was unknown, this not considered blocking but
		  // a message will be printed out
		  if (cmdbegin) cout << "?command " << keyword << endl;
		  else cout << "?argument " << keyword << endl;
		}
 	} // end SVF file parser loop

}
void SVFInterpreter::parse_bitmask(JTAGCmdBuffer *cmdbuffer, unsigned int id) {
	bool numstart(false);

	char c1, c2;

	// point the the specific buffer
	uint8_t *buffer(0x0);
	if (id==0) buffer = cmdbuffer->getTDI();
	else if (id==1) buffer = cmdbuffer->getSMask();
	else if (id==2) buffer = cmdbuffer->getTDO();
	else if (id==3) buffer = cmdbuffer->getMask();

	size_t pos(0);

	while(1) { // parsing loop
		_svffile >> c1;

		if (c1=='(') {
			numstart = true;
		}
		else if (c1==')') {
			if (!numstart) cerr << "*** possible bitmask parsing error";
			break;
		}
		else {
			_svffile >> c2;

			if (pos==cmdbuffer->getBufferSize()) {
				cerr << "Incorrect size of the buffer, writing position " << pos << ", size is " << cmdbuffer->getBufferSize() << endl;
			}

			/*
	      ostringstream nums;
	      nums << c1 << c2;
	      buffer[pos] = stol(nums.str(),0x0,16);
			 */
			buffer[pos] = (_Char2Int(c1)<<4) | _Char2Int(c2);
			pos += 1;
		}
	} // end parsing loop
}

void SVFInterpreter::printSummary() {
	cout << "Total number of bits: " << _totnbits << endl;
	cout << "Total number of TDO errors: " << _nTDOerrors <<  " (Tot checks " << _nTDOchecks << ")" << endl;
	cout << "Total RUNTEST IDLE time " << _TotIDLETime << " secs, Tcks " << _TotIDLETck << endl;
}

void SVFInterpreter::send_command(const JTAGCmdBuffer &cmd) {
	// sum the number of bits
	_totnbits += cmd.getNBits();
}

bool SVFInterpreter::checkTDOs(const JTAGCmdBuffer &cmd, const uint8_t *tdores) {
	bool checkRes = cmd.compareTDO(tdores);
	_nTDOchecks += 1;
	_nTDOerrors += !checkRes;
	if (_verbose || (!checkRes && _hasCable)) {
		cout << "Check: " << (checkRes? "OK" : "Failed") << " --> (#elements = " << cmd.getBufferSize() << ")"<< endl;
		const unsigned int nrowels(16);
		for (unsigned int irow=0; irow!=(cmd.getBufferSize()+nrowels-1)/nrowels; ++irow) {
			for (size_t pos=0; pos!=nrowels; ++pos) {
				const unsigned int gpos(pos+irow*nrowels);
				if (gpos<=cmd.getBufferSize())
					cout << setfill('0') << setw(2) << hex << (unsigned int) (tdores[gpos]&cmd.getMask(gpos));
				else cout << "  ";
			}
			cout << "    ";
			for (size_t pos=0; pos!=nrowels; ++pos) {
				const unsigned int gpos(pos+irow*nrowels);
				if (gpos<=cmd.getBufferSize())
					cout << setfill('0') << setw(2) << hex << (unsigned int) (cmd.getTDO(gpos)&cmd.getMask(gpos));
				else cout << "  ";
			}
			cout << dec << endl;
		}
	}

	return checkRes;
}
#endif
