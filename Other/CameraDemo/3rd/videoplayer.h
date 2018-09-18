#ifndef WORKER_H
#define WORKER_H

#include <QObject>
#include <QThread>
#include <QDebug>
#include <QCamera>
#include <QCameraInfo>
#include <QApplication>
#include "cameratype.h"

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
    void startPlay();
    void run();

    QString getCameraUrl() const;
    void setCameraUrl(const QString &value);

    CAMERATYPE getCameraType() const;
    void setCameraType(const CAMERATYPE &value);

    static init();

signals:
    void sig_GetOneFrame(QImage);

protected slots:

private:
    void show_vfw_device();
    void show_dshow_device();
    void show_dshow_device_option();
    void show_avfoundation_device();

    QString cameraUrl;
    CAMERATYPE cameraType;//0-local 1-web
};

#endif // WORKER_H
