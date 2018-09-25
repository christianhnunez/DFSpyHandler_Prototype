#include <iostream>
#include <fstream>
#include <map>
#include <set>
#include <queue>
#include <fstream>
#include <sstream>
#include <string>
#include <tuple>
#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
#include <stdlib.h>
// tdaq
#include "Xeventfrag.h"
#include "Xeventfragftk.h"
#include "Xeventfraghit.h"
// real
#include "/Users/ChristianHaroldNunez/Desktop/ftkcommon/ftkcommon/EventFragment.h"
#include "/Users/ChristianHaroldNunez/Desktop/ftkcommon/ftkcommon/EventFragmentFTKPacket.h"
#include "/Users/ChristianHaroldNunez/Desktop/ftkcommon/ftkcommon/EventFragmentHitClusters.h"
#include "/Users/ChristianHaroldNunez/Desktop/ftkcommon/ftkcommon/Utils.h"

// Advanced Testing
std::vector<std::string> getSpyFilesFromDirectory(const std::string& name);
std::vector<unsigned int> spyfileToVector(std::string spyfile);
std::string getSourceIDFromFilename(std::string filename);
int getBoardFromSourceID(std::string sourceID);
std::tuple<int, bool> getLaneFromSourceID(std::string sourceID);


// LUT Prototypes
std::map<int, bool>mapInspyBoardEnable(std::string filepath);
std::map<int, std::tuple<int, int>> mapROBIDtoBoardAndLane(std::string filepath);
std::map<std::tuple<int, int>, std::set<int>> mapBoardAndLaneToModIDsInspy(std::string moduleListFile, std::map<int, std::tuple<int, int>> &robIDtoBoardLane);
std::map<std::tuple<int, int>, std::tuple<int, int>> mapTowerPlaneToBoardLane(std::string sysFile, std::map<int, bool> &boardEnabled);
std::map<std::tuple<int, int>, std::set<int>> mapBoardAndLaneModIDsOutspy(std::string moduleListFile, std::map<std::tuple<int, int>, std::tuple<int, int>> &towerPlaneToBoardLane);
std::map<int, std::set< std::tuple<int, int>>> mapModID2BoardAndLaneOutspy(std::map<std::tuple<int, int>, std::set<int>> &boardAndLaneToModIDsOutspy);
// New LUT Prototypes
std::map<unsigned int, std::map<int, std::vector<std::tuple<int, int>> > > getROBmapFromModList(std::string moduleListFile);
void parseSysFile(std::string sysFile, std::map<int, std::tuple<int, int>> &boardmap, std::map<int, int> &plane2outlane);
std::map<int, std::set<std::tuple<int, int>>> getModsToOutlanes(std::map<int, std::tuple<int, int>> &boardmap,
                                                                std::map<int, int> &plane2outlane,
                                                                std::map<unsigned int, std::map<int, std::vector<std::tuple<int, int>>>> &ROBmap,
                                                                std::map<int, bool> &boardEnabled);


// Utilities
//std::vector<std::string> split(std::string str);
unsigned int hexStringToInt(std::string hexString);
unsigned long long hexStringToULL(std::string str);
std::vector<std::string> tokenizeModListLine(std::string line);
std::vector<std::string> split(std::string s);

// Test Prototypes
void eventFragTest();
void splitTest(std::string testString);
void printMap(std::map<int, bool> myMap);
void printMap2(std::map<std::string, std::tuple<int, int> > myMap);
std::string get2TupleString(std::tuple<int, int> t);


/* ########################################
 * #              STRUCTS                 #
 * ######################################## */

struct Buffer {
    std::vector<daq::ftk::EventFragmentHitClusters*> frags;
    int board;
    int lane;
    bool isInspy;
};

struct TwoTuple // EDIT: RENAME
{
    TwoTuple(int b, int l)
    : boardNumber(b)
    , laneNumber(l)
    { }

    int boardNumber;
    int laneNumber;

    bool operator==(const TwoTuple &t) const {
        return (boardNumber == t.boardNumber &&
                laneNumber == t.laneNumber);
    }

    bool operator<(const TwoTuple &t) const {
        return (boardNumber < t.boardNumber ||
                laneNumber  < t.laneNumber);
    }

};

struct ThreeTuple // EDIT: RENAME
{
    ThreeTuple(int b, int l, unsigned int l1id)
    : boardNumber(b)
    , laneNumber(l)
    , L1ID(l1id)
    { }

    int boardNumber;
    int laneNumber;
    unsigned int L1ID;

    bool operator==(const ThreeTuple &t) const {
        return (boardNumber == t.boardNumber &&
                laneNumber == t.laneNumber   &&
                L1ID       == t.L1ID);
    }

    bool operator<(const ThreeTuple &t) const {
        return (boardNumber < t.boardNumber ||
                laneNumber  < t.laneNumber  ||
                L1ID        < t.L1ID);
    }
};

struct FiveTuple // EDIT: RENAME
{
    FiveTuple(int m, unsigned int l, int b, int o)
    : moduleID(m)
    , L1ID(l)
    , boardNumber(b)
    , outputLane(o)
    { }

    int moduleID;
    unsigned int L1ID;
    int boardNumber;
    int outputLane;
    //std::vector<EventFragmentHitClusters*> hitClusters;

    bool operator==(const FiveTuple &t) const {
        return (moduleID    == t.moduleID &&
                L1ID        == t.L1ID     &&
                (unsigned int)boardNumber == t.L1ID     &&
                outputLane  == t.outputLane);
    }

    bool operator<(const FiveTuple &t) const {
        return (moduleID    < t.moduleID ||
                L1ID        < t.L1ID     ||
                (unsigned int) boardNumber < t.L1ID     ||
                outputLane  < t.outputLane);
    }
};

//struct ModuleHitCluster {
//    int m_module_number;
//};

//struct EventFragmentHitClusters {
//    int L1ID;
//    std::vector< std::shared_ptr<ModuleHitCluster> > m_modules;
//};

// Parsing Functions
void parseNewInputBuffer(std::vector<daq::ftk::EventFragmentHitClusters*> eventFragments,
                         std::map<std::tuple<int, int>, std::set<int>>& boardAndLaneToModIDsInspy,
                         std::map< int, std::set<std::tuple<int, int>> >& modIDsToBoardOutLane,
                         std::map< ThreeTuple, std::queue<FiveTuple> >& queueMapInspy, // not passed in (private collection)
                         std::map< ThreeTuple, std::set<FiveTuple> >& setMapInspy, // not passed in (private collection)
                         std::map< TwoTuple, std::queue<ThreeTuple> >& queueMapOutspy, // not passed in (private collection)
                         std::map< ThreeTuple, daq::ftk::EventFragmentHitClusters*>& pointerMapOutspy, // not passed in (private collection)
                         int board, int inlane, int& expectedModulesInspyCount, int& totalModulesInspy, std::set<int>& uniqueModulesInspy); // not passed in (get lane from source ID)

