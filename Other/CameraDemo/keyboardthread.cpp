#include "keyboardthread.h"

KeyBoardThread *KeyBoardThread::Init = new KeyBoardThread();
KeyBoardThread::KeyBoardThread(QObject *parent) : QThread(parent)
{
    _IsRunning = false;
}

void KeyBoardThread::setStop()
{
    _IsRunning = false;
}

void KeyBoardThread::run()
{
#ifdef Q_OS_LINUX
    _IsRunning = true;
    wiringPiSetup();
    pinMode(P0, INPUT);
    pinMode(P1, INPUT);
    pinMode(P2, INPUT);
    pinMode(P3, INPUT);
    pullUpDnControl(P0, PUD_UP);
    pullUpDnControl(P1, PUD_UP);
    pullUpDnControl(P2, PUD_UP);
    pullUpDnControl(P3, PUD_UP);
    while(_IsRunning)
    {
        if(digitalRead(P0) == 0)
        {
            emit onKey(0);
            this->msleep(1000);
        }
        //qDebug() << digitalRead(P0) << digitalRead(P1) <<
        //            digitalRead(P2) << digitalRead(P3);
        this->msleep(20);
    }
#endif
}
