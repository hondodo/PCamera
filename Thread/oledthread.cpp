#include "oledthread.h"

OLedThread::OLedThread(QObject *parent) : QThread(parent)
{
    _isRunning = false;
    oled.Init(0x3c);
    oled.DisplayOn();
}

OLedThread::~OLedThread()
{
    setStop();
}

void OLedThread::setStop()
{
    _isRunning = false;
    oled.DisplayOff();
}

void OLedThread::run()
{
    _isRunning = true;
    oled.Init(0x3c);
    oled.DisplayOn();
    while(_isRunning)
    {
        QImage img = imageCache.copy();
        oled.WriteImage(&img);
        for(int i = 0; i < 50; i++)
        {
            if(!_isRunning)
            {
                break;
            }
            this->msleep(20);
        }
    }
    setStop();
}

void OLedThread::setImage(QImage image)
{
    imageCache = image.copy();
}

void OLedThread::setMessage(QString message)
{
    imageCache = QImage(128, 64, QImage::Format_ARGB32);
    QPainter painter(&imageCache);
    painter.drawText(imageCache.rect(), message);
}
