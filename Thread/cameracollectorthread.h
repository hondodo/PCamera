#ifndef CAMERACOLLECTORTHREAD_H
#define CAMERACOLLECTORTHREAD_H

#include <QThread>
#include <QTime>
#include <QDateTime>
#include <QDebug>
#include "opencv/cv.h"
#include "opencv2/video/background_segm.hpp"
#include "opencv2/calib3d/calib3d.hpp"
#include "Core/facedetecthelper.h"

class CameraCollectorThread : public QThread
{
    Q_OBJECT
public:
    explicit CameraCollectorThread(QObject *parent = nullptr);
    ~CameraCollectorThread();

    void setStop();

protected:
    void run();

signals:

public slots:

private:
    bool _isRunning;

};

#endif // CAMERACOLLECTORTHREAD_H
