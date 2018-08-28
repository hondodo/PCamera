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
    recDir = "REC";
    recMinSecond = 10 * 1000;
    recMaxSencond = 600 * 1000;
    targetSize = QSize(400, 300);
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

    cv::Mat lastMat;
    cv::Ptr<cv::BackgroundSubtractorMOG2> mog = cv::createBackgroundSubtractorMOG2(1000, 25, false);
    mog->setNMixtures(2);
    mog->setDetectShadows(0);
    std::vector<std::vector<cv::Point> > cnts;

    vector<Rect_<int> > faces;
    vector<Rect_<int> > eyes;

    VideoWriter capWriter;
    int fps = capture.get(CAP_PROP_FPS);
    int els = 40;
    int width = capture.get(CAP_PROP_FRAME_WIDTH);
    int height = capture.get(CAP_PROP_FRAME_HEIGHT);

    int defwidth = 1280;
    int defheight = 720;
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

    if(fps <= 0)
    {
        fps = 25;
    }
    els = 1000 / fps;
    if(els <= 0)
    {
        els = 40;
    }

    if(width < 10)
    {
        width = 400;
    }
    if(height < 10)
    {
        height = 300;
    }

    VideoProp prop;
    prop.setFileNameTag(QString::number(camaraId));
    prop.setFps(fps);
    prop.setWidth(width);
    prop.setHeight(height);
    CameraCollectorThread::Init->addVideoProp(camaraId, prop);

    QDateTime needRecLastTime = QDateTime::currentDateTime().addYears(-1);
    int maxFrame = fps * recMaxSencond;
    int frameIndex = 0;
    bool isRecording = false;

#ifdef Q_OS_WIN//:/Data/haarcascades/haarcascade_frontalcatface_extended.xml
    bool canDetectFace = faceHelper.init("D:/Potatokid/OpenCV/sources/data/haarcascades/haarcascade_frontalface_alt.xml",
                                         "");
#else
    bool canDetectFace = faceHelper.init("/home/pi/Source/PCamera/Data/haarcascades/haarcascade_frontalcatface.xml",
                                         "");
#endif

    Mat cap, small, smallmog;

    double scale = width / 480.0;
    if(scale < 1.0)
    {
        scale = 1.0;
    }

    double scalemog = width / 200.0;
    if(scalemog < 1.0)
    {
        scalemog = 1.0;
    }

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
            resize(cap, small, Size(cap.rows / scale, cap.cols / scale), 0, 0, CV_8SC1);
            cvtColor(small, small, CV_BGR2GRAY);
            resize(cap, smallmog, Size(cap.rows / scalemog, cap.cols / scalemog),
                   0, 0, CV_8SC1);

            mog->apply(smallmog, lastMat);
            cv::threshold(lastMat, lastMat, 130, 255, cv::THRESH_BINARY);
            cv::medianBlur(lastMat, lastMat, 3);
            cv::erode(lastMat, lastMat, cv::Mat());
            cv::dilate(lastMat, lastMat, cv::Mat());
            findContours(lastMat, cnts, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, cv::Point(0, 0) );
            float area;
            Rect rect;
            std::vector<Point> m;
            for (int i = cnts.size() - 1; i >= 0; i--)
            {
                vector<Point> c = cnts[i];
                area = contourArea(c);
                if (area < 100)
                {
                    continue;
                }
                else
                {
                    m = c;
                }
                //-------//
                needRecLastTime = QDateTime::currentDateTime();
                break;
                //-------//
                rect = boundingRect(m);
                rectangle(cap, rect, Scalar(0, 255, 0), 2);
            }
            mogtime =timeOpenCVOP.elapsed();
            timeOpenCVOP.restart();
            drawTime(cap);
            drawtimetime = timeOpenCVOP.elapsed();
            timeOpenCVOP.restart();

            /*
            int recelsp = QDateTime::currentDateTime().toMSecsSinceEpoch() - needRecLastTime.toMSecsSinceEpoch();
            recelsp = recelsp;
            if(recelsp <= recMinSecond)
            {
                if(frameIndex > maxFrame)
                {
                    isRecording = false;
                }
                if(!isRecording)
                {
                    frameIndex = 0;
                    isRecording = true;
                    //filename = recDir + "/" + QDateTime::currentDateTime().toString("yyyyMMddHHmmss") +
                    //        "_" + QString::number(camaraId, 'f', 0) + "_" + ".avi";
                    filename = prop.getFileName();
                    if(capWriter.isOpened())
                    {
                        capWriter.release();
                    }
                    capWriter.open(filename.toStdString(), CV_FOURCC('D', 'I', 'V', 'X'), fps, Size(width, height));
                }
                if(capWriter.isOpened())
                {
                    frameIndex++;
                    capWriter.write(cap);
                }
            }
            else
            {
                if(isRecording)
                {
                    isRecording = false;
                    frameIndex = 0;
                    capWriter.release();
                }
            }
            */
            CameraCollectorThread::Init->addRecCache(camaraId, cap);

            savetime = timeOpenCVOP.elapsed();
            timeOpenCVOP.restart();

            if(_isDetectFace && canDetectFace)
            {
                faceHelper.detectFaces(small, faces);
                faceHelper.detectEyes(small, eyes);
                if(!faces.empty() && (int)faces.size() > 0)
                {
                    emit onFaceDetected((int)faces.size());
                    faceHelper.detectFacialFeaures(cap, faces, eyes);
                }
            }
            facetime = timeOpenCVOP.elapsed();
            timeOpenCVOP.restart();

            QImage image = ImageFormat::Mat2QImage(cap);
            image = image.scaled(targetSize, Qt::KeepAspectRatio);
            emit onImage(image);

            showtime = timeOpenCVOP.elapsed();
            timeOpenCVOP.restart();

            int frameels = timerFrame.elapsed();
            if(frameels > 0)
            {
                framefps = 1000.0 / frameels;
            }
            else
            {
                framefps = 0.0;
            }
            message = QString("R:%1M:%2D:%3F:%4S:%5H:%6@%7FPS@%8").arg(
                        QString::number(readtime, 'f', 0),
                        QString::number(mogtime, 'f', 0),
                        QString::number(drawtimetime, 'f', 0),
                        QString::number(facetime, 'f', 0),
                        QString::number(savetime, 'f', 0),
                        QString::number(showtime, 'f', 0),
                        QString::number(framefps, 'f', 2),
                        QString::number(this->camaraId, 'f', 0)
                        );
            emit onTip(message);
        }
        else
        {
            if(_isConnect)
            {
                _isConnect = false;
                emit onConnectChanged(false);
            }
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
            waitKey(els - elapsed);
        }
        time.restart();
    }
    if(capWriter.isOpened())
    {
        capWriter.release();
    }
    capture.release();
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
    //int baseline;
    //cv::Size textsize = cv::getTextSize(text, fontface, fontscale, thickness, &baseline);
    cv::Point origin;
    origin.x = 50;//img.cols() / 2 - textsize.width / 2;
    origin.y = 50;//img.rows() / 2 + textsize.height / 2;
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

QString CamaraThread::getRecDir() const
{
    return recDir;
}

void CamaraThread::setRecDir(const QString &value)
{
    recDir = value;
}

int CamaraThread::getCamaraId() const
{
    return camaraId;
}

void CamaraThread::setCamaraId(int value)
{
    camaraId = value;
}