void parseNewOutputBuffer(std::vector<daq::ftk::EventFragmentHitClusters*> eventFragments,
                        std::map< std::tuple<int, int>, std::set<int>>& boardAndLaneToModIDsOutspy,
                        std::map< int, std::set<std::tuple<int, int>> >& modIDsToBoardOutLane,
                        std::map< ThreeTuple, std::queue<FiveTuple>>& queueMapInspy, // not passed in (private collection)
                        std::map< ThreeTuple, std::set<FiveTuple>>& setMapInspy, // not passed in (private collection)
                        std::map< TwoTuple, std::queue<ThreeTuple>>& queueMapOutspy, // not passed in (private collection)
                        std::map< ThreeTuple, daq::ftk::EventFragmentHitClusters*>& pointerMapOutspy, // not passed in (private collection)
                        int board, int outlane, int &expectedModulesOutspyCount, int& totalModulesOutspy, std::set<int>& uniqueModulesOutspy); // not passed in (get lane from source ID)

std::string makePacketString(ThreeTuple& t, bool isOutlane);

// DFConstants (separate class) (include DF CONSTANTS!)
const int NUMBER_OF_FTK_TOWERS = 64;

using namespace std;

/* ########################################
 * #                MAIN                  #
 * ######################################## */

int main()
{
    // **********
    // * SET UP *
    // **********
    bool createLookUpTables = true;
    bool local = true;
    bool parseDirectory = true;
    bool makeFrags = true;
    bool doParsing = true;
    bool compareModMaps = true;
    std::string modulelist;
    std::string multiboard;
    std::string systemcf;
    std::string dumpdir;
    if(local) {
        modulelist = "/Users/ChristianHaroldNunez/Desktop/CERN/ATLAS/advanced/modulelist_Data2018_64T.txt";
        multiboard = "/Users/ChristianHaroldNunez/Desktop/CERN/ATLAS/advanced/Slice_lab4_config_Data2018.txt";
        systemcf = "/Users/ChristianHaroldNunez/Desktop/CERN/ATLAS/advanced/df_system_config_64towers.txt";
        dumpdir = "/Users/ChristianHaroldNunez/Desktop/CERN/ATLAS/advanced/SpyDump_20180817_103301/";
    } else {
        modulelist = "/afs/cern.ch/user/c/chnunez/public/dfparse/advanced/modulelist_Data2017_64T.txt";
        multiboard = "/afs/cern.ch/user/c/chnunez/public/dfparse/advanced/DF20_lab4_config_Data2017_TRACKS.txt";
        systemcf = "/afs/cern.ch/user/c/chnunez/public/dfparse/advanced/df_system_config_64towers.txt";
        dumpdir = "/afs/cern.ch/user/c/chnunez/public/dfparse/advanced/SpyDump_20180807_143118/";
    }

//    // HexString Tester
//    std::string hexString = "0Xffffffff";
//    unsigned int op1 = hexStringToInt(hexString);
//    unsigned long long op2 = hexStringToULL(hexString);
//    Int_tf op3 = daq::ftk::readHex(hexString);

//    std::cout << op1 << std::endl;
//    std::cout << op2 << std::endl;
//    std::cout << op3 << std::endl;

//    std::string hexString1 = "0Xffffffff";
//    unsigned int op11 = hexStringToInt(hexString1);
//    unsigned long long op21 = hexStringToULL(hexString1);
//    Int_tf op31 = daq::ftk::readHex(hexString1);

//    std::cout << op11 << std::endl;
//    std::cout << op21 << std::endl;
//    std::cout << op31 << std::endl;

//    unsigned int t1 = op1 | (op11 << 32);
//    unsigned long long t2 = op2 | (op21 << 32);
//    Int_tf t3 = op3 | (op31 << 32);

//    std::cout << t1 << std::endl;
//    std::cout << t2 << std::endl;
//    std::cout << t3 << std::endl;



    // **********
    // *  BEGIN *
    // **********
    if(createLookUpTables) {
        std::cout << "\n\n\n\n START: \n\n\n\n";
        std::cout << "======== CONFIG FILES ========" << std::endl;
        std::cout << "MODULE LIST:  " << modulelist << std::endl;
        std::cout << "MULITBOARD:   " << multiboard << std::endl;
        std::cout << "SYSTEM:       " << systemcf << std::endl;
        std::cout << "SPY DUMP:     " << dumpdir << std::endl;
        std::cout << "==============================" << std::endl;
        std::cout << "\n" << std::endl;
        std::cout << "======== JUMP TO CONTENTS ========" << std::endl;
        std::cout << "[P1] BOARD -> ENABLED?" << std::endl;
        std::cout << "[P2] ROBID -> (Board, Lane)" << std::endl;
        std::cout << "[P3] (Board, Lane) -> {inspy modules}" << std::endl;
        std::cout << "[P4] (Tower, Plane) -> (Board, Lane)" << std::endl;
        std::cout << "[P5] (Board, Lane) -> {outspy modules}" << std::endl;
        std::cout << "[P6] Outspy module # -> {(Board, Lane)}" << std::endl;
        std::cout << "==============================" << std::endl;

        std::cout << "\n\n\n\n NEXT: [P1] BOARD -> ENABLED? \n\n\n\n";
        std::map<int, bool> boardEnabled = mapInspyBoardEnable(multiboard);
        std::cout << "\n\n\n\n NEXT: [P2] ROBID -> (Board, Lane) \n\n\n\n";
        std::map<int, std::tuple<int, int>> ROBIDtoBoardAndLane = mapROBIDtoBoardAndLane(multiboard);
        std::cout << "\n\n\n\n NEXT: [P3] (Board, Lane) -> {inspy modules} \n\n\n\n";
        std::map<std::tuple<int, int>, std::set<int>> boardAndLaneToModIDsInspy = mapBoardAndLaneToModIDsInspy(modulelist, ROBIDtoBoardAndLane);
        std::cout << "\n\n\n\n NEXT: [P4] (Tower, Plane) -> (Board, Lane) \n\n\n\n";
        std::map<std::tuple<int, int>, std::tuple<int, int>> towerPlaneToBoardLane = mapTowerPlaneToBoardLane(systemcf, boardEnabled);
        std::cout << "\n\n\n\n NEXT: [P5] (Board, Lane) -> {outspy modules} \n\n\n\n";
        std::map<std::tuple<int, int>, std::set<int>> boardAndLaneToModIDsOutspy = mapBoardAndLaneModIDsOutspy(modulelist, towerPlaneToBoardLane);
        std::cout << "\n\n\n\n NEXT: [P6] Outspy module # -> {(Board, Lane)} \n\n\n\n";
        std::map<int, std::set<std::tuple<int, int>>> modID2BoardandLaneOutspy = mapModID2BoardAndLaneOutspy(boardAndLaneToModIDsOutspy);
        std::cout << "\n\n\n\n NEXT: [P7] OUTSPY CONFIGGING \n\n\n\n";
        std::map<unsigned int, std::map<int, std::vector<std::tuple<int, int>>>> ROBmap = getROBmapFromModList(modulelist);
        std::map<int, std::tuple<int, int>> boardmap;
        std::map<int, int> plane2outlane;
        parseSysFile(systemcf, boardmap, plane2outlane);
        std::map<int, std::set<std::tuple<int, int>>> mods2boardAndoutlanes = getModsToOutlanes(boardmap, plane2outlane, ROBmap, boardEnabled);

        //  std::map<int, std::set<std::tuple<int, int>>> mods2boardAndoutlanes
        if(compareModMaps) {
            std::cout << "\n          ROB              |               CHRISTIAN            " << std::endl;
            for(std::map<int, std::set<std::tuple<int, int>>>::iterator moditer = mods2boardAndoutlanes.begin(); moditer != mods2boardAndoutlanes.end(); ++moditer) {
                int modn = moditer->first;
                std::cout << "MODULE " << modn << " : ";
                for(std::tuple<int, int> boardoutlane : moditer->second) {
                    std::cout << get2TupleString(boardoutlane) << "  ";
                }

                std::cout << "      |       ";
                for(std::tuple<int, int> boardoutlane2 : modID2BoardandLaneOutspy[modn]) {
                    std::cout << get2TupleString(boardoutlane2) << "  ";
                }
                std::cout << std::endl;
            }
        }

        if(parseDirectory) {
            size_t sumFrags = 0, maxFrags = 0, minFrags = SIZE_MAX;
            std::vector<std::string> vec = getSpyFilesFromDirectory(dumpdir);
            std::vector<Buffer> buffers;
            for(std::vector<std::string>::iterator itvec = vec.begin(); itvec != vec.end(); ++itvec) {
                std::cout << "\n NEXT SPYFILE \n" << std::endl;
                std::cout << "Path:         " << *itvec << std::endl;
                std::string filename = itvec->substr(itvec->find("DF_"));
                std::cout << "Filename:     " << filename << std::endl;
                std::string sourceID = getSourceIDFromFilename(filename);
                std::cout << "SourceID:     " << sourceID << std::endl;
                int board = getBoardFromSourceID(sourceID);
                std::cout << "Board #:      " << getBoardFromSourceID(sourceID) << std::endl;
                std::tuple<int, bool> lane = getLaneFromSourceID(sourceID);
                std::cout << "Lane:         " << std::get<0>(lane) << ((std::get<1>(lane) == true) ? " (Inspy)" : " (Outspy)") << std::endl;
                if(makeFrags) {
                    // We're making 4tups: (eventFrags, board, lane, isInspy)
                    unsigned int ctrlPos = 16;
                    std::vector<unsigned int> data = spyfileToVector(*itvec);
                    std::cout << "data size:    " << data.size() << std::endl;
                    std::cout << "== Errors/notices in EventFragment parsing of spyfile: ==" << std::endl;
                    //std::vector<std::shared_ptr<daq::ftk::EventFragmentHitClusters>> frags = FTKPacket_parseFragments<daq::ftk::EventFragmentHitClusters>(data, ctrlPos);
                    std::vector<daq::ftk::EventFragmentHitClusters*> frags = FTKPacket_parseFragments<daq::ftk::EventFragmentHitClusters>(data, ctrlPos);
                    std::cout << "== End errors/notices ==" << std::endl;
                    std::cout << "# of frags:   " << frags.size() << std::endl;
                    // Update frag stats:
                    sumFrags += frags.size();
                    if(frags.size() > maxFrags) maxFrags = frags.size();
                    if(frags.size() < minFrags) minFrags = frags.size();
                    // Prepare buffer vector:
                    board = 5; // EDIT: HARDCODED BOARD
                    Buffer buffer = {frags, board, std::get<0>(lane), std::get<1>(lane)};
                    buffers.push_back(buffer);
                }
            }
            std::cout << "\n \n DIRECTORY PARSING COMPLETE" << std::endl;
            std::cout << "         STATISTICS             " << std::endl;
            std::cout << "Spyfiles parsed:              " << buffers.size() << std::endl;
            std::cout << "Average frags per buffer:     " << sumFrags/vec.size() << std::endl;
            std::cout << "Maximum frags:                " << maxFrags << std::endl;
            std::cout << "Minimum frags:                " << minFrags << std::endl;

            if(doParsing) {
                //std::map<int, std::set<std::tuple<int, int>>> modIDsToBoardOutLane;
                std::map<ThreeTuple, std::queue<FiveTuple>> queueMapInspy;
                std::map<ThreeTuple, std::set<FiveTuple>> setMapInspy;
                std::map<TwoTuple, std::queue<ThreeTuple>> queueMapOutspy;
                std::map<ThreeTuple, daq::ftk::EventFragmentHitClusters*> pointerMapOutspy;
                int expectedModulesInspyCount = 0, totalModulesInspy = 0;
                int expectedModulesOutspyCount = 0, totalModulesOutspy = 0;

                // MODULE UNIQUENESS:
                set<int> uniqueModulesOutspy;
                set<int> uniqueModulesInspy;
                // END MODULE UNIQUENESS
                std::cout << "Preparing to parse " << buffers.size() << " buffers..." << std::endl;
                for(int i = 0; i < 13; i += 1) {
                    //if(i == 19 || i==27) continue;
                    Buffer buffer = buffers[i];
                    std::string btype;
                    buffer.isInspy ? btype = "INSPY" : btype = "OUTSPY";
                    std::cout << "\n Buffer Channel:   " << btype << " Lane " << buffer.lane << " | frags: " << buffer.frags.size() << std::endl;
                    if(!buffer.isInspy) {
                        // switch mods2boardAndoutlanes for modID2BoardandLaneOutspy
                        parseNewOutputBuffer(buffer.frags, boardAndLaneToModIDsOutspy, modID2BoardandLaneOutspy,
                                                 queueMapInspy, setMapInspy, queueMapOutspy, pointerMapOutspy,
                                                 5, buffer.lane, expectedModulesOutspyCount, totalModulesOutspy, uniqueModulesOutspy);
                    } else {
                        parseNewInputBuffer(buffer.frags, boardAndLaneToModIDsInspy, modID2BoardandLaneOutspy,
                                queueMapInspy, setMapInspy, queueMapOutspy, pointerMapOutspy,
                                5, buffer.lane, expectedModulesInspyCount, totalModulesInspy, uniqueModulesInspy);
                    }
                    std::cout << "Buffer Channel:   " << btype << " Lane " << buffer.lane << " parsing complete!" << std::endl;
                }

                double percentageOutspy = ((double)expectedModulesOutspyCount/totalModulesOutspy) * 100;
                std::cout << "\n OUTSPY:     " << percentageOutspy << "% (" << expectedModulesOutspyCount << " / " << totalModulesOutspy << ") of expected modules checks were successful." << std::endl;
                double percentageInspy = ((double)expectedModulesInspyCount/totalModulesInspy) * 100;
                std::cout << "\n INSPY:      " << percentageInspy << "% (" << expectedModulesInspyCount << " / " << totalModulesInspy << ") of expected modules checks were successful." << std::endl;

                std::cout << "\n\n\n\n BUFFER PARSING STATISTICS \n\n\n\n";
                std::cout << "Queue Map Inspy Size:         " << queueMapInspy.size() << std::endl;
                std::cout << "Set Map Inspy Size:           " << setMapInspy.size() << std::endl;
                std::cout << "Queue Map Outspy Size:        " << queueMapOutspy.size() << std::endl;
                std::cout << "Pointer Map Outspy Size:      " << pointerMapOutspy.size() << std::endl;

                std::cout << "Unique modules inspy:         " << uniqueModulesInspy.size() << std::endl;
                std::cout << "Unique modules outspy:        " << uniqueModulesOutspy.size() << std::endl;
            }
        }
    }
    return 0;
}

