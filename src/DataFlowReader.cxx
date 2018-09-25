//#include "ftkcommon/DataFlowReader.h"
//#include "ftkcommon/ReadSRFromISVectors.h"

////OnlineServices to check if running within a partition
//#include <RunControl/Common/OnlineServices.h>
//#include <RunControl/Common/Exceptions.h>
//#include <ipc/partition.h>
//#include <ipc/core.h>
//#include <memory>

//// ers
//#include <ers/ers.h>
//#include "ftkcommon/exceptions.h"

//// is

//#include <is/infoT.h>
//#include "ftkcommon/dal/FtkDataFlowSummaryNamed.h"

//using namespace std;
//using namespace daq::ftk;

///************************************************************/
//DataFlowReader::DataFlowReader(std::shared_ptr<OHRawProvider<>> ohRawProvider,  bool forceIS)
///************************************************************/
//{
//  m_useIS = false;
//  // used inside RC, but forced to use IS as a source
//  if(forceIS)
//    m_useIS = true;

//  m_ohRawProvider = ohRawProvider;

//  }

///************************************************************/
//void DataFlowReader::init(const string& deviceName, const string& partitionName, const string& isServerName)
///************************************************************/
//{
// try {
//    m_ipcp = daq::rc::OnlineServices::instance().getIPCPartition();
//  } catch (daq::rc::ConfigurationIssue &ex) {
//    // used outside of RC. Need to get info from IS as a source
//    m_useIS = true;
//  } catch (daq::rc::NotInitialized &ex) {
//    // used outside of RC. Need to get info from IS as a source
//    m_useIS = true;
//  }


//  // if needed, set up infrastructure for IS access
//  if( m_useIS )
//  {
//    ERS_LOG("Prepare to use IS as an input source.");
//    if(m_ipcp == NULL)
//    {
//      ERS_LOG("Need to create IPCPartition handle for "<<partitionName<<"...");
//      int argc = 0;
//      char** argv = nullptr;
//      try {
//        IPCCore::init(argc, argv);
//      } catch (daq::ipc::AlreadyInitialized &ex) {
//        ERS_LOG("IPCCore already initialised");
//      } catch (daq::ipc::CannotInitialize &ex) {
//        ERS_LOG("Can not initialize IPCCore...");
//      }
//      m_ipcp = IPCPartition(partitionName);
//      ERS_LOG("Done");
//    }

//    string isReadObjName = isServerName + ".FTK." + deviceName;
//    vector<string> isNames = getISVectorNames();
//    // reserve the vector space to fit the number of collections
//    m_data.reserve( isNames.size() );
//    m_data_info.reserve( isNames.size() );
//    m_is_v.reserve( isNames.size() );
//    m_is_info_v.reserve( isNames.size() );
//    //configure IS readers
//    for (string isAttrName : isNames )
//    {
//      ERS_LOG("About to start using IS object "<<isReadObjName<<" with attributes "<<isAttrName<<" and "<<isAttrName<<"_info");
//      m_is_v     .push_back( std::make_unique<StatusRegisterISVector>(m_ipcp, isReadObjName, isAttrName,           srType::srOther));
//      m_is_info_v.push_back( std::make_unique<StatusRegisterISVector>(m_ipcp, isReadObjName, isAttrName + "_info", srType::srOther));
//    }

//    //configure IS writer
//    isd		= std::make_unique<ISInfoDictionary>( m_ipcp );
//    m_isWriteObjName = isServerName + ".FTK." + m_ohRawProvider->getName();// + "_common"; not needed, as it comes from the OHRawProvider anme already
//    m_theSummary = std::make_shared<FtkDataFlowSummaryNamed>(m_ipcp,name_dataflow());
//    ERS_LOG("Initialized the pointer"<<name_dataflow()<<" for dataflow");

//  }
//  else
//  {
//    ERS_LOG("Will get vectors directly.");
//  }
//}

///************************************************************/
//DataFlowReader::~DataFlowReader()
///************************************************************/
//{
//  m_data.clear();
//  m_data.shrink_to_fit();
//  m_data_info.clear();
//  m_data_info.shrink_to_fit();

