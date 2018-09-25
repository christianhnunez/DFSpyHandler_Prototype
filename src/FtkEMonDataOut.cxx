////*****************************************************************************/
//// file: FtkEMonDataOut.cpp
//// desc: Library to implement an interface to emon for FTK
//// auth: 25/11/2015 M. Facchini
////*****************************************************************************/

//#include <iostream>
//#include <sys/types.h>
//#include <sstream>
//#include <vector>
//#include "DFSubSystemItem/Config.h"
//#include "DFDebug/DFDebug.h"
//#include "ROSBufferManagement/Buffer.h"
//#include "rcc_time_stamp/tstamp.h"
//#include "ftkcommon/FtkEMonDataOut.h"
//#include "eformat/write/eformat.h" // Including the headers to create eformat fragments
//#include "ers/ers.h" // Including the headers to send exceptions
//#include "emon/exceptions.h" // Including the headers to send exceptions
//#include "emon/Util.h" // Including the headers to send exceptions
//#include <map> // Including the headers to work with the buffer
//#include <memory> // Including the headers to work with the buffer
//#include <utility> // Including the header to create eformat fragments
//#include <sys/time.h>
//#include <is/infodictionary.h> // Including the headers to find the runnumber and lumiblock
//#include "rc/RunParams.h" // Including the headers to find the runnumber and lumiblock
//#include "TTCInfo/LumiBlock.h" // Including the headers to find the runnumber and lumiblock
//#include <functional> // Including the headers to construct the global id
//#include <unistd.h> // Including the header to see the thread id
//#include <sys/syscall.h>// Including the header to see the thread id
//#include <sys/types.h> // Including the header to see the thread id
//#include <signal.h> // Including the header to see the thread id
//#include "ftkcommon/SourceIDSpyBuffer.h" // Including the header to work on the SourceID
//#include <is/exceptions.h> // IS exceptions header
//#include <ftkcommon/exceptions.h> //FTK exceptions header
//#include <eformat/SourceIdentifier.h> //Header for the SubDetector ID
//#include "config/Configuration.h"
//#include <sys/syscall.h> //for the printing of the Thread number
//#include "DFSubSystemItem/ConfigException.h"
//#include "RunControl/Common/OnlineServices.h"
//#include "DFdal/ReadoutConfiguration.h"
//#include "DFdal/RCD.h"
//#include "dal/Partition.h"
//#include "dal/util.h"

//namespace daq {
//namespace ftk {

///// Constructor of the FtkEMonDataOut class
//FtkEMonDataOut::FtkEMonDataOut(const Type myType) : ROS::DataOut(myType)
//{
//  ERS_LOG( "Constructor entered.");
//  m_type=myType;

//  m_scalingFactor=1;

//  // reset statistics
//  m_stats.fragmentsOutput = 0;
//  m_stats.dataOutput = 0;
//  m_sendCalled = 0;

//  // construction of emon objects
//  m_eventSampler = 0;
//  m_emonfactory = 0;
//  m_address = 0;

//  // TODO: make it configurable? 100 to start
//  m_maxchannels = 100;

//  ERS_LOG( "Constructor completed.");

//}


///// Destructor of FtkEMonDataOut class
//FtkEMonDataOut::~FtkEMonDataOut() noexcept
//{
//  ERS_LOG( "Destructor entered.");
   
//  if (m_eventSampler!=0)
//  {
//    delete m_eventSampler;
//  }

//  delete m_address;

//  ERS_LOG( "Destructor completed.");
//}



///// setup method of FtkEMonDataOut class
//void FtkEMonDataOut::setup(DFCountedPointer<ROS::Config> configuration)
//{
//  ERS_LOG("Entered");

//  //Reading the number of active ReadoutModules from the Partition schema
//  auto dal_rcdAppl    = daq::rc::OnlineServices::instance().getApplication().cast<daq::df::RCD>();
//  m_numberOfRMs=0;
  
//  ::Configuration *m_confDB = new Configuration("");
//  std::string m_partition_name = getenv("TDAQ_PARTITION");
//  const daq::core::Partition *m_dbPartition = daq::core::get_partition(*m_confDB, m_partition_name);
//  const std::vector<const daq::core::ResourceBase*> rm_num = dal_rcdAppl->get_Contains();
//  for (auto i : rm_num)
//    if( !(i && i->disabled(*m_dbPartition)) ) {
//      m_numberOfRMs++;
//    }
//  ERS_LOG( "Number of active ReadoutModules = " << m_numberOfRMs);

