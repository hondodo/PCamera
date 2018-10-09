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
#include "Core/facedetecthelper.h"
#include "Core/ImageFormat.h"
#include "Core/mogdetectobject.h"

class CameraCollectorThread : public QObject
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

    std::vector<cv::Rect> findMog(int cid);
    std::vector<cv::Rect> findMog(int cid, cv::Mat mat);
    bool findMogBOOL(int cid, cv::Mat mat);
    void removeMogRcs(int cid);
    std::vector<cv::Rect_<int> > findFace(cv::Mat mat);
    std::vector<cv::Rect_<int> > findFace(int cid);
    void emitOnImage(int cameraId, cv::Mat cap);
    void saveRec(int cid);
    void saveRec(int cid, cv::Mat mat);
    void endRec(int cid);
    bool newRec(int cid);


protected:
    void run();

signals:
    void onMog(int cameraId);
    void onFace(int cameraId, int count);
    void onImage(int cameraId, const QImage &image);

public slots:

private:
    bool _isRunning;
    int maxRecCacheCount;
    QMap<int, cv::Mat> camIdMogCache;
    QMap<int, cv::Mat> camIdFaceCache;
    QMap<int, QQueue<cv::Mat> > camIdRecCache;
    QMap<int, cv::VideoWriter * > camIdWriterCache;
    QMap<int, VideoProp> camIdProp;
    QMap<int, MogDetectObject> camIdMogObj;
    void saveRec();
    FaceDetectHelper faceHelper;

    //------FACE-------//
    std::vector<cv::Rect_<int> > faces;
    std::vector<cv::Rect_<int> > eyes;
    bool canDetectFace;
    void findFace();
    void findMog();
    cv::Mat kernel;
};

#endif // CAMERACOLLECTORTHREAD_H
