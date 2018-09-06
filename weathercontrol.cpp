#include "weathercontrol.h"
#include "ui_weathercontrol.h"

const QString XinQi[] = {"*", "一", "二", "三", "四", "五", "六", "日"};
WeatherControl::WeatherControl(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WeatherControl)
{
    ui->setupUi(this);
    timeId = 0;
    isViewLive = false;
    canShowLive = false;
    canShowReport = false;
    switchView();
    utf8Code = QTextCodec::codecForName("UTF-8");
    manager = new QNetworkAccessManager();
    connect(manager, SIGNAL(sslErrors(QNetworkReply*,QList<QSslError>)),
            this, SLOT(sslErrors(QNetworkReply*,QList<QSslError>)));
    connect(manager, SIGNAL(finished(QNetworkReply*)),
            this, SLOT(finished(QNetworkReply*)));
    liveUrl = QString("https://restapi.amap.com/v3/weather/weatherInfo?key=90ff0faabf905ddd468a2dfe596e7b87&city=440883&extensions=base&output=json");
    reportUrl = QString("https://restapi.amap.com/v3/weather/weatherInfo?key=90ff0faabf905ddd468a2dfe596e7b87&city=440883&extensions=all&output=json");
}

WeatherControl::~WeatherControl()
{
    if(timeId > 0)
    {
        killTimer(timeId);
        timeId = 0;
    }
    if(manager != Q_NULLPTR)
    {
        manager->deleteLater();
        manager = Q_NULLPTR;
    }
    delete ui;
}

void WeatherControl::switchView()
{
    isViewLive = !isViewLive;
    ui->labelReportTime->setVisible(canShowLive);
    if(canShowLive && canShowReport)
    {
        ui->widgetToday->setVisible(isViewLive);
        ui->widgetReport->setVisible(!isViewLive);
    }
    else
    {
        ui->widgetReport->setVisible(canShowReport);
        ui->widgetToday->setVisible(canShowLive);
    }
}

void WeatherControl::updateWeather()
{
    manager->get(QNetworkRequest(QUrl(liveUrl)));
    manager->get(QNetworkRequest(QUrl(reportUrl)));
}

void WeatherControl::finished(QNetworkReply *reply)
{
    if(reply != Q_NULLPTR)
    {
        QByteArray array = reply->readAll();
        qDebug() << array;
        reply->deleteLater();
        reply = Q_NULLPTR;
        dealMessage(array);
    }
}

void WeatherControl::sslErrors(QNetworkReply *reply, const QList<QSslError> &errors)
{
    qDebug() << "SSL ERROR";
    reply->ignoreSslErrors(errors);
}

