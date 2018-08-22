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
        this->msleep(30);
    }
    capture0.release();
}
