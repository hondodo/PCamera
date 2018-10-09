#include "cameracollectorhelper.h"

using namespace std;
using namespace cv;

CameraCollectorThread *CameraCollectorThread::Init = new CameraCollectorThread();
CameraCollectorThread::CameraCollectorThread(QObject *parent) : QObject(parent)
{
    _isRunning = false;
    maxRecCacheCount = 25 * 60 * 2;//25fp/s * 60s * 2
#ifdef Q_OS_WIN
    canDetectFace = faceHelper.init("D:/Potatokid/OpenCV/sources/data/haarcascades/haarcascade_frontalface_alt.xml",
                                    "");
#else
    canDetectFace = faceHelper.init("/home/pi/Source/PCamera/Data/haarcascades/haarcascade_frontalface_alt.xml",
                                    "");
#endif
    kernel = (cv::Mat_<float>(3, 3) << 0, -1, 0, 0, 5, 0, 0, -1, 0);
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
        cv::Mat mat = cap.clone();
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
    int framecount = (&camIdRecCache[cid])->count();
    if(framecount > 0)
    {
        if(!camIdWriterCache.contains(cid))
        {
            return;
        }
        VideoWriter *vw = camIdWriterCache[cid];
        if(!vw->isOpened())
        {
            return;
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
            //this->msleep(2);
        }

        while((&camIdRecCache[cid])->count() > maxRecCacheCount)
        {
            Mat mat = (&camIdRecCache[cid])->dequeue();
            mat.release();
        }
    }
}

void CameraCollectorThread::saveRec(int cid, Mat mat)
{
    if(mat.empty())
    {
        return;
    }
    if(!camIdWriterCache.contains(cid))
    {
        return;
    }
    VideoWriter *vw = camIdWriterCache[cid];
    if(vw->isOpened())
    {
        vw->write(mat);
    }
}

void CameraCollectorThread::endRec(int cid)
{
    if(camIdWriterCache.contains(cid))
    {
        VideoWriter *vw = camIdWriterCache[cid];
        vw->release();
    }
}

