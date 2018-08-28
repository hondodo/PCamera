#ifndef OLEDTHREAD_H
#define OLEDTHREAD_H

#include <QThread>
#include <QPainter>
#include <QPen>
#include "Core/oled12864.h"

class OLedThread : public QThread
{
    Q_OBJECT
public:
    explicit OLedThread(QObject *parent = nullptr);
    ~OLedThread();

    void setStop();

protected:
    void run();

signals:

public slots:
    void setImage(QImage image);
    void setMessage(QString message);

private:
    bool _isRunning;
    OLed12864 oled;
    QImage imageCache;
    QString message;
};

#endif // OLEDTHREAD_H
