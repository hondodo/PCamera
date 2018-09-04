#include "camarathread.h"

using namespace std;
using namespace cv;

CamaraThread::CamaraThread(QObject *parent) : QThread(parent)
{
    _isRunning = false;
    _isConnect = false;
    _isDetectFace = false;
    camaraId = 0;
    time.start();
    recMinSecond = 10;
    recMaxSencond = 600;
    targetSize = QSize(400, 300);
    //connect(CameraCollectorThread::Init, SIGNAL(onFace(int,int)),
    //        this, SLOT(onFace(int,int)));
    connect(CameraCollectorThread::Init, SIGNAL(onImage(int,QImage)),
            this, SLOT(onImageShow(int,QImage)));
}

void CamaraThread::setStop()
{
    _isRunning = false;
    _isConnect = false;
}

void CamaraThread::run()
{
    _isConnect = false;
    onTip(tr("Openning camera"));
    VideoCapture capture;
    capture.open(camaraId);
    if (!capture.isOpened())
    {
        onTip(tr("Cannot open camera"));
        emit onConnectChanged(false);
        return;
    }

    onTip(tr("Open camera success"));
    _isRunning = true;


    int width = capture.get(CAP_PROP_FRAME_WIDTH);
    int height = capture.get(CAP_PROP_FRAME_HEIGHT);

    int defwidth = 960;//1280;
    int defheight = 540;//720;

    defwidth = 1280;
    defheight = 720;


    if(capture.set(CAP_PROP_FRAME_WIDTH, defwidth) && capture.set(CAP_PROP_FRAME_HEIGHT, defheight))
    {
        width = defwidth;
        height = defheight;
    }
    else
    {
        capture.set(CAP_PROP_FRAME_WIDTH, width);
        capture.set(CAP_PROP_FRAME_HEIGHT, height);
    }


    width = capture.get(CAP_PROP_FRAME_WIDTH);
    height = capture.get(CAP_PROP_FRAME_HEIGHT);

    //capture.set(CAP_PROP_FPS, 10);
    int fps = capture.get(CAP_PROP_FPS);
    int els = 40;

    fps = fps <= 0? 25 : fps;
    els = 1000 / fps;
    els = els <= 0? 40 : els;
    width = width < 10? 400 : width;
    height = height < 10? 300 : height;

    VideoProp prop;
    prop.setFileNameTag(QString::number(camaraId));
    prop.setFps(fps);
    prop.setWidth(width);
    prop.setHeight(height);
    cv::Rect faceRect;
    if(width == defwidth)
    {
        faceRect.x = 615;
        faceRect.x = 845;
    }
    else
    {
        faceRect.x = width - 300;
    }

#ifdef Q_OS_WIN
    faceRect.y = 0;
    faceRect.width = 300;
    faceRect.height = 300;
#else
    faceRect.x = 407;
    faceRect.y = 197;
    faceRect.width = 230;
    faceRect.height = 250;
#endif

    prop.setFaceRect(faceRect);
    diskHelper.setPath(prop.getBaseDir());
    CameraCollectorThread::Init->addVideoProp(camaraId, prop);

    QDateTime needRecLastTime = QDateTime::currentDateTime().addYears(-1);
    int maxFrame = fps * recMaxSencond;
    int frameIndex = 0;
    bool isRecording = false;


    QTime timerFrame;
    timerFrame.start();
    QTime timeOpenCVOP;
    timeOpenCVOP.start();
    QString message = "";
    int readtime = 0;
    int mogtime = 0;
    int drawtimetime = 0;
    int facetime = 0;
    int savetime = 0;
    int showtime = 0;
    double framefps = 0;
    bool isNewMessage = false;

    int index = 0;
    int runIndex = 0;
    quint64 totalSpace, freeSpace;

    Mat cap, faceCap;
    std::vector<cv::Rect> mogRect;

    qDebug() << "PROP" << width << height << fps;
    while (_isRunning)
    {
        timerFrame.restart();
        timeOpenCVOP.restart();
        capture >> cap;
        if (!cap.empty())
        {
            if(!_isConnect)
            {
                _isConnect = true;
                emit onConnectChanged(true);
            }
            readtime = timeOpenCVOP.elapsed();
            timeOpenCVOP.restart();

            drawTime(cap);
            drawtimetime = timeOpenCVOP.elapsed();
            timeOpenCVOP.restart();

            timeOpenCVOP.restart();
            CameraCollectorThread::Init->emitOnImage(camaraId, cap);
            showtime = timeOpenCVOP.elapsed();
            timeOpenCVOP.restart();

            ///***********FACE*******
            if(_isDetectFace && index % 3 == 0)
            {
                timeOpenCVOP.restart();
                /*
                CameraCollectorThread::Init->addFaceCache(camaraId, cap);
                std::vector<cv::Rect_<int> > faces =CameraCollectorThread::Init->findFace(camaraId);
                if(!faces.empty() && faces.size() > 0)
                {
                    int count = (int)faces.size();
                    for(int i = 0; i < count; i++)
                    {
                        cv::Rect_<int> rect = faces.at(i);
                        rect.x += prop.getFaceRect().x;
                        rect.y += prop.getFaceRect().y;
                        rectangle(cap, rect, Scalar(0, 0, 255), 2);
                    }
                    emit onFaceDetected(count);
                }
                */
                faceCap = cap(prop.getFaceRect());
                std::vector<cv::Rect_<int> > faces = CameraCollectorThread::Init->findFace(faceCap);
                if(!faces.empty() && (int)faces.size() > 0)
                {
                    int count = (int)faces.size();
                    emit onFaceDetected(count);
                    //qDebug() << "face found";
                }
                facetime = timeOpenCVOP.elapsed();
                timeOpenCVOP.restart();
            }
            //*/
            if(index % 5 == 0)
            {
                timeOpenCVOP.restart();
                //CameraCollectorThread::Init->addMogCache(camaraId, cap);
                mogRect = CameraCollectorThread::Init->findMog(camaraId, cap);
                if(!mogRect.empty() && mogRect.size() > 0)
                {
                    needRecLastTime = QDateTime::currentDateTime();
                    if(!isRecording)
                    {
                        CameraCollectorThread::Init->newRec(camaraId);
                        frameIndex = 0;
                    }
                }
                mogtime =timeOpenCVOP.elapsed();
                timeOpenCVOP.restart();
            }
            if(index % 1 == 0)
            {
                timeOpenCVOP.restart();
                int recesp = (QDateTime::currentDateTime().toMSecsSinceEpoch() - needRecLastTime.toMSecsSinceEpoch());
                recesp = recesp / 1000;
                if(recesp < 10)
                {
                    //CameraCollectorThread::Init->addRecCache(camaraId, cap);
                    CameraCollectorThread::Init->saveRec(camaraId, cap);
                    if(!isRecording)
                    {
                        isRecording = true;
                        frameIndex++;
                        if(frameIndex > maxFrame)
                        {
                            isRecording = false;
                        }
                    }
                }
                else
                {
                    if(isRecording)
                    {
                        isRecording = false;
                        frameIndex = 0;
                        CameraCollectorThread::Init->endRec(camaraId);
                    }
                    this->msleep(2);
                }
                savetime = timeOpenCVOP.elapsed();
                timeOpenCVOP.restart();
            }    

            index++;
            if(index > 10000)
            {
                index = 0;
            }

            int frameels = timerFrame.elapsed();
            if(frameels > 0)
            {
                framefps = 1000.0 / frameels;
            }
            else
            {
                framefps = 0.0;
            }
            isNewMessage = true;
            message = QString("R:%1M:%2D:%3F:%4S:%5H:%6@%7FPS\n%8:%9").arg(
                        QString::number(readtime, 'f', 0),
                        QString::number(mogtime, 'f', 0),
                        QString::number(drawtimetime, 'f', 0),
                        QString::number(facetime, 'f', 0),
                        QString::number(savetime, 'f', 0),
                        QString::number(showtime, 'f', 0),
                        QString::number(framefps, 'f', 2),
                        QString::number(this->camaraId, 'f', 0),
                        isRecording? "Rec" : "Stop"
                        );
        }
        else
        {
            if(_isConnect)
            {
                _isConnect = false;
                emit onConnectChanged(false);
            }
            isNewMessage = true;
            message = tr("Capture is empty");
        }

        if(runIndex % 100 == 0)
        {
            totalSpace = diskHelper.bytesTotal();
            freeSpace = diskHelper.bytesAvailable();
        }

        message += RingThread::_isRunning? "[Ring]" : "";

        runIndex++;
        if(runIndex > 10000)
        {
            runIndex = 0;
        }
        message += QString("@Free:%1/%2").arg(diskHelper.toSizeInfo(freeSpace),
                                            diskHelper.toSizeInfo(totalSpace));

        if(isNewMessage)
        {
            isNewMessage = false;
            emit onTip(message);
        }

        int elapsed = time.elapsed();
        if(elapsed < 0)
        {
            elapsed = 0;
        }
        if(elapsed > els)
        {}
        else
        {
            this->msleep(els - elapsed);
        }
        time.restart();
    }
    capture.release();
}