//  m_is_v.clear();
//  m_is_v.shrink_to_fit();
//  m_is_info_v.clear();
//  m_is_info_v.shrink_to_fit();
//}

///************************************************************/
//void DataFlowReader::readIS(uint32_t verb)
///************************************************************/
//{
//  if( m_useIS )
//  {
//    m_data.clear();
//    m_data_info.clear();
//    for( auto& srv : m_is_v )
//    {
//      srv->readout();
//      m_data.push_back(srv->getValues());
//      if(verb == 1)
//      {
//        ERS_LOG("Update time of object " << srv->getObjName() << " attribute " << srv->getAttrName() << " is : " << srv->getTime());
//      }

//    }
//    for( auto& srv : m_is_info_v )
//    {
//      srv->readout();
//      m_data_info.push_back(srv->getValues());
//    }
//  }
//  else
//  {
//    ERS_LOG("Nothing to be done- no IS usage initiated");
//  }
//}

///************************************************************/
//void DataFlowReader::setDataAndInfo(const std::vector< std::vector<uint32_t> >& data, const std::vector< std::vector<uint32_t> >& data_info)
///************************************************************/
//{
//    m_data.clear();
//    m_data_info.clear();
//    m_data.reserve( data.size() );
//    m_data_info.reserve( data_info.size() );
//    m_data = data;
//    m_data_info = data_info;
//}

///************************************************************/
//void DataFlowReader::setDataAndInfo(const std::vector< std::vector<uint32_t>* >& data, const std::vector< std::vector<uint32_t>* >& data_info)
///************************************************************/
//{
//    m_data.clear();
//    m_data_info.clear();
//    m_data.reserve( data.size() );
//    m_data_info.reserve( data_info.size() );
//    for( auto srv : data ) {
//      m_data.push_back(*srv);
//    }
//    for( auto srv : data_info ) {
//      m_data_info.push_back(*srv);
//    }
//}


///************************************************************/
//uint32_t DataFlowReader::readRegister(uint32_t reg, uint32_t fpga, uint32_t sel, uint32_t read)
///************************************************************/
//{
//  return ReadSRFromISVectors(reg, m_data, m_data_info, fpga, sel, read);
//}


///************************************************************/
//void DataFlowReader::publishAllHistos(uint32_t option)
///************************************************************/
//{
 
//  std::vector<int64_t> srvi;
 
//  srvi.clear();
//  getFifoInBusy(srvi);
//  publishSingleTH1F(srvi,"FifoInBusy");
//  m_theSummary->FifoInBusy = srvi;

//  srvi.clear();
//  getFifoOutBusy(srvi);
//  publishSingleTH1F(srvi,"FifoOutBusy");
//  m_theSummary->FifoOutBusy = srvi;

//  srvi.clear();
//  getFifoInEmpty(srvi);
//  publishSingleTH1F(srvi,"FifoInEmpty");
//  m_theSummary->FifoInEmpty = srvi;

//  srvi.clear();
//  getFifoOutEmpty(srvi);
//  publishSingleTH1F(srvi,"FifoOutEmpty");
//  m_theSummary->FifoOutEmpty = srvi;

//  std::vector<float> srvf;
//  srvf.clear();
//  getBusyFraction(srvf, option);
//  publishSingleTH1F(srvf,"BusyFraction");
//  m_theSummary->BusyFraction = srvf;

//  srvf.clear();
//  getFifoInBusyFraction(srvf, option);
//  publishSingleTH1F(srvf,"FifoInBusyFraction");
//  m_theSummary->FifoInBusyFraction = srvf;

//  srvf.clear();
//  getFifoOutBusyFraction(srvf, option);
//  publishSingleTH1F(srvf,"FifoOutBusyFraction");
//  m_theSummary->FifoOutBusyFraction = srvf;

//  srvf.clear();
//  getFifoInEmptyFraction(srvf, option);
//  publishSingleTH1F(srvf,"FifoInEmptyFraction");
//  m_theSummary->FifoInEmptyFraction = srvf;

