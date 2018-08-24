#include "cameracollectorthread.h"

using namespace std;
using namespace cv;

CameraCollectorThread::CameraCollectorThread(QObject *parent) : QThread(parent)
{
    _isRunning = false;
}

CameraCollectorThread::~CameraCollectorThread()
{
    setStop();
}

void CameraCollectorThread::setStop()
{
    _isRunning = false;
}

void CameraCollectorThread::run()
{
    VideoCapture capture0;
    capture0.open(0);
    if (!capture0.isOpened())
    {
        qDebug() << "0 not opened";
        return;
    }
    //VideoCapture capture1;
    //capture1.open(1);
    //if (!capture1.isOpened())
    //{
    //    qDebug() << "1 not opened";
    //    return;
    //}


    capture0.set(CV_CAP_PROP_BUFFERSIZE, 3);
    capture0.set(CV_CAP_PROP_FRAME_WIDTH, 600);
    capture0.set(CV_CAP_PROP_FRAME_HEIGHT, 480);
    capture0.set(CV_CAP_PROP_FPS, 25);
    int fa = capture0.get(CV_CAP_PROP_FPS);
    qDebug() << "frame" << fa;
    Mat mat0, mat1;
    QTime timer;
    timer.start();
    _isRunning = true;
    while (_isRunning)
    {
        timer.restart();
        capture0.read(mat0);
        //capture1.read(mat1);
        qDebug() << timer.elapsed();
        this->msleep(10);
    }
    capture0.release();
}
