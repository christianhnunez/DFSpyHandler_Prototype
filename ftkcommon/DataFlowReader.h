#ifndef DATAFLOWREADER_H_
#define DATAFLOWREADER_H_

#include <vector>
#include <string>
#include <memory>

// is
#include "ftkcommon/StatusRegister/StatusRegisterISVector.h"
#include "ftkcommon/dal/FtkDataFlowSummaryNamed.h"
#include <oh/OHRawProvider.h>
#include <is/infodictionary.h>
#include <is/infoT.h>


namespace daq {
namespace ftk {

  /*! \brief Base class for DataFlow information extraction
   * 
   * An interface for extraction of the DataFlow infromation from status registers. 
   * In each case one has to implement all getter functions according to  board specifics.
   * If information is not available a dummy function has to be created or used from the base.
   * A dummy implementation publishing hard-coded values is available as DataFlowReaderDummy
   * class in the same header file.
   * DataFlowReaderDummyIS is a more complete dummy implementation,
   * that reads from IS the values published by StatusRegisterDummyDirect.
   *
   * The common usage is to read from IS the register values using DataFlowReader::readIS()
   * and then parse them and publish histograms using DataFlowReader::publishAllHistos().
   * In addiiton to the common getter functions, that are defined in the base class, 
   * DataFlowReader::publishAllHistos() also calls DataFlowReader::publishExtraHistos(),
   * which is empty in the base implementation 
   * and can implement user-specific monitoring to be buplished to OH as well
   * 
   */

  class DataFlowReader {
  public:
    /*! \brief Constructor: sets the input scheme (IS inputs or vectors)
     *  \param ohRawProvider Pointer to initialized  OHRawProvider object
     *  \param forceIS Force the code to use IS as the source of information (default is 0)
     */
    DataFlowReader(std::shared_ptr<OHRawProvider<>> ohRawProvider,
		bool forceIS = false);

    /*! \brief initialization: connects to IS if needed. Must be called after constructor
     *  \param deviceName Device name (Name attribute of corresponding object) as appears in OKS and also used in IS naming convention
     *  \param partitionName Partition name. Used if IS is the source (default is FTK)
     *  \param isServerName IS server name. Used if IS is the source (default is DF for DataFlow)
     */
    void init(const string& deviceName,
		const std::string& partitionName = "FTK", 
		const std::string& isServerName = "DF");

    /*! \brief Destructor: cleans the stored data and addresses
     */
    virtual ~DataFlowReader();

    /*! \brief Update stored information from IS. To be used _only_ when IS is the source
     *  \param verb Verbosity of printout: prints time of the last update on IS (defualt is 0)
     */
     void readIS(uint32_t verb = 0);

    /*! \brief Update stored information directly. To be used _both_ when values are directly set
     *  \param data Vector of register contents
     *  \param data_info Vector storing metadata information about registers: 
     *    - firstAddr: first address included in the collection 
     *    - finalAddr: first address NOT included in the collection 
     *    - addrIncrement: address increment  in the sequence 
     *    - fpgaNum: FPGA number
     *    - selectorAddr: *only for special access types* the address where one writes an address to read it from readerAddr later
     *    - readerAddr: *only for special access types* the value read out if selector access is used
     *    - prescale: the prescale factor with which readout for a collection is scaled down. 
     *    Used to define the collection with higherst update rate to use it for register value
     */
    void setDataAndInfo(const std::vector< std::vector<uint32_t> >& data, const std::vector< std::vector<uint32_t> >& data_info);

    /*! \brief Update stored information directly. To be used _both_ when values are directly set or IS is readout
     *  \param data Vector of register contents
     *  \param data_info Vector storing metadata information about registers: 
     *    - firstAddr: first address included in the collection 
     *    - finalAddr: first address NOT included in the collection 
     *    - addrIncrement: address increment  in the sequence 
     *    - fpgaNum: FPGA number
     *    - selectorAddr: *only for special access types* the address where one writes an address to read it from readerAddr later
     *    - readerAddr: *only for special access types* the value read out if selector access is used
     *    - prescale: the prescale factor with which readout for a collection is scaled down. 
     *    Used to define the collection with higherst update rate to use it for register value
     */
    void setDataAndInfo(const std::vector< std::vector<uint32_t>* >& data, const std::vector< std::vector<uint32_t>* >& data_info);



    /*! \brief Publish all distributions to IS as histograms
     *  \param option To be defined (default is 0)
     */
    void publishAllHistos(uint32_t option = 0);

    /*! \brief Publish extra distributions to IS as histograms
     *  \param option To be defined (default is 0)
     */
    virtual void publishExtraHistos(uint32_t /*option = 0*/) {};

