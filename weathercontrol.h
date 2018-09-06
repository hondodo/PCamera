#ifndef WEATHERCONTROL_H
#define WEATHERCONTROL_H

#include <QWidget>
#include <QtNetwork/QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QTextCodec>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>
#include <QTimerEvent>

namespace Ui {
class WeatherControl;
}

class WeatherControl : public QWidget
{
    Q_OBJECT

public:
    explicit WeatherControl(QWidget *parent = 0);
    ~WeatherControl();

    void switchView();
    void updateWeather();

    bool getCanShowLive() const;
    void setCanShowLive(bool value);

    bool getCanShowReport() const;
    void setCanShowReport(bool value);

    void startDateTime();

protected:
    void timerEvent(QTimerEvent *event);

private slots:
    void finished(QNetworkReply *reply);
    void sslErrors(QNetworkReply *reply, const QList<QSslError> &errors);

private:
    Ui::WeatherControl *ui;
    bool isViewLive;
    QNetworkAccessManager *manager;
    QString liveUrl, reportUrl;
    QTextCodec *utf8Code;
    void dealMessage(QByteArray array);
    bool canShowLive;
    bool canShowReport;
    int timeId;
};

#endif // WEATHERCONTROL_H