//  bool interactive=configuration->getBool("interactive");
//  if (!interactive)
//  {
//    // not modified from the original FtkEMonDataOut.cpp
//    std::string partitionName=configuration->getString("partition");
//    IPCPartition partition(partitionName);

//    ISInfoDictionary infoDict(partition);
//    m_infoDict = infoDict;
   
//    m_runnumber = 0;
//    m_lumiblocknumber = 0;

//    std::string key;
//    std::string value;
//    configuration->dump();
//    if (configuration->isKey("key") && configuration->isKey("value"))
//    {
//      ERS_LOG("Reading the address");
//      key=configuration->getString("key");
//      value=configuration->getString("value");
  
//      //checking if the "value" is empty; in that case the value is set as the name of the RCD.
//      if(value.empty())
//      {
//        value=configuration->getString("appName");
//      }

//      //checking if the "key" is empty; in that case the key is set as the name of the RCD.
//      if(key.empty())
//      {
//        key="ReadoutApplication";
//      }
//    }
//    else
//    {
//      ERS_LOG("Using a standard address");
//      key="ReadoutApplication";
//      value=configuration->getString("appName");
//    }

//    ERS_LOG("EmonDataOut:setup setting address to key="
//                 << key << "  value=" << value);

//    m_address = new emon::SamplingAddress(key, value);

//    //hashing of the sampling address
//    std::hash<std::string> str_hash;
//    m_sampaddrhash = str_hash(value);

      
//    m_emonfactory = new FTKPullSamplingFactory(this);
    
    
//    m_eventSampler = new emon::EventSampler(partition,
//                                            *m_address,
//                                            m_emonfactory ,
//                                            m_maxchannels);

//  }

//  ERS_LOG( "Completed");
//}

///// ROS::Buffer has been deprecated. Use the next one
//  void FtkEMonDataOut::sendData(const ROS::Buffer* buffer ,  NodeID, u_int , unsigned int )
//{
//  buffer++; // To silent the compiler
//  ftk::ftkException e(ERS_HERE, "FTKEMonDataOut","This method is deprecated. Check the documentation.");
//  ers::warning(e);
//  //throw e;
//}

///// sendData method of FtkEMonDataOut class
//void FtkEMonDataOut::sendData( std::vector< std::pair< SpyBufShPtr, uint32_t > > v_spybuffer )
//{
//  // ERS_LOG("FtkEMonDataOut::sendData, send_called = " << m_sendCalled);
//  if (( m_scalingFactor > 0 && (m_sendCalled%m_scalingFactor) == 0) || (m_type == DEBUGGING))
//  {
//    for(uint j = 0; j < v_spybuffer.size(); j++)
//    {
//      m_sendCalled++;
//      m_dataStorage.AddSpyBuff(v_spybuffer[j].first , v_spybuffer[j].second);

//      //statistics
//      uint N_bytes_buffer = v_spybuffer[j].first->getBuffer().size() * 4;
//      m_stats.dataOutput += (float)N_bytes_buffer/(float)(1024*1024);
//      m_stats.fragmentsOutput++;
//    }
//  }
//}



///// GetRunNumber() method of FtkEMonDataOut class
//uint32_t FtkEMonDataOut::GetRunNumber()
//{
//  // taking the runnumber
//  if( m_runnumber != 0 )
//  {
//    return m_runnumber;
//  }

//  static bool warning_sent = false;
//  RunParams rpi;

//  try
//  {
//    m_infoDict.findValue("RunParams.RunParams", rpi);
//    m_runnumber = rpi.run_number;
//  }
//  catch(daq::is::Exception& e)
//  {
//	  ftk::FTKInfoNotAcquired f(ERS_HERE, "Run Number", e);
//    if(!warning_sent)
//    {
//	    ers::warning(f);
//      warning_sent = true;
//    }
//    else
//      ers::log(f);
//    m_runnumber = 0;
//  }
//  return m_runnumber;
//}


///// GetAddress method of FtkEMonDataOut class
//const emon::SamplingAddress* FtkEMonDataOut::GetAddress()
//{
//  return m_address;
//}

