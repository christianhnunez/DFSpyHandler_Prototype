#include "/Users/ChristianHaroldNunez/Desktop/ftkcommon/ftkcommon/EventFragmentFTKPacket.h"
#include "/Users/ChristianHaroldNunez/Desktop/ftkcommon/ftkcommon/core.h"

#include <iostream>

//#include "aux/Report.h"

using namespace daq::ftk;

EventFragmentFTKPacket::EventFragmentFTKPacket()
  : EventFragment(),
    m_runNumber(0), m_TTCrcBCID(0), m_l1tt(0), m_evTypROD(0), m_TIM(0),
    m_eFlags(0), m_reservedTrailerWord(0), m_lastTrailerWord(0)
{ }

EventFragmentFTKPacket::~EventFragmentFTKPacket()
{ }

void EventFragmentFTKPacket::parseFragment(const std::vector<unsigned int> &data)
{
  // Extract header info
  m_runNumber	=data[2];
  setL1ID(data[3]);
  m_TTCrcBCID	=data[4]&0xFFF;
  m_l1tt	=data[5]&0xFF;
  m_evTypROD	=(data[6]>>16)&0xFF;
  m_TIM		=data[6]&0xF;
//  report::debug("Got L1ID packet %08x",getL1ID());
  
  // Extract the payload
  std::vector<unsigned int> payload;
  unsigned int i=0;
  for(i=7;i<data.size();i++)
  {
    if(((data[i]>>16)&0xFFFF)==0xE0DA) break;
    payload.push_back(data[i]);
  }
  parsePayload(payload);
  payload.clear();

  //check if l1id matches in the header and in the trailer
  if(getL1ID() != data[i+1]) {
      std::cout << "L1ID doesn't match in the header and trailer: " << getL1ID() << "  vs  " << data[i+1] << std::endl;
      //ERS_DEBUG(2, "L1ID doesn't match in the header and trailer: "<<getL1ID()<<"  vs  "<<data[i+1]);
  }

  m_trailer_lvl1id      =data[i+1];
  m_eFlags		=data[i+2];
  m_reservedTrailerWord	=data[i+3];
  m_lastTrailerWord	=data[i+4];
}

std::vector<unsigned int> EventFragmentFTKPacket::bitstream() const
{
  std::vector<unsigned int> data;
  data.push_back(0xB0F0B0F0);
  data.push_back(0xFF1234FF);

  data.push_back(m_runNumber);
  data.push_back(getL1ID());
  data.push_back(m_TTCrcBCID);
  data.push_back(m_l1tt);
  data.push_back(m_evTypROD<<16|m_TIM);

  std::vector<unsigned int> payload=bitstreamPayload();
  data.insert(data.end(),payload.begin(),payload.end());

  data.push_back(0xE0DAE0DA);

  data.push_back(getL1ID());
  data.push_back(m_eFlags);
  data.push_back(m_reservedTrailerWord);
  data.push_back(m_lastTrailerWord);

  data.push_back(0xE0F0E0F0);

  return data;
}