/* ########################################
 * #          New Config Parsing          #
 * ######################################## */
std::map<int, std::set<std::tuple<int, int>>> getModsToOutlanes(std::map<int, std::tuple<int, int>> &boardmap,
                                                                std::map<int, int> &plane2outlane,
                                                                std::map<unsigned int, std::map<int, std::vector<std::tuple<int, int>>>> &ROBmap,
                                                                std::map<int, bool> &boardEnabled) {
    std::map<int, int> topTowers; // tower -> board number
    std::map<int, int> botTowers; // tower -> board number
    for(std::map<int, std::tuple<int, int>>::iterator itkey = boardmap.begin(); itkey != boardmap.end(); ++itkey) {
        int boardNum = itkey->first;
        if(boardEnabled.find(boardNum) == boardEnabled.end()) continue;
        std::tuple<int, int> towers = itkey->second;
        // Edit, may want to make this only boards enabled.
        topTowers[std::get<0>(towers)] = boardNum;
        botTowers[std::get<1>(towers)] = boardNum;
    }

    std::map<int, std::set<std::tuple<int, int>>> mod2outlanes;
    // Iterate over ROBId keys
    for(std::map<unsigned int, std::map<int, std::vector<std::tuple<int, int>>>>::iterator ROBit = ROBmap.begin(); ROBit != ROBmap.end(); ++ROBit) {
        std::map<int, std::vector<std::tuple<int, int>>> towerdict = ROBmap[ROBit->first];
        // Iterate over towers
        for(std::map<int, std::vector<std::tuple<int, int>>>::iterator TOWERit = towerdict.begin(); TOWERit != towerdict.end(); ++TOWERit) {
            int tower = TOWERit->first;
            std::vector<std::tuple<int, int>> modlist = towerdict[tower];
            // Iterate over (mod, plane) pairs
            for(std::tuple<int, int> MPpair : modlist) {
                int mod = std::get<0>(MPpair);
                int plane = std::get<1>(MPpair);
                int outlane = plane2outlane[plane];

                if(mod2outlanes.find(mod) == mod2outlanes.end()) {
                    std::set<std::tuple<int, int>> boardNumAndOutlane;
                    mod2outlanes.insert(std::pair<int, std::set<std::tuple<int, int>>> (mod, boardNumAndOutlane));
                }

                if(topTowers.find(tower) != topTowers.end()) {
                    int board = topTowers[tower];
                    if(outlane == 8) {                                  // SSB
                        mod2outlanes[mod].insert(std::make_tuple(board, 16));
                    } else {                                            // AUX
                        mod2outlanes[mod].insert(std::make_tuple(board, outlane));
                    }
                }

                if(botTowers.find(tower) != botTowers.end()) {
                    int board = botTowers[tower];
                    if(outlane == 8) {                                  // SSB
                        mod2outlanes[mod].insert(std::make_tuple(board, 17));
                    } else {                                            // AUX
                        mod2outlanes[mod].insert(std::make_tuple(board, outlane + 18));
                    }
                }
            }
        }
    }
    return mod2outlanes;
}

