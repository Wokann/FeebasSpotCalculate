#ifndef CALMETHOD_H
#define CALMETHOD_H

struct SpotCountsInfo
{
    u16 SpotCounts[448];
    u16 MaxSpot;
    u16 MaxSpotCounts;
};
struct GroupMatchedInfo
{
    u8 Group[65536];
    u16 MatchedCounts;
};
#endif