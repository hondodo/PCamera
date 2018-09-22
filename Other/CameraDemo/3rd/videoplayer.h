#ifndef WORKER_H
#define WORKER_H

#include <QObject>
#include <QThread>
#include <QDebug>
#include <QCamera>
#include <QCameraInfo>
#include <QApplication>
#include <QTime>
#include <QDateTime>
#include "cameratype.h"
#include "mathelper.h"
#include "ffmpeghelper.h"
#include "videofilehelper.h"

extern "C"
{
#include "libavformat/avformat.h"
#include "libavcodec/avcodec.h"
#include "libswscale/swscale.h"
#include <libavdevice/avdevice.h>
}

class VideoPlayer:public QThread
{
    Q_OBJECT
public:
    VideoPlayer();
    ~VideoPlayer();
    void setStop();

    QString getCameraUrl() const;
    void setCameraUrl(const QString &value);

    CAMERATYPE getCameraType() const;
    void setCameraType(const CAMERATYPE &value);

    bool getCheckBrighness() const;
    void setCheckBrighness(bool value);

    bool getFixBrighnessByTime() const;
    void setFixBrighnessByTime(bool value);

    QString getCameraName() const;
    void setCameraName(const QString &value);

protected:
    void run() override;

signals:
    void onFrame(QImage);

protected slots:

private:
    bool _isRunning;

    QString cameraUrl;
    CAMERATYPE cameraType;//0-local 1-web
    QString cameraName;
    bool checkBrighness;
    bool fixBrighnessByTime;
};

#endif // WORKER_H
