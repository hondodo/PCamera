#include "chineseyear.h"

static unsigned int LunarCalendarTable[202] =
{
    0x84BEBF,
    0x04AE53,0x0A5748,0x5526BD,0x0D2650,0x0D9544,0x46AAB9,0x056A4D,0x09AD42,0x24AEB6,0x04AE4A,/*BEGIN_YEAR-1910*/
    0x6A4DBE,0x0A4D52,0x0D2546,0x5D52BA,0x0B544E,0x0D6A43,0x296D37,0x095B4B,0x749BC1,0x049754,/*1911-1920*/
    0x0A4B48,0x5B25BC,0x06A550,0x06D445,0x4ADAB8,0x02B64D,0x095742,0x2497B7,0x04974A,0x664B3E,/*1921-1930*/
    0x0D4A51,0x0EA546,0x56D4BA,0x05AD4E,0x02B644,0x393738,0x092E4B,0x7C96BF,0x0C9553,0x0D4A48,/*1931-1940*/
    0x6DA53B,0x0B554F,0x056A45,0x4AADB9,0x025D4D,0x092D42,0x2C95B6,0x0A954A,0x7B4ABD,0x06CA51,/*1941-1950*/
    0x0B5546,0x555ABB,0x04DA4E,0x0A5B43,0x352BB8,0x052B4C,0x8A953F,0x0E9552,0x06AA48,0x6AD53C,/*1951-1960*/
    0x0AB54F,0x04B645,0x4A5739,0x0A574D,0x052642,0x3E9335,0x0D9549,0x75AABE,0x056A51,0x096D46,/*1961-1970*/
    0x54AEBB,0x04AD4F,0x0A4D43,0x4D26B7,0x0D254B,0x8D52BF,0x0B5452,0x0B6A47,0x696D3C,0x095B50,/*1971-1980*/
    0x049B45,0x4A4BB9,0x0A4B4D,0xAB25C2,0x06A554,0x06D449,0x6ADA3D,0x0AB651,0x093746,0x5497BB,/*1981-1990*/
    0x04974F,0x064B44,0x36A537,0x0EA54A,0x86B2BF,0x05AC53,0x0AB647,0x5936BC,0x092E50,0x0C9645,/*1991-2000*/
    0x4D4AB8,0x0D4A4C,0x0DA541,0x25AAB6,0x056A49,0x7AADBD,0x025D52,0x092D47,0x5C95BA,0x0A954E,/*2001-2010*/
    0x0B4A43,0x4B5537,0x0AD54A,0x955ABF,0x04BA53,0x0A5B48,0x652BBC,0x052B50,0x0A9345,0x474AB9,/*2011-2020*/
    0x06AA4C,0x0AD541,0x24DAB6,0x04B64A,0x69573D,0x0A4E51,0x0D2646,0x5E933A,0x0D534D,0x05AA43,/*2021-2030*/
    0x36B537,0x096D4B,0xB4AEBF,0x04AD53,0x0A4D48,0x6D25BC,0x0D254F,0x0D5244,0x5DAA38,0x0B5A4C,/*2031-2040*/
    0x056D41,0x24ADB6,0x049B4A,0x7A4BBE,0x0A4B51,0x0AA546,0x5B52BA,0x06D24E,0x0ADA42,0x355B37,/*2041-2050*/
    0x09374B,0x8497C1,0x049753,0x064B48,0x66A53C,0x0EA54F,0x06B244,0x4AB638,0x0AAE4C,0x092E42,/*2051-2060*/
    0x3C9735,0x0C9649,0x7D4ABD,0x0D4A51,0x0DA545,0x55AABA,0x056A4E,0x0A6D43,0x452EB7,0x052D4B,/*2061-2070*/
    0x8A95BF,0x0A9553,0x0B4A47,0x6B553B,0x0AD54F,0x055A45,0x4A5D38,0x0A5B4C,0x052B42,0x3A93B6,/*2071-2080*/
    0x069349,0x7729BD,0x06AA51,0x0AD546,0x54DABA,0x04B64E,0x0A5743,0x452738,0x0D264A,0x8E933E,/*2081-2090*/
    0x0D5252,0x0DAA47,0x66B53B,0x056D4F,0x04AE45,0x4A4EB9,0x0A4D4C,0x0D1541,0x2D92B5,          /*2091-2099*/
    0xD5249
};
static const int BEGIN_YEAR = 1900;
static int MonthAdd[12] = {0,31,59,90,120,151,181,212,243,273,304,334};
static const char *ChDay[] = {"*","初一","初二","初三","初四","初五",
                       "初六","初七","初八","初九","初十",
                       "十一","十二","十三","十四","十五",
                       "十六","十七","十八","十九","二十",
                       "廿一","廿二","廿三","廿四","廿五",
                       "廿六","廿七","廿八","廿九","三十"
                      };
