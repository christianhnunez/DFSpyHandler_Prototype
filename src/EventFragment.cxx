#include "/Users/ChristianHaroldNunez/Desktop/ftkcommon/ftkcommon/EventFragment.h"

#include <iostream>
#include <fstream>
#include <iomanip>

using namespace daq::ftk;

namespace daq {
  namespace ftk {
    EventFragment::EventFragment()
    { }

    EventFragment::~EventFragment()
    { }

    u_int 
    EventFragment::getL1ID() const
    { 
      return m_l1id; 
    }

    void 
    EventFragment::setL1ID(u_int L1ID)
    { 
      m_l1id = L1ID; 
    }

    void
    dump_eventFragments(const std::vector<EventFragment*>& eventFragments, const std::string& outPath)
    {
      std::ofstream fh;
      fh.open(outPath);

      fh << std::hex << std::setfill('0');
      for(auto eventFragment : eventFragments)
	{
	  std::vector<unsigned int> bitstream=eventFragment->bitstream();
	  for(auto word : bitstream)
	    {
	      fh << std::setw(8) << word << std::endl;
	    }
	}

      fh.close();
    }

  } // namespace ftk
} // namespace daq
