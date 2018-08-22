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

class CamaraThread : public QThread
{
    Q_OBJECT
public:
    explicit CamaraThread(QObject *parent = nullptr);
    void setStop();

    int getCamaraId() const;
    void setCamaraId(int value);

    QString getRecDir() const;
    void setRecDir(const QString &value);

    int getRecMinSecond() const;
    void setRecMinSecond(int value);

    bool getIsDetectFace() const;
    void setIsDetectFace(bool isDetectFace);

protected:
    void run();

signals:
    void onImage(QImage const &image);
    void onTip(QString const &tip);
    void onNotify();
    void onConnectChanged(bool connected);
    void onFaceDetected(int faceCount);

public slots:

private:
    bool _isRunning;
    bool _isDetectFace;
    int camaraId;
    int recMinSecond;
    int recMaxSencond;
    QTime time;
    bool _isConnect;
    QString recDir;
    FaceDetectHelper faceHelper;
};

#endif // CAMARATHREAD_H
