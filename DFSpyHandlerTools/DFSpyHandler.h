#ifndef DFSPYHANDLER_H
#define DFSPYHANDLER_H

#include <map>
#include <vector>
#include <set>
#include <queue>
#include <tuple>
// Files:
#include "structs_dfspy.h" // Buffer, TwoTuple, ThreeTuple, FiveTuple
#include "ftkcommon/EventFragment.h"
#include "ftkcommon/EventFragmentFTKPacket.h"
#include "ftkcommon/EventFragmentHitClusters.h"
#include "ftkcommon/Cluster.h"
#include "ftkcommon/SourceIDSpyBuffer.h"
#include "ftkcommon/SpyBuffer.h"



class DFSpyHandler
{
public:
    DFSpyHandler(std::string multiboard, std::string modulelist, std::string system_config);
    //~DFSpyHandler();
    void performBitlevelChecks(const std::map< daq::ftk::SourceIDSpyBuffer, std::shared_ptr<daq::ftk::SpyBuffer> >& bufferMap);
    // For testing purposes only:
    void performBitLevelChecksTESTER(std::string dumpdir, bool parseDirectory, bool makeFrags, bool doParsing);

private:
    // ======= Private functions:
    // Main Parsing Functions
    void parseNewInputBuffer(std::vector<std::shared_ptr<daq::ftk::EventFragmentHitClusters> > eventFragments,
                             int board, int inlane);

    void parseNewOutputBuffer(std::vector<std::shared_ptr<daq::ftk::EventFragmentHitClusters> > eventFragments,
                            int board, int outlane);

    // LUT Formation Functions
    void buildLUTs(std::string multiboard, std::string modulelist, std::string system_config);
    std::map<int, bool>mapInspyBoardEnable(std::string multiboard);
    std::map<int, std::vector<int> > mapROBIDtoBoardAndLane(std::string multiboard);
    std::map<std::vector<int>, std::set<int> > mapBoardAndLaneToModIDsInspy(std::string moduleListFile);
    std::map<std::vector<int>, std::vector<int> > mapTowerPlaneToBoardLane(std::string sysFile);
    std::map<std::vector<int>, std::set<int> > mapBoardAndLaneModIDsOutspy(std::string moduleListFile);
    std::map<int, std::set< std::vector<int> > > mapModID2BoardAndLaneOutspy();

    // Parsing helper functions
    void printBonusBufferStats(int board, int lane, std::set<int>& uniqueBufferModules);
    void safePushInspy(TwoTuple &boardLane, FiveTuple &ft);
    void safePushOutspy(TwoTuple &boardLane, ThreeTuple &packet);
    void performModAndHitClusterCheckInspy(ThreeTuple &packet, int modNum, std::shared_ptr<daq::ftk::ModuleHitClusters> &mhc);
    void registerPacketAsRequest(ThreeTuple &packet, std::set<ThreeTuple> &packetsToRequest, FiveTuple* &ft);
    void initialOutspyCleaner(TwoTuple &boardLane, ThreeTuple &packet);

    // Check-specific helpers
    bool compareModuleHitClusters(std::shared_ptr<daq::ftk::ModuleHitClusters>& mhc1, std::shared_ptr<daq::ftk::ModuleHitClusters>& mhc2);
    bool compareSCTClusters(daq::ftk::SCTCluster* a, daq::ftk::SCTCluster* b);
    bool comparePixelClusters(daq::ftk::PixelCluster* a, daq::ftk::PixelCluster* b);

    // Viewing
    std::string makePacketString(ThreeTuple& t, bool isOutlane);
    void displayContentsQueueMapOutspy();
    void displayContentsQueueMapInspy();
    std::string get2TupleString(std::vector<int> t);

    // ======= Private data structures:
    // LUTs
    std::map<int, bool> boardEnabled;
    std::map<int, std::vector<int> > ROBIDtoBoardAndLane;
    std::map<std::vector<int>, std::set<int> > boardAndLaneToModIDsInspy;
    std::map<std::vector<int>, std::vector<int> > towerPlaneToBoardLane;
    std::map<std::vector<int>, std::set<int> > boardAndLaneToModIDsOutspy;
    std::map<int, std::set< std::vector<int> > > modID2BoardandLaneOutspy;

    // == Inspy:
    std::map<TwoTuple, std::queue<FiveTuple> > queueMapInspy;
    std::map<ThreeTuple, std::set<FiveTuple> > setMapInspy;
    // == Outspy:
    std::map<TwoTuple, std::queue<ThreeTuple> > queueMapOutspy;
    std::map<ThreeTuple, std::shared_ptr<daq::ftk::EventFragmentHitClusters> > pointerMapOutspy;

    // For testing purposes only, only places used are parseNewInputBuffer and parseNewOutputBuffer
    std::set<int> uniqueModulesOutspy;
    std::set<int> uniqueModulesInspy;
    int expectedModulesInspyCount, totalModulesInspy;
    int expectedModulesOutspyCount, totalModulesOutspy;
       // For performBitLevelChecksTESTER
    std::vector<std::string> getSpyFilesFromDirectory(const std::string& name);
    std::vector<unsigned int> spyfileToVector(std::string spyfile);
    std::string getSourceIDFromFilename(std::string filename);
    int getBoardFromSourceID(std::string sourceID);
    //std::tuple<int, bool> getLaneFromSourceID(std::string sourceID);
    int getAbsoluteChannelFromSourceID(std::string sourceID);
};

#endif // DFSPYHANDLER_H