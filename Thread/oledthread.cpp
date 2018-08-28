#include "oledthread.h"

OLedThread::OLedThread(QObject *parent) : QThread(parent)
{
    _isRunning = false;
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
    oled.CleanScreen();
    QFont font = QFont();
    font.setPixelSize(10);
    while(_isRunning)
    {
        QImage img = QImage(128, 64, QImage::Format_ARGB32);
        QPainter painter(&img);
        painter.fillRect(img.rect(), Qt::black);
        painter.setPen(Qt::white);
        QFont font = painter.font();
        font.setPixelSize(10);
        painter.setFont(font);
        painter.drawText(img.rect(), message);
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
    this->message = message;
}
