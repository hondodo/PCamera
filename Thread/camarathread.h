#ifndef CAMARATHREAD_H
#define CAMARATHREAD_H

#include <QThread>
#include <QDateTime>
#include "opencv/cv.h"
#include "Core/ImageFormat.h"
#include "opencv2/video/background_segm.hpp"

class CamaraThread : public QThread
{
    Q_OBJECT
public:
    explicit CamaraThread(QObject *parent = nullptr);
    void setStop();

    int getCamaraId() const;
    void setCamaraId(int value);

protected:
    void run();

signals:
    void onImage(QImage const &image);
    void onTip(QString const &tip);
    void onNotify();
    void onConnectChanged(bool connected);

public slots:

private:
    bool _isRunning;
    int camaraId;
    QTime time;
    bool _isConnect;
};

#endif // CAMARATHREAD_H
