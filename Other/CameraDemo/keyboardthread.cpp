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
    temperature = 0;
    lastCheckRing = lastCheckDark = lastCheckPeople = lastCheckLight = QDateTime::currentDateTime().toMSecsSinceEpoch();
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
    qint64 ringels = 0, peopleels = 0, darkels = 0, lightels = 0;
    qint64 now = 0;
    _IsRunning = true;
    while(_IsRunning)
    {
        now = QDateTime::currentDateTime().toMSecsSinceEpoch();
        ringels = now - lastCheckRing;
        peopleels = now - lastCheckPeople;
        darkels = now - lastCheckDark;
        lightels = now - lastCheckLight;
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
        if((lightels < 0 || lightels > 1000) )
        {
            lightels = now;
            _lightIsTurnOn = digitalRead(P3) == 1;
#ifdef Q_OS_LINUX
            temperature = getTemp(P4);
#endif
        }
        this->msleep(20);
    }
    _IsRunning = false;
#endif
}

double KeyBoardThread::getTemperature() const
{
    return temperature;
}

void KeyBoardThread::setTemperature(double value)
{
    temperature = value;
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

#ifdef Q_OS_LINUX
int KeyBoardThread::oneWriteReset(int pin)
{
    int ack = 0;
    pinMode(pin, OUTPUT);
    digitalWrite(pin, HIGH);
    digitalWrite(pin, LOW);
    delayMicroseconds(480);
    digitalWrite(pin, HIGH);
    delayMicroseconds(30);
    pinMode(pin, INPUT);
    if(digitalRead(pin) == LOW)
    {
        ack = 1;
    }
    else
    {
        ack = 0;
    }
    delayMicroseconds(450);
    return ack;
}

void KeyBoardThread::writeBit(int pin, int bit)
{
    pinMode(pin, OUTPUT);
    digitalWrite(pin, LOW);
    delayMicroseconds(2);
    digitalWrite(pin, bit);
    delayMicroseconds(30);
    digitalWrite(pin, HIGH);
    delayMicroseconds(1);
}

void KeyBoardThread::oneWriteSendComm(int pin, int byte)
{
    for(int i = 0; i < 8; i++)
    {
        int sta = byte & 0x01;
        writeBit(pin, sta);
        byte >>= 1;
    }
}

int KeyBoardThread::readBit(int pin)
{
    int tmp = 0;
    pinMode(pin, OUTPUT);
    digitalWrite(pin, HIGH);
    digitalWrite(pin, LOW);
    delayMicroseconds(2);
    digitalWrite(pin, HIGH);
    pinMode(pin, INPUT);
    delayMicroseconds(2);
    tmp = digitalRead(pin);
    delayMicroseconds(40);
    return tmp;
}

int KeyBoardThread::oneWriteReceive(int pin)
{
    int i = 0, j = 0, k = 0;
    for(i = 0; i < 8; i++)
    {
        j = readBit(pin);
        k = (j << 7) | (k >> 1);
    }
    k = k & 0x00FF;
    return k;
}

double KeyBoardThread::tempChange(int lsb, int msb)
{
    float temp = 0;
    int tem = 0;
    if(msb >= 0xF0)
    {
        msb = 255 - msb;
        lsb = 256 - lsb;
        tem = -(msb * 16 * 16 + lsb);
    }
    else
    {
        tem = (msb * 16 * 16 + lsb);
    }
    temp = tem * 0.0625;
    return temp;
}

double KeyBoardThread::getTemp(int pin)
{
    double temp = 0;
    if(oneWriteReset(pin))
    {
        oneWriteSendComm(pin, 0xCC);
        oneWriteSendComm(pin, 0x44);
        if(oneWriteReset(pin))
        {
            oneWriteSendComm(pin, 0xCC);
            oneWriteSendComm(pin, 0xBE);
            int lsb = oneWriteReceive(pin);
            int msb = oneWriteReceive(pin);
            temp = tempChange(lsb, msb);
        }
    }
    return temp;
}
#endif
