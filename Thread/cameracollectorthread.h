#ifndef CAMERACOLLECTORTHREAD_H
#define CAMERACOLLECTORTHREAD_H

#include <QThread>
#include <QTime>
#include <QDateTime>
#include <QDebug>
#include <QMap>
#include <QQueue>
#include "opencv/cv.h"
#include "opencv2/video/background_segm.hpp"
#include "opencv2/calib3d/calib3d.hpp"
#include "Core/facedetecthelper.h"
#include "Core/videoprop.h"

class CameraCollectorThread : public QThread
{
    Q_OBJECT
public:
    explicit CameraCollectorThread(QObject *parent = nullptr);
    static CameraCollectorThread *Init;
    ~CameraCollectorThread();

    void setStop();
    void addMogCache(int cameraId, cv::Mat cap);
    void addFaceCache(int cameraId, cv::Mat cap);
    void addRecCache(int cameraId, cv::Mat cap);
    void addVideoProp(int cameraId, VideoProp prop);

protected:
    void run();

signals:
    void onMog(int cameraId);
    void onFace(int cameraId);

public slots:

private:
    bool _isRunning;
    int maxRecCacheCount;
    QMap<int, cv::Mat> camIdMogCache;
    QMap<int, cv::Mat> camIdFaceCache;
    QMap<int, QQueue<cv::Mat> > camIdRecCache;
    QMap<int, cv::VideoWriter * > camIdWriterCache;
    QMap<int, VideoProp> camIdProp;
};

#endif // CAMERACOLLECTORTHREAD_H
