#include "datetimecontrol.h"
#include "ui_datetimecontrol.h"

const QString XinQi[] = {"日", "一", "二", "三", "四", "五", "六", "日"};

DateTimeControl::DateTimeControl(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DateTimeControl)
{
    ui->setupUi(this);
    timeId = 0;
    ChineseYear::InitLunar();
}

DateTimeControl::~DateTimeControl()
{
    if(timeId > 0)
    {
        killTimer(timeId);
        timeId = 0;
    }
    delete ui;
}

void DateTimeControl::start()
{
    timeId = startTimer(100);
}

void DateTimeControl::timerEvent(QTimerEvent *event)
{
    if(event->timerId() == timeId)
    {
        QDateTime time = QDateTime::currentDateTime();
        int lunaryear = 0, lunarmonth = 0, lunarday = 0;
        QString lunarstring = ChineseYear::GetLunarStringX(time.date().year(),
                                                           time.date().month(),
                                                           time.date().day(),
                                                           lunaryear,
                                                           lunarmonth,
                                                           lunarday);

        ui->labelData->setText(time.toString("yyyy-MM-dd"));
        ui->labelTime->setText(time.toString("hh:mm:ss"));
        ui->labelChineseData->setText(lunarstring + " 星期" + XinQi[time.date().dayOfWeek()]);
        QString shengxiao = ChineseYear::GetShengXiao(lunaryear);
        QString xinzuo = ChineseYear::GetXinZuo(time.date().month(), time.date().day());
        QString yearname = ChineseYear::GetYearName(lunaryear);
        ui->labelShengXiaoXinZuo->setText(yearname + QString("年") + " " + shengxiao + " " + xinzuo);
    }
}