void CamaraThread::onFace(int camId, int faceCount)
{
    if(_isDetectFace && camId == camaraId)
    {
        emit onFaceDetected(faceCount);
    }
}

void CamaraThread::onImageShow(int camId, const QImage &image)
{
    if(camId == camaraId)
    {
        emit onImage(image);
    }
}

void CamaraThread::drawTime(InputOutputArray img)
{
    if(img.empty())
    {
        return;
    }
    std::string text = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss").toStdString();
    int fontface = cv::FONT_HERSHEY_SIMPLEX;
    double fontscale = 1;
    int thickness = 2;
    cv::Point origin;
    origin.x = 50;
    origin.y = 50;
    cv::Point offsetpoint;
    offsetpoint.x = origin.x + 2;
    offsetpoint.y = origin.y + 2;
    cv::putText(img, text, offsetpoint, fontface, fontscale, cv::Scalar(0, 0, 0), thickness + 1, 8, 0);
    cv::putText(img, text, origin, fontface, fontscale, cv::Scalar(255, 255, 255), thickness, 8, 0);
}

QSize CamaraThread::getTargetSize() const
{
    return targetSize;
}

void CamaraThread::setTargetSize(const QSize &value)
{
    targetSize = value;
}

bool CamaraThread::getIsDetectFace() const
{
    return _isDetectFace;
}

void CamaraThread::setIsDetectFace(bool isDetectFace)
{
    _isDetectFace = isDetectFace;
}

int CamaraThread::getRecMinSecond() const
{
    return recMinSecond;
}

void CamaraThread::setRecMinSecond(int value)
{
    recMinSecond = value;
}

int CamaraThread::getCamaraId() const
{
    return camaraId;
}

void CamaraThread::setCamaraId(int value)
{
    camaraId = value;
}
