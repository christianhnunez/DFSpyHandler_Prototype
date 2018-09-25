#include <iostream>
#include <fstream>
#include <fstream>
#include <sstream>
#include <string>
#include <tuple>
#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
#include <stdlib.h>
#include <algorithm>
// real
#include "/Users/ChristianHaroldNunez/Desktop/ftkcommon/ftkcommon/EventFragment.h"
#include "/Users/ChristianHaroldNunez/Desktop/ftkcommon/ftkcommon/EventFragmentFTKPacket.h"
#include "/Users/ChristianHaroldNunez/Desktop/ftkcommon/ftkcommon/EventFragmentHitClusters.h"
#include "/Users/ChristianHaroldNunez/Desktop/ftkcommon/ftkcommon/Utils.h"
#include "/Users/ChristianHaroldNunez/Desktop/ftkcommon/ftkcommon/Cluster.h"

#include "/Users/ChristianHaroldNunez/BufferParser/DFSpyHandler.h"
#include "/Users/ChristianHaroldNunez/BufferParser/Utils_dfspy.h"

// DFConstants (separate class) (include DF CONSTANTS!)
const int NUMBER_OF_FTK_TOWERS = 64;
const int INSPY_QUEUE_MAX_LENGTH = 250;
const int OUTSPY_QUEUE_MAX_LENGTH = 20;

/*
 * Constructor
 * ----
 * Requires three config files from FTKSpyHandler. Builds LUTs.
 */
DFSpyHandler::DFSpyHandler(std::string multiboard, std::string modulelist, std::string system_config) {
    buildLUTs(multiboard, modulelist, system_config);
    expectedModulesInspyCount = 0, totalModulesInspy = 0;
    expectedModulesOutspyCount = 0, totalModulesOutspy = 0;
}

// Destructor
//DFSpyHandler::~DFSpyHandler() {
//    // Not yet implemented
//}

// ======= Public functions:

void DFSpyHandler::performBitlevelChecks( const std::map< daq::ftk::SourceIDSpyBuffer, std::shared_ptr<daq::ftk::SpyBuffer> >& bufferMap) {
    // Not yet implemented, but has very simple implementation:
    // Iterate through keys of bufferMap, call parseNewInputBuffer or parseNewOutputBuffer on the vector of EventFragmentHitClusters!
    // Tester below uses text files to generate these vectors -- this is the function for the final product.
    return;
}