//  srvf.clear();
//  getFifoOutEmptyFraction(srvf, option);
//  publishSingleTH1F(srvf,"FifoOutEmptyFraction");
//  m_theSummary->FifoOutEmptyFraction = srvf;

//  srvi.clear();
//  getL1id(srvi);
//  publishSingleTH1F(srvi,"L1id");
//  m_theSummary->L1id = srvi;

//  srvi.clear();
//  getDataErrors(srvi);
//  publishSingleTH1F(srvi,"DataErrors");
//  m_theSummary->DataErrors = srvi;

//  srvi.clear();
//  getEventRate(srvi);
//  publishSingleTH1F(srvi,"EventRate");
//  m_theSummary->EventRate = srvi;

//  srvi.clear();
//  getLinkInStatus(srvi);
//  publishSingleTH1F(srvi,"LinkInStatus");
//  m_theSummary->LinkInStatus = srvi;

//  srvi.clear();
//  getLinkOutStatus(srvi);
//  publishSingleTH1F(srvi,"LinkOutStatus");
// m_theSummary->LinkOutStatus = srvi;

//  srvi.clear();
//  getFpgaTemperature(srvi);
//  publishSingleTH1F(srvi,"FpgaTemperature");
//  m_theSummary->FpgaTemperature = srvi;

//  srvi.clear();
//  getNEventsProcessed(srvi);
//  publishSingleTH1F(srvi,"NEventsProcessed");
//  m_theSummary->NEventsProcessed = srvi;
 
//  srvi.clear();
//  getNPacketsDiscarded(srvi);
//  publishSingleTH1F(srvi,"NpacketsDiscarded");
//  m_theSummary->NpacketsDiscarded = srvi;
  
//  srvi.clear();
//  getNPacketsHeldSync(srvi);
//  publishSingleTH1F(srvi,"NpacketsHeldSync");
//  m_theSummary->NpacketsHeldSync = srvi;

//  srvi.clear();
//  getNPacketsDiscardedSync(srvi);
//  publishSingleTH1F(srvi,"NpacketsDiscardedSync");
//  m_theSummary->NpacketsDiscardedSync = srvi;

//  srvi.clear();
//  getNPacketsTimeoutSync(srvi);
//  publishSingleTH1F(srvi,"NpacketsTimeoutSync");
//  m_theSummary->NpacketsTimeoutSync = srvi;

//  srvi.clear();
//  getNPacketsTimedout(srvi);
//  publishSingleTH1F(srvi,"NPacketsTimedout");
//  m_theSummary->NpacketsTimedout = srvi;

//  srvi.clear();
//  getInputSkewSync(srvi);
//  publishSingleTH1F(srvi,"InputSkewSync");
//  m_theSummary->InputSkewSync = srvi;

//  srvi.clear();
//  getPacketCompletionTime(srvi);
//  publishSingleTH1F(srvi,"PacketCompletionTime");
//  m_theSummary->PacketCompletionTime = srvi;

//  srvi.clear();
//  getMaxSkewTime(srvi);
//  publishSingleTH1F(srvi,"MaxSkewTime");
//  m_theSummary->MaxSkewTime = srvi;

//  publishExtraHistos(option);

//  m_theSummary->checkin();
//  ERS_LOG("checkin");
//}


////#################################################################
////  Dummy definitions that have to be defined in derived classes
////#################################################################
///************************************************************/
//void DataFlowReader::getBusyFraction(vector<float>& srv, uint32_t /*option*/)
///************************************************************/
//{
//  srv.clear();
//  srv.push_back(-1);
//}

///************************************************************/
//void DataFlowReader::getFifoInBusyFraction(vector<float>& srv, uint32_t /*option*/)
///************************************************************/
//{
//  srv.clear();
//  srv.push_back(-1);
 
//}

///************************************************************/
//void DataFlowReader::getFifoOutBusyFraction(vector<float>& srv, uint32_t /*option*/)
///************************************************************/
//{
//  srv.clear();
//  srv.push_back(-1);
  
//}

