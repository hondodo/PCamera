#ifndef KEYBOARDTHREAD_H
#define KEYBOARDTHREAD_H

#include <QObject>
#include <QThread>
#include <QDebug>
#include <QDateTime>
#ifdef Q_OS_LINUX
#include <wiringPi.h>
#endif

#define P0 4
#define P1 5
#define P2 6
#define P3 0

class KeyBoardThread : public QThread
{
    Q_OBJECT
public:
    explicit KeyBoardThread(QObject *parent = 0);
    static KeyBoardThread *Init;
    void setStop();

    bool isDark() const;

    bool isPeople() const;

protected:
    void run();

signals:
    void onKey(int key);
    void onDark();
    void onPeople();

public slots:

private:
    bool _IsRunning;

    bool _isRing;
    bool _isDark;
    bool _isPeople;

    qint64 lastCheckRing, lastCheckDark, lastCheckPeople;
};

#endif // KEYBOARDTHREAD_H
