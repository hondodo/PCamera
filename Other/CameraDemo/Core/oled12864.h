#ifndef OLED12864_H
#define OLED12864_H

#include <qsystemdetection.h>
#include <QImage>
#include <QDebug>
#ifdef Q_OS_UNIX
#include <wiringPi.h>
#include <wiringPiI2C.h>
#endif

class OLed12864
{
public:
    OLed12864();
    ~OLed12864();

    int Init(int devId);
    void WriteCMD(int reg, int cmd);
    void DisplayOff();
    void DisplayOn();
    void SetVcomh(int value);
    void CleanScreen();
    void WriteImage(const QImage *image);

private:
    int fd;

};

#endif // OLED12864_H
