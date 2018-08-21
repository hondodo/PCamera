#include "camarathread.h"

using namespace std;
using namespace cv;

CamaraThread::CamaraThread(QObject *parent) : QThread(parent)
{
    _isRunning = false;
    _isConnect = false;
    camaraId = 0;
    time.start();
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

    std::vector<Point> allpoints;

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
                rect = boundingRect(m);
                rectangle(cap, rect, Scalar(0, 255, 0), 2);

                if(rect.y > 195 && rect.y < 205)
                {
                    allpoints.push_back(Point(rect.x, rect.y));
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
        if(elapsed > 30)
        {}
        else
        {
            waitKey(30 - elapsed);
        }
        time.restart();
    }
    capture.release();
}

int CamaraThread::getCamaraId() const
{
    return camaraId;
}

void CamaraThread::setCamaraId(int value)
{
    camaraId = value;
}