void DFSpyHandler::performBitLevelChecksTESTER(std::string dumpdir, bool parseDirectory, bool makeFrags, bool doParsing) {
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
            // std::tuple<int, bool> lane = getLaneFromSourceID(sourceID); // Replaced below:
            int absChannel = getAbsoluteChannelFromSourceID(sourceID);
            int lane;
            bool isInspy;
            if(absChannel < 36) {
                isInspy = false;
                lane = absChannel;
            } else {
                isInspy = true;
                lane = absChannel - 36;
            }
            std::cout << "Lane:         " << lane << ((isInspy) ? " (Inspy)" : " (Outspy)") << std::endl;
            if(makeFrags) {
                // We're making 4tups: (eventFrags, board, lane, isInspy)
                unsigned int ctrlPos = 16;
                std::vector<unsigned int> data = spyfileToVector(*itvec);
                std::cout << "data size:    " << data.size() << std::endl;
                std::cout << "== Errors/notices in EventFragment parsing of spyfile: ==" << std::endl;
                std::vector<std::shared_ptr<daq::ftk::EventFragmentHitClusters> > frags = FTKPacket_parseFragments<daq::ftk::EventFragmentHitClusters>(data, ctrlPos);
                std::cout << "== End errors/notices ==" << std::endl;
                std::cout << "# of frags:   " << frags.size() << std::endl;
                // Update frag stats:
                sumFrags += frags.size();
                if(frags.size() > maxFrags) maxFrags = frags.size();
                if(frags.size() < minFrags) minFrags = frags.size();
                // Prepare buffer vector:
                board = 5; // EDIT: HARDCODED BOARD
                std::sort(frags.begin(), frags.end(), dfspy::compareByL1ID);
                Buffer buffer = {frags, board, lane, isInspy};
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
            std::cout << "Preparing to parse " << buffers.size() << " buffers..." << std::endl;
            //INSPY THEN OUTSPY
            for(int i = 0; i < 52; i++) {
                if(i < 36) continue;
                Buffer buffer = buffers[i];
                std::string btype;
                buffer.isInspy ? btype = "INSPY" : btype = "OUTSPY";
                std::cout << "\n|------------------------------------------------------------------------------------------------------------|" << std::endl;
                std::cout << "|                                         NEXT BUFFER CHANNEL                                                |" << std::endl;
                std::cout << "|------------------------------------------------------------------------------------------------------------|" << std::endl;
                std::cout << "Buffer Channel:   " << btype << " Lane " << buffer.lane << " | frags: " << buffer.frags.size() << std::endl;
                if(!buffer.isInspy) {
                    parseNewOutputBuffer(buffer.frags, 5, buffer.lane); // Note the board is hardcoded as 5 because of Lab4 Readout issues.
                } else {
                    parseNewInputBuffer(buffer.frags, 5, buffer.lane);
                }
                std::cout << "Buffer Channel:   " << btype << " Lane " << buffer.lane << " parsing complete!" << std::endl;
            }

            for(int i = 0; i < 52; i++) {
                if((i > 7 && i < 16) || (i > 25 && i < 36) || i > 35 || i == 19) continue;
                Buffer buffer = buffers[i];
                std::string btype;
                buffer.isInspy ? btype = "INSPY" : btype = "OUTSPY";
                std::cout << "\n|------------------------------------------------------------------------------------------------------------|" << std::endl;
                std::cout << "|                                         NEXT BUFFER CHANNEL                                                |" << std::endl;
                std::cout << "|------------------------------------------------------------------------------------------------------------|" << std::endl;
                std::cout << "\n Buffer Channel:   " << btype << " Lane " << buffer.lane << " | frags: " << buffer.frags.size() << std::endl;
                if(!buffer.isInspy) {
                    parseNewOutputBuffer(buffer.frags, 5, buffer.lane);
                } else {
                    parseNewInputBuffer(buffer.frags, 5, buffer.lane);
                }
                std::cout << "Buffer Channel:   " << btype << " Lane " << buffer.lane << " parsing complete!" << std::endl;
            }

            std::cout << "\n\n\n\n BUFFER PARSING STATISTICS \n\n";
            double percentageOutspy = ((double)expectedModulesOutspyCount/totalModulesOutspy) * 100;
            std::cout << "OUTSPY:     " << percentageOutspy << "% (" << expectedModulesOutspyCount << " / " << totalModulesOutspy << ") of expected modules checks were successful." << std::endl;
            double percentageInspy = ((double)expectedModulesInspyCount/totalModulesInspy) * 100;
            std::cout << "INSPY:      " << percentageInspy << "% (" << expectedModulesInspyCount << " / " << totalModulesInspy << ") of expected modules checks were successful. \n" << std::endl;
            std::cout << "Queue Map Inspy Size:         " << queueMapInspy.size() << std::endl;
            displayContentsQueueMapInspy();
            std::cout << "Set Map Inspy Size:           " << setMapInspy.size() << "\n" << std::endl;
            std::cout << "Queue Map Outspy Size:        " << queueMapOutspy.size() << std::endl;
            displayContentsQueueMapOutspy();
            std::cout << "Pointer Map Outspy Size:      " << pointerMapOutspy.size() << "\n" << std::endl;
            std::cout << "Unique modules inspy:         " << uniqueModulesInspy.size() << std::endl;
            std::cout << "Unique modules outspy:        " << uniqueModulesOutspy.size() << std::endl;

            int counter = 0;
            std::cout << "\nSET MAP INSPY KEYS:  " << std::endl;
            for(std::map<ThreeTuple, std::set<FiveTuple> >::iterator iter = setMapInspy.begin(); iter != setMapInspy.end(); ++iter) {
                ThreeTuple pack = iter->first;
                std::cout << makePacketString(pack, true) << std::endl;
                counter++;
            }
            std::cout << "total count: " << counter << std::endl;

            counter = 0;
            std::cout << "\nPOINTER MAP OUTSPY KEYS:  " << std::endl;
            for(std::map<ThreeTuple, std::shared_ptr<daq::ftk::EventFragmentHitClusters> >::iterator iter = pointerMapOutspy.begin(); iter != pointerMapOutspy.end(); ++iter) {
                ThreeTuple pack = iter->first;
                std::cout << makePacketString(pack, true) << std::endl;
                counter++;
            }
            std::cout << "total count: " << counter << std::endl;
        }
    }
}


// ======= Private functions:
/* ########################################
 * #        Main Parsing Functions        #
 * ######################################## */
