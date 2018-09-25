#ifndef UTILS_DFSPY_H
#define UTILS_DFSPY_H

#include <string>
#include <vector>
#include "ftkcommon/EventFragment.h"
#include "ftkcommon/EventFragmentFTKPacket.h"
#include "ftkcommon/EventFragmentHitClusters.h"
#include "ftkcommon/Utils.h"
#include "ftkcommon/Cluster.h"

namespace dfspy {

    // String manipulation
    unsigned int hexStringToInt(std::string hexString);
    unsigned long long hexStringToULL(std::string str);
    std::vector<std::string> split(std::string s);
    bool compareByL1ID(const std::shared_ptr<daq::ftk::EventFragmentHitClusters> &a, const std::shared_ptr<daq::ftk::EventFragmentHitClusters> &b);
    std::vector<int> make_tuple(int elem0, int elem1);
}

#endif // UTILS_DFSPY_H