///// GetMapAddr method of FtkEMonDataOut class
//FtkEMonDataOut::TSafeMap* FtkEMonDataOut::GetMapAddr()
//{
//  TSafeMap * buf_temp = &m_dataStorage;
//  return buf_temp;
//}

///// GetSamplingHash method of FtkEMonDataOut class
//uint32_t FtkEMonDataOut::GetSamplingHash()
//{
//  return m_sampaddrhash;
//}

///// GetLumiBlock method of FtkEMonDataOut class
//uint32_t FtkEMonDataOut::GetLumiBlock()
//{
//  static bool warning_sent = false;

//  LumiBlock lb;
//  try
//  {
//    m_infoDict.findValue("RunParams.LumiBlock",lb);
//    m_lumiblocknumber = lb.LumiBlockNumber;
//  }
//  catch(daq::is::Exception& e)
//  {
//	  ftk::FTKInfoNotAcquired f(ERS_HERE, "LumiBlock Number", e);
//    if(!warning_sent)
//    {
//	    ers::warning(f);
//      warning_sent = true;
//	  }
//    else
//      ers::log(f);
//    m_lumiblocknumber = 0;
//  }
//  return m_lumiblocknumber;
//}

///// Constructor of FTKPullSamplingFactory class
///// It allocates the buffer and the address of the provider of the fragments
//FtkEMonDataOut::FTKPullSamplingFactory::FTKPullSamplingFactory(FtkEMonDataOut * ftkemondataout)
//{
//  //ERS_LOG("Thread ID of FTKPullSamplingFactory:" << syscall(SYS_gettid));
//  ERS_LOG("FTKPullSamplingFactory constructor entered.");
  
//  m_tidMutex = DFFastMutex::Create();

//  m_ftkemondataout = ftkemondataout;

//  m_fulfilledRequests = 0;
//  m_lastCall = 0;

//  ERS_LOG("FTKPullSamplingFactory constructor complete.");
//}



///// Destructor of FtkEMonDataOut::FTKPullSamplingFactory class
//FtkEMonDataOut::FTKPullSamplingFactory::~FTKPullSamplingFactory()
//{
//  m_tidMutex->destroy();
//  ERS_LOG("FTKPullSamplingFactory destructor entered.");
//  ERS_LOG("FTKPullSamplingFactory destructor complete.");
//}


///* startSampling method of FtkEMonDataOut::FTKPullSamplingFactory class
// * When a monitoring application requests a fragment from a provider,
// * this method checks the selection criteria and starts the sampling thread
// * if they are correct
// */
//emon::PullSampling* FtkEMonDataOut::FTKPullSamplingFactory::startSampling(const emon::SelectionCriteria & sc) throw (emon::BadCriteria, emon::NoResources)
//{

//  SourceIDSpyBuffer decodedSourceID;
//  if(!sc.m_status_word.m_ignore)
//  {
//    decodedSourceID = decode_SourceIDSpyBuffer(sc.m_status_word.m_value);
//  }
//  else
//  {
//    decodedSourceID = decode_SourceIDSpyBuffer(0x007f8000);
//  }
  
//  // ERS_LOG if the Lvl1 Trigger Type is requested
//  // TODO: ADD CHECK FOR LVL1 TRIGGER INFO AND STREAM TAGS
//  if(!sc.m_lvl1_trigger_type.m_ignore)
//  {
//    ERS_LOG("FTK SpyBuffer sampling: Level 1 Trigger Type not accepted as selection criteria");
//  }
//  //Checking if specific elements of the requested source ID are correct
//  if(decodedSourceID.subDetector != eformat::TDAQ_FTK || // eformat::TDAQ_FTK = 0x7f
//     decodedSourceID.dataType != SPYBUFFER) // SPYBUFFER = 1
//  {
//    ERS_LOG("FTK SpyBuffer sampling: wrong selection criteria.");
//    throw emon::BadCriteria(ERS_HERE, construct_name(*m_ftkemondataout->GetAddress()), construct_name(sc));
//  }
//  //Starting the sampling
//  else
//  {
//    const emon::SelectionCriteria s_crit = sc;
//    m_fulfilledRequests++;
//    return new FTKPullSampling(*this, s_crit);
//  }
//}


