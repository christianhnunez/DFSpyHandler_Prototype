// -*- c++ -*-
#ifndef FTKEMONDATAOUT_H
#define FTKEMONDATAOUT_H

//*****************************************************************************/
// file: FtkEMonDataOut.h
// desc: Library to implement an interface to emon for FTK
// auth: 25/11/2015 M. Facchini
//*****************************************************************************/

#include <string>
#include <vector>
#include <map>
#include "ROSCore/DataOut.h"
#include "ROSInfo/DataOutInfo.h"
#include "DFDebug/DFDebug.h"
#include <emon/EventSampler.h>
#include <DFThreads/DFThread.h>
#include <DFThreads/DFFastMutex.h>
#include <utility> // Including the headers to work with the buffer 
#include <memory> // Including the headers to work with the buffer 
#include "eformat/write/eformat.h" // Including the header to create eformat fragments
#include <sys/time.h> // Including the header to insert the time
#include "ftkcommon/SpyBuffer.h"
#include <atomic> 
#include <mutex>

namespace daq {
  
  namespace ftk {

    /* 
     * \brief FtkEMonDataOut class for FTK.
     * It provides an interface with the emon infrastructure allowing 
     * monitoring tasks to require data fragments from the boards using
     * the SourceID as selection criteria. 
     * \note Since the SourceID is not an emon criterium, 
     *       the StatusWord field is used for selection
     */
    class FtkEMonDataOut : public ROS::DataOut
    {

      public:

        typedef std::shared_ptr< daq::ftk::SpyBuffer > SpyBufShPtr;

        FtkEMonDataOut(const Type myType=SAMPLED);
        virtual ~FtkEMonDataOut() noexcept;
      
        void setup(DFCountedPointer<ROS::Config> configuration);
    
        /// sendData method of FtkEMonDataOut
        /// ROS::Buffer has been deprecated. Use the other one
        void sendData(const ROS::Buffer* buffer , const NodeID,
                      const u_int tid=1,const unsigned int status=0 );
    

        /// sendData method of FtkEMonDataOut
        void sendData(std::vector< std::pair< SpyBufShPtr, uint32_t > > v_spybuffer);

        /// Method to get the run number 
        uint32_t GetRunNumber();

        /// Method to get the sampling address 
        const emon::SamplingAddress * GetAddress();

        /// Method to get the hash of 'value' parameter in the selection criteria
        uint32_t GetSamplingHash();

        /// Method to get the lumiblock number
        uint32_t GetLumiBlock();
   
        /// Send statistics
        ISInfo* getISInfo();


        /// FTKPullSamplingFactory class; it creates the samplers.
        class FTKPullSamplingFactory : public emon::PullSamplingFactory 
        {
          public:
      
            FTKPullSamplingFactory( daq::ftk::FtkEMonDataOut * ftkemondataout );
      
            ~FTKPullSamplingFactory();
      
           /* 
            * \brief sendEvent method of FtkEMonDataOut::FTKPullSamplingFactory class.
            * This is the object that actually sends the FullFragment through the channel    
            */
            bool sendEvent(emon::EventChannel& channel, const emon::SelectionCriteria  sc);
             
           /* 
            * \brief startSampling method of FtkEMonDataOut::FTKPullSamplingFactory class 
            * When a monitoring application requests a fragment from a provider,
            * this method checks the selection criteria and starts the sampling 
	          * thread.
            * It throws two kind of errors:
            *   - BadCriteria for problems with the selection criteria.
            *   - NoResources for problems with the buffer.
            * \note The status word is abused, i.e.: the client requests a fragment 
            *       specifying the sourceID in the StatusWord field  
            */
            emon::PullSampling* startSampling(const emon::SelectionCriteria & sc)
               throw (emon::BadCriteria, emon::NoResources);      
            
            bool checkSpybuffer(int sendCall, pid_t ttid);    ///< Method to check if a Spybuffer has already been shipped

          friend FtkEMonDataOut;

          private:
            FtkEMonDataOut*              m_ftkemondataout;     ///< Pointer to the FtkEMonDataOut instance
            uint32_t                     m_fulfilledRequests;  ///< Total number of samplings created by the factory
            std::atomic<int>             m_lastCall;           ///< Last m_sendCalled number accepted 
            std::vector<pid_t>           m_tid;                ///< Vector containing the tid of the call
            DFFastMutex*                 m_tidMutex;          ///< Mutex used in the function checkSpybuffer
        };

    

        /// FTKPullSampling class.
        /// It sends the requested data fragment calling FTKPullSamplingFactory::sendEvent
        class FTKPullSampling : public emon::PullSampling 
        {
          public:

	          FTKPullSampling(FTKPullSamplingFactory & factory, const emon::SelectionCriteria sc);
        
            ~FTKPullSampling();

            void sampleEvent(emon::EventChannel &channel);

          private:
            FTKPullSamplingFactory & m_factory; ///< reference to the factory which started the sampling
            const emon::SelectionCriteria  m_scrit; ///< selection criteria requested by the monitoring task

        };

       /// Thread safe buffer class.
       /// It contains the SpyBuffer pointers and the methods to work safely on them
        class TSafeMap 
        {
          public:
            /// Costructor
	          TSafeMap();

            /// Memory safe destructor
	          ~TSafeMap();

	          /// AddSpyBuffer method of TSafeMap class.
            /// Thread safe method to store new spy buffers
	          void AddSpyBuff(SpyBufShPtr spy_frag , uint32_t source_id); 

	          /// GetSpyBuff method of TSafeMap class.
            /// Thread safe method to get spy buffers according to choosen selection criteria.
	          std::vector< std::pair< uint32_t, SpyBufShPtr > > GetSpyBuff(const emon::SelectionCriteria sc);

            /// Empty method of TSafeMap class.
	          bool Empty();

	        private:
	          std::map< uint32_t, SpyBufShPtr > m_data;
            DFFastMutex*                      m_accessMutex;
        };


        /// Method to get the address of FtkEMonDataOut::m_dataStorage
        TSafeMap* GetMapAddr();

      private: 

        TSafeMap                     m_dataStorage;
        emon::EventSampler*          m_eventSampler;
        FTKPullSamplingFactory*      m_emonfactory;     ///< Object which starts the samplings
        ISInfoDictionary             m_infoDict;        ///< Object used to retrieve the Run number and the Lumiblock number
        uint32_t                     m_runnumber;       ///< Run number
        uint16_t                     m_lumiblocknumber; ///< Lumiblock number
        const emon::SamplingAddress* m_address;         ///< Address of the provider of the fragments
        uint32_t                     m_sampaddrhash;    ///< Hash of the sampling address
        int                          m_sendCalled;      ///< Total number of data fragments copied in the buffer
        uint32_t                     m_numberOfRMs;     ///< Number of the ReadoutModules


// FIXME: remove this hack when migration to cmake is complete
#if __GNUC__ == 4
        ROS::DataOutInfo             m_stats;           ///< Statistics 
#else
        DataOutInfo                  m_stats;           ///< Statistics
#endif
        Type                         m_type;            ///< Type of data
        uint                         m_maxchannels;     ///< Max number of channels
    };
 
  }// namespace ftk

}  //namespace daq

#endif	// EMONDATAOUT
