#ifndef CAMARATHREAD_H
#define CAMARATHREAD_H

#include <QThread>
#include <QDateTime>
#include <QTime>
#include <QDebug>
#include <QDir>
#include "opencv/cv.h"
#include "Core/ImageFormat.h"
#include "opencv2/video/background_segm.hpp"
#include "opencv2/calib3d/calib3d.hpp"
#include "Core/facedetecthelper.h"
#include "Core/videoprop.h"
#include "Thread/cameracollectorthread.h"
#include "Core/diskhelper.h"
#include "Thread/ringthread.h"

class CamaraThread : public QThread
{
    Q_OBJECT
public:
    explicit CamaraThread(QObject *parent = nullptr);
    void setStop();

    int getCamaraId() const;
    void setCamaraId(int value);

    int getRecMinSecond() const;
    void setRecMinSecond(int value);

    bool getIsDetectFace() const;
    void setIsDetectFace(bool isDetectFace);

    QSize getTargetSize() const;
    void setTargetSize(const QSize &value);

    bool getIsRecording() const;
    void setIsRecording(bool value);

protected:
    void run();

signals:
    void onImage(QImage const &image);
    void onTip(QString const &tip);
    void onNotify();
    void onConnectChanged(bool connected);
    void onFaceDetected(int faceCount);

public slots:
    void onFace(int camId, int faceCount);
    void onImageShow(int camId, QImage const &image);

private:
    bool _isRunning;
    bool _isDetectFace;
    bool isRecording;
    int camaraId;
    int recMinSecond;
    int recMaxSencond;
    QTime time;
    bool _isConnect;
    FaceDetectHelper faceHelper;
    QSize targetSize;
    void drawTime(cv::InputOutputArray img);
    DiskHelper diskHelper;
};

#endif // CAMARATHREAD_H
