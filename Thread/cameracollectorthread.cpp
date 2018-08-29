#include "cameracollectorthread.h"

using namespace std;
using namespace cv;

CameraCollectorThread *CameraCollectorThread::Init = new CameraCollectorThread();
CameraCollectorThread::CameraCollectorThread(QObject *parent) : QThread(parent)
{
    _isRunning = false;
    maxRecCacheCount = 25 * 60 * 2;//25fp/s * 60s * 2
#ifdef Q_OS_WIN
    canDetectFace = faceHelper.init("D:/Potatokid/OpenCV/sources/data/haarcascades/haarcascade_frontalface_alt.xml",
                                    "");
#else
    canDetectFace = faceHelper.init("/home/pi/Source/PCamera/Data/haarcascades/haarcascade_frontalcatface.xml",
                                    "");
#endif
}

CameraCollectorThread::~CameraCollectorThread()
{
    setStop();
}

void CameraCollectorThread::setStop()
{
    _isRunning = false;
    int reccount = camIdWriterCache.count();
    for(int i = 0; i < reccount; i++)
    {
        VideoWriter *vw = camIdWriterCache.values().at(i);
        if(vw != NULL)
        {
            if(vw->isOpened())
            {
                vw->release();
            }
            delete vw;
            vw = NULL;
        }
    }
    camIdWriterCache.clear();
}

void CameraCollectorThread::emitOnImage(int cameraId, Mat cap)
{
    QImage image = ImageFormat::Mat2QImage(cap);
    emit onImage(cameraId, image);
}

void CameraCollectorThread::addMogCache(int cameraId, Mat cap)
{
    if(!camIdMogCache.contains(cameraId))
    {
        cv::Mat mat = cap.clone();
        camIdMogCache.insert(cameraId, mat);
    }
}

void CameraCollectorThread::addFaceCache(int cameraId, Mat cap)
{
    if(!camIdFaceCache.contains(cameraId))
    {
        cv::Mat mat;
        resize(cap, mat, Size(200, 150), 0, 0);
        cvtColor(mat, mat, cv::COLOR_BGR2GRAY);
        camIdFaceCache.insert(cameraId, mat);
    }
}

void CameraCollectorThread::addRecCache(int cameraId, Mat cap)
{
    cv::Mat mat = cap.clone();
    if(!camIdRecCache.contains(cameraId))
    {
        QQueue<cv::Mat> queue;
        queue.enqueue(mat);
        camIdRecCache.insert(cameraId, queue);
    }
    else
    {
        (&camIdRecCache[cameraId])->enqueue(mat);
    }
}

void CameraCollectorThread::addVideoProp(int cameraId, VideoProp prop)
{
    camIdProp[cameraId] = prop;
}

void CameraCollectorThread::saveRec(int cid)
{
    if(!camIdRecCache.contains(cid)) return;
    if(!camIdProp.contains(cid))
    {
        assert("no camera prop @ camera collector thread");
    }
    int framecount = (&camIdRecCache[cid])->count();
    VideoProp prop = camIdProp[cid];
    if(framecount > 0)
    {
        if(!camIdWriterCache.contains(cid))
        {
            VideoWriter *vw = new VideoWriter();
            camIdWriterCache[cid] = vw;
        }
        VideoWriter *vw = camIdWriterCache[cid];
        if(!vw->isOpened())
        {
            vw->open(prop.getFileName().toStdString(), CV_FOURCC('D', 'I', 'V', 'X'),
                     prop.getFps(), Size(prop.getWidth(), prop.getHeight()));
        }

        for(int j = 0; j < framecount; j++)
        {
            Mat mat = (&camIdRecCache[cid])->dequeue();
            if(vw->isOpened())
            {
                vw->write(mat);
            }
            mat.release();
            if(j > maxRecCacheCount)
            {
                break;
            }
            this->msleep(2);
        }

        while((&camIdRecCache[cid])->count() > maxRecCacheCount)
        {
            Mat mat = (&camIdRecCache[cid])->dequeue();
            mat.release();
        }
    }
}

void CameraCollectorThread::saveRec()
{
    int recCount = camIdRecCache.count();
    for(int i = 0; i < recCount; i++)
    {
        int cid = camIdRecCache.keys().at(i);
        saveRec(cid);
    }
}

void CameraCollectorThread::findFace(int cid)
{
    if(camIdFaceCache.contains(cid))
    {
        cv::Mat mat = camIdFaceCache.value(cid);
        faceHelper.detectFaces(mat, faces);
        faceHelper.detectEyes(mat, eyes);
        if(!faces.empty() && (int)faces.size() > 0)
        {
            qDebug() << "On Face: " << cid;
            emit onFace(cid, (int)faces.size());
        }
        mat.release();
        camIdFaceCache.remove(cid);
    }
}

void CameraCollectorThread::findFace()
{
    if(canDetectFace && !camIdFaceCache.isEmpty())
    {
        while(camIdFaceCache.count())
        {
            int cid = camIdFaceCache.keys().first();
            findFace(cid);
        }
        camIdFaceCache.clear();
    }
}

void CameraCollectorThread::findMog(int cid)
{
    if(camIdMogCache.contains(cid))
    {
        cv::Mat mat = camIdMogCache.value(cid);
        mat.release();
        camIdMogCache.remove(cid);
    }
}

void CameraCollectorThread::findMog()
{
    if(canDetectFace && !camIdMogCache.isEmpty())
    {
        while(camIdMogCache.count() > 0)
        {
            int cid = camIdMogCache.keys().first();
            findMog(cid);
        }
        camIdMogCache.clear();
    }
}

void CameraCollectorThread::run()
{
    return;

    _isRunning = true;

    while (_isRunning)
    {
        //Save
        saveRec();
        //Face
        findFace();
        //MOG
        findMog();
        this->msleep(20);
    }
}
