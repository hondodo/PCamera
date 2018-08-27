#include "oled12864.h"

OLed12864::OLed12864()
{
    fd = 0;
}

OLed12864::~OLed12864()
{
    DisplayOff();
}

int OLed12864::Init(int devId)
{
    int result = 0;
#ifdef Q_OS_UNIX
    result = wiringPiSetup();
    if(result == 0)
    {
        fd = wiringPiI2CSetup(devId);
        if(fd > 0)
        {
            wiringPiI2CWriteReg8(fd, 0x00, 0xa1);
            wiringPiI2CWriteReg8(fd, 0x00, 0xc8);
            wiringPiI2CWriteReg8(fd, 0x00, 0x8d);
            wiringPiI2CWriteReg8(fd, 0x00, 0x14);
            wiringPiI2CWriteReg8(fd, 0x00, 0xa6);
            wiringPiI2CWriteReg8(fd, 0x00, 0x21);
            wiringPiI2CWriteReg8(fd, 0x00, 0x00);
            wiringPiI2CWriteReg8(fd, 0x00, 0x7f);
            //wiringPiI2CWriteReg8(fd, 0x00, 0x8d);//set DC-DC enable
            //wiringPiI2CWriteReg8(fd, 0x00, 0x14);//
        }
    }
#endif
    return result;
}

void OLed12864::WriteCMD(int reg, int cmd)
{
#ifdef Q_OS_UNIX
    if(fd > 0)
    {
        wiringPiI2CWriteReg8(fd, reg, cmd);
    }
#endif
}

void OLed12864::DisplayOff()
{
    WriteCMD(0x00, 0xae);
}

void OLed12864::DisplayOn()
{
    WriteCMD(0x00, 0xaf);
}

void OLed12864::SetVcomh(int value)
{
    WriteCMD(0x00, 0xdb);
    WriteCMD(0x00, value);
}

void OLed12864::CleanScreen()
{
#ifdef Q_OS_UNIX
    int m, n;
    for(m = 0; m < 8; m++)
    {
        WriteCMD(0x00, 0xb0 + m);//page0 - page1
        for(n = 0; n < 128; n++)
        {
            WriteCMD(0x40, 0x00);
        }
    }
#endif
}

void OLed12864::WriteImage(const QImage *image)
{
    if(image != Q_NULLPTR && !image->isNull())
    {
        QImage tmp = image->convertToFormat(QImage::Format_RGB32);
        tmp = tmp.scaled(128, 64, Qt::IgnoreAspectRatio);
        for(int row = 0; row < 8; row++)
        {
            WriteCMD(0x00, 0xb0 + row);
            for(int w = 0; w < 128; w++)
            {
                int data = 0x00;
                for(int i = 0; i < 8; i++)
                {
                    QColor color = tmp.pixel(w, row * 8 + i);
                    data = data >> 1;
                    int c = color.red() * 0.39 + color.green() * 0.51 + color.blue() * 0.11;
                    bool bit = c > 128;
                    if(bit)
                    {
                        data = data | 0x80;
                    }
                }
                WriteCMD(0x40, data);
            }
        }
    }
}
