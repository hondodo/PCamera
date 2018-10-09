#include "weatherreportcontrol.h"
#include "ui_weatherreportcontrol.h"

const QString XinQi[] = {"*", "星期一", "星期二", "星期三", "星期四", "星期五", "星期六", "星期日"};

WeatherReportControl::WeatherReportControl(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WeatherReportControl)
{
    ui->setupUi(this);
}

WeatherReportControl::~WeatherReportControl()
{
    delete ui;
}

void WeatherReportControl::SetWeather(QString date, int weekIndex, QString daytemp, QString dayweather,
                                      QString nighttemp, QString nightweather, bool isShowingToday,
                                      QString addctionMessage)
{
    QString week = QString("(") + XinQi[weekIndex] + QString(")");
    ui->labelDate->setText(date);
    ui->labelWeek->setText(week);
    ui->labelReTempDay->setText(daytemp);
    ui->labelReWeaDay->setText(dayweather);
    ui->labelReTempNight->setText(nighttemp);
    ui->labelReWeaNight->setText(nightweather);
    QPixmap pix = GetWeatherIcon(dayweather, true);
    ui->labelIcon->setPixmap(pix);
    if(isShowingToday)
    {
        ui->labelDate->setText(QString("今天"));
    }
    if(addctionMessage.isNull() || addctionMessage.isEmpty())
    {}
    else
    {
        QString addtext = addctionMessage + "(" + date + ")";
        ui->labelDate->setText(addtext);
    }
    bool issame = dayweather == nightweather;
    ui->labelTo->setVisible(!issame);
    //ui->labelReWeaNight->setVisible(!issame);
    if(issame)
    {
        ui->labelReWeaNight->setText("");
    }
    bool issametemp = daytemp == nighttemp;
    ui->labelToTemp->setVisible(!issametemp);
    ui->labelReTempDay->setVisible(!issametemp);
    ui->labelDayDec->setVisible(!issametemp);
}

QPixmap WeatherReportControl::GetWeatherIcon(QString weather, bool isday, int width, int height)
{
    QImage image(getIconFileName(weather, isday));
    if(image.isNull())
    {
        image = QImage(":/Rcs/sunny.png");
    }
    image = image.scaled(width, height, Qt::KeepAspectRatioByExpanding);
    QPixmap pix = QPixmap::fromImage(image);
    return pix;
}

QString WeatherReportControl::getIconFileName(QString weather, bool isday)
{
    QString filename = "sunny";
    if(!isday) filename = "sunny_night";
    if(weather.contains("云"))
    {
        filename = "cloudy1";
        if(!isday) filename = "cloudy1_night";
    }
    else if(weather.contains("雨"))
    {
        filename = "light_rain";
        if(!isday) filename = "light_rain";
    }
    else if(weather.contains("雾"))
    {
        filename = "mist";
        if(!isday) filename = "mist_night";
    }
    else if(weather.contains("雪"))
    {
        filename = "snow1";
        if(!isday) filename = "snow1_night";
    }


    if(weather == QString("晴"))
    {
        filename = "sunny";
        if(!isday) filename = "sunny_night";
    }
    else if(weather == QString("多云"))
    {
        filename = "cloudy2";
        if(!isday) filename = "cloudy2_night";
    }
    else if(weather == QString("阴"))
    {
        filename = "cloudy5";
        if(!isday) filename = "cloudy5";
    }
    else if(weather == QString("阵雨"))
    {
        filename = "shower2";
        if(!isday) filename = "shower2_night";
    }
    else if(weather == QString("小雨"))
    {
        filename = "light_rain";
        if(!isday) filename = "light_rain";
    }
    else if(weather == QString("雷阵雨"))
    {
        filename = "tstorm3";
        if(!isday) filename = "tstorm3";
    }
    else if(weather == QString("中雨"))
    {
        filename = "shower3";
        if(!isday) filename = "shower3";
    }
    else if(weather == QString("大雨"))
    {
        filename = "shower3";
        if(!isday) filename = "shower3";
    }
    else if(weather == QString("暴雨"))
    {
        filename = "shower3";
        if(!isday) filename = "shower3";
    }
    else if(weather == QString("特大暴雨"))
    {
        filename = "shower3";
        if(!isday) filename = "shower3";
    }

    filename = ":/Rcs/" + filename + ".png";
    return filename;
}