void WeatherControl::dealMessage(QByteArray array)
{
    if(array.isNull() || array.isEmpty())
    {
        return;
    }
    QString text = "";
    if(utf8Code != 0)
    {
        text = utf8Code->toUnicode(array);
    }
    else
    {
        text = QString(array);
    }
    if(text.isNull() || text.isEmpty())
    {
        return;
    }
    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(array, &error);
    if(error.error == QJsonParseError::NoError)
    {
        QJsonObject json = doc.object();
        if(json.contains("status"))
        {
            if(json["status"] == "1")//success
            {
                if(json.contains("lives"))
                {
                    if(json["lives"].isArray())
                    {
                        QJsonArray lives = json["lives"].toArray();
                        if(!lives.isEmpty() && lives.count() > 0)
                        {
                            QJsonObject live = lives.first().toObject();
                            if(!live.isEmpty())
                            {
                                QString city = live["city"].toString();
                                QString temp = live["temperature"].toString();
                                QString weather = live["weather"].toString();
                                QString reporttime = live["reporttime"].toString();
                                ui->labelCity->setText(city);
                                ui->labelTempNow->setText(temp);
                                ui->labelWeaNow->setText(weather);
                                ui->labelReportTime->setText(reporttime);
                                canShowLive = true;
                            }
                        }
                    }
                }
                else if(json.contains("forecasts"))
                {
                    if(json["forecasts"].isArray())
                    {
                        QJsonArray forecasts = json["forecasts"].toArray();
                        if(!forecasts.isEmpty() && forecasts.count() > 0)
                        {
                            QJsonObject forecast = forecasts.at(0).toObject();
                            if(forecast.contains("casts") && forecast["casts"].isArray())
                            {
                                QJsonArray casts = forecast["casts"].toArray();
                                if(!casts.isEmpty() && casts.count() > 0)
                                {
                                    canShowReport = true;
                                    for(int i = 0; i < 4; i++)
                                    {
                                        if(casts.count() >= (i + 1))
                                        {
                                            QJsonObject cast = casts.at(i).toObject();
                                            if(!cast.isEmpty())
                                            {
                                                QString date = QDate::fromString(cast["date"].toString(), "yyyy-MM-dd").toString("MM-dd");
                                                int index = cast["week"].toString().toInt();
                                                if(index < 0 || index > 7)
                                                {
                                                    index = 0;
                                                }
                                                QString week = QString("周") + XinQi[index];
                                                QString daytemp = cast["daytemp"].toString();
                                                QString dayweather = cast["dayweather"].toString();
                                                QString nighttemp = cast["nighttemp"].toString();
                                                QString nightweather = cast["nightweather"].toString();
                                                if(i == 0)
                                                {
                                                    ui->labelDate0->setText(date);
                                                    ui->labelWeek0->setText(week);
                                                    ui->labelReTemp0Day->setText(daytemp);
                                                    ui->labelReWea0Day->setText(dayweather);
                                                    ui->labelReTemp0Night->setText(nighttemp);
                                                    ui->labelReWea0Night->setText(nightweather);
                                                    ui->labelTemp0Day->setText(daytemp);
                                                    ui->labelWea0Day->setText(dayweather);
                                                    ui->labelTemp0Night->setText(nighttemp);
                                                    ui->labelWea0Night->setText(nightweather);
                                                }
                                                else if(i == 1)
                                                {
                                                    ui->labelDate1->setText(date);
                                                    ui->labelWeek1->setText(week);
                                                    ui->labelReTemp1Day->setText(daytemp);
                                                    ui->labelReWea1Day->setText(dayweather);
                                                    ui->labelReTemp1Night->setText(nighttemp);
                                                    ui->labelReWea1Night->setText(nightweather);
                                                }
                                                else if(i == 2)
                                                {
                                                    ui->labelDate2->setText(date);
                                                    ui->labelWeek2->setText(week);
                                                    ui->labelReTemp2Day->setText(daytemp);
                                                    ui->labelReWea2Day->setText(dayweather);
                                                    ui->labelReTemp2Night->setText(nighttemp);
                                                    ui->labelReWea2Night->setText(nightweather);
                                                }
                                                else if(i == 3)
                                                {
                                                    ui->labelDate3->setText(date);
                                                    ui->labelWeek3->setText(week);
                                                    ui->labelReTemp3Day->setText(daytemp);
                                                    ui->labelReWea3Day->setText(dayweather);
                                                    ui->labelReTemp3Night->setText(nighttemp);
                                                    ui->labelReWea3Night->setText(nightweather);
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

bool WeatherControl::getCanShowReport() const
{
    return canShowReport;
}

void WeatherControl::setCanShowReport(bool value)
{
    canShowReport = value;
}

void WeatherControl::startDateTime()
{
    timeId = startTimer(100);
}

void WeatherControl::timerEvent(QTimerEvent *event)
{
    if(event->timerId() == timeId)
    {
        ui->labelTime->setText(QDateTime::currentDateTime().toString("hh:mm:ss"));
    }
}

bool WeatherControl::getCanShowLive() const
{
    return canShowLive;
}

void WeatherControl::setCanShowLive(bool value)
{
    canShowLive = value;
}
