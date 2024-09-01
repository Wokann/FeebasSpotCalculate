#include <stdio.h>
#include "../include/type.h"
#include "../include/calspots.h"
#include "../include/random.h"

static u32 sFeebasRngValue = 0;
static u16 FeebasRandom(void);
static void FeebasSeedRng(u16 seed);

int main(void)
{
    FILE *fileorigin;
    fileorigin = fopen("../data/rand2spots.txt","w");
    if (fileorigin == NULL)
    {
        printf("Can not open file.\n");
        return 1;
    }
    FILE *filetrim;
    filetrim = fopen("../data/rand2spots_trim.txt","w");
    if (filetrim == NULL)
    {
        printf("Can not open file.\n");
        return 1;
    }

    u8 i;
    u32 j;
    u16 rand;
    u16 feebasSpots[NUM_FEEBAS_SPOTS];
    static struct FeebaSpots gFeebaSpotsOrigin[65536];
    static struct FeebaSpots gFeebaSpotsTrim[65536];

    for(j=0; j<65536 ;j++)
    {
        rand = j;
        FeebasSeedRng(rand);
        gFeebaSpotsOrigin[j].FeebaSeed = sFeebasRngValue;
        for (i = 0; i != NUM_FEEBAS_SPOTS;)
        {
            feebasSpots[i] = FeebasRandom() % NUM_FISHING_SPOTS;
            if (feebasSpots[i] == 0)
                feebasSpots[i] = NUM_FISHING_SPOTS;

            gFeebaSpotsOrigin[j].Spots[i] = feebasSpots[i];

            if (feebasSpots[i] < 1 || feebasSpots[i] >= 4)
                i++;
        }

        u8 EffectiveSpots[6] = {1,1,1,1,1,1};
        u8 InvalidSpots[6] = {0,0,0,0,0,0};
        u8 BridgeSpots[6] = {0,0,0,0,0,0};
        u8 RepetitiveSpots[6] = {0,1,2,3,4,5};
        u8 ErrorType;
        u8 EffectiveType;
        u8 sumEffectiveSpots = 0;
        u8 sumInvalidSpots = 0;
        u8 sumBridgeSpots = 0;
        u8 sumRepetitiveSpots = 0;
        for(i = 0; i < NUM_FEEBAS_SPOTS; i++)
        {   //检查是否存在异常块，无法钓
            if(feebasSpots[i] == 105 || feebasSpots[i] == 119 || feebasSpots[i] == 144 || 
                feebasSpots[i] == 296 || feebasSpots[i] == 297 || feebasSpots[i] == 298)
            {
                InvalidSpots[i] = 1;
                EffectiveSpots[i] = 0;
            }
            //检查是否存在132号异常块，映射至桥底10块均可钓
            if(feebasSpots[i] == 132)
            {
                BridgeSpots[i] = 1;
                EffectiveSpots[i] = 0;
            }
            //检查是否存在重复块
            //RepetitiveSpots每个值均为当前钓点索引值，若与前者重复，则改为前者对应钓点的索引值
            //由于仅从前往后计算重复，故综合值必定小于等于0+1+2+3+4+5=15
            //当总和值小于15时，可通过查看存在哪些重复值，以及重复值的数量，判别总共有几组重复值，每组各重复了多少。
            for(u8 k = i+1; k < NUM_FEEBAS_SPOTS; k++)
            {   
                if(RepetitiveSpots[k] != k)
                    continue;
                if(feebasSpots[i] == feebasSpots[k])
                {
                    RepetitiveSpots[k] = i;
                    EffectiveSpots[k] = 0;
                }
            }

            sumEffectiveSpots += EffectiveSpots[i];
            sumInvalidSpots += InvalidSpots[i];
            sumBridgeSpots += BridgeSpots[i];
            sumRepetitiveSpots += RepetitiveSpots[i];
        }

        //判定重复及异常类型
        //有效值总和值为6，表明不存在重复或异常块
        if(sumEffectiveSpots == 6)
            ErrorType = NO_REPETITIVE_NO_INVAILD;
        else if(sumEffectiveSpots != 6)
        {   
            //检查重复值之和是否等于15，以及异常值（含常规异常值和桥底值）是否不为0
            if(sumRepetitiveSpots != 15 && (sumInvalidSpots + sumBridgeSpots) == 0)
                ErrorType = HAVE_REPETITIVE;
            else if(sumRepetitiveSpots == 15 && (sumInvalidSpots + sumBridgeSpots) != 0)
                ErrorType = HAVE_INVAILD;
            else if(sumRepetitiveSpots != 15 && (sumInvalidSpots + sumBridgeSpots) != 0)
                ErrorType = HAVE_REPETITIVE_HAVE_INVAILD;
        }

        //判定有效钓点类型
        EffectiveType = 6 - sumEffectiveSpots;
        if(sumBridgeSpots != 0)
            EffectiveType += 3;

        gFeebaSpotsOrigin[j].ErrorType = ErrorType;
        gFeebaSpotsOrigin[j].EffectiveType = EffectiveType;
        gFeebaSpotsTrim[j]=gFeebaSpotsOrigin[j];
        if(ErrorType != NO_REPETITIVE_NO_INVAILD)
        {
            for(i = 0; i < NUM_FEEBAS_SPOTS; i++)
            {
                if(InvalidSpots[i] == 1)
                    gFeebaSpotsTrim[j].Spots[i] = 0;
                if(RepetitiveSpots[i] != i)
                    gFeebaSpotsTrim[j].Spots[i] = 0;
            }

            for(i = 0; i < NUM_FEEBAS_SPOTS; i++)
            {
                if(gFeebaSpotsTrim[j].Spots[i] == 0)
                {
                    for(u8 k = i; k < NUM_FEEBAS_SPOTS - 1; k++)
                    {
                        gFeebaSpotsTrim[j].Spots[k] = gFeebaSpotsTrim[j].Spots[k+1];
                        gFeebaSpotsTrim[j].Spots[k+1] = 0;
                    }
                }
            }
        }
    }

    for(j=0; j<65536 ;j++)
    {
        fprintf(fileorigin,"0x%04x,",gFeebaSpotsOrigin[j].FeebaSeed);
        for(i = 0; i < NUM_FEEBAS_SPOTS; i++)
            fprintf(fileorigin,"%03d,",gFeebaSpotsOrigin[j].Spots[i]);
        fprintf(fileorigin,"%d,%d\n",gFeebaSpotsOrigin[j].ErrorType,gFeebaSpotsOrigin[j].EffectiveType);
    }

    for(j=0; j<65536 ;j++)
    {
        fprintf(filetrim,"0x%04x,",gFeebaSpotsTrim[j].FeebaSeed);
        for(i = 0; i < NUM_FEEBAS_SPOTS; i++)
            fprintf(filetrim,"%03d,",gFeebaSpotsTrim[j].Spots[i]);
        fprintf(filetrim,"%d,%d,\n",gFeebaSpotsTrim[j].ErrorType,gFeebaSpotsTrim[j].EffectiveType);
    }
    fclose(fileorigin);
    fclose(filetrim);

    return 0;
}

static u16 FeebasRandom(void)
{
    sFeebasRngValue = ISO_RANDOMIZE2(sFeebasRngValue);
    return sFeebasRngValue >> 16;
}

static void FeebasSeedRng(u16 seed)
{
    sFeebasRngValue = seed;
}