/////sendEvent method of FtkEMonDataOut class.
/////This is the object that actually sends the FullFragment through the channel
//bool FtkEMonDataOut::FTKPullSamplingFactory::sendEvent(emon::EventChannel& channel, const emon::SelectionCriteria sc)
//{
//  ERS_LOG("Entering function sendEvent, check if the buffer is empty or not. \nIf no more logs it means it is empty" );

//  // If the buffer is empty, then wait (see sampleEvent)
//  if(m_ftkemondataout->GetMapAddr()->Empty())
//    return false;

//  // Tid print
//  pid_t tid = syscall(SYS_gettid);

//  // Check for the correct number of Call for sendData
//  int sendCalled = m_ftkemondataout->m_sendCalled;
//  int numberOfRMs = m_ftkemondataout->m_numberOfRMs;
//  int mod = sendCalled%numberOfRMs;
  
//  //check that every ReadOut module has been read
//  if( mod != 0 )
//    return false;

//  if( !checkSpybuffer( sendCalled, tid))
//    return false;
 
//  ERS_LOG("Proceding with sendEvent for the call\t" << sendCalled );
  
  
//  //Get time of day and using it misusing the bunch crossing time
//  timeval now;
//  gettimeofday(&now , NULL);
//  uint32_t bc_time_secs = now.tv_sec;
//  uint32_t bc_time_nsec = now.tv_usec * 1000;

//  // To construct the global ID, we use the following algorithm: the lower section is
//  // m_fulfilledRequests, while the upper section is the hash of the sampling address
//  uint64_t global_id = m_ftkemondataout->GetSamplingHash();
//  global_id <<= 32;
//  global_id |= m_fulfilledRequests;

//  uint32_t runtype = 1; // Legend:
//                        // 0x00000000 = PYSICS
//                        // 0x00000001 = CALIBRATION
//                        // 0x00000002 = COSMICS
//                        // 0x0000000f = TEST
//                        // 0x80000000 = SIMULATION

//  uint16_t lumiblock = m_ftkemondataout->GetLumiBlock();
//  uint16_t bc_id = 0; // not applicable, set to zero
  
//  uint32_t rob_source_id = 0x007f8000; // SubDetector ID = FTK and DataType = true
//  uint32_t rod_run_no = m_ftkemondataout->GetRunNumber();
//  uint32_t rod_lvl1_id = 0; // not applicable, set to zero
//  uint32_t rod_lvl1_type = 0; // not applicable, set to zero


//  eformat::write::FullEventFragment full_frag(rob_source_id,
//                                              bc_time_secs,
//                                              bc_time_nsec,
//                                              global_id,
//                                              runtype,
//                                              rod_run_no,
//                                              lumiblock,
//                                              rod_lvl1_id,
//                                              bc_id,
//                                              rod_lvl1_type);

//  // vector of pointers to ROBFragments. Useful to save the addresses of the
//  // fragments to delete them at the end (ROBFragment destructor is base destructor)
//  std::vector< eformat::write::ROBFragment * > rob_fragments;
 
//  //vector of spybuffers consistent with the requested sourceID
//  std::vector< std::pair< uint32_t, SpyBufShPtr > > consistent_SpyBuff = m_ftkemondataout->GetMapAddr()->GetSpyBuff(sc);
  
//  if(!consistent_SpyBuff.size())
//    return false;

//  for (auto it = consistent_SpyBuff.begin(); it != consistent_SpyBuff.end(); it++)
//  {
//    uint32_t DetEvType = 0; // not applicable, set to zero.  It could be
//                            // used  to classify the SpyBuffers if bits
//                            // in Source ID are not enough.
//    eformat::write::ROBFragment* rb_temp =
//      new eformat::write::ROBFragment(it->first,
//                                      rod_run_no,
//                                      rod_lvl1_type,
//                                      bc_id,
//                                      rod_lvl1_type,
//                                      DetEvType,
//                                      it->second->getBuffer().size(),
//                                      it->second->getBuffer().data(),
//                                      1);

//    rob_fragments.push_back(rb_temp);
//    full_frag.append(rb_temp);
//  }
//  ERS_LOG("FTK SpyBuffer sampling debug: starting the serialization");

