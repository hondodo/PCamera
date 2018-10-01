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
    ringThread = Q_NULLPTR;
    nextTipTime = QDateTime::currentDateTime().addMSecs(1000 * 60 * 60).time().hour();
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
        //TipTime
        {
            if(nextTipTime == QDateTime::currentDateTime().time().hour())
            {
                QString filename = QString::number(nextTipTime, 'f', 0) + ".mp3";
                nextTipTime = QDateTime::currentDateTime().addMSecs(1000 * 60 * 60).time().hour();
    #ifdef Q_OS_WIN
                filename = "D:/Data/TimeMp3/" + filename;
    #else
                filename = "/home/pi/Music/TimeMp3/" + filename;
    #endif
                if(nextTipTime >= 6 && nextTipTime <= 21)
                {
                    startNewRingThread(filename);
                }
            }
        }
    }
}

void DateTimeControl::deleteRingThread()
{
    if(ringThread != Q_NULLPTR)
    {
        ringThread->setStop();
        ringThread->wait(1000);
        ringThread->terminate();
        ringThread = Q_NULLPTR;
    }
}

void DateTimeControl::startNewRingThread(QString filename)
{
    if(QDateTime::currentDateTime().time().hour() > 6 &&
            QDateTime::currentDateTime().time().hour() <= 21)
    {
        ringThread = new RingThread();
        ringThread->setFileName(filename);
        ringThread->start();
    }
}