    /*! \brief Publish a single distribution to IS and to OH as histograms (used for floats and ints)
     *  \param option To be defined (default is 0)
     */
    template<typename T>
    void publishSingleTH1F(const std::vector<T> &srv, const std::string &str)
    {
      std::string strTmp;
      strTmp = str + ":1d";
   
      //publish to IS
      ISInfoT<std::vector<T>> isv(srv);
      isd->checkin(m_isWriteObjName + ":" + str, isv);

			//publish to OH
			//Added a trick to not publish in OH int64_t (they are casted in int32).
			if (std::is_same<T, int64_t>::value)
			{
				std::vector<int> Vec32(srv.begin(), srv.end());
      	m_ohRawProvider->publish(strTmp, str,
                               OHMakeAxis<double>("X Axis", Vec32.size(), 0,1),
                               Vec32.data(),
                               (T *) 0,
                               false);
			}
			else
			{
				m_ohRawProvider->publish(strTmp, str,
                               OHMakeAxis<double>("X Axis", srv.size(), 0,1),
                               srv.data(),
                               (T *) 0,
                               false);
			}
    }

    std::string name_dataflow(){return m_isWriteObjName;};
  // std::shared_ptr<FtkDataFlowSummary> m_theSummary = std::make_shared<FtkDataFlowSummary>();


    /*! \brief Extract a register from the stored vectors.
     * \param reg Register address to be accessed (or value writen to selector, is sel != 0)
     * \param fpga FPGA number (starts from 0)
     * \param sel Selector register address, if any (DataFormatter schema)
     * \param read Reader register address, if selector is used (DataFormatter schema)
     */
    uint32_t readRegister(uint32_t reg, uint32_t fpga = 0, uint32_t sel = 0, uint32_t read = 0);



//#################################################################
    /*! \brief Get fraction of BUSY from downstream
     *
     *  \param option The type of the average, depends on particular implementation
     *
     */
    virtual void getBusyFraction(std::vector<float>& srv, uint32_t /*option = 0*/);


    /*! \brief Get fraction of Input FIFO BUSY 
     *
     *  \param option The type of the average, depends on particular implementation
     *
     */
    virtual void getFifoInBusyFraction(std::vector<float>& srv, uint32_t /*option = 0*/);
    /*! \brief Get fraction of Output FIFO BUSY 
     *
     *  \param option The type of the average, depends on particular implementation
     *
     */
    virtual void getFifoOutBusyFraction(std::vector<float>& srv, uint32_t /*option = 0*/);
    /*! \brief Get fraction of Input FIFO Empty 
     *
     *  \param option The type of the average, depends on particular implementation
     *
     */
    virtual void getFifoInEmptyFraction(std::vector<float>& srv, uint32_t /*option = 0*/);
    /*! \brief Get fraction of Output FIFO Empty 
     *
     *  \param option The type of the average, depends on particular implementation
     *
     */
    virtual void getFifoOutEmptyFraction(std::vector<float>& srv, uint32_t /*option = 0*/);


    /*! \brief Get current Input FIFO BUSY 
     */
    virtual void getFifoInBusy(std::vector<int64_t>& srv);
    /*! \brief Get current Output FIFO BUSY 
     */
    virtual void getFifoOutBusy(std::vector<int64_t>& srv);
    /*! \brief Get current Input FIFO Empty 
     */
    virtual void getFifoInEmpty(std::vector<int64_t>& srv);
    /*! \brief Get current Output FIFO Empty 
     */
    virtual void getFifoOutEmpty(std::vector<int64_t>& srv);


    /*! \brief Get L1ID seen on each Input channel
     */
    virtual void getL1id(std::vector<int64_t>& srv);


    /*! \brief Get data errors on each Input channel
     */
    virtual void getDataErrors(std::vector<int64_t>& srv);


    /*! \brief Get event rate on each Input channel
     */
    virtual void getEventRate(std::vector<int64_t>& srv);


    /*! \brief Get status of input links (1: ON, 0: OFF, 2..10: Errors seen)
     */
    virtual void getLinkInStatus(std::vector<int64_t>& srv);

    /*! \brief Get status of output links (1: ON, 0: OFF, 2..10: Errors seen)
     */
    virtual void getLinkOutStatus(std::vector<int64_t>& srv);

    /*! \brief Get FPGA temperature on each FPGA, if possible
     */
    virtual void getFpgaTemperature(std::vector<int64_t>& srv);

    /*! \brief Get number of events processed on each output channel
     */
    virtual void getNEventsProcessed(std::vector<int64_t>& srv);

     /*! \brief Get number of packets discarded
     */
    virtual void getNPacketsDiscarded(std::vector<int64_t>& srv); 

    /*! \brief Get number of packets timedout by the PacketHandler
    */ 
    virtual void getNPacketsTimedout(std::vector<int64_t>& srv); 
    