//  // Now, the object that can be sent through the channel is a serialized array.
//  // This array is represented with a pointer of type uint32_t* and its dimension.
//  uint32_t * frag_array = new uint32_t[full_frag.size_word()];

//  eformat::write::copy(*full_frag.bind(), frag_array, full_frag.size_word());

//  ERS_LOG("FTK SpyBuffer sampling debug: end of the serialization. Sending the fragment");

//  // pushing the event in the channel
//  channel.pushEvent(frag_array , full_frag.size_word());
 
//  // deleting the remaining memory. We can forget about the shared pointers
//  for(uint n = 0; n < rob_fragments.size(); n++)
//    delete rob_fragments[n];

//  delete frag_array;

//  return true;
//}


/////check Thread number method of FtkEMonDataOut::FTKPullSampling class
/////This method check if a Spybuffer has already been shipped or not
//bool FtkEMonDataOut::FTKPullSamplingFactory::checkSpybuffer(int sendCall, pid_t ttid)
//{
//  m_tidMutex->lock();

//  //ERS_LOG("Entreing the function checkSpybuffer. \n sendCall= " << sendCall << "\t TID= "<< ttid);
  
//  //tidbool: true if the tid is present in the list (old tid)
//  bool callBool = m_lastCall == sendCall;

//  //micount count the times the tid is present in the tid vector
//  int mycount = std::count (m_tid.begin(), m_tid.end(), ttid);
//  //ERS_LOG("\nThe Tid " << ttid << "is counted " << mycount << " times \n");


//    // if the tid is present more than 3 times and the call is old, skip the data sending
//  if (callBool and (mycount >= 2))
//  {
//    m_tidMutex->unlock();
//    ERS_LOG("Event already sent, waiting for the next ");
//    return false;
//  }

//  if(mycount <= 2)
//    m_tid.push_back(ttid);

//    //if the call is new, set the lastcall to the new one
//  if (!callBool)
//    m_lastCall = sendCall;

//  ERS_LOG("The Spybuffer can be sent ");
//  m_tidMutex->unlock();

//  return true;
//}


///// Constructor of the FtkEMonDataOut::FTKPullSampling class
///// These classes are in one to one correspondance with the sampling threads
//FtkEMonDataOut::FTKPullSampling::FTKPullSampling(FTKPullSamplingFactory & factory, const emon::SelectionCriteria sc): m_factory(factory), m_scrit(sc)
//{
//  // ERS_LOG("Thread ID of FTKPullSampling:" << syscall(SYS_gettid));
//  ERS_LOG("FTKPullSampling constructor: starting the sampling.");
//}

///// Destructor of the FtkEMonDataOut::FTKPullSampling class
//FtkEMonDataOut::FTKPullSampling::~FTKPullSampling()
//{
//  ERS_LOG("FTKPullSampling destructor: stopping the sampling.");
//}


///// sampleEvent method of FtkEMonDataOut::FTKPullSampling class
///// This method executes FTKPullSamplingFactory::sendEvent which actually sends the fragment
//void FtkEMonDataOut::FTKPullSampling::sampleEvent(emon::EventChannel &channel)
//{
//  m_factory.sendEvent(channel, m_scrit);
//  ERS_LOG("FTK SpyBuffer sampling: sending request done.");
//}


///// Constructor of FtkEMonDataOut::TSafeMap class
//FtkEMonDataOut::TSafeMap::TSafeMap()
//{
//  ERS_LOG("TSafeMap constructor entered.");
//  m_accessMutex = DFFastMutex::Create();
//  ERS_LOG("TSafeMap constructor complete.");
//}


///// Destructor of FtkEMonDataOut::TSafeMap class
//FtkEMonDataOut::TSafeMap::~TSafeMap()
//{
//  ERS_LOG("TSafeMap destructor entered.");
//  m_accessMutex->destroy();
//  ERS_LOG("TSafeMap destructor complete.");
//}


///// AddSpyBuff method of FtkEMonDataOut::TSafeMap class
//void FtkEMonDataOut::TSafeMap::AddSpyBuff(SpyBufShPtr spy_frag , uint32_t source_id )
//{
//  m_accessMutex->lock();
//    //If there's already a fragment with the given source id, then delete it before overwriting
//    if(m_data.count(source_id))
//    {
//      m_data[source_id].reset();
//      // shared_ptr::reset() doesn't throw exceptions because it's defined with noexcept
//    }
//    m_data[source_id] = spy_frag;

