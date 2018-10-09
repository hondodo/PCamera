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
    dateFontSize = 62;
    timeFontSize = 150;
    weekFontSize = 62;
    yearFontSize = 52;
    setMinimumSize(QSize(10, 10));
    setMaximumSize(QSize(2048, 2048));
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
        resizeFontSize();
        ui->label->setText(buildHtmlText());
//        QDateTime time = QDateTime::currentDateTime();
//        int lunaryear = 0, lunarmonth = 0, lunarday = 0;
//        QString lunarstring = ChineseYear::GetLunarStringX(time.date().year(),
//                                                           time.date().month(),
//                                                           time.date().day(),
//                                                           lunaryear,
//                                                           lunarmonth,
//                                                           lunarday);

//        ui->labelData->setText(time.toString("yyyy-MM-dd"));
//        ui->labelTime->setText(time.toString("hh:mm:ss"));
//        ui->labelChineseData->setText(lunarstring + " 星期" + XinQi[time.date().dayOfWeek()]);
//        QString shengxiao = ChineseYear::GetShengXiao(lunaryear);
//        QString xinzuo = ChineseYear::GetXinZuo(time.date().month(), time.date().day());
//        QString yearname = ChineseYear::GetYearName(lunaryear);
//        ui->labelShengXiaoXinZuo->setText(yearname + QString("年") + " " + shengxiao + " " + xinzuo);
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

void DateTimeControl::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event);
    //resizeFontSize();
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

QString DateTimeControl::buildHtmlText()
{
    QDateTime time = QDateTime::currentDateTime();
    int lunaryear = 0, lunarmonth = 0, lunarday = 0;
    QString lunarstring = ChineseYear::GetLunarStringX(time.date().year(),
                                                       time.date().month(),
                                                       time.date().day(),
                                                       lunaryear,
                                                       lunarmonth,
                                                       lunarday);

    //ui->labelData->setText(time.toString("yyyy-MM-dd"));
    //ui->labelTime->setText(time.toString("hh:mm:ss"));
    //ui->labelChineseData->setText(lunarstring + " 星期" + XinQi[time.date().dayOfWeek()]);
    QString shengxiao = ChineseYear::GetShengXiao(lunaryear);
    QString xinzuo = ChineseYear::GetXinZuo(time.date().month(), time.date().day());
    QString yearname = ChineseYear::GetYearName(lunaryear);
    //ui->labelShengXiaoXinZuo->setText(yearname + QString("年") + " " + shengxiao + " " + xinzuo);

    QString html = buildHtmlParagraph(time.toString("yyyy-MM-dd"), dateFontSize) +
            buildHtmlParagraph(time.toString("hh:mm:ss"), timeFontSize) +
            buildHtmlParagraph(lunarstring + " 星期" + XinQi[time.date().dayOfWeek()], weekFontSize) +
            buildHtmlParagraph(yearname + QString("年") + " " + shengxiao + " " + xinzuo, yearFontSize);

    return html;
}

QString DateTimeControl::buildHtmlParagraph(QString text, int fontsize)
{
    //<p><span style=" font-size:200px;">This is a paragraph.</span></p>
    QString html ="<p align=\"center\"><span style=\" font-size:" + QString::number(fontsize, 'f', 0) +
                  "px;\">" + text +
                  "</span></p>";
    return html;
}

void DateTimeControl::trimFonSize(bool up)
{
    if(up)
    {
        if(isCanUp())
        {
            dateFontSize++;
            timeFontSize++;
            weekFontSize++;
            yearFontSize++;
        }
    }
    else
    {
        if(isCanDown())
        {
            dateFontSize--;
            timeFontSize--;
            weekFontSize--;
            yearFontSize--;
        }
    }
}

bool DateTimeControl::isCanDown()
{
    return dateFontSize > 10 && timeFontSize > 10 && weekFontSize > 10 && yearFontSize > 10;
}

bool DateTimeControl::isCanUp()
{
    return dateFontSize < 250 && timeFontSize < 250 && weekFontSize < 250 && yearFontSize < 250;
}

void DateTimeControl::resizeFontSize()
{
    int wwidth = 5 * timeFontSize;
    int wheight = dateFontSize + timeFontSize + weekFontSize + yearFontSize;
    int swidth = ui->scrollArea->width() - 50;
    int sheight = ui->scrollArea->height() - 200;
    while((wwidth < swidth || wheight < sheight) && isCanUp())
    {
        trimFonSize(true);
        wwidth = 5 * timeFontSize;
        wheight = 20 + dateFontSize + timeFontSize + weekFontSize + yearFontSize;
    }
    while((wwidth > swidth || wheight > sheight) && isCanDown())
    {
        trimFonSize(false);
        wwidth = 5 * timeFontSize;
        wheight = dateFontSize + timeFontSize + weekFontSize + yearFontSize;
    }
    //ui->label->setText(buildHtmlText());
}