static const char *ChMonth[] = {"*","正","二","三","四","五","六","七","八","九","十","十一","腊"};

static const QString ZOX[] = {"鼠","牛","虎","兔","龙","蛇","马","羊","猴","鸡","狗","猪"};
static const QString LUNAR_YEAR0[] = {"甲","乙","丙","丁","戊","己","庚","辛","壬","癸"};
static const QString LUNAR_YEAR1[] = {"子","丑","寅","卯","辰","巳","午","未","申","酉","戌","亥"};
static QPair<QString,QString> starx[13][33];
static QString lunar_year[61];

ChineseYear::ChineseYear()
{
}

int ChineseYear::LunarCalendarX(int year,int month,int day,int & lunaryear,int & lunarmonth,int & lunarday,unsigned int & LunarCalendarDay)
{
    int Spring_NY,Sun_NY,StaticDayCount;
    int index,flag;
    //Spring_NY 记录春节离当年元旦的天数。
    //Sun_NY 记录阳历日离当年元旦的天数。
    if ( ((LunarCalendarTable[year-BEGIN_YEAR] & 0x0060) >> 5) == 1)
        Spring_NY = (LunarCalendarTable[year-BEGIN_YEAR] & 0x001F) - 1;
    else
        Spring_NY = (LunarCalendarTable[year-BEGIN_YEAR] & 0x001F) - 1 + 31;
    Sun_NY = MonthAdd[month-1] + day - 1;
    if ( ((year%4==0 && year%100!=0) || year%400==0) && (month > 2) )
        Sun_NY++;
    //StaticDayCount记录大小月的天数 29 或30
    //index 记录从哪个月开始来计算。
    //flag 是用来对闰月的特殊处理。
    //判断阳历日在春节前还是春节后
    if (Sun_NY >= Spring_NY)//阳历日在春节后（含春节那天）
    {
        Sun_NY -= Spring_NY;
        month = 1;
        index = 1;
        flag = 0;
        if ( ( LunarCalendarTable[year - BEGIN_YEAR] & (0x80000 >> (index-1)) ) ==0)
            StaticDayCount = 29;
        else
            StaticDayCount = 30;
        while (Sun_NY >= StaticDayCount)
        {
            Sun_NY -= StaticDayCount;
            index++;
            if ((unsigned)month == ((LunarCalendarTable[year - BEGIN_YEAR] & 0xF00000) >> 20) )
            {
                flag = ~flag;
                if (flag == 0)
                    month++;
            }
            else
                month++;
            if ( ( LunarCalendarTable[year - BEGIN_YEAR] & (0x80000 >> (index-1)) ) ==0)
                StaticDayCount=29;
            else
                StaticDayCount=30;
        }
        day = Sun_NY + 1;
    }
    else //阳历日在春节前
    {
        Spring_NY -= Sun_NY;
        year--;
        month = 12;
        if ( ((LunarCalendarTable[year - BEGIN_YEAR] & 0xF00000) >> 20) == 0)
            index = 12;
        else
            index = 13;
        flag = 0;
        if ( ( LunarCalendarTable[year - BEGIN_YEAR] & (0x80000 >> (index-1)) ) ==0)
            StaticDayCount = 29;
        else
            StaticDayCount = 30;
        while (Spring_NY > StaticDayCount)
        {
            Spring_NY -= StaticDayCount;
            index--;
            if (flag == 0)
                month--;
            if ((unsigned)month == ((LunarCalendarTable[year - BEGIN_YEAR] & 0xF00000) >> 20))
                flag = ~flag;
            if ( ( LunarCalendarTable[year - BEGIN_YEAR] & (0x80000 >> (index-1)) ) ==0)
                StaticDayCount = 29;
            else
                StaticDayCount = 30;
        }
        day = StaticDayCount - Spring_NY + 1;
    }
    LunarCalendarDay |= day;
    LunarCalendarDay |= (month << 6);

    lunaryear = year;
    lunarmonth = (LunarCalendarDay & 0x3C0) >> 6;
    lunarday = LunarCalendarDay & 0x3F;

    if ((unsigned)month == ((LunarCalendarTable[year - BEGIN_YEAR] & 0xF00000) >> 20))
        return 1;
    else
        return 0;
}

