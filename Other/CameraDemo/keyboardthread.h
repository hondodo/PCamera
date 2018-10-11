#ifndef KEYBOARDTHREAD_H
#define KEYBOARDTHREAD_H

#include <QObject>
#include <QThread>
#include <QDebug>
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

protected:
    void run();

signals:
    void onKey(int key);

public slots:

private:
    bool _IsRunning;
};

#endif // KEYBOARDTHREAD_H
