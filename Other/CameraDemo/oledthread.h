#ifndef OLEDTHREAD_H
#define OLEDTHREAD_H

#include <QThread>
#include <QPainter>
#include <QPen>
#include <opencv/cv.h>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include "Core/oled12864.h"
#include "Core/ImageFormat.h"

class OLedThread : public QThread
{
    Q_OBJECT
public:
    explicit OLedThread(QObject *parent = nullptr);
    static OLedThread *Init;
    ~OLedThread();

    void setStop();
    void testFontMargin(int fontPixe);

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
    int top, btm, mid;
    cv::Mat pixmap;
    void writeText(cv::Mat image, std::string text);
};

#endif // OLEDTHREAD_H