//  m_accessMutex->unlock();
  
//}

///// AddSpyBuff method of FtkEMonDataOut::TSafeMap class
//std::vector< std::pair< uint32_t, FtkEMonDataOut::SpyBufShPtr> > FtkEMonDataOut::TSafeMap::GetSpyBuff(const emon::SelectionCriteria sc)
//{
//  // Decoding the source ID requested from the client
//  SourceIDSpyBuffer decodedSourceID;
//  if(!sc.m_status_word.m_ignore)
//  {
//    decodedSourceID = decode_SourceIDSpyBuffer(sc.m_status_word.m_value);
//  }
//  else
//  {
//    decodedSourceID = decode_SourceIDSpyBuffer(0x007f8000);
//  }
 
//  // creating the vector of the pairs to be returned
//  std::vector< std::pair< uint32_t, SpyBufShPtr > > consistentSpyBuffers;

//  // all fragments requested
//  if(decodedSourceID.boardType == FTK || decodedSourceID.boardType > Flic)
//  {
//    ERS_LOG("FTK SpyBuffer sampling debug: all fragments requested");
//    for(auto it = m_data.begin(); it != m_data.end(); it++)
//    {
//      m_accessMutex->lock();
//        consistentSpyBuffers.push_back(*it); // a copy of the shared pointer is created here
//      m_accessMutex->unlock();
//    }
//  }
//  // Other three options
//  else
//  {
//    // first: request of  every fragment from a single board type
//    if(decodedSourceID.boardNumber == 0xff)
//    {
//      ERS_LOG("FTK SpyBuffer sampling debug: all fragments from a single board requested");
//      for(auto it = m_data.begin(); it != m_data.end(); it++)
//      {
//        SourceIDSpyBuffer SpyBuffSourceID = decode_SourceIDSpyBuffer(it->first);

//        if(SpyBuffSourceID.boardType == decodedSourceID.boardType)
//        {
//          m_accessMutex->lock();
//	          consistentSpyBuffers.push_back(*it); // a copy of the shared pointer is created here.
//          m_accessMutex->unlock();
//        }
//      }
//    }
//    else
//    {
//      //second: all the fragments from a defined board number
//      if(decodedSourceID.position == 0x3)
//      {
//        ERS_LOG("FTK SpyBuffer sampling debug: all fragments from a single board type and number requested");
//        for(auto it = m_data.begin(); it != m_data.end(); it++)
//        {
//          SourceIDSpyBuffer SpyBuffSourceID = decode_SourceIDSpyBuffer(it->first);

//          if(SpyBuffSourceID.boardNumber == decodedSourceID.boardNumber and SpyBuffSourceID.boardType == decodedSourceID.boardType)
//          {
//            m_accessMutex->lock();
//             consistentSpyBuffers.push_back(*it); // a copy of the shared pointer is created here.
//            m_accessMutex->unlock();
//          }
//        }
//      }
//      // third: just a fragment was requested.
//      else
//      {
//        ERS_LOG("FTK SpyBuffer sampling debug: single fragment requested");
//        m_accessMutex->lock();
//	        if(m_data.count(sc.m_status_word.m_value))
//          {
//            auto it = m_data.find(sc.m_status_word.m_value);
//	          consistentSpyBuffers.push_back(*it);
//          }
//        m_accessMutex->unlock();
//      }
//    }
//  }

//  return consistentSpyBuffers;
//}



/////Empty method of FtkEMonDataOut::TSafeMap class
//bool FtkEMonDataOut::TSafeMap::Empty()
//{
//  return m_data.empty();
//}


//ISInfo* FtkEMonDataOut::getISInfo()
//{
//   return &m_stats;
//}

//} // Namespace daq
//} // Namespace ftk

////------------------------------------------------------------------------------
//// shared library entry point

//extern "C"
//{
//  extern ROS::DataOut* createFTKEMonDataOut(const ROS::DataOut::Type* myType);
//}

//ROS::DataOut* createFTKEMonDataOut(const ROS::DataOut::Type* myType)
//{
//  return (new daq::ftk::FtkEMonDataOut(*myType));
//}
