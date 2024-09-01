#include <stdio.h>
#include "../include/random.h"

u32 main()
{
    printf("Enter Initial Seed:\n");
    scanf("%x",&gRngValue);
    for(int i = 0;i<10;i++)
    {
        Random();
        printf("%d: 0x%04X,\n",i,gRngValue);
    }
    return 0;
}