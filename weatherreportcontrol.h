#ifndef WEATHERREPORTCONTROL_H
#define WEATHERREPORTCONTROL_H

#include <QWidget>
#include <QDateTime>

namespace Ui {
class WeatherReportControl;
}

class WeatherReportControl : public QWidget
{
    Q_OBJECT

public:
    explicit WeatherReportControl(QWidget *parent = 0);
    ~WeatherReportControl();

    void SetWeather(QString date, int weekIndex, QString daytemp,
                    QString dayweather, QString nighttemp, QString nightweather, bool isShowingToday = false);

    QPixmap GetWeatherIcon(QString weather, bool isday, int width = 52, int height = 52);

private:
    Ui::WeatherReportControl *ui;
    QString getIconFileName(QString weather, bool isday);
};

#endif // WEATHERREPORTCONTROL_H
