#include <stdio.h>
#include <stdbool.h>

struct RtcInfo
{
    unsigned short int year;
    unsigned char month;
    unsigned char day;
    unsigned char hour;
    unsigned char minute;
    unsigned char second;
    unsigned short int year_BCD;
    unsigned char month_BCD;
    unsigned char day_BCD;
    unsigned char hour_BCD;
    unsigned char minute_BCD;
    unsigned char second_BCD;
};
enum
{
    MONTH_JAN = 1,
    MONTH_FEB,
    MONTH_MAR,
    MONTH_APR,
    MONTH_MAY,
    MONTH_JUN,
    MONTH_JUL,
    MONTH_AUG,
    MONTH_SEP,
    MONTH_OCT,
    MONTH_NOV,
    MONTH_DEC
};
static struct RtcInfo CurrentRtc;
static int RTCFixed = 0;
int IsLeapYear(unsigned short int year);
int CheckInvalidTime();
int ConvertDateToDatCount(unsigned short int year,unsigned char month,unsigned char day);

int main()
{
    int version;
    printf("Calculate Initial Seed For Pokemon Ruby/Sapphire.\n");
    printf("Enter Cartridge/Rom Version:(RTC Not Fixed: 0, RTC Fixed: 1)\n");
    if (scanf("%d", &version) != 1) 
    {
        printf("Version input error1.\n");
        return 1;
    }
    if(version < 0 || version > 1)
    {
        printf("Version input error2.\n");
        return 1;
    }
    getchar();

    char time_str[20] = {0};
    printf("Enter RTC time like: \'2024-12-31,23:59\'\n");
    if(fgets(time_str, sizeof(time_str),stdin) == NULL)
    {
        printf("load error.\n");
        return 1;
    }
    if (sscanf(time_str, "%d-%d-%d,%d:%d", &CurrentRtc.year, &CurrentRtc.month, &CurrentRtc.day, &CurrentRtc.hour, &CurrentRtc.minute) != 5) 
    {
        printf("time input error1.\n");
        return 1;
    }
    if(CheckInvalidTime())
    {
        printf("time input error2.\n");
        return 1;
    }

    CurrentRtc.hour_BCD = (CurrentRtc.hour / 10) * 0x10 + (CurrentRtc.hour % 10);
    CurrentRtc.minute_BCD = (CurrentRtc.minute / 10) * 0x10 + (CurrentRtc.minute % 10);

    RTCFixed = version;
    unsigned int DayCounts, MinuteCounts, seed;
    DayCounts = ConvertDateToDatCount(CurrentRtc.year, CurrentRtc.month, CurrentRtc.day);
    MinuteCounts = (24 * 60) * DayCounts + 60 * CurrentRtc.hour_BCD + CurrentRtc.minute_BCD;
    seed = (MinuteCounts >> 16) ^ (MinuteCounts & 0xFFFF);
    //printf("%d,%d\n",DayCounts,MinuteCounts);
    printf("Initial Seed of this time is: 0x%04X\n", seed);
    getchar();

    return 0;
}

int CheckInvalidTime()
{
    if(CurrentRtc.year < 2000 || CurrentRtc.year > 2099)
    {
        printf("Invalid year. Please enter year in 2000-2099. (error year:%d)\n",CurrentRtc.year);
        return 1;
    }

    if(CurrentRtc.month < 1 || CurrentRtc.month > 12)
    {
        printf("Invalid month. Please enter month in 1-12. (error month:%d)\n", CurrentRtc.month);
        return 1;
    }

    switch(CurrentRtc.month)
    {
        case 1:
        case 3:
        case 5:
        case 7:
        case 8:
        case 10:
        case 12:
            if(CurrentRtc.day < 1 || CurrentRtc.day > 31)
            {
                printf("Invalid day. Please enter day in 1-31. (error day:%d)\n", CurrentRtc.day);
                return 1;
            }
            break;
        case 4:
        case 6:
        case 9:
        case 11:
            if(CurrentRtc.day < 1 || CurrentRtc.day > 30)
            {
                printf("Invalid day. Please enter day in 1-30. (error day:%d)\n", CurrentRtc.day);
                return 1;
            }
            break;
        case 2:
            if(IsLeapYear(CurrentRtc.year) == 1)
            {
                if(CurrentRtc.day < 1 || CurrentRtc.day > 29)
                {
                    printf("Invalid day. Please enter day in 1-29. (error day:%d)\n", CurrentRtc.day);
                    return 1;
                }
            }
            else
            {
                if(CurrentRtc.day < 1 || CurrentRtc.day > 28)
                {
                    printf("Invalid day. Please enter day in 1-28. (error day:%d)\n", CurrentRtc.day);
                    return 1;
                }
            }
            break;
    }

    if(CurrentRtc.hour < 0 || CurrentRtc.hour > 23)
    {
        printf("Invalid hour. Please enter hour in 0-23. (error hour:%d)\n", CurrentRtc.hour);
        return 1;
    }

    if(CurrentRtc.minute < 0 || CurrentRtc.minute > 59)
    {
        printf("Invalid minute. Please enter minute in 0-59. (error minute:%d)\n", CurrentRtc.minute);
        return 1;
    }

    return 0;
}

int IsLeapYear(unsigned short int year)
{
    if ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0))
        return 1;

    return 0;
}

static const int sNumDaysInMonths[12] =
{
    31,
    28,
    31,
    30,
    31,
    30,
    31,
    31,
    30,
    31,
    30,
    31,
};

int ConvertDateToDatCount(unsigned short int year,unsigned char month,unsigned char day)
{
    int i;
    int dayCount = 0;

    if(RTCFixed == 0)
    {
        for (i = year - 2001; i > 0; i--)
        {
            dayCount += 365;

            if (IsLeapYear(i) == 1)
                dayCount++;
        }
    }
    else
    {
        for (i = year - 2001; i >= 0; i--)
        {
            dayCount += 365;

            if (IsLeapYear(i) == 1)
                dayCount++;
        }
    }

    for (i = 0; i < month - 1; i++)
        dayCount += sNumDaysInMonths[i];

    if (month > MONTH_FEB && IsLeapYear(year) == 1)
        dayCount++;

    dayCount += day;

    return dayCount;
}