// Sys file
// boardmap: Board# -> (TopTow, BotTow)
// plane2outlane: Plane -> Outbit(lane)
void parseSysFile(std::string sysFile, std::map<int, std::tuple<int, int>> &boardmap, std::map<int, int> &plane2outlane) {
    ifstream myfile(sysFile);
    std::string line;
    if(myfile.is_open()) {
        while( getline(myfile, line) ) {
            std::vector<std::string> tokens = split(line);
            if(tokens.size() < 3) continue;
            // Sort by Key (tokens[0]):
            if(tokens[0] == "BoardNToTopTower") {
                int boardNum = std::stoi(tokens[1], nullptr);
                if(boardmap.find(boardNum) == boardmap.end()) {
                    boardmap.insert(std::pair<int, std::tuple<int, int>>(boardNum, std::make_tuple(-1, -1)));
                }
                int towerNum = std::stoi(tokens[2], nullptr);
                int elem1 = std::get<1>(boardmap[boardNum]);
                boardmap[boardNum] = std::make_tuple(towerNum, elem1);

            } else if(tokens[0] == "BoardNToBotTower") {
                int boardNum = std::stoi(tokens[1], nullptr);
                if(boardmap.find(boardNum) == boardmap.end()) {
                    boardmap.insert(std::pair<int, std::tuple<int, int>>(boardNum, std::make_tuple(-1, -1)));
                }
                int towerNum = std::stoi(tokens[2], nullptr);
                int elem0 = std::get<0>(boardmap[boardNum]);
                boardmap[boardNum] = std::make_tuple(elem0, towerNum);

            } else if(tokens[0] == "PlaneToOutBit") {
                int planeNum = std::stoi(tokens[1], nullptr);
                int outBit = std::stoi(tokens[2], nullptr);
                plane2outlane[planeNum] = outBit;
            }
        }
    }
}

// Module list
// Return a map contaning all ROBIds -> tower number -> [ (modid, plane) ]
std::map<unsigned int, std::map<int, std::vector<std::tuple<int, int>> > > getROBmapFromModList(std::string moduleListFile) {
    std::map<unsigned int, std::map<int, std::vector<std::tuple<int, int>> > > ROBmap;
    std::string outString;
    std::string line;
    ifstream myfile(moduleListFile);
    if(myfile.is_open()) {
        while( getline(myfile, line) ) {
            outString = "";
            std::vector<std::string> tokens = split(line);
            if(tokens.size() < 4) continue;

            unsigned int robID = hexStringToInt(tokens[0]); // ROBId previously in hex
            int modID = stoi(tokens[1], nullptr); // modID in dec

            if(robID >= 0x200000) modID |= (0x1 << 15); // Add extra isSCT for sct modules

            unsigned long long tow_0_31_bits = hexStringToULL(tokens[2]); // tower map 0-31 in hex
            unsigned long long tow_32_63_bits = hexStringToULL(tokens[3]); // tower map 32-63 in hex
            int plane = stoi(tokens[4], nullptr); // plane in dec

            unsigned long long tow_all_bits = (tow_0_31_bits) | (tow_32_63_bits << 32);

//            if(ROBmap.find(robID) == ROBmap.end()) {
//                std::map<unsigned int, std::vector<std::tuple<int, int>>> empty;
//                ROBmap.insert(std::pair<unsigned int, std::map<int, std::vector<std::tuple<int, int>>>>(robID, empty));
//            }

            for(int itow = 0; itow < NUMBER_OF_FTK_TOWERS; itow++) {
                if((0x1 << itow) & tow_all_bits) {
                    if(ROBmap[robID].find(itow) == ROBmap[robID].end()) {
                        std::vector<std::tuple<int, int>> emptyVec;
                        ROBmap[robID].insert(std::pair<int, std::vector<std::tuple<int, int>>>(itow, emptyVec));
                    }
                    ROBmap[robID][itow].push_back(std::make_tuple(modID, plane));
                }
            }
        }

    } else {
        std::cerr << "Unable to open module list file" << std::endl;
    }
    return ROBmap;
}


/* ########################################
 * #          Advanced Testing            #
 * ######################################## */