bool CameraCollectorThread::newRec(int cid)
{
    bool result = false;
    if(!camIdProp.contains(cid))
    {
        assert("no camera prop @ camera collector thread");
    }
    (&camIdProp[cid])->setFileNameBuildNew();
    VideoProp prop = camIdProp[cid];

    VideoWriter *vw = NULL;
    if(!camIdWriterCache.contains(cid))
    {
        vw = new VideoWriter();
        camIdWriterCache[cid] = vw;
    }
    else
    {
        vw = camIdWriterCache[cid];
        if(vw->isOpened())
        {
            vw->release();
        }
    }
    result = vw->open(prop.getFileName().toStdString(), CV_FOURCC('D', 'I', 'V', 'X'),
             prop.getFps(), Size(prop.getWidth(), prop.getHeight()));
    return result;

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

std::vector<cv::Rect_<int> > CameraCollectorThread::findFace(cv::Mat mat)
{
    std::vector<cv::Rect_<int> > result;
    if(mat.empty())
    {
        return result;
    }
    cv::cvtColor(mat, mat, cv::COLOR_BGR2GRAY);
    cv::filter2D(mat ,mat, CV_8UC3, kernel);
    faceHelper.detectFaces(mat, faces);
    //cv::imwrite("D:/texta.png", mat);
    result = faces;
    return result;
}

std::vector<cv::Rect_<int> > CameraCollectorThread::findFace(int cid)
{
    std::vector<cv::Rect_<int> > result;
    if(camIdFaceCache.contains(cid))
    {
        cv::Mat mat = camIdFaceCache.value(cid);

        if(!camIdProp.contains(cid))
        {
            assert("no camera prop @ camera collector thread");
        }

        cv::Mat cut;

        VideoProp prop = camIdProp[cid];
        cv::Rect rect = prop.getFaceRect();
        int w = rect.width + rect.x;
        int h = rect.height + rect.y;
        if(mat.rows >= h && mat.cols >= w)
        {
            cut = mat(rect);
        }
        faceHelper.detectFaces(cut, faces);
        faceHelper.detectEyes(cut, eyes);
        if(!faces.empty() && (int)faces.size() > 0)
        {
            result = faces;
            //emit onFace(cid, (int)faces.size());
        }
        mat.release();
        camIdFaceCache.remove(cid);
    }
    return result;
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

std::vector<Rect> CameraCollectorThread::findMog(int cid)
{
    std::vector<Rect> result;
    if(camIdMogCache.contains(cid))
    {
        cv::Mat mat = camIdMogCache.value(cid);
        if(!camIdMogObj.contains(cid))
        {
            MogDetectObject obj;
            camIdMogObj[cid] = obj;
        }

        MogDetectObject *mogobj = (&camIdMogObj[cid]);
        resize(mat, mogobj->smallMat, Size(200, 150), 0, 0);
        mogobj->mog->apply(mogobj->smallMat, mogobj->lastMat);
        cv::threshold(mogobj->lastMat, mogobj->lastMat, 130, 255, cv::THRESH_BINARY);
        cv::medianBlur(mogobj->lastMat, mogobj->lastMat, 3);
        cv::erode(mogobj->lastMat, mogobj->lastMat, cv::Mat());
        cv::dilate(mogobj->lastMat, mogobj->lastMat, cv::Mat());
        findContours(mogobj->lastMat, mogobj->cnts, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, cv::Point(0, 0) );
        float area;
        Rect rect;
        std::vector<Point> m;
        for (int i = mogobj->cnts.size() - 1; i >= 0; i--)
        {
            vector<Point> c = mogobj->cnts[i];
            area = contourArea(c);
            if (area < 500)
            {
                continue;
            }
            else
            {
                m = c;
            }
            /*
            //-------//
            qDebug() << "on mog: " << cid;
            emit onMog(cid);
            break;
            //-------//
            */
            rect = boundingRect(m);
            //rectangle(mat, rect, Scalar(0, 255, 0), 2);
            result.push_back(rect);
        }

        mat.release();
        camIdMogCache.remove(cid);
    }
    return result;
}

std::vector<Rect> CameraCollectorThread::findMog(int cid, Mat mat)
{
    std::vector<Rect> result;
    if(!camIdMogObj.contains(cid))
    {
        MogDetectObject obj;
        camIdMogObj[cid] = obj;
    }

    MogDetectObject *mogobj = (&camIdMogObj[cid]);
    resize(mat, mogobj->smallMat, Size(200, 150), 0, 0);
    mogobj->mog->apply(mogobj->smallMat, mogobj->lastMat);
//    cv::threshold(mogobj->lastMat, mogobj->lastMat, 130, 255, cv::THRESH_BINARY);
//    cv::medianBlur(mogobj->lastMat, mogobj->lastMat, 3);
//    cv::erode(mogobj->lastMat, mogobj->lastMat, cv::Mat());
//    cv::dilate(mogobj->lastMat, mogobj->lastMat, cv::Mat());
    findContours(mogobj->lastMat, mogobj->cnts, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, cv::Point(0, 0) );
    float area;
    Rect rect;
    std::vector<Point> m;
    for (int i = mogobj->cnts.size() - 1; i >= 0; i--)
    {
        vector<Point> c = mogobj->cnts[i];
        area = contourArea(c);
        if (area < 40)
        {
            continue;
        }
        else
        {
            m = c;
        }
        rect = boundingRect(m);
        result.push_back(rect);
    }

    mat.release();
    camIdMogCache.remove(cid);
    return result;
}

bool CameraCollectorThread::findMogBOOL(int cid, Mat mat)
{
    bool result = false;
    if(!camIdMogObj.contains(cid))
    {
        MogDetectObject obj;
        camIdMogObj[cid] = obj;
    }

    MogDetectObject *mogobj = (&camIdMogObj[cid]);
    resize(mat, mogobj->smallMat, Size(200, 150), 0, 0);
    mogobj->mog->apply(mogobj->smallMat, mogobj->lastMat);
    findContours(mogobj->lastMat, mogobj->cnts, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, cv::Point(0, 0) );
    float area;
    for (int i = mogobj->cnts.size() - 1; i >= 0; i--)
    {
        vector<Point> c = mogobj->cnts[i];
        area = contourArea(c);
        if (area < 40)
        {
            continue;
        }
        else
        {
            result = true;
            break;
        }
    }
    mat.release();
    camIdMogCache.remove(cid);
    return result;
}

void CameraCollectorThread::removeMogRcs(int cid)
{
    if(camIdMogObj.contains(cid))
    {
        MogDetectObject *mogobj = (&camIdMogObj[cid]);
        delete mogobj;
        mogobj = NULL;
    }
    camIdMogObj.remove(cid);
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
        //this->msleep(20);
    }
}