///************************************************************/
//void DataFlowReader::getFifoInEmptyFraction(vector<float>& srv, uint32_t /*option*/)
///************************************************************/
//{
//  srv.clear();
//  srv.push_back(-1);
  
//}

///************************************************************/
//void DataFlowReader::getFifoOutEmptyFraction(vector<float>& srv, uint32_t /*option*/)
///************************************************************/
//{
//  srv.clear();
//  srv.push_back(-1);
//}

///************************************************************/
//void DataFlowReader::getFifoInBusy(vector<int64_t>& srv)
///************************************************************/
//{
//  srv.clear();
//  srv.push_back(-1);
//}

///************************************************************/
//void DataFlowReader::getFifoOutBusy(vector<int64_t>& srv)
///************************************************************/
//{
//  srv.clear();
//  srv.push_back(-1);
//}

///************************************************************/
//void DataFlowReader::getFifoInEmpty(vector<int64_t>& srv)
///************************************************************/
//{
//  srv.clear();
//  srv.push_back(-1);
//}

///************************************************************/
//void DataFlowReader::getFifoOutEmpty(vector<int64_t>& srv)
///************************************************************/
//{
//  srv.clear();
//  srv.push_back(-1);
//}

///************************************************************/
//void DataFlowReader::getL1id(vector<int64_t>& srv)
///************************************************************/
//{
//  srv.clear();
//  srv.push_back(-1);
//}

///************************************************************/
//void DataFlowReader::getDataErrors(vector<int64_t>& srv)
///************************************************************/
//{
//  srv.clear();
//  srv.push_back(-1);
//}

///************************************************************/
//void DataFlowReader::getEventRate(vector<int64_t>& srv)
///************************************************************/
//{
//  srv.clear();
//  srv.push_back(-1);
// }

///************************************************************/
//void DataFlowReader::getLinkInStatus(vector<int64_t>& srv)
///************************************************************/
//{
//  srv.clear();
//  srv.push_back(-1);
//}

///************************************************************/
//void DataFlowReader::getLinkOutStatus(vector<int64_t>& srv)
///************************************************************/
//{
//  srv.clear();
//  srv.push_back(-1);
//}

///************************************************************/
//void DataFlowReader::getFpgaTemperature(vector<int64_t>& srv)
///************************************************************/
//{
//  srv.clear();
//  srv.push_back(-1);
//}

///************************************************************/
//void DataFlowReader::getNEventsProcessed(vector<int64_t>& srv)
///************************************************************/
//{
//  srv.clear();
//  srv.push_back(-1);
//}

///***************************************************************/
//void DataFlowReader::getNPacketsDiscarded(vector<int64_t>& srv)
///***************************************************************/
//{
//  srv.clear();
//  srv.push_back(-1);
//}

///***************************************************************/
//void DataFlowReader::getNPacketsHeldSync(vector<int64_t>& srv)
///***************************************************************/
//{
//  srv.clear();
//  srv.push_back(-1);
//}
///***************************************************************/
//void DataFlowReader::getNPacketsDiscardedSync(vector<int64_t>& srv)
///***************************************************************/
//{
//  srv.clear();
//  srv.push_back(-1);
//}

///***************************************************************/
//void DataFlowReader::getNPacketsTimedout(vector<int64_t>& srv)
///***************************************************************/
//{
//  srv.clear();
//  srv.push_back(-1);
//}

///***************************************************************/
//void DataFlowReader::getNPacketsTimeoutSync(vector<int64_t>& srv)
///***************************************************************/
//{
//  srv.clear();
//  srv.push_back(-1);
//}

///***************************************************************/
//void DataFlowReader::getInputSkewSync(vector<int64_t>& srv)
///***************************************************************/
//{
//  srv.clear();
//  srv.push_back(-1);
//}

///***************************************************************/
//void DataFlowReader::getPacketCompletionTime(vector<int64_t>& srv)
///***************************************************************/
//{
//  srv.clear();
//  srv.push_back(-1);
//}

///***************************************************************/
//void DataFlowReader::getMaxSkewTime(vector<int64_t>& srv)
///***************************************************************/
//{
//  srv.clear();
//  srv.push_back(-1);
//}