// Returns absolute path to each spyfile
std::vector<std::string> getSpyFilesFromDirectory(const std::string& name) {
    std::vector<std::string> v;
    DIR* dirp = opendir(name.c_str());
    struct dirent * dp;
    while ((dp = readdir(dirp)) != NULL) {
        if(dp->d_name[0] == 'D') {
            // Construct absPath
            std::string absPath = name;
            if(name[name.size()-1] != '/') {
                absPath += "/";
            }
            absPath += dp->d_name;
            v.push_back(absPath);
        }
    }
    closedir(dirp);
    return v;
}

// Convert single spyfile to vector of unsigned ints (representing data)
std::vector<unsigned int> spyfileToVector(std::string spyfile) {
    std::vector<unsigned int> data;
    std::string line;
    ifstream myfile(spyfile);
    std::stringstream ss;
    if(myfile.is_open()) {
        while( getline(myfile, line) ) {
            unsigned int dataword = hexStringToInt(line);
            data.push_back(dataword);
        }
    } else {
        std::cerr << "Could not open spyfile." << std::endl;
    }
    return data;
}


// Returns string hex source ID sans the 0x.
std::string getSourceIDFromFilename(std::string filename) {
    return filename.substr(filename.find("0x") + 2, filename.find(".txt") - 5);
}

// Returns board number from string hex source ID
int getBoardFromSourceID(std::string sourceID) {
    return hexStringToInt(sourceID.substr(6, 8));
}

// Board internal bits: 31-24 of source ID (first two hex digit)
// Spytype: value is 0-35 for outspy, 36-52 for inspy,
// chnum: for outspy, chnum = value; inspy, chnum = value - 36
// i.e. value of 13 ---> outspy chnum #13; value of 38 ---> inspy chnum #2
std::tuple<int, bool> getLaneFromSourceID(std::string sourceID) {
    int value = hexStringToInt(sourceID.substr(0,2));
    bool isInspy;
    int chnum;
    if(value < 36) {
        isInspy = false;
        chnum = value;
    } else {
        isInspy = true;
        chnum = value - 36;
    }
    return std::make_tuple(chnum, isInspy);
}

/* ########################################
 * #          Parsing functions           #
 * ######################################## */

void parseNewOutputBuffer(std::vector<daq::ftk::EventFragmentHitClusters*> eventFragments,
                        std::map< std::tuple<int, int>, std::set<int>>& boardAndLaneToModIDsOutspy,
                        std::map< int, std::set<std::tuple<int, int>> >& modIDsToBoardOutLane,
                        std::map< ThreeTuple, std::queue<FiveTuple>>& queueMapInspy, // not passed in (private collection)
                        std::map< ThreeTuple, std::set<FiveTuple>>& setMapInspy, // not passed in (private collection)
                        std::map< TwoTuple, std::queue<ThreeTuple>>& queueMapOutspy, // not passed in (private collection)
                        std::map<ThreeTuple, daq::ftk::EventFragmentHitClusters*>& pointerMapOutspy, // not passed in (private collection)
                        int board, int outlane, int &expectedModulesOutspyCount, int& totalModulesOutspy, set<int>& uniqueModulesOutspy) // not passed in (get lane from source ID)
{
    std::set<int> uniqueBufferModules;
    for(auto eventFragment : eventFragments) {
        unsigned int L1ID = eventFragment->getL1ID();
        ThreeTuple packet = {board, outlane, L1ID}; // Packet for this loop
        bool packetInInspy = false;
        std::set<FiveTuple> ftSet;
        if(setMapInspy.find(packet) != setMapInspy.end()) {
            packetInInspy = true; // Determine if module check necessary
            std::set<FiveTuple> ftSet = setMapInspy[packet];
        }
        unsigned int nModules = eventFragment->getNModules();
        // Loop over all modules in each eventFragment in this buffer
        // Check 1: Check if this module is in the set of expected modules:
        std::set<int> expectedModules = boardAndLaneToModIDsOutspy[std::make_tuple(board, outlane)];

        // WE EXPECT
        std::cout << "=== WE EXPECT: ";
        std::cout << expectedModules.size() << " total modules. They are: ";
        for(int mod : expectedModules) {
            std::cout << mod << "  ";
        }
        std::cout << std::endl;
        // END WE EXPECT

        for(unsigned int m = 0; m < nModules; m++) {
            daq::ftk::ModuleHitClusters* mhc = eventFragment->getModule(m);
            int modNum = mhc->getModuleNumber();
            if(mhc->getModuleType() == 1) {
                modNum |= (0x1 << 15);
            }
            uniqueBufferModules.insert(modNum); // modules in buffer
            uniqueModulesOutspy.insert(modNum); // uniqueness
            totalModulesOutspy++;
            // === Outspy vs. Config ===
            if(expectedModules.find(modNum) != expectedModules.end()) {
                std::cout << "EXPECTED module " << modNum << " in OUTSPY board " << board << " and outlane " << outlane << std::endl;
                expectedModulesOutspyCount++;
            } else {
                // Perhaps add unexpected module counter
                std::cout << "Found module " << modNum << "     in OUTSPY board " << board <<
                             " and outlane " << outlane << ". Module was not expected (Outspy vs. Config failed)." << std::endl;
            }
            // === Outspy vs. Inspy ===
            if(packetInInspy && ftSet.size() != 0) {
                // Do module check - i.e. check if this module is actually showing up where it should:
                for(FiveTuple ft : ftSet) {
                    if(ft.moduleID == modNum) {
                        std::cout << "OUTSPY: Module check module found in outspy function" << std::endl;
                        auto iter = ftSet.find(ft);
                        ftSet.erase(iter);
                        break;
                    }
                }
            } else {
                //std::cout << "OUTSPY: Packet " << makePacketString(packet, true) << " received in outspy, not registered as requested in inspy. Adding to SEEN data structures." << std::endl;
                // Insert into "seen" map
                auto result = pointerMapOutspy.insert(std::pair<ThreeTuple, daq::ftk::EventFragmentHitClusters*>(packet, eventFragment));
                // SAFELY Add to the queue (just create separate function):
                TwoTuple* boardLane = new TwoTuple(board, outlane);
                queueMapOutspy[*boardLane].push(packet);
            }
            //delete mhc;
        }
        // Loops over the ModuleHitClusters has finished, therefore we clean up the inspy.
        // If it true that the packet was registered as requested in the inspy, now that the checks are done, we can remove from the inspy data structures.
        // 1. Remove from inspy map:
        setMapInspy.erase(packet);
        // 2. As much as possible, remove from Queue.
        TwoTuple boardLane = {board, outlane};
        if(queueMapOutspy[boardLane].front() == packet) {
            queueMapOutspy[boardLane].pop();
        }
        //delete eventFragment;
    }
    std::cout << "\nBUFFER MODULES (" << "Board " << board << ", Lane " << outlane << "):   " << std::endl;
    std::cout << uniqueBufferModules.size() << " unique modules:   " << std::endl;
    for(int mod : uniqueBufferModules) {
        std::cout << mod << " ";
    }
    std::cout << std::endl;
}

std::string makePacketString(ThreeTuple& t, bool isOutlane) {
   std::string p;
   std::string l;
   isOutlane ? l = "outlane" : l = "inlane";
   p = "(board " + to_string(t.boardNumber) + ", " + l + " " + to_string(t.laneNumber) + ", L1ID " + to_string(t.L1ID) + ")";
   return p;
}