QString ChineseYear::GetLunarStringX(int year,int month,int day,int & lunaryear,int & lunarmonth,int & lunarday)
{
    char str[30] = "";
    unsigned int LunarCalendarDay = 0;
    if (LunarCalendarX(year,month,day,lunaryear,lunarmonth,lunarday,LunarCalendarDay))
    {
        strcat(str,"闰");
        strcat(str,ChMonth[(LunarCalendarDay & 0x3C0) >> 6]);
    }
    else
    {
        strcat(str,ChMonth[(LunarCalendarDay & 0x3C0) >> 6]);
    }
    strcat(str,"月");
    strcat(str,ChDay[LunarCalendarDay & 0x3F]);
    return QString(str);
}

QString ChineseYear::GetShengXiao(int lyear)
{
    int index = (lyear - 1900 + 12) % 12;
    return ZOX[index];
}

QString ChineseYear::GetXinZuo(int month, int day)
{
    return starx[month][day].first;
}

void ChineseYear::InitLunar()
{
    //初始化星座
    for(int month=1;month<=12;month++)
    {
        for(int day=1;day<=31;day++)
        {
            if((month==3 && day>=21) || (month==4 && day<=19)) starx[month][day]=qMakePair(QString("白羊座"),QString(":/zodiac/ico/")+QString("aries.ico"));
            else if((month==4 && day>=20) || (month==5 && day<=20)) starx[month][day]=qMakePair(QString("金牛座"),QString(":/zodiac/ico/")+QString("taurus.ico"));
            else if((month==5 && day>=21) || (month==6 && day<=21)) starx[month][day]=qMakePair(QString("双子座"),QString(":/zodiac/ico/")+QString("gemini.ico"));
            else if((month==6 && day>=22) || (month==7 && day<=22)) starx[month][day]=qMakePair(QString("巨蟹座"),QString(":/zodiac/ico/")+QString("cancer.ico"));
            else if((month==7 && day>=23) || (month==8 && day<=22)) starx[month][day]=qMakePair(QString("狮子座"),QString(":/zodiac/ico/")+QString("lion.ico"));
            else if((month==8 && day>=23) || (month==9 && day<=22)) starx[month][day]=qMakePair(QString("处女座"),QString(":/zodiac/ico/")+QString("virgo.ico"));
            else if((month==9 && day>=23) || (month==10 && day<=23)) starx[month][day]=qMakePair(QString("天秤座"),QString(":/zodiac/ico/")+QString("libra.ico"));
            else if((month==10 && day>=24) || (month==11 && day<=22)) starx[month][day]=qMakePair(QString("天蝎座"),QString(":/zodiac/ico/")+QString("scorpio.ico"));
            else if((month==11 && day>=23) || (month==12 && day<=21)) starx[month][day]=qMakePair(QString("射手座"),QString(":/zodiac/ico/")+QString("sagittarius.ico"));
            else if((month==12 && day>=22) || (month==1 && day<=19)) starx[month][day]=qMakePair(QString("摩羯座"),QString(":/zodiac/ico/")+QString("capricorn.ico"));
            else if((month==1 && day>=20) ||  (month==2 && day<=18)) starx[month][day]=qMakePair(QString("水瓶座"),QString(":/zodiac/ico/")+QString("aquarius.ico"));
            else if((month==2 && day>=19) ||  (month==3 && day<=20)) starx[month][day]=qMakePair(QString("双鱼座"),QString(":/zodiac/ico/")+QString("pisces.ico"));
        }
    }
    int tot=0;
    for(int i=0;i<10;i++)
    {
        for(int j=i;;)
        {
            lunar_year[tot++] = LUNAR_YEAR0[i] + LUNAR_YEAR1[j];
            if((j+10)%12==i) break;
            j=(j+10)%12;
        }
    }
}

QString ChineseYear::GetYearName(int lyear)
{
    int x = (lyear - 1864) % 60;
    QString yearName = lunar_year[x % 10 * 6 + (int)(x / 10)];
    return yearName;
}

