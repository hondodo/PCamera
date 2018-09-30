#ifndef CHINESEYEAR_H
#define CHINESEYEAR_H

#include <time.h>
#include <stdio.h>
#include <string.h>
#include <QString>
#include <QPair>

class ChineseYear
{
public:
    ChineseYear();

    static void InitLunar();
    static QString GetYearName(int lyear);
    static QString GetShengXiao(int lyear);
    static QString GetXinZuo(int month, int day);
    static QString GetLunarStringX(int year, int month, int day,
                            int &lunaryear, int &lunarmonth, int &lunarday);
    static int LunarCalendarX(int year, int month, int day,
                       int &lunaryear, int &lunarmonth, int &lunarday,
                       unsigned int &LunarCalendarDay);

private:
};

#endif // CHINESEYEAR_H
