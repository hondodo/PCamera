#include "keyboardthread.h"

KeyBoardThread *KeyBoardThread::Init = new KeyBoardThread();
KeyBoardThread::KeyBoardThread(QObject *parent) : QThread(parent)
{
    _IsRunning = false;
    _isRing = false;
    _isDark = false;
    _isPeople = true;
    _isShowingDarkForm = false;
    _lightIsTurnOn = false;
    _isSetup = false;
    lastCheckRing = lastCheckDark = lastCheckPeople = QDateTime::currentDateTime().toMSecsSinceEpoch();
}

void KeyBoardThread::setStop()
{
    if(_isSetup)
    {
#ifdef Q_OS_LINUX
        digitalWrite(P3, 0);
#endif
    }
    _IsRunning = false;
    _isSetup = false;
}

void KeyBoardThread::run()
{
#ifdef Q_OS_LINUX
    _isSetup = wiringPiSetup() == 0;
    pinMode(P0, INPUT);
    pinMode(P1, INPUT);
    pinMode(P2, INPUT);
    pinMode(P3, OUTPUT);
    pullUpDnControl(P0, PUD_UP);//门铃 按下为0
    pullUpDnControl(P1, PUD_UP);//红外 有人为1
    pullUpDnControl(P2, PUD_UP);//光感 暗为1
    pullUpDnControl(P3, PUD_DOWN);//继电器 1 为接通
    qint64 ringels = 0, peopleels = 0, darkels = 0;
    qint64 now = 0;
    _IsRunning = true;
    while(_IsRunning)
    {
        now = QDateTime::currentDateTime().toMSecsSinceEpoch();
        ringels = now - lastCheckRing;
        peopleels = now - lastCheckPeople;
        darkels = now - lastCheckDark;
        if((ringels < 0 || ringels > 1000) && digitalRead(P0) == 0)
        {
            lastCheckRing = now;
            emit onKey(0);
        }
        if((peopleels < 0 || peopleels > 1000))
        {
            if(digitalRead(P1) == 1)
            {
                lastCheckPeople = now;
                _isPeople = true;
                emit onPeople();
            }
            else
            {
                _isPeople = false;
            }
        }
        if((darkels < 0 || darkels > 1000))
        {
            if(digitalRead(P2) == 1)
            {
                lastCheckDark = now;
                _isDark = true;
                emit onDark();
            }
            else
            {
                _isDark = false;
            }
        }
        _lightIsTurnOn = digitalRead(P3) == 1;
        this->msleep(20);
    }
    _IsRunning = false;
#endif
}

bool KeyBoardThread::lightIsTurnOn() const
{
    return _lightIsTurnOn;
}

void KeyBoardThread::setLightIsTurnOn(bool lightIsTurnOn)
{
    if(_isSetup && _IsRunning)
    {
#ifdef Q_OS_LINUX
        digitalWrite(P3, lightIsTurnOn? 1 : 0);
#endif
    }
    _lightIsTurnOn = lightIsTurnOn;
}

bool KeyBoardThread::isShowingDarkForm() const
{
    return _isShowingDarkForm;
}

void KeyBoardThread::setIsShowingDarkForm(bool isShowingDarkForm)
{
    _isShowingDarkForm = isShowingDarkForm;
}

bool KeyBoardThread::isPeople() const
{
    return _isPeople;
}


bool KeyBoardThread::isDark() const
{
    return _isDark;
}