/*
 * Function: parseNewInputBuffer
 * -----
 * When the DFSpyHandler is in operation, this function works in tandem with parseNewOutputBuffer
 * to continuously parse novel buffers and perform configuration checks on a per buffer basis.
 * The method for doing so can be described as the following:
 * The inspy queues keeps track of "check requests" -- in other words, the inspy queues
 * keeps track of the modules received in the inspy that we want to check if in the output (and
 * are not currently in the "seen" queue of the outspy).
 * The outspy queues, similarly, keep track of "modules seen" -- in other words, the outspy queues keep
 * track of modules that not have been requested for a check yet.
 */
void parseNewInputBuffer(std::vector<daq::ftk::EventFragmentHitClusters*> eventFragments,
                         std::map<std::tuple<int, int>, std::set<int>>& boardAndLaneToModIDsInspy,
                         std::map< int, std::set<std::tuple<int, int>> >& modID2BoardandLaneOutspy,
                         std::map< ThreeTuple, std::queue<FiveTuple> >& queueMapInspy, // not passed in (private collection)
                         std::map< ThreeTuple, std::set<FiveTuple> >& setMapInspy, // not passed in (private collection)
                         std::map< TwoTuple, std::queue<ThreeTuple> >& queueMapOutspy, // not passed in (private collection)
                         std::map<ThreeTuple, daq::ftk::EventFragmentHitClusters*>& pointerMapOutspy, // not passed in (private collection)
                         int board, int inlane, int& expectedModulesNum, int& totalModules, set<int>& uniqueModulesInspy) // not passed in (get lane from source ID)
{
    // Loop over event fragments:
    set<int> uniqueBufferModules;
    for(auto eventFragment : eventFragments) {
        bool packetSeen = false;
        unsigned int L1ID = eventFragment->getL1ID();
        unsigned int nModules = eventFragment->getNModules();
        // Loop over all modules in each eventFragment
        // == Inspy vs. Config == Check if this module is in the set of expected modules:
        std::set<int> expectedModules = boardAndLaneToModIDsInspy[std::make_tuple(board, inlane)];
        for(unsigned int m = 0; m < nModules; m++) {
            daq::ftk::ModuleHitClusters* mhc = eventFragment->getModule(m);
            int modNum = mhc->getModuleNumber();
            if(mhc->getModuleType() == 1) {
                modNum |= (0x1 << 15);
            }
            uniqueBufferModules.insert(modNum);
            uniqueModulesInspy.insert(modNum);
            totalModules++;
            if(expectedModules.find(modNum) != expectedModules.end()) {
                expectedModulesNum++;
                // == Inspy vs. Outspy ==
                // For this modID, we have to get all of its outlanes (in form of (board, outlane) pairs:
                std::set<std::tuple<int, int>> boardOutlanes = modID2BoardandLaneOutspy[modNum];
                // Construct a set of 5-tuples based on this:
                std::set<FiveTuple*> fivetuples;
                for(auto BLpair : boardOutlanes) {
                    int outlane = std::get<1>(BLpair);
                    FiveTuple* fivetuple = new FiveTuple(modNum, L1ID, board, outlane);
                    fivetuples.insert(fivetuple);
                }
                // Loop over the fivetuples pointers in the set
                for(auto ft : fivetuples) {
                    ThreeTuple packet = {ft->boardNumber, ft->outputLane, ft->L1ID};
                    // Now, check if this packet has been "seen" by the output buffer parser.
                    packetSeen = pointerMapOutspy.find(packet) != pointerMapOutspy.end();
                    if(packetSeen) {
                        // Module check
                        unsigned int nPacketModules = pointerMapOutspy[packet]->getNModules();
                        bool moduleFound = false;
                        for(unsigned int pm = 0; pm < nPacketModules; pm++) {
                            daq::ftk::ModuleHitClusters* pmhc = pointerMapOutspy[packet]->getModule(pm);
                            int checkMod = pmhc->getModuleNumber();
                            if(pmhc->getModuleType() == 1) checkMod |= (0x1 << 15);
                            if(checkMod == modNum) {
                                moduleFound = true;
                                std::cout << "INSPY: Module " << modNum << " from packet " << makePacketString(packet, true) << " registered as requested in inspy was successfully found in pointerMapOutspy." << std::endl;
                                break; // Module found.
                            }
                        }
                        if(moduleFound == false) {
                            std::cout << "For L1ID " << L1ID << ", output lane " << packet.laneNumber
                                      << " should have had module " << modNum << " from input lane " << inlane
                                      << ", but didn't!" << std::endl;
                        }
                        // Hitcluster check! Do this!
                        // After looping over all the modules, if the packet was seen, we remove form the pointerMapOutspy.
                        std::cout << "Packet seen, removing from pointerMapOutspy" << std::endl;
                        pointerMapOutspy.erase(packet);
                    } else {
                        // If the packet is not in the pointerMapOutspy, then it must be registered as a request.
                        // Thus, add fivetuple to the queue (safely, create func) and register its existence in the set map.
                        std::cout << "INSPY: Packet " << makePacketString(packet, true) << " found in inspy, not registered as seen in outspy. Adding to \"requested\" data structures." << std::endl;
                        queueMapInspy[packet].push(*ft);
                        setMapInspy[packet].insert(*ft);
                    }
                }
            } else {
                // Action: unexpected module (inspy) counter increases, change to ERS below
                std::cout << "L1ID: " << L1ID << " -- Found module " << modNum << " in INSPY board " << board
                          << " and inlane " << inlane << ". Module was not expected (Inspy vs. Config failed)." << std::endl;
            }
        }
    }
    std::cout << "\nBUFFER MODULES (" << "Board " << board << ", Lane " << inlane << "):   " << std::endl;
    std::cout << uniqueBufferModules.size() << " unique modules:   " << std::endl;
    for(int mod : uniqueBufferModules) {
        std::cout << mod << " ";
    }
    std::cout << std::endl;
}

/* ########################################
 * #      Look Up Table Construction      #
 * ######################################## */

// Inspy tables (using the Global CF (multiboard) file
std::map<int, bool> mapInspyBoardEnable(std::string multiboardFile) {
    std::map<int, bool> boardEnabled;
    std::string outString;
    std::string line;
    ifstream myfile(multiboardFile);
    if(myfile.is_open()) {
        while( getline(myfile, line) ) {
            std::vector<std::string> tokens = split(line);
            if(tokens[0] == "boardEnable") {
                int boardNum = std::stoi(tokens[1], nullptr);
                if(tokens[2] == "1") {
                    auto result = boardEnabled.insert(std::pair<int, bool> (boardNum, true));
                    result.second ? outString = "SUCCESS:    " : outString = "FAIL:     ";
                    std::cout << outString << "Board# -> Enabled?: Inserted key [ " << boardNum << " ] with value TRUE" << std::endl;
                } else {
                    auto result = boardEnabled.insert(std::pair<int, bool> (boardNum, false));
                    result.second ? outString = "SUCCESS:    " : outString = "FAIL:     ";
                    std::cout << outString << "Board# -> Enabled?: Inserted key [ " << boardNum << " ] with value FALSE" << std::endl;
                }
            }
        }
        myfile.close();
    } else {
        std::cerr << "Unable to open Global CF (multiboard) file" << std::endl;
    }
    return boardEnabled;
}

