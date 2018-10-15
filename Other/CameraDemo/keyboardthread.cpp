#include "keyboardthread.h"

KeyBoardThread *KeyBoardThread::Init = new KeyBoardThread();
KeyBoardThread::KeyBoardThread(QObject *parent) : QThread(parent)
{
    _IsRunning = false;
    _isRing = false;
    _isDark = false;
    _isPeople = false;
    lastCheckRing = lastCheckDark = lastCheckPeople = QDateTime::currentDateTime();
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
    pullUpDnControl(P0, PUD_UP);//门铃 按下为0
    pullUpDnControl(P1, PUD_UP);//红外 有人为1
    pullUpDnControl(P2, PUD_UP);//光感 暗为1
    pullUpDnControl(P3, PUD_UP);
    while(_IsRunning)
    {
        if(digitalRead(P0) == 0)
        {
            emit onKey(0);
            this->msleep(1000);
        }
        if(digitalRead(P1) == 1)
        {
            emit onPeople();
        }
        if(digitalRead(P2) == 1)
        {
            emit onDark();
        }
        this->msleep(20);
    }
#endif
}
