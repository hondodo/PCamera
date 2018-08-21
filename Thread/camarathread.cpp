#include "camarathread.h"

using namespace std;
using namespace cv;

CamaraThread::CamaraThread(QObject *parent) : QThread(parent)
{
    _isRunning = false;
    _isConnect = false;
    camaraId = 0;
    time.start();
    recDir = "REC";
    recMinSecond = 10;
    recMaxSencond = 600;
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

    VideoWriter capWriter;
    int fps = capture.get(CAP_PROP_FPS);
    int els = 40;
    int width = capture.get(CAP_PROP_FRAME_WIDTH);
    int height = capture.get(CAP_PROP_FRAME_HEIGHT);

    while(recDir.endsWith("/"))
    {
        recDir.remove(recDir.length() - 2, 1);
    }
    QDir dir = QDir(recDir);
    if(!dir.exists(recDir))
    {
        dir.mkpath(recDir);
    }

    QString filename = recDir + "/" + QDateTime::currentDateTime().toString("yyyyMMddHHmmss") + ".avi";

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

    QDateTime needRecLastTime = QDateTime::currentDateTime().addYears(-1);
    int maxFrame = fps * recMaxSencond;
    int frameIndex = 0;
    bool isRecording = false;
    while (_isRunning)
    {
        Mat cap;
        capture >> cap;
        if (!cap.empty())
        {
            if(!_isConnect)
            {
                _isConnect = true;
                emit onConnectChanged(true);
            }
            mog->apply(cap, lastMat);
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
                if (area < 1000)
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

            int recelsp = QDateTime::currentDateTime().toSecsSinceEpoch() - needRecLastTime.toSecsSinceEpoch();
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
                    filename = recDir + "/" + QDateTime::currentDateTime().toString("yyyyMMddHHmmss") +
                            "_" + QString::number(camaraId, 'f', 0) + "_" + ".avi";
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
                    capWriter.release();
                }
            }

            QImage image = ImageFormat::Mat2QImage(cap);
            image = image.scaled(QSize(400, 300), Qt::KeepAspectRatio);
            onImage(image);
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
