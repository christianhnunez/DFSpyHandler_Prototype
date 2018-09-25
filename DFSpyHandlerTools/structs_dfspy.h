#ifndef STRUCTS_DFSPY_H
#define STRUCTS_DFSPY_H

struct Buffer {
    std::vector<std::shared_ptr<daq::ftk::EventFragmentHitClusters> > frags;
    int board;
    int lane;
    bool isInspy;
};

struct TwoTuple
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

    // Strict weak ordering
    bool operator<(const TwoTuple &t) const {
        if(laneNumber < t.laneNumber) return true;
        if(t.laneNumber < laneNumber) return false;

        if(boardNumber < t.boardNumber) return true;
        return false; // early out
    }

};

struct ThreeTuple // Packet identifier
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

    // Strict weak ordering
    bool operator<(const ThreeTuple &t) const {
        if (L1ID < t.L1ID) return true;
        if(t.L1ID < L1ID) return false;

        if(laneNumber < t.laneNumber) return true;
        if(t.laneNumber < laneNumber) return false;

        if(boardNumber < t.boardNumber) return true;
        return false; // early out

    }

};

struct FiveTuple // Packet identifier + module & hitclusters
{
    FiveTuple(int m, unsigned int l, int b, int o, std::shared_ptr<daq::ftk::ModuleHitClusters> mhc)
    : moduleID(m)
    , L1ID(l)
    , boardNumber(b)
    , outputLane(o)
    , mHC(mhc)
    { }

    int moduleID;
    unsigned int L1ID;
    int boardNumber;
    int outputLane;
    std::shared_ptr<daq::ftk::ModuleHitClusters> mHC;

    bool operator==(const FiveTuple &other) const {
        return (moduleID    == other.moduleID     &&
                //*mHC        == *(other.mHC)       &&
                L1ID        == other.L1ID         &&
                boardNumber == other.boardNumber  &&
                outputLane  == other.outputLane);
    }

    // Strict weak ordering
    bool operator<(const FiveTuple &other) const {
        if(moduleID < other.moduleID) return true;
        if(other.moduleID < moduleID) return false;

        if(L1ID < other.L1ID) return true;
        if(other.L1ID < L1ID) return false;

        if(outputLane < other.outputLane) return true;
        if(other.outputLane < outputLane) return false;

        if(boardNumber < other.boardNumber) return true;
        return false;
    }
};

#endif // STRUCTS_DFSPY_H