void DFSpyHandler::parseNewInputBuffer(std::vector<std::shared_ptr<daq::ftk::EventFragmentHitClusters> > eventFragments, int board, int inlane) {
    // Loop over event fragments:
    std::set<int> uniqueBufferModules;
    for(size_t e = 0; e < eventFragments.size(); e++) {
        std::shared_ptr<daq::ftk::EventFragmentHitClusters> eventFragment = eventFragments[e];
        std::set<ThreeTuple> packetsToRequest;
        bool packetInOutspy = false;
        unsigned int L1ID = eventFragment->getL1ID();
        std::cout << "\n**** Analyzing EventFragment in **Board " << board << " **Inlane " << inlane << " **L1ID " << L1ID << std::endl;
        unsigned int nModules = eventFragment->getNModules();
        // Loop over all modules in each eventFragment
        std::set<int> expectedModules = boardAndLaneToModIDsInspy[dfspy::make_tuple(board, inlane)];
        for(unsigned int m = 0; m < nModules; m++) {
            std::shared_ptr<daq::ftk::ModuleHitClusters> mhc = eventFragment->getModule(m);
            int modNum = mhc->getModuleNumber();
            if(mhc->getModuleType() == 1) modNum |= (0x1 << 15);
            uniqueBufferModules.insert(modNum);
            uniqueModulesInspy.insert(modNum);
            totalModulesInspy++;
             // == Inspy vs. Config == Check if this module is in the set of expected modules:
            if(expectedModules.find(modNum) != expectedModules.end()) {
                expectedModulesInspyCount++;
                // == Inspy vs. Outspy ==
                // For this modID, we have to get all of its outlanes (in form of (board, outlane) pairs:
                std::set<std::vector<int> > boardOutlanes = modID2BoardandLaneOutspy[modNum];
                // Construct a set of 5-tuples based on this:
                std::set<FiveTuple*> fivetuples;
                for(auto BLpair : boardOutlanes) {
                    int outlane = BLpair[1];
                    FiveTuple* fivetuple = new FiveTuple(modNum, L1ID, board, outlane, mhc);
                    fivetuples.insert(fivetuple);
                }
                // Loop over the fivetuples pointers in the set
                for(auto ft : fivetuples) {
                    ThreeTuple packet = {ft->boardNumber, ft->outputLane, ft->L1ID};
                    // Now, check if this packet has been "seen" by the output buffer parser.
                    packetInOutspy = (pointerMapOutspy.count(packet) > 0);
                    if(packetInOutspy) {
                        performModAndHitClusterCheckInspy(packet, modNum, mhc);
                    } else {
                        registerPacketAsRequest(packet, packetsToRequest, ft);
                    }
                }
            } else {
                // Action: unexpected module (inspy) counter increases, change to ERS below
                std::cout << "(X) INSPY: L1ID: " << L1ID << " -- Found module " << modNum << " in INSPY board " << board
                          << " and inlane " << inlane << ". Module was not expected (Inspy vs. Config failed)." << std::endl;
            }
        } // End module loop
    } // End event fragment loop
    printBonusBufferStats(board, inlane, uniqueBufferModules);
}


void DFSpyHandler::parseNewOutputBuffer(std::vector<std::shared_ptr<daq::ftk::EventFragmentHitClusters> > eventFragments, int board, int outlane) {

    std::set<int> uniqueBufferModules;
    for(auto eventFragment : eventFragments) {
        std::set<FiveTuple> checkedFiveTuples;
        checkedFiveTuples.clear();
        unsigned int L1ID = eventFragment->getL1ID();
        std::cout << "\n**** Analyzing EventFragment in **Board " << board << " **Outlane " << outlane << " **L1ID " << L1ID << std::endl;
        ThreeTuple packet = {board, outlane, L1ID}; // Packet identifier for this loop
        bool packetInInspy = false;
        if(setMapInspy.count(packet) > 0) {
            std::cout << "(++) OUTSPY INT:" << makePacketString(packet, true) << " found in INSPY setMapInspy (keys: " << setMapInspy.count(packet) << ")" << std::endl;
            packetInInspy = true; // Determine if module check necessary
        }
        unsigned int nModules = eventFragment->getNModules();
        // Check 1: Check if this module is in the set of expected modules:
        std::set<int> expectedModules = boardAndLaneToModIDsOutspy[dfspy::make_tuple(board, outlane)];
        for(unsigned int m = 0; m < nModules; m++) {
            std::shared_ptr<daq::ftk::ModuleHitClusters> mhc = eventFragment->getModule(m);
            int modNum = mhc->getModuleNumber();
            if(mhc->getModuleType() == 1) {
                modNum |= (0x1 << 15);
            }
            uniqueBufferModules.insert(modNum); // modules in buffer
            uniqueModulesOutspy.insert(modNum); // uniqueness
            totalModulesOutspy++;

            // === Outspy vs. Config ===
            if(expectedModules.find(modNum) != expectedModules.end()) {
                //std::cout << "EXPECTED module " << modNum << " in OUTSPY board " << board << " and outlane " << outlane << std::endl;
                expectedModulesOutspyCount++;
            } else {
                // Perhaps add unexpected module counter
                std::cout << "(X) OUTSPY: Found module " << modNum << "     in OUTSPY board " << board <<
                             " and outlane " << outlane << ". Module was not expected (Outspy vs. Config failed)." << std::endl;
            }

            // === Outspy vs. Inspy === (if possible)
            if(packetInInspy) {
                if(setMapInspy[packet].size() != 0) {
                    // Do module check - i.e. check if this module is actually showing up where it should:
                    for(FiveTuple ft : setMapInspy[packet]) {
                        if(ft.moduleID == modNum) {
                            std::cout << "(+) OUTSPY VS. INSPY: (Module check) module " << modNum << " found in outspy function. setMapInspy[" << makePacketString(packet, true) << "] was size " << setMapInspy[packet].size();
                            //auto iter = setMapInspy[packet].find(ft);
                            //setMapInspy[packet].erase(iter);
                            std::cout << " now size " << setMapInspy[packet].size() << std::endl;

                            // Hit Clusters check:
                            if(compareModuleHitClusters(ft.mHC, mhc)) {
                                std::cout << "(HC+) Hit clusters check successful for comparison of module " << modNum << " in inspy and outspy!" << std::endl;
                            } else {
                                std::cout << "(HC-) Hit clusters check failed for comparison of module " << modNum << " in inspy and outspy!" << std::endl;
                            }

                            // FiveTuples of setMapInspy[packet] that have been checked
                            checkedFiveTuples.insert(ft);
                            break;
                        }
                    }
                }
            }
        } // End module loop for this event fragment

        //If the packet is not yet registered in the inspy, we must add to SEEN data structures.
        if(!packetInInspy) {
            std::cout << "(-) OUTSPY VS. INSPY: Packet " << makePacketString(packet, true) << " received in outspy, not registered as requested in inspy. Adding to SEEN data structures." << std::endl;
            std::string ots;
            auto result = pointerMapOutspy.insert(std::pair<ThreeTuple, std::shared_ptr<daq::ftk::EventFragmentHitClusters> >(packet, eventFragment));
            result.second ? ots = "YES: " : ots = "NO:  ";
            std::cout << ots << "insertion of packet " << makePacketString(packet, true) << " with shared_ptr<EventFragmentHitClusters>) pair into pointerMapOutspy " << std::endl;
            TwoTuple boardLane = {board, outlane};
            safePushOutspy(boardLane, packet);
        }

         //If the packet was registered as requested in the inspy, now that the checks are done, we can remove the inspy data structures.
        TwoTuple boardLane = {board, outlane};
        if(packetInInspy) {
            // Clean the queue for this board and lane
            while(setMapInspy[packet].find(queueMapInspy[boardLane].front()) != setMapInspy[packet].end()) {
                FiveTuple popped = queueMapInspy[boardLane].front();
                int size = queueMapInspy[boardLane].size();
                queueMapInspy[boardLane].pop();
                std::cout << "(M) pNOB: FiveTuple of mod " << popped.moduleID << " removed from the queue queueMapInspy[Board " << boardLane.boardNumber << ", Lane " << boardLane.laneNumber << "]"
                                          << " which was length " << size << " and is now length " << queueMapInspy[boardLane].size() << std::endl;
                if(queueMapInspy[boardLane].size() == 0) break;
            }

            // 2. Remove packet from inspy map: // REMEMBER, FIX
            int modDifference = checkedFiveTuples.size() - setMapInspy[packet].size();
            std::cout << "(S) pNOB: Erasing packet key " << makePacketString(packet, true) << " from setMapInspy. Missed checks: " << modDifference << std::endl;
            auto it = setMapInspy.find(packet);
            if(it != setMapInspy.end()) {
                setMapInspy.erase(it);
            }

            // Clean up outspy (add bool)
            TwoTuple boardLane = {board, outlane};
            initialOutspyCleaner(boardLane, packet);

        }
    } // End EventFragment loop
    printBonusBufferStats(board, outlane, uniqueBufferModules);
}

