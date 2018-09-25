//#include "ftkcommon/StatusRegister/StatusRegisterFactory.h"

//#include "ftkcommon/exceptions.h"

//using namespace std;

//namespace daq {
//namespace ftk {

//StatusRegisterFactory::StatusRegisterFactory(
//	std::string boardName,
//	std::string registersToRead,
//	std::string isServerName
//	) {
//	// Store passed variables
//	m_name = boardName;

//	// Determine which collections of registers to read
//	for(uint i = 0; i < registersToRead.length(); i++) {
//		// Copies the registersToRead string into the member variable m_registersToRead, ensuring all the characters are upper case.
//		m_inputString.push_back(toupper(registersToRead.at(i)));
//	}

//	// IS Publishing Setup
//	m_isServerName = isServerName;
//	m_isProvider   = m_isServerName + ".FTK." + m_name;
//	ERS_LOG("IS: publishing in " << m_isProvider);

//	//Set defaults
//	m_nread = 0;
//	m_defPrescale = 1;
//	m_useBlockTransfer = 0;
//	m_prescaleStr = "Prescale";
//}

//StatusRegisterFactory::~StatusRegisterFactory() {
//	// delete SRCollection Objects here
//	deleteSRCollectionObjects();

//}

//void StatusRegisterFactory::readout() {
//	std::unique_lock<std::mutex> lck (m_isReadingOut);

//	//keep track of the # of readout calls. This is safe against overflow for uint and return 0, when the max is reached
//	m_nread++;

//	// Calls readout() function of all collections
//	for (uint i = 0; i < m_collections.size(); i++) {
//	  // Check if the collection names require prescaling, and check if default prescale faactor is reached
//	  if( m_collections.at(i)->getName()     .find(m_prescaleStr) != std::string::npos ||
//	      m_collections.at(i)->getNameShort().find(m_prescaleStr) != std::string::npos ) {
//		if( m_nread % m_defPrescale != 1 ) {
//		  continue;
//		}
//	  }
//	  freeze(m_collections.at(i)->getAccess(), m_isObjectInfoVectors.at(i)->at(3),true);
//	  m_collections.at(i)->readout();
//	  freeze(m_collections.at(i)->getAccess(), m_isObjectInfoVectors.at(i)->at(3),false);
//	}

//	// Publish registers to IS
//	publishToIS();
//}

//void StatusRegisterFactory::clear() {
//	for (uint i = 0; i < m_isObjectVectors.size(); i++) {
//		m_isObjectVectors.at(i)->clear();
//	}
//	m_nread = 0;
//}

//void StatusRegisterFactory::setDefPrescale(uint p) {
//	m_defPrescale = p;
//	//propagate the prescale values to the meta-data vectors on IS
//	for (uint i = 0; i < m_collections.size(); i++) {
//	  // Check if the collection names require prescaling
//	  if( m_collections.at(i)->getName()     .find(m_prescaleStr) != std::string::npos ||
//	      m_collections.at(i)->getNameShort().find(m_prescaleStr) != std::string::npos ) {
//		m_isObjectInfoVectors.at(i)->push_back(m_defPrescale);
//	  } else {
//		// No prescale is needed
//		m_isObjectInfoVectors.at(i)->push_back(1);
//	  }
//	}
//}


////-------------------------------------
//// Protected Member Functions
////-------------------------------------
//void StatusRegisterFactory::setupCollectionInfo(
//		std::vector<uint>* ISObjectInfoVector,
//		uint firstAddress,
//		uint finalAddress,
//		uint addrIncrement,
//		uint fpgaNum,
//                uint selectorAddress,
//                uint readerAddress)
//{
//  ISObjectInfoVector->clear(); // should be empty, just to make sure
//  ISObjectInfoVector->push_back(firstAddress);
//  ISObjectInfoVector->push_back(finalAddress);
//  ISObjectInfoVector->push_back(addrIncrement);
//  ISObjectInfoVector->push_back(fpgaNum);
//  ISObjectInfoVector->push_back(selectorAddress);
//  ISObjectInfoVector->push_back(readerAddress);
//  m_isObjectInfoVectors.push_back(ISObjectInfoVector);
//}


//// Private Member Functions
//void StatusRegisterFactory::deleteSRCollectionObjects() {
//	//for (uint i = 0; i < m_collections.size(); i++) {
//	//	delete m_collections.at(i);
//	//}
//}

//void StatusRegisterFactory::publishToIS() {
//	// Loops through the collections, storing the read values in the corresponding ISObject vectors
//	for (uint i = 0; i < m_collections.size(); i++) {
//		*(m_isObjectVectors.at(i)) = m_collections.at(i)->getValues();
//	}

//	// Publish the values to IS
//	ERS_LOG("Publishing in IS");
//	try {
//		m_srxNamed->checkin();
//	}	catch ( daq::oh::Exception & ex) { // Raise a warning or throw an exception.
//		daq::ftk::ISException e(ERS_HERE, "", ex);  // Append the original exception (ex) to the new one
//		ers::warning(e);  //or throw e;
//	}
//	ERS_LOG("Publishing Finished");
//}

//} // namespace ftk
//} // namespace daq