    /*! \brief Get number of packets held by synchronizer
    */ 
    virtual void getNPacketsHeldSync(std::vector<int64_t>& srv);
 
    /*! \brief Get number of packets discarded by sync 
    */ 
    virtual void getNPacketsDiscardedSync(std::vector<int64_t>& srv);

    /*! \brief Get numbe rof packets timeout by sync
    */  
    virtual void getNPacketsTimeoutSync(std::vector<int64_t>& srv);

    /* \brief Get time difference between packet arrival on different input lanes
    */  
    virtual void getInputSkewSync(std::vector<int64_t>& srv);

    /* \brief Get # clock cycles between valid header and valid trailer, not counting BP
    */  
    virtual void getPacketCompletionTime(std::vector<int64_t>& srv);

    /* \brief Get the total skew time for each event processed
    */  
    virtual void getMaxSkewTime(std::vector<int64_t>& srv);

   
 
  protected:
    /*! \brief Get names of IS vectors to be read out. Must be defined in a derived class
     */
    virtual std::vector<std::string> getISVectorNames() = 0;
   // virtual std::string name();
    /*! \brief Tool to publish to IS
     */
    std::unique_ptr<ISInfoDictionary> isd;
    std::string m_isWriteObjName;
    IPCPartition m_ipcp;
    std::shared_ptr<FtkDataFlowSummaryNamed> m_theSummary;


  private:

    /*! \brief Use IS as a source or get vector directly
     */
    bool m_useIS;

    /*! \brief Vector of register vectors
     */
    std::vector< std::vector<uint32_t> > m_data;
    /*! \brief Vector of information/metadata vectors (defined by the SRFactory)
     */
    std::vector< std::vector<uint32_t> > m_data_info;
    /*! \brief Vector of IS register vectors
     */
    std::vector< std::unique_ptr<StatusRegisterISVector> > m_is_v;
    /*! \brief Vector of IS information/metadata vectors
     */
    std::vector< std::unique_ptr<StatusRegisterISVector> > m_is_info_v;

    std::shared_ptr<OHRawProvider<>> m_ohRawProvider;
   // std::shared_ptr<FtkDataFlowSummaryNamed> m_theSummary;

   
  }; // DataFlowReader


  class DataFlowReaderDummy: public DataFlowReader {
  public:
    /*! \brief Constructor: sets the input scheme (IS inputs or vectors)
     *  \param ohRawProvider Pointer to initialized  OHRawProvider object
     *  \param forceIS Force the code to use IS as the source of information (default is 0)
     */
  DataFlowReaderDummy(std::shared_ptr<OHRawProvider<>> ohRawProvider, 
		bool forceIS = false)
	: DataFlowReader(ohRawProvider, forceIS) {};

  protected:
    /*! \brief Get names of IS vectors to be read out. Must be defined in a derived class
     */
    std::vector<std::string> getISVectorNames()
	{vector<string> srv_names; return srv_names;};

  public:
    virtual void getLinkOutStatus(std::vector<int64_t>& srv)
        {srv = {0,0,1,2,4,1,0};}
    virtual void getLinkInStatus(std::vector<int64_t>& srv)
        {srv = {1,1,1,2,4,1,0};}
    void getFifoInBusyFraction(std::vector<float>& srv, uint32_t /*option = 0*/)
	{srv = {0,0,0.2,0.56,0.87,0.99,1.0,2.0};}
    void getFifoInEmptyFraction(std::vector<float>& srv, uint32_t /*option = 0*/)
	{srv = {0,10,20,56,87.456,99,100,200};}
    void getFifoOutEmptyFraction(std::vector<float>& srv, uint32_t /*option = 0*/)
	{srv = {0,0,0.2,0.56,0.87,0.99,1.0,2.0};}
    virtual void getFifoInBusy(std::vector<int64_t>& srv)
        {srv = {0,0,1,1,0};}
    virtual void getFifoOutEmpty(std::vector<int64_t>& srv)
        {srv = {0,0,1,1,0};}
    virtual void getL1id(std::vector<int64_t>& srv)
        {srv = {4001,4001,4001,4005,4001};}
    //virtual void getDataErrors(std::vector<int64_t>& srv)
    virtual void getEventRate(std::vector<int64_t>& srv)
        {srv = {0,0,1000,1000,0};}
    virtual void getFpgaTemperature(std::vector<int64_t>& srv)
        {srv = {40,30,31,1,51,71,81};}
    virtual void getNEventsProcessed(std::vector<int64_t>& srv)
        {srv = {4000001,4000001,4000001,4000005,4000001};}
  }; // DataFlowReaderDummy


} // ftk
} // daq
#endif // DATAFLOWREADER_H_
