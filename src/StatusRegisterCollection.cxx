//#include "ftkcommon/StatusRegister/StatusRegisterCollection.h"
//#include "ftkcommon/exceptions.h"
////#include <iostream>

//using namespace daq::ftk;

////=====================================================================
//StatusRegisterCollection::StatusRegisterCollection(srType type, srAccess access, uint blockTransfer)
// :  m_useBlockTransfer(blockTransfer), m_type(type), m_access(access)
//{}

//StatusRegisterCollection::~StatusRegisterCollection()
//{
//  //for(uint32_t i = 0; i < m_registers.size(); i++)
//  //  delete m_registers[i];
//  m_registers.clear();
//}

////=====================================================================
//void
//StatusRegisterCollection::addStatusRegister(std::unique_ptr<StatusRegister> sr)
//{
//  //FIXME add back in "&& (sr->getAccess() == m_access)", which stoped working before 02-00-05 due to incomplete sw setup to propagate SRCollection access type
//  if ( (sr->getType() == m_type) )
//    m_registers.push_back(std::move(sr));
//}

////=====================================================================
//StatusRegister*
//StatusRegisterCollection::getStatusRegister(uint32_t idx) const
//{
//  if(m_registers.size() > idx)
//    return m_registers[idx].get();
//  throw daq::ftk::StatusRegisterIssue(ERS_HERE,"can not get the requested register: id > the size of the collection");
//}

////=====================================================================
//void
//StatusRegisterCollection::readout()
//{
//  if(m_useBlockTransfer == 0)
//  {
//    for(uint32_t i = 0; i < m_registers.size(); i++)
//    {
//      m_registers[i]->readout();
//    }
//  } else if(m_useBlockTransfer == 1)
//  {
//    std::vector<uint32_t> vov = m_registers[0]->readoutBlockTransfer(m_registers.size());
//    for(uint32_t i = 0; i < m_registers.size(); i++)
//    {
//      (i < vov.size()) ? m_registers[i]->setValue(vov[i]) : m_registers[i]->setValue(0) ;
//    }
//  }
//}

////=====================================================================
//std::vector<uint32_t>
//StatusRegisterCollection::getValues() const
//{
//  std::vector<uint32_t> vov;
//  for(uint32_t i = 0; i < m_registers.size(); i++)
//  {
//    vov.push_back(m_registers[i]->getValue());
//  }
//  return vov;
//}

////=====================================================================
//uint32_t
//StatusRegisterCollection::getValue(uint32_t idx) const
//{
//  if(m_registers.size() > idx)
//    return m_registers[idx]->getValue();
//  else
//  {
//    daq::ftk::StatusRegisterIssue ex(ERS_HERE,"can not get the requested register: id > the size of the collection");
//    ers::warning(ex);
//    return srNoneValue;
//  }
//}

