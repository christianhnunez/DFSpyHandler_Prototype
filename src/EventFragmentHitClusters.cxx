#include "/Users/ChristianHaroldNunez/Desktop/ftkcommon/ftkcommon/EventFragmentHitClusters.h"
#include "/Users/ChristianHaroldNunez/Desktop/ftkcommon/ftkcommon/Utils.h"


#include <iostream>
#include <iomanip>

 
using namespace daq::ftk;

EventFragmentHitClusters::EventFragmentHitClusters()
  : EventFragmentFTKPacket()
{ }
 
EventFragmentHitClusters::~EventFragmentHitClusters()
{
  // Delete modules (not for shared_ptr)
//  for(auto module : m_modules)
//    delete module;
  m_modules.clear();
}
 
unsigned int EventFragmentHitClusters::getNModules() const
{ return m_modules.size(); }
 
std::shared_ptr<ModuleHitClusters> EventFragmentHitClusters::getModule(unsigned int idx) const
{ return m_modules[idx]; }
 
void EventFragmentHitClusters::parsePayload(const std::vector<unsigned int> &data)
{
  //ModuleHitClusters *tmp_mod=0;
  auto tmp_mod = std::make_shared<ModuleHitClusters>();
  tmp_mod = nullptr;
 
  //ERS_DEBUG(1,"Start parsing payload");

  //if(data.size() == 0 ) ERS_LOG("No data in payload");
    
  

  for(uint32_t iloop=0; iloop<data.size(); iloop++){
    
    uint32_t word = data.at(iloop);
    if(iloop==0 && !daq::ftk::getBit(word, 31)) {

      //ERS_LOG("No module information found at beginning of payload, first data word = " << hex << "0X" << word << " -- corrupt payload");
      continue;
    }
    
    
      if(daq::ftk::getBit(word, 31))
	{
      // tmp_mod = new ModuleHitClusters();
      tmp_mod = std::make_shared<ModuleHitClusters>();
	  // Set the info stored in the first word
	  tmp_mod->setModuleNumber(daq::ftk::getBitRange(word,0,10));
	  tmp_mod->setModuleType(daq::ftk::getBitRange(word,15,15));
      if(tmp_mod->getModuleType() == 1) {
          tmp_mod->setModuleNumber(daq::ftk::getBitRange(word,0,12));
      }
	  if(!(tmp_mod->getModuleType() == 0 || tmp_mod->getModuleType() == 1)){
//	    ERS_LOG("Unknown module type = " << tmp_mod->getModuleType() << " -- corrupt data");
	  }
 
	  m_modules.push_back(tmp_mod);
//	  ERS_DEBUG(2,"Parsing the " << m_modules.size() << " module");
	  
	  // Process clusters next
	}
      else if (m_modules.size()==0)
	{
//	  ERS_LOG("No Module found -- corrupt data");
	}
      else
	{
	  tmp_mod->addCluster(word);
	}
    
    
  }

  //ERS_DEBUG(1,"Finished parsing payload");
}


std::vector<unsigned int> EventFragmentHitClusters::bitstreamPayload() const
{
  std::vector<unsigned int> result;
  result.clear();
  return result;
}
