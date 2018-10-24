#ifndef KEYBOARDTHREAD_H
#define KEYBOARDTHREAD_H

#include <QObject>
#include <QThread>
#include <QDebug>
#include <QDateTime>
#include <QQueue>
#ifdef Q_OS_LINUX
#include <wiringPi.h>
#endif

#define P0 4
#define P1 5
#define P2 6
#define P3 0
#define P4 2

class KeyBoardThread : public QThread
{
    Q_OBJECT
public:
    explicit KeyBoardThread(QObject *parent = 0);
    static KeyBoardThread *Init;
    void setStop();
    bool isDark() const;
    bool isPeople() const;

    bool isShowingDarkForm() const;
    void setIsShowingDarkForm(bool isShowingDarkForm);

    bool lightIsTurnOn() const;
    void setLightIsTurnOn(bool lightIsTurnOn);

    float getTemperature() const;
    void setTemperature(float value);

protected:
    void run();

signals:
    void onKey(int key);
    void onDark();
    void onPeople();

public slots:

private:
    bool _IsRunning;
    bool _isSetup;

    bool _isRing;
    bool _isDark;
    bool _isPeople;

    bool _lightIsTurnOn;

    int ringKeyOnCount;

    bool _isShowingDarkForm;
    float temperature;
    float tempMax, tempMin;
    QQueue<float> tempList;

    qint64 lastCheckRing, lastCheckDark, lastCheckPeople, lastCheckLight;

#ifdef Q_OS_LINUX
    int oneWriteReset(int pin);
    void writeBit(int pin, int bit);
    void oneWriteSendComm(int pin, int byte);
    int readBit(int pin);
    int oneWriteReceive(int pin);
    double tempChange(int lsb, int msb);
    double getTemp(int pin);
#endif
};

#endif // KEYBOARDTHREAD_H
