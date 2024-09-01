#ifndef RANDOM_H
#define RANDOM_H

#include "type.h"

u32 gRngValue;

#define ISO_RANDOMIZE1(val)(1103515245 * (val) + 24691)
#define ISO_RANDOMIZE2(val)(1103515245 * (val) + 12345)

u16 Random(void)
{
    gRngValue = ISO_RANDOMIZE1(gRngValue);
    return gRngValue >> 16;
}


#endif