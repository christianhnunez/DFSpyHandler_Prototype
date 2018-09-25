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
#include <algorithm>

#include "ftkcommon/EventFragment.h"
#include "ftkcommon/EventFragmentFTKPacket.h"
#include "ftkcommon/EventFragmentHitClusters.h"
#include "ftkcommon/Utils.h"
#include "ftkcommon/Cluster.h"

// THE TOOL
#include "DFSpyHandlerTools/DFSpyHandler.h"

// Running this tool should result in the following (which can be seen at the bottom of the
// printed result test):

// BUFFER PARSING STATISTICS 

// OUTSPY:     100% (2190 / 2190) of expected modules checks were successful.
// INSPY:      100% (655 / 655) of expected modules checks were successful. 

// Queue Map Inspy Size:         9
// [Board 5, Lane 0)  queue size: 20
// [Board 5, Lane 1)  queue size: 5
// [Board 5, Lane 2)  queue size: 215
// [Board 5, Lane 3)  queue size: 0
// [Board 5, Lane 7)  queue size: 0
// [Board 5, Lane 16)  queue size: 176
// [Board 5, Lane 17)  queue size: 100
// [Board 5, Lane 20)  queue size: 4
// [Board 5, Lane 25)  queue size: 0
// Set Map Inspy Size:           34

// Queue Map Outspy Size:        16
// (Board 5, Lane 0)  queue size: 0
// (Board 5, Lane 1)  queue size: 0
// (Board 5, Lane 2)  queue size: 0
// (Board 5, Lane 3)  queue size: 0
// (Board 5, Lane 4)  queue size: 7
// (Board 5, Lane 5)  queue size: 7
// (Board 5, Lane 6)  queue size: 7
// (Board 5, Lane 7)  queue size: 0
// (Board 5, Lane 17)  queue size: 0
// (Board 5, Lane 18)  queue size: 11
// (Board 5, Lane 20)  queue size: 0
// (Board 5, Lane 21)  queue size: 11
// (Board 5, Lane 22)  queue size: 8
// (Board 5, Lane 23)  queue size: 10
// (Board 5, Lane 24)  queue size: 9
// (Board 5, Lane 25)  queue size: 0
// Pointer Map Outspy Size:      70

// Unique modules inspy:         108
// Unique modules outspy:        253

// end of buffer parsing statistics.

int main() {
    std::string modulelist = "example/modulelist_Data2018_64T.txt";
    std::string multiboard = "example/Slice_lab4_config_Data2018.txt";
    std::string system_config = "example/df_system_config_64towers.txt";
    std::string dumpdir = "example/SpyDump_20180817_103301/";
    DFSpyHandler* dfspy = new DFSpyHandler(multiboard, modulelist, system_config);
    dfspy->performBitLevelChecksTESTER(dumpdir, true, true, true);
    return 0;
}