std::map<int, std::tuple<int, int>> mapROBIDtoBoardAndLane(std::string multiboardfile) {
    std::map<int, std::tuple<int, int>> m;
    std::string outString;
    std::string line;
    std::set<int> robIDset;
    ifstream myfile(multiboardfile);
    if(myfile.is_open()) {
        while( getline(myfile, line) ) {
            std::vector<std::string> tokens = split(line);
            if(tokens[0] == "rodToBoard") {
                int robID = hexStringToInt(tokens[1]);
                robIDset.insert(robID); // EDIT: REMOVE
                int boardNum = std::stoi(tokens[2], nullptr);
                int IMLane = std::stoi(tokens[3], nullptr);
                auto result = m.insert(std::pair<int, std::tuple<int, int>> (robID, std::make_tuple(boardNum, IMLane)));
                result.second ? outString = "SUCCESS:   " : outString = "FAIL:      ";
                std::cout << outString << "ROBID -> (Board, Lane): Inserted key [ " << robID << " ] with tuple " << "( "
                          << boardNum << " , " << IMLane << " )" << std::endl;
            }
        }
        myfile.close();
    } else {
        std::cerr << "Unable to open Global CF (multiboard) file" << std::endl;
    }
    // std::cout << "\n\n\n\n ROB ID SET SIZE FROM MULTIBOARD: " << robIDset.size() << "\n\n\n\n\n" << std::endl;
    return m;
}



//                  SYSTEM CF
std::map<std::tuple<int, int>, std::tuple<int, int>> mapTowerPlaneToBoardLane(std::string sysFile, std::map<int, bool> &boardEnabled) {
    // Declare maps:
    std::map<std::tuple<int, int>, std::tuple<int, int>> towerPlaneToBoardLane;
    std::map<int, int> towerToBoard;
    std::map<int, bool> towerToTop;
    std::map<int, int> planeToOutbit;
    std::string outString;
    // Parse file:
    std::string line;
    ifstream myfile(sysFile);
    if(myfile.is_open()) {
        while( getline(myfile, line) ) {
            std::vector<std::string> tokens = split(line);
            if(tokens.size() < 3) continue;
            // Sort by Key (tokens[0]):
            if(tokens[0] == "BoardNToTopTower") {
                int boardNum = std::stoi(tokens[1], nullptr);
                int towerNum = std::stoi(tokens[2], nullptr);
                // Tower to Top/Bottom (bool)
                auto result = towerToTop.insert(std::pair<int, bool> (towerNum, true));
                result.second ? outString =  "SUCCESS:   " : outString = "FAIL:      ";
                std::cout << outString << "Tower -> Board#: Adding key Tower " << towerNum << " with value TRUE" << std::endl;
                // Tower to Board#
                auto result2 = towerToBoard.insert(std::pair<int, int> (towerNum, boardNum));
                result2.second ? outString = "SUCCESS:   " : outString = "FAIL:      ";
                std::cout << outString << "Tower -> Board#: Adding key Tower " << towerNum << " with value Board " << boardNum << std::endl;

            } else if(tokens[0] == "BoardNToBotTower") {
                int boardNum = std::stoi(tokens[1], nullptr);
                int towerNum = std::stoi(tokens[2], nullptr);
                // Tower to Top/Bottom (bool)
                auto result = towerToTop.insert(std::pair<int, bool> (towerNum, false));
                result.second ? outString = "SUCCESS:   " : outString = "FAIL:      ";
                std::cout << outString << "Tower -> Board#: Adding key Tower " << towerNum << " with value FALSE" << std::endl;
                // Tower to Board#
                auto result2 = towerToBoard.insert(std::pair<int, int> (towerNum, boardNum));
                result2.second ? outString = "SUCCESS:   " : outString = "FAIL:      ";
                std::cout << outString << "Tower -> Board#: Adding key Tower " << towerNum << " with value Board " << boardNum << std::endl;

            } else if(tokens[0] == "PlaneToOutBit") {
                int planeNum = std::stoi(tokens[1], nullptr);
                int outBit = std::stoi(tokens[2], nullptr);
                auto result = planeToOutbit.insert(std::pair<int, int> (planeNum, outBit));
                result.second ? outString = "SUCCESS:   " : outString = "FAIL:        ";
                std::cout << outString << "Plane -> Outbit: Adding key plane " << planeNum << " with value outbit " << outBit << std::endl;
            }
        }
        // Now construct map
        std::cout << "\n TOWER PLANE TO BOARD LANE \n" << std::endl;
        int lane = 0;
        for(int tower = 0; tower <= 63; tower++) {
            int board = towerToBoard[tower];
            if(boardEnabled.find(board) == boardEnabled.end() || !boardEnabled[board]) {
                continue;
            }

            for(int plane = 0; plane <= 11; plane++) {
                bool topTowerBool = towerToTop[tower];
                int outbit = planeToOutbit[plane];
                if(topTowerBool) {
                    if(outbit == 8) {        // SSB
                        lane = 16;
                    } else {                 // AUX
                        lane = outbit;
                    }
                } else if(!topTowerBool) {
                    if(outbit == 8) {        // SSB
                        lane = 17;
                    } else {                 // AUX
                        lane = outbit + 18;
                    }
                }
                std::tuple<int, int> towerPlane = std::make_tuple(tower, plane);
                std::tuple<int, int> boardLane = std::make_tuple(board, lane);
                auto result = towerPlaneToBoardLane.insert(std::pair<std::tuple<int, int>, std::tuple<int, int>> (towerPlane, boardLane));
                result.second ? outString = "SUCCESS:   " : outString = "FAIL:     ";
                std::cout << outString << "(Tower, Plane) -> (Board, Lane): Added (Tower, Plane) key " <<
                             get2TupleString(towerPlane) << " with (Board, Lane) value " << get2TupleString(boardLane) << std::endl;
            }
        }

    } else {
        std::cerr << "Unable to open system config file" << std::endl;
    }
    return towerPlaneToBoardLane;
}

