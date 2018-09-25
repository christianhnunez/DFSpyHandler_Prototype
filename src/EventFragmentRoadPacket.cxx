//#include "ftkcommon/EventFragmentRoadPacket.h"
//#include "ftkcommon/core.h"

//#include <iostream>

////#include "aux/Report.h"

//using namespace daq::ftk;

//EventFragmentRoadPacket::EventFragmentRoadPacket()
//  : EventFragment(),
//    m_runNumber(0), m_TTCrcBCID(0), m_l1tt(0), m_evTypROD(0), m_TIM(0),
//    m_eFlags(0), m_reservedTrailerWord(0), m_lastTrailerWord(0)
//{ }

//EventFragmentRoadPacket::~EventFragmentRoadPacket()
//{ }

//void EventFragmentRoadPacket::parseFragment(const std::vector<unsigned int> &data)
//{
//  // No Header in AMB road packets
//  // The following assumes data format just prior to AMB Round Robin in AUX processor FPGAs.
//  // End event word from AMB is 0xf7000001 for L1ID 0x1 (which cannot be detected without the datak word from the transceiver)
//  // Will use 0xf780 for the control flag which is what is done inside the AUX to avoid having to propagate the datak word.
//  // This is possible because bit 23 is reserved

//  // Extract the payload
//  std::vector<unsigned int> payload;
//  unsigned int i=0;
//  for(i=0;i<data.size();i++)
//  {
//    if(((data[i]>>16)&0xFFFF)==0xE0DA){
//      setL1ID(data[i]&0xFFFF);
//      break;
//    }
//    payload.push_back(data[i]);
//  }
//  parsePayload(payload);
//  payload.clear();

//  m_trailer_lvl1id      =data[i]&0xFFFF;
//}

//std::vector<unsigned int> EventFragmentRoadPacket::bitstream() const
//{
//  std::vector<unsigned int> data;

//  std::vector<unsigned int> payload=bitstreamPayload();
//  data.insert(data.end(),payload.begin(),payload.end());

//  data.push_back(0xE0DA<<16|getL1ID());

//  return data;
//}