/* ########################################
 * #      Look Up Table Construction      #
 * ######################################## */

void DFSpyHandler::buildLUTs(std::string multiboard, std::string modulelist, std::string system_config) {
    std::cout << "\n\n\n\n START: \n\n\n\n";
    std::cout << "======== CONFIG FILES ========" << std::endl;
    std::cout << "MODULE LIST:  " << modulelist << std::endl;
    std::cout << "MULITBOARD:   " << multiboard << std::endl;
    std::cout << "SYSTEM:       " << system_config << std::endl;
    std::cout << "==============================\n" << std::endl;
    std::cout << "======== JUMP TO CONTENTS ========" << std::endl;
    std::cout << "[P1] BOARD -> ENABLED?" << std::endl;
    std::cout << "[P2] ROBID -> (Board, Lane)" << std::endl;
    std::cout << "[P3] (Board, Lane) -> {inspy modules}" << std::endl;
    std::cout << "[P4] (Tower, Plane) -> (Board, Lane)" << std::endl;
    std::cout << "[P5] (Board, Lane) -> {outspy modules}" << std::endl;
    std::cout << "[P6] Outspy module # -> {(Board, Lane)}" << std::endl;
    std::cout << "==============================" << std::endl;

    std::cout << "\n\n\n\n NEXT: [P1] BOARD -> ENABLED? \n\n\n\n";
    boardEnabled = mapInspyBoardEnable(multiboard);
    std::cout << "\n\n\n\n NEXT: [P2] ROBID -> (Board, Lane) \n\n\n\n";
    ROBIDtoBoardAndLane = mapROBIDtoBoardAndLane(multiboard);
    std::cout << "\n\n\n\n NEXT: [P3] (Board, Lane) -> {inspy modules} \n\n\n\n";
    boardAndLaneToModIDsInspy = mapBoardAndLaneToModIDsInspy(modulelist);
    std::cout << "\n\n\n\n NEXT: [P4] (Tower, Plane) -> (Board, Lane) \n\n\n\n";
    towerPlaneToBoardLane = mapTowerPlaneToBoardLane(system_config);
    std::cout << "\n\n\n\n NEXT: [P5] (Board, Lane) -> {outspy modules} \n\n\n\n";
    boardAndLaneToModIDsOutspy = mapBoardAndLaneModIDsOutspy(modulelist);
    std::cout << "\n\n\n\n NEXT: [P6] Outspy module # -> {(Board, Lane)} \n\n\n\n";
    modID2BoardandLaneOutspy = mapModID2BoardAndLaneOutspy();
    std::cout << "LUT Table Construction Complete" << std::endl;
}

