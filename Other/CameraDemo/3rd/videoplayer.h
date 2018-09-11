#ifndef WORKER_H
#define WORKER_H

#include <QObject>
#include <QThread>
#include <QDebug>
#include <QCamera>
#include <QCameraInfo>
#include <QApplication>

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
signals:
    void sig_GetOneFrame(QImage);
protected slots:
private:
    void show_vfw_device();
    void show_dshow_device();
    void show_dshow_device_option();
    void show_avfoundation_device();
};

#endif // WORKER_H
