#include <stdio.h>
#include <string.h>
#include "../include/random.h"
#include "../include/dewfordtrend.h"

enum {
    SORT_MODE_NORMAL,
    SORT_MODE_MAX_FIRST,
    SORT_MODE_FULL,
};
void InitDewfordTrend(void);
static void SortTrends(struct DewfordTrend *, u16, u8);
static bool8 CompareTrends(struct DewfordTrend *a, struct DewfordTrend *b, u8 mode);
static void SeedTrendRng(struct DewfordTrend *trend);

int main()
{   
    FILE *file;
    file = fopen("../data/trendword2rand.txt","w");
    if (file == NULL)
    {
        printf("Can not open file.\n");
        return 1;
    }
    
    printf("Enter Initial Seed:\n");
    scanf("%x",&gRngValue);
    PlayerID.SID = Random() & 0xFFFF;
    PlayerID.TID = Random() & 0xFFFF;
    Random();
    Random();
    InitDewfordTrend();
        fprintf(file,"SID:%04X, TID:%04X(%05d):\n", PlayerID.SID, PlayerID.TID, PlayerID.TID);
    for(int i = 0; i<5;i++)
    {
        fprintf(file,"[%d]: trendiness:%03d, maxTrendiness:%03d, gainingTrendiness:%d, rand:%04X, words1:%04X(%s), words2:%04X(%s).\n", i+1, dewfordTrends[i].trendiness, dewfordTrends[i].maxTrendiness, dewfordTrends[i].gainingTrendiness, dewfordTrends[i].rand, dewfordTrends[i].words[0], dewfordTrends[i].name1, dewfordTrends[i].words[1], dewfordTrends[i].name2);
    };
    fclose(file);
    return 0;
}

/*
1、设定第1组第1个单词，
    Random() % 69  (EC_GROUP_CONDITIONS里选)；
2、设定第1组第2个单词，
    首先计算Random() & 1
    若为1，则为Random() % 45  (EC_GROUP_LIFESTYLE里选)；
    若为0，则为Random() % 54  (EC_GROUP_HOBBIES里选)；
4、判定流行趋势上升或下降，
    Random() & 1；
5、设置最大流行度、当前流行度、rand
首先判定rand
    rand = Random() % 98;（取值0-97）
    若rand <= 50则无事
    若rand > 50，
        rand = Random() % 98;（取值0-97）
        若rand <= 80则无事
        若rand > 80，
            rand = Random() % 98;（取值0-97）
接着最大流行度取
    rand+30，（30-127）
然后当前流行度取
    (Random() % (rand + 1)) + 30，（30-127）
最后rand取
    Random()；
6、重复第1步开始第2-5组。
7、6组结束后，对比各个流行语进行排序
    首先对比1和2，
        若最大流行度2大，则1、2位置互换，
        若1大，则不变，
        若2=1，则Random() & 1，为1则互换，为0则不变
    然后对比1和3，（含12已经互换过），重复上述步骤；
    直到对比完1和6，
    接着2和3开始对比，2和4，2和5，2和6，3和4，3和5，3和6，4和5，4和6，5和6
最终得到互换结束后的最终序列

*/
void InitDewfordTrend(void)
{
    for (int i = 0; i < SAVED_TRENDS_COUNT; i++)
    {   
        int index = Random() % 69;
        dewfordTrends[i].words[0] = easyword_CONDITIONS[index].code;
        strcpy(dewfordTrends[i].name1, easyword_CONDITIONS[index].name_en);
        if (Random() & 1)
        {
            index = Random() % 45;
            dewfordTrends[i].words[1] = easyword_LIFESTYLE[index].code;
            strcpy(dewfordTrends[i].name2, easyword_LIFESTYLE[index].name_en);
        }
        else
        {
            index = Random() % 54;
            dewfordTrends[i].words[1] = easyword_HOBBIES[index].code;
            strcpy(dewfordTrends[i].name2, easyword_HOBBIES[index].name_en);
        }
        dewfordTrends[i].gainingTrendiness = Random() & 1;
        SeedTrendRng(&(dewfordTrends[i]));
    }
    SortTrends(dewfordTrends, SAVED_TRENDS_COUNT, SORT_MODE_NORMAL);
}
static void SortTrends(struct DewfordTrend *trends, u16 numTrends, u8 mode)
{
    u16 i;
    for (i = 0; i < numTrends; i++)
    {
        u16 j;
        for (j = i + 1; j < numTrends; j++)
        {
            if (CompareTrends(&trends[j], &trends[i], mode))
            {
                struct DewfordTrend temp;
                SWAP(trends[j], trends[i], temp);
            }
        }
    }
}
static bool8 CompareTrends(struct DewfordTrend *a, struct DewfordTrend *b, u8 mode)
{
    switch (mode)
    {
    case SORT_MODE_NORMAL:
        if (a->trendiness > b->trendiness) return TRUE;
        if (a->trendiness < b->trendiness) return FALSE;

        if (a->maxTrendiness > b->maxTrendiness) return TRUE;
        if (a->maxTrendiness < b->maxTrendiness) return FALSE;
        break;
    case SORT_MODE_MAX_FIRST: // Unused
        if (a->maxTrendiness > b->maxTrendiness) return TRUE;
        if (a->maxTrendiness < b->maxTrendiness) return FALSE;

        if (a->trendiness > b->trendiness) return TRUE;
        if (a->trendiness < b->trendiness) return FALSE;
        break;
    case SORT_MODE_FULL:
        if (a->trendiness > b->trendiness) return TRUE;
        if (a->trendiness < b->trendiness) return FALSE;

        if (a->maxTrendiness > b->maxTrendiness) return TRUE;
        if (a->maxTrendiness < b->maxTrendiness) return FALSE;

        if (a->rand > b->rand) return TRUE;
        if (a->rand < b->rand) return FALSE;

        if (a->words[0] > b->words[0]) return TRUE;
        if (a->words[0] < b->words[0]) return FALSE;

        if (a->words[1] > b->words[1]) return TRUE;
        if (a->words[1] < b->words[1]) return FALSE;
        return TRUE;
    }

    // Invalid mode given, or trends are equal in SORT_MODE_NORMAL or SORT_MODE_MAX_FIRST
    // Randomly pick one of the phrases
    return Random() & 1;
}

static void SeedTrendRng(struct DewfordTrend *trend)
{
    u16 rand;

    rand = Random() % 98;
    if (rand > 50)
    {
        rand = Random() % 98;
        if (rand > 80)
            rand = Random() % 98;
    }
    trend->maxTrendiness = rand + 30;
    trend->trendiness = (Random() % (rand + 1)) + 30;
    trend->rand = Random();
}