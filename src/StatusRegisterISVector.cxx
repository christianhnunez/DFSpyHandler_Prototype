//#include "ftkcommon/StatusRegister/StatusRegisterISVector.h"
//#include "ftkcommon/exceptions.h"

//#include <is/infodynany.h>

//using namespace daq::ftk;

//StatusRegisterISVector::StatusRegisterISVector (const IPCPartition& partition, const std::string& objName, const std::string& attrName, srType type)
//{
//  isd		= std::make_unique<ISInfoDictionary>( partition );

//  m_isObjName	= objName;
//  m_node	= attrName;
//  m_access	= srAccess::IS;
//  m_type	= type;

//  m_values.clear();

//}


//// ====================================================
//StatusRegisterISVector::~StatusRegisterISVector()
//{
//  m_values.clear();
//}

//// ====================================================
//void
//StatusRegisterISVector::readout()
//{
//  ISInfoDynAny isda;
//  try {
//    isd->getValue( m_isObjName.c_str(), isda );
//    m_values = isda.getAttributeValue<vector<unsigned int>>( m_node );

//    char* time_cstr = isda.time().c_str();
//    m_time   = time_cstr;
//    delete[] time_cstr;
//  } catch (daq::is::Exception ex) {
//    m_values.clear();
//    m_values.push_back(srNoneValue);
//    daq::ftk::ftkException ex2(ERS_HERE, "StatusRegisterISVector", "Failed to read out a status register vector from IS: check if object "+
//	m_isObjName+" with attribute "+m_node+" exists!",ex);
//    ers::warning(ex2);
//  }
//}