//                      MODULE LIST
// Edit: We eventually won't pass in the ROBId to Board,Lane map, it'll just be a member variable of the
// DFSpyHandler class.
std::map<std::tuple<int, int>, std::set<int>>
mapBoardAndLaneToModIDsInspy(std::string moduleListFile, std::map<int, std::tuple<int, int>> &robIDtoBoardLane) {
    std::map<std::tuple<int, int>, std::set<int>> boardAndLaneToModIDs;
    std::string outString;
    std::string line;
    ifstream myfile(moduleListFile);
    if(myfile.is_open()) {
        while( getline(myfile, line) ) {
            outString = "";
            std::vector<std::string> tokens = split(line);
            int robID = hexStringToInt(tokens[0]);
            int modID = stoi(tokens[1], nullptr);
            if(robID >= 0x200000) modID |= (0x1 << 15); // Add extra isSCT for sct modules
            if(robIDtoBoardLane.find(robID) == robIDtoBoardLane.end()) {
                // Modulelist contains all robIDs, so of course many will not be included in the multiboard file.
                continue;
            }
            std::tuple<int, int> boardAndLane = robIDtoBoardLane[robID];
            auto result = boardAndLaneToModIDs[boardAndLane].insert(modID);
            std::string boardLaneString = get2TupleString(boardAndLane);
            result.second ? outString = "SUCCESS:   " : outString = "FAIL:      ";
            std::cout << outString << "(Board, Lane) -> {mod IDs}: Found ROBID " << robID << " which maps to (Board, Lane) "
                      << boardLaneString << " which maps to mod ID " << modID << std::endl;
        }

    } else {
        std::cerr << "Unable to open module list file" << std::endl;
    }

    // Condensed format:
    std::cout << " \n CONDENSED FORMAT (BoardAndLane -> ModIDs Inspy) \n " << std::endl;
    for(std::map<std::tuple<int, int>, std::set<int>>::iterator itkey = boardAndLaneToModIDs.begin(); itkey != boardAndLaneToModIDs.end(); ++itkey) {
        std::tuple<int, int> key = itkey->first;
        std::cout << "For board " << std::get<0>(key) << " and inlane " << std::get<1>(key) << " we have " << boardAndLaneToModIDs[key].size() << " mods: " << std::endl;
        for(int mod : boardAndLaneToModIDs[key]) {
            std::cout << mod << ", ";
        }
        std::cout << "and STOP." << std::endl;

    }
    return boardAndLaneToModIDs;
}

std::map<std::tuple<int, int>, std::set<int>> mapBoardAndLaneModIDsOutspy(std::string moduleListFile, std::map<std::tuple<int, int>, std::tuple<int, int>> &towerPlaneToBoardLane) {
    std::map<std::tuple<int, int>, std::set<int>> boardAndLaneToModIDsOutspy;
    std::string outString;
    std::string line;
    ifstream myfile(moduleListFile);
    if(myfile.is_open()) {
        while( getline(myfile, line) ) {
            outString = "";
            std::vector<std::string> tokens = split(line);
            int robID = hexStringToInt(tokens[0]);
            int modID = std::stoi(tokens[1], nullptr);
            if(robID >= 0x200000) modID |= (0x1 << 15); // Add extra isSCT bit for sct modules
            unsigned long long tower_0_31_bits = hexStringToULL(tokens[2]);
            unsigned long long tower_32_63_bits = hexStringToULL(tokens[3]);
            int plane = std::stoi(tokens[4], nullptr);
            // Utilize tower bits
            unsigned long long tow_all_bits = (tower_0_31_bits) | (tower_32_63_bits << 32);
            for(int tow = 0; tow < NUMBER_OF_FTK_TOWERS; tow++) {
                if((0x1 << tow) & tow_all_bits) {
                    std::tuple<int, int> towerPlane = std::make_tuple(tow, plane);
                    if(towerPlaneToBoardLane.find(towerPlane) != towerPlaneToBoardLane.end()) {
                        std::tuple<int, int> boardLane = towerPlaneToBoardLane[towerPlane];
                        auto result = boardAndLaneToModIDsOutspy[boardLane].insert(modID);
                        result.second ? outString = "SUCCESS:   " : "FAIL:     ";
                        std::cout << outString << "Added (Board, Lane) pair " << get2TupleString(boardLane) << " with value mod ID:  " << modID << std::endl;
                    }
                }
            }
        }
    } else {
        std::cerr << "Unable to open module list file" << std::endl;
    }
    // Print keys
    for(std::map<std::tuple<int, int>, std::set<int>>::iterator itkey = boardAndLaneToModIDsOutspy.begin(); itkey != boardAndLaneToModIDsOutspy.end(); ++itkey) {
        std::tuple<int, int> key = itkey->first;
        std::cout << get2TupleString(key) << std::endl;

    }
    return boardAndLaneToModIDsOutspy;
}

std::map<int, std::set< std::tuple<int, int>>> mapModID2BoardAndLaneOutspy(std::map<std::tuple<int, int>, std::set<int>> &boardAndLaneToModIDsOutspy) {
    std::map<int, std::set<std::tuple<int, int>>> modID2BoardAndLaneOutspy;
    std::string outString = "";
    for(std::map<std::tuple<int, int>, std::set<int>>::iterator it = boardAndLaneToModIDsOutspy.begin(); it != boardAndLaneToModIDsOutspy.end(); ++it) {
        std::tuple<int, int> boardLane = it->first;
        std::set<int> modSet = it->second;
        for(std::set<int>::iterator itset = modSet.begin(); itset != modSet.end(); ++itset) {
            auto result = modID2BoardAndLaneOutspy[*itset].insert(boardLane);
            result.second ? outString = "SUCCESS:   " : outString = "FAIL:      ";
            std::cout << outString << "For module " << *itset << " (Board, Lane) pair " << get2TupleString(boardLane)
                      << " was added to set. Set is now size: " << modID2BoardAndLaneOutspy[*itset].size() << std::endl;
        }
    }
    return modID2BoardAndLaneOutspy;
}

/* ########################################
 * #              Utilities               #
 * ######################################## */


std::vector<std::string> split(std::string s) {
    stringstream A(s);
    std::vector<std::string> tokens;
    std::string t;
    while (A>>t) tokens.push_back(t);
    return tokens;
}

std::vector<std::string> tokenizeModListLine(std::string line) {
    std::vector<std::string> tokens;
    std::vector<std::string> rawTokens;
    rawTokens = split(line);
    // Due to the format of the moduleList file (even rawTokens correspond to the values we want)
    // odd rawTokens are spaces
    for(int i = 0; i < 5; i++) {
        tokens.push_back(rawTokens[2*i]);
    }
    return tokens;
}

unsigned int hexStringToInt(std::string hexString) {
    unsigned int x;
    std::stringstream ss;
    ss << std::hex << hexString;
    ss >> x;
    return x;
}

unsigned long long hexStringToULL(std::string hexString) {
    const char *str = hexString.c_str();
    char *end;
    unsigned long long result;
    errno = 0;
    result = strtoull(str, &end, 16);
    if (result == 0 && end == str) {
       std::cout <<  "/* str was not a number */" << std::endl;
    } else if (result == ULLONG_MAX && errno) {
        std::cout <<  "/* the value of str does not fit in unsigned long long */" << std::endl;
    } else if (*end) {
        std::cout <<  "/* str began with a number but has junk left over at the end */" << std::endl;
    }
    return result;
}



/* ########################################
 * #                Tests                 #
 * ######################################## */
//void eventFragTest() {
//    EventFragFTK* event = new EventFragFTK({1,2,3});
//    std::string L1ID = event->getL1ID();
//    std::cout << L1ID << std::endl;
//}

void splitTest(std::string testString) {
    std::vector<std::string> testVec;
    testVec = split(testString);
    for(size_t i = 0; i < testVec.size(); i++) {
        std::cout << testVec[i] << std::endl;
    }
}

void printMap(std::map<int, bool> myMap) {
    for(map<int, bool>::iterator it = myMap.begin(); it != myMap.end(); it++) {
        std::cout << it->first << " " << myMap[it->first] << std::endl;
    }
}

std::string get2TupleString(std::tuple<int, int> t) {
    std::string tuple2 = " ";
    tuple2 = "( "  + to_string(std::get<0>(t)) + ", " + to_string(std::get<1>(t)) + " )";
    return tuple2;
}