/*
 * Function: mapInspyBoardEnable
 * -----
 * Returns Board -> Enabled? (bool)
 */
std::map<int, bool> DFSpyHandler::mapInspyBoardEnable(std::string multiboardFile) {
    std::map<int, bool> boardEnabled;
    std::string outString;
    std::string line;
    std::ifstream myfile(multiboardFile);
    if(myfile.is_open()) {
        while(std::getline(myfile, line) ) {
            std::vector<std::string> tokens = dfspy::split(line);
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

/*
 * Function: mapROBIDtoBoardAndLane
 * -----
 * Returns ROBID -> (Board, Inlane)
 */
std::map<int, std::vector<int> > DFSpyHandler::mapROBIDtoBoardAndLane(std::string multiboardfile) {
    std::map<int, std::vector<int> > m;
    std::string outString;
    std::string line;
    std::ifstream myfile(multiboardfile);
    if(myfile.is_open()) {
        while(std::getline(myfile, line) ) {
            std::vector<std::string> tokens = dfspy::split(line);
            if(tokens[0] == "rodToBoard") {
                int robID = dfspy::hexStringToInt(tokens[1]);
                int boardNum = std::stoi(tokens[2], nullptr);
                int IMLane = std::stoi(tokens[3], nullptr);
                auto result = m.insert(std::pair<int, std::vector<int> > (robID, dfspy::make_tuple(boardNum, IMLane)));
                result.second ? outString = "SUCCESS:   " : outString = "FAIL:      ";
                std::cout << outString << "ROBID -> (Board, Lane): Inserted key [ " << robID << " ] with tuple " << "( "
                          << boardNum << " , " << IMLane << " )" << std::endl;
            }
        }
        myfile.close();
    } else {
        std::cerr << "Unable to open Global CF (multiboard) file" << std::endl;
    }
    return m;
}

/*
 * Function: mapTowerPlaneToBoardLane
 * -----
 * Returns (Tower, Plane) -> (Board, Outlane)
 */
std::map<std::vector<int>, std::vector<int> > DFSpyHandler::mapTowerPlaneToBoardLane(std::string sysFile) {
    // Declare maps:
    std::map< std::vector<int>, std::vector<int> > towerPlaneToBoardLane;
    std::map<int, int> towerToBoard;
    std::map<int, bool> towerToTop;
    std::map<int, int> planeToOutbit;
    std::string outString;
    // Parse file:
    std::string line;
    std::ifstream myfile(sysFile);
    if(myfile.is_open()) {
        while(std::getline(myfile, line) ) {
            std::vector<std::string> tokens = dfspy::split(line);
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
                std::vector<int> towerPlane = dfspy::make_tuple(tower, plane);
                std::vector<int> boardLane = dfspy::make_tuple(board, lane);
                auto result = towerPlaneToBoardLane.insert(std::pair<std::vector<int> , std::vector<int> > (towerPlane, boardLane));
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

/*
 * Function:
 * ----
 * Returns (Board, Inlane) -> {mod IDs}
 */
std::map<std::vector<int>, std::set<int> > DFSpyHandler::mapBoardAndLaneToModIDsInspy(std::string moduleListFile) {
    std::map<std::vector<int>, std::set<int> > boardAndLaneToModIDs;
    std::string outString;
    std::string line;
    std::ifstream myfile(moduleListFile);
    if(myfile.is_open()) {
        while(std::getline(myfile, line) ) {
            outString = "";
            std::vector<std::string> tokens = dfspy::split(line);
            int robID = dfspy::hexStringToInt(tokens[0]);
            int modID = std::stoi(tokens[1], nullptr);
            if(robID >= 0x200000) modID |= (0x1 << 15); // Add extra isSCT for sct modules
            if(ROBIDtoBoardAndLane.find(robID) == ROBIDtoBoardAndLane.end()) {
                // Modulelist contains all robIDs, so of course many will not be included in the multiboard file.
                continue;
            }
            std::vector<int> boardAndLane = ROBIDtoBoardAndLane[robID];
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
    for(std::map<std::vector<int>, std::set<int> >::iterator itkey = boardAndLaneToModIDs.begin(); itkey != boardAndLaneToModIDs.end(); ++itkey) {
        std::vector<int> key = itkey->first;
        std::cout << "For board " << key[0] << " and inlane " << key[1] << " we have " << boardAndLaneToModIDs[key].size() << " mods: " << std::endl;
        for(int mod : boardAndLaneToModIDs[key]) {
            std::cout << mod << ", ";
        }
        std::cout << "and STOP." << std::endl;

    }
    return boardAndLaneToModIDs;
}


/*
 * Function: mapBoardAndLaneModIDsOutspy
 * ---
 * Returns (Board, Outlane) -> {mod IDs}
 */
std::map<std::vector<int>, std::set<int> > DFSpyHandler::mapBoardAndLaneModIDsOutspy(std::string moduleListFile) {
    std::map<std::vector<int>, std::set<int> > boardAndLaneToModIDsOutspy;
    std::string outString;
    std::string line;
    std::ifstream myfile(moduleListFile);
    if(myfile.is_open()) {
        while(std::getline(myfile, line) ) {
            outString = "";
            std::vector<std::string> tokens = dfspy::split(line);
            int robID = dfspy::hexStringToInt(tokens[0]);
            int modID = std::stoi(tokens[1], nullptr);
            if(robID >= 0x200000) modID |= (0x1 << 15); // Add extra isSCT bit for sct modules
            unsigned long long tower_0_31_bits = dfspy::hexStringToULL(tokens[2]);
            unsigned long long tower_32_63_bits = dfspy::hexStringToULL(tokens[3]);
            int plane = std::stoi(tokens[4], nullptr);
            // Utilize tower bits
            uint64_t tow_all_bits = (tower_0_31_bits) | (tower_32_63_bits << 32);
            for(int tow = 0; tow < NUMBER_OF_FTK_TOWERS; tow++) {
                if((0x1 << tow) & tow_all_bits) {
                    std::vector<int> towerPlane = dfspy::make_tuple(tow, plane);
                    if(towerPlaneToBoardLane.find(towerPlane) != towerPlaneToBoardLane.end()) {
                        std::vector<int> boardLane = towerPlaneToBoardLane[towerPlane];
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
    for(std::map<std::vector<int>, std::set<int> >::iterator itkey = boardAndLaneToModIDsOutspy.begin(); itkey != boardAndLaneToModIDsOutspy.end(); ++itkey) {
        std::vector<int> key = itkey->first;
        std::cout << get2TupleString(key) << std::endl;

    }
    return boardAndLaneToModIDsOutspy;
}

/*
 * Function: mapModID2BoardAndLaneOutspy()
 * ----
 * Returns map: modID -> {(Board, Outlane)}
 */
std::map<int, std::set< std::vector<int> > > DFSpyHandler::mapModID2BoardAndLaneOutspy() {
    std::map<int, std::set<std::vector<int> > > modID2BoardAndLaneOutspy;
    std::string outString = "";
    for(std::map<std::vector<int>, std::set<int> >::iterator it = boardAndLaneToModIDsOutspy.begin(); it != boardAndLaneToModIDsOutspy.end(); ++it) {
        std::vector<int> boardLane = it->first;
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
 * #        Parsing Helper Functions      #
 * ######################################## */

void DFSpyHandler::printBonusBufferStats(int board, int lane, std::set<int>& uniqueBufferModules) {
    std::cout << "\nBUFFER MODULES (" << "Board " << board << ", Lane " << lane << "):   " << std::endl;
    std::cout << uniqueBufferModules.size() << " unique modules:   " << std::endl;
    for(int mod : uniqueBufferModules) {
        std::cout << mod << " ";
    }
    std::cout << std::endl;
}

void DFSpyHandler::safePushInspy(TwoTuple &boardLane, FiveTuple &ft)
{
    if(queueMapInspy[boardLane].size() >= INSPY_QUEUE_MAX_LENGTH) {
        // Then, pop and check.
        bool warning = false;
        while(warning == false) {
            FiveTuple popped = queueMapInspy[boardLane].front();
            ThreeTuple packetFromFT = {popped.boardNumber, popped.outputLane, popped.L1ID};
            if(setMapInspy.count(packetFromFT) > 0) {
                // When something is inserted into the setMapInspy, it is also inserted into the queueMapInspy.
                // When Inspy vs. Outspy occurs, the packet key in setMapInspy is definitely erased, but the corresponding FiveTuples may remain in the queueMapInspy.
                // Thus, if the packet to be popped exists in setMapInspy and pointerMapOutspy, we should perform an Inspy vs. Outspy check and shorten the queue as much as possible.

                // Check if the packet to be popped exists in the pointerMapOutspy
                if(pointerMapOutspy.count(packetFromFT) > 0) {
                    performModAndHitClusterCheckInspy(packetFromFT, popped.moduleID, popped.mHC);
                } else {
                    std::cout << "Warning: FiveTuple popped from queueMapInspy[" << boardLane.boardNumber << ", " << boardLane.laneNumber << "] due to overflow while the setMapInspy still contains its packet information and the pointerMapOutspy." << std::endl;
                    warning = true;
                }
            }
            queueMapInspy[boardLane].pop(); // REMEMBER: FIX THIS POP.
        }
    }
    queueMapInspy[boardLane].push(ft);
}

void DFSpyHandler::safePushOutspy(TwoTuple &boardLane, ThreeTuple &packet) {
    if(queueMapOutspy[boardLane].size() >= OUTSPY_QUEUE_MAX_LENGTH) {
        ThreeTuple popped = queueMapOutspy[boardLane].front();
        // Delete popped from pointerMapOutspy
        if(pointerMapOutspy.count(popped) > 0) pointerMapOutspy.erase(popped);
        // Now pop from the queue
        queueMapOutspy[boardLane].pop();
    }
    queueMapOutspy[boardLane].push(packet);
}

void DFSpyHandler::performModAndHitClusterCheckInspy(ThreeTuple &packet, int modNum, std::shared_ptr<daq::ftk::ModuleHitClusters> &mhc)
{
        // Module check
        unsigned int nPacketModules = pointerMapOutspy[packet]->getNModules();
        bool moduleFound = false;
        for(unsigned int pm = 0; pm < nPacketModules; pm++) {
            std::shared_ptr<daq::ftk::ModuleHitClusters> pmhc = pointerMapOutspy[packet]->getModule(pm);
            int checkMod = pmhc->getModuleNumber();
            if(pmhc->getModuleType() == 1) checkMod |= (0x1 << 15);
            if(checkMod == modNum) {
                moduleFound = true;
                std::cout << "(P +) INSPY VS. OUTSPY: Module " << modNum << " from packet " << makePacketString(packet, true) << " registered as requested in inspy was successfully found in pointerMapOutspy." << std::endl;
                // Hit Clusters check:
                if(compareModuleHitClusters(mhc, pmhc)) {
                    std::cout << "(P HC+) Hit clusters match for module ID " << modNum << "!" << std::endl;
                } else {
                    std::cout << "(P HC-) hit clusters do not match between the ModuleHitCluster objects with the same module ID: " << modNum << std::endl;
                }
                break;
            }
        }
        if(moduleFound == false) {
            std::cout << "(P -) INSPY VS. OUTSPY: For L1ID " << packet.L1ID << ", output lane " << packet.laneNumber
                      << " should have had module " << modNum
                      << ", but didn't!" << std::endl;
        }
}

void DFSpyHandler::registerPacketAsRequest(ThreeTuple &packet, std::set<ThreeTuple> &packetsToRequest, FiveTuple* &ft) {
    // If the packet is not in the pointerMapOutspy, then it and its set of FiveTuples must be registed as a request.
    if(packetsToRequest.find(packet) == packetsToRequest.end()) {
        packetsToRequest.insert(packet);
        std::cout << "(-) INSPY VS. OUTSPY: Packet " << makePacketString(packet, true) << " found in inspy, which was not registered as seen in outspy. Adding to \"requested\" data structures." << std::endl;
    }
    // Now, add fivetuple to the queue (safely, create func) and register its existence in the set map.
    TwoTuple boardLane = {ft->boardNumber, ft->outputLane};
    int size = queueMapInspy[boardLane].size();
    safePushInspy(boardLane, *ft);
    std::cout << "(M) INSPY VS. OUTSPY: queueMapInspy[Board " << boardLane.boardNumber << ", " << boardLane.laneNumber << "] was size " << size << " and is now size " << queueMapInspy[boardLane].size() << std::endl;
    auto res = setMapInspy[packet].insert(*ft);
}

// QUEUE MAP OUTSPY CLEANER FOR FIRST DUMP: This is more of a cleanup for the queueMapOutspy. It continually looks for the packet or packets of smaller L1ID to clear from the queueMapOutspy.
// The reason to clear packets of smaller L1ID initially is because in that board&outlane in the queueMapOutspy, no L1IDs will be coming in that are
// smaller than that one.
void DFSpyHandler::initialOutspyCleaner(TwoTuple &boardLane, ThreeTuple &packet)
{
    std::cout << "Starting one-time outspy cleaner function" << std::endl;
    while(queueMapOutspy[boardLane].size() != 0 && (queueMapOutspy[boardLane].front() == packet || packet.L1ID > queueMapOutspy[boardLane].front().L1ID)) {
        ThreeTuple popped = queueMapOutspy[boardLane].front();
        int size = queueMapOutspy[boardLane].size();
        queueMapOutspy[boardLane].pop();
        pointerMapOutspy.erase(popped);
        std::cout << "(M) pNOB: Packet " << makePacketString(popped, true) << " removed from the queue queueMapOutspy[Board " << boardLane.boardNumber << ", Lane " << boardLane.laneNumber << "]"
                  << " which was length " << size << " and is now length " << queueMapOutspy[boardLane].size() << std::endl;
    }
}

/* ########################################
 * #       Check-Specific Helpers         #
 * ######################################## */
bool DFSpyHandler::compareModuleHitClusters(std::shared_ptr<daq::ftk::ModuleHitClusters>& mhc1, std::shared_ptr<daq::ftk::ModuleHitClusters>& mhc2) {
    if(mhc1->getModuleType() != mhc2->getModuleType()) return false;
    if(mhc1->getNClusters() != mhc2->getNClusters()) {
        std::cout << "Unequal amount of clusters. Hit cluster check failed. ";
        return false;
    }
    for(unsigned int i = 0; i < mhc1->getNClusters(); i++) {
        if(mhc1->getModuleType()) {
            if(!(compareSCTClusters(mhc1->getSctCluster(i), mhc2->getSctCluster(i)))) return false;
        } else {
            if(!(comparePixelClusters(mhc1->getPixelCluster(i), mhc2->getPixelCluster(i)))) return false;
        }
    }
    return true;
}

bool DFSpyHandler::compareSCTClusters(daq::ftk::SCTCluster* a, daq::ftk::SCTCluster* b) {
    return (a->hit_coord() == b->hit_coord() &&
            a->hit_coord() == b->hit_coord());
}

bool DFSpyHandler::comparePixelClusters(daq::ftk::PixelCluster* a, daq::ftk::PixelCluster* b) {
    return  (a->column_width()  == b->column_width()  &&
             a->column_coord()  == b->column_coord()  &&
             a->split_cluster() == b->split_cluster() &&
             a->row_width()     == b->row_width()     &&
             a->row_coord()     == b->row_coord());
}



/* ########################################
 * #               Viewing                #
 * ######################################## */
std::string DFSpyHandler::makePacketString(ThreeTuple& t, bool isOutlane) {
   std::string p;
   std::string l;
   isOutlane ? l = "outlane" : l = "inlane";
   p = "(board " + std::to_string(t.boardNumber) + ", " + l + " " + std::to_string(t.laneNumber) + ", L1ID " + std::to_string(t.L1ID) + ")";
   return p;
}

void DFSpyHandler::displayContentsQueueMapOutspy() {
    for(std::map<TwoTuple, std::queue<ThreeTuple> >::iterator it = queueMapOutspy.begin(); it != queueMapOutspy.end(); ++it) {
        TwoTuple boardOutlane = it->first;
        std::string bl =  "(Board " + std::to_string(boardOutlane.boardNumber) + ", Lane " + std::to_string(boardOutlane.laneNumber) + ")";
        std::cout << bl << "  queue size: " << queueMapOutspy[boardOutlane].size() << std::endl;
    }
}

void DFSpyHandler::displayContentsQueueMapInspy() {
    for(std::map<TwoTuple, std::queue<FiveTuple> >::iterator it = queueMapInspy.begin(); it != queueMapInspy.end(); ++it) {
        TwoTuple bl = it->first;
        std::cout << "[" << "Board " << bl.boardNumber << ", Lane " << bl.laneNumber << ")  queue size: " << queueMapInspy[bl].size() << std::endl;
    }
}

std::string DFSpyHandler::get2TupleString(std::vector<int> t) {
    std::string tuple2 = " ";
    tuple2 = "( "  + std::to_string(t[0]) + ", " + std::to_string(t[1]) + " )";
    return tuple2;
}


/* ########################################
 * #               Testing                #
 * ######################################## */
// Returns absolute path to each spyfile
std::vector<std::string> DFSpyHandler::getSpyFilesFromDirectory(const std::string& name) {
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
std::vector<unsigned int> DFSpyHandler::spyfileToVector(std::string spyfile) {
    std::vector<unsigned int> data;
    std::string line;
    std::ifstream myfile(spyfile);
    std::stringstream ss;
    if(myfile.is_open()) {
        while(std::getline(myfile, line) ) {
            unsigned int dataword = dfspy::hexStringToInt(line);
            data.push_back(dataword);
        }
    } else {
        std::cerr << "Could not open spyfile." << std::endl;
    }
    return data;
}


// Returns string hex source ID sans the 0x.
std::string DFSpyHandler::getSourceIDFromFilename(std::string filename) {
    return filename.substr(filename.find("0x") + 2, filename.find(".txt") - 5);
}

// Returns board number from string hex source ID
int DFSpyHandler::getBoardFromSourceID(std::string sourceID) {
    return dfspy::hexStringToInt(sourceID.substr(6, 8));
}

// Uses std::tuple, so only works with std c++11.
// Board internal bits: 31-24 of source ID (first two hex digit)
// Spytype: value is 0-35 for outspy, 36-52 for inspy,
// chnum: for outspy, chnum = value; inspy, chnum = value - 36
// i.e. value of 13 ---> outspy chnum #13; value of 38 ---> inspy chnum #2
//std::tuple<int, bool> DFSpyHandler::getLaneFromSourceID(std::string sourceID) {
//    int value = dfspy::hexStringToInt(sourceID.substr(0,2));
//    bool isInspy;
//    int chnum;
//    if(value < 36) {
//        isInspy = false;
//        chnum = value;
//    } else {
//        isInspy = true;
//        chnum = value - 36;
//    }
//    return std::make_tuple(chnum, isInspy);
//}

int DFSpyHandler::getAbsoluteChannelFromSourceID(std::string sourceID) {
    return dfspy::hexStringToInt(sourceID.substr(0,2));
}










