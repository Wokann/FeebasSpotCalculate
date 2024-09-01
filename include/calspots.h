#ifndef CALSPOTS_H
#define CALSPOTS_H

#include "type.h"

#define NUM_FEEBAS_SPOTS 6
#define NUM_FISHING_SPOTS_1 131
#define NUM_FISHING_SPOTS_2 167
#define NUM_FISHING_SPOTS_3 149
#define NUM_FISHING_SPOTS (NUM_FISHING_SPOTS_1 + NUM_FISHING_SPOTS_2 + NUM_FISHING_SPOTS_3)

enum
{
    NO_REPETITIVE_NO_INVAILD,       //无重复块、异常块
    HAVE_REPETITIVE,                //存在重复块
    HAVE_INVAILD,                   //存在异常块
    HAVE_REPETITIVE_HAVE_INVAILD,   //存在重复块，且存在异常块
};

enum
{
    SIX_EFFECTIVE,                      //有效块数量为6块。
    FIVE_EFFECTIVE,                     //有效块数量为5块。
    FOUR_EFFECTIVE,                     //有效块数量为4块。
    THREE_EFFECTIVE,                    //有效块数量为3块。
    FIVE_EFFECTIVE_WITH_TEN_BRIDGE,     //有效块数量为5块+上游桥底10块。
    FOUR_EFFECTIVE_WITH_TEN_BRIDGE,     //有效块数量为4块+上游桥底10块。
    THREE_EFFECTIVE_WITH_TEN_BRIDGE,    //有效块数量为3块+上游桥底10块。
};
struct FeebaSpots
{
    u16 FeebaSeed;
    u16 Spots[6];
    u8 ErrorType;
    u8 EffectiveType;
};

#endif