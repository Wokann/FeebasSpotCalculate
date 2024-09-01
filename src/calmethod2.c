#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <limits.h>
#include "../include/type.h"
#include "../include/calspots.h"
#include "../include/calmethod.h"

static struct FeebaSpots gFeebaSpotsTrim[65536] = {0};
static struct SpotCountsInfo SpotCounts = {0};
static struct GroupMatchedInfo GroupMatched = {0};
static int SpotGroupList = 0;
static int rolecount = 4;
void UpdateSpotCounts();
void greedySelection();

FILE *filemethod;

int main() 
{

    FILE *file;
    int count = 0;
    int i, j;

    // 打开输入文件以读取数据
    file = fopen("result_trim.txt", "r");
    if (file == NULL) {
        printf("Can not open file.\n");
        return 1;
    }

    // 打开输入文件以读取数据
    filemethod = fopen("method2.txt", "w");
    if (filemethod == NULL) {
        printf("Can not open file.\n");
    }

    // 逐行读取数据并解析填充到结构体数组中
    while (fscanf(file, "0x%x,%hu,%hu,%hu,%hu,%hu,%hu,%hhu,%hhu,\n",
                  &gFeebaSpotsTrim[count].FeebaSeed,
                  &gFeebaSpotsTrim[count].Spots[0],
                  &gFeebaSpotsTrim[count].Spots[1],
                  &gFeebaSpotsTrim[count].Spots[2],
                  &gFeebaSpotsTrim[count].Spots[3],
                  &gFeebaSpotsTrim[count].Spots[4],
                  &gFeebaSpotsTrim[count].Spots[5],
                  &gFeebaSpotsTrim[count].ErrorType,
                  &gFeebaSpotsTrim[count].EffectiveType) == 9) {
        count++;
    }
    fclose(file);

    greedySelection();

    fclose(filemethod);
    return 0;
}

void UpdateSpotCounts()
{
    u32 i,j;
    
    for(i = 4; i < 448; i++)
        SpotCounts.SpotCounts[i] = 0;
    SpotCounts.MaxSpot = 0;
    SpotCounts.MaxSpotCounts = 0;

    for(i = 0; i < 65536; i++)
    {
        if(GroupMatched.Group[i] == 0)
        {
            for(j = 0; j < 6; j++)
            {
                if(gFeebaSpotsTrim[i].Spots[j] != 0)
                    SpotCounts.SpotCounts[gFeebaSpotsTrim[i].Spots[j]]++;
            }
        }
        //printf("%X\n",i);
    }

    //for(i = 4; i < 448; i++)
    //{
    //    if(SpotCounts.SpotCounts[i] >= SpotCounts.MaxSpotCounts)
    //    {
            while(SpotCounts.SpotCounts[rolecount] == 0)
            {
                rolecount++;
            };
            SpotCounts.MaxSpotCounts = SpotCounts.SpotCounts[rolecount];
            SpotCounts.MaxSpot = rolecount;
            rolecount++;
            if(rolecount == 105 || rolecount == 119 || rolecount == 144)
                rolecount++;
            if(rolecount == 296)// || rolecount == 297 || rolecount == 298)
                rolecount += 3;
    //    }
    //}
    if(GroupMatched.MatchedCounts == 0)
    {
        FILE *filecounts;
        // 打开输入文件以读取数据
        filecounts = fopen("SpotGroupCounts.txt", "w");
        if (filecounts == NULL) {
            printf("Can not open file.\n");
        }    
        for(i = 4; i < 448; i++)
            fprintf(filecounts,"%d(%d),\n",i,SpotCounts.SpotCounts[i]);
        fclose(filecounts);
    }
}

void greedySelection()
{
    u32 i,j;

    UpdateSpotCounts();

    fprintf(filemethod,"%d:%d(%d),\n",SpotGroupList++,SpotCounts.MaxSpot,SpotCounts.MaxSpotCounts);

    for(i = 0; i < 65536; i++)
    {
        if(GroupMatched.Group[i] != 1)
        {
            for(j = 0; j < 6; j++)
            {
                if(SpotCounts.MaxSpot == gFeebaSpotsTrim[i].Spots[j])
                {
                    GroupMatched.Group[i] = 1;
                    GroupMatched.MatchedCounts++;
                }
            }
        }
    }
    if(GroupMatched.MatchedCounts != 0xFFFF)
        greedySelection();
}