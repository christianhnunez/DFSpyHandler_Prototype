#include "Utils_dfspy.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <string>
#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
#include <stdlib.h>
#include <vector>

namespace dfspy {

    std::vector<std::string> split(std::string s) {
        std::stringstream A(s);
        std::vector<std::string> tokens;
        std::string t;
        while (A>>t) tokens.push_back(t);
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
        result = std::strtoull(str, &end, 16);
        if (result == 0 && end == str) {
           std::cout <<  "/* str was not a number */" << std::endl;
        } else if (result == ULLONG_MAX && errno) {
            std::cout <<  "/* the value of str does not fit in unsigned long long */" << std::endl;
        } else if (*end) {
            std::cout <<  "/* str began with a number but has junk left over at the end */" << std::endl;
        }
        return result;
    }

    /*
     * Function: compareByL1ID
     * -----
     * Used for sorting shared_ptr<EventFragmentHitClusters> vectors with std::sort
     */
    bool compareByL1ID(const std::shared_ptr<daq::ftk::EventFragmentHitClusters> &a, const std::shared_ptr<daq::ftk::EventFragmentHitClusters> &b) {
        return a->getL1ID() < b->getL1ID();
    }

    std::vector<int> make_tuple(int elem0, int elem1) {
        std::vector<int> v;
        v.push_back(elem0);
        v.push_back(elem1);
        return v;
    }

}