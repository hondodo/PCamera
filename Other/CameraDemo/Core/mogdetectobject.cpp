#include "mogdetectobject.h"

MogDetectObject::MogDetectObject()
{
    mog = cv::createBackgroundSubtractorMOG2(100, 25, false);
    mog->setNMixtures(2);
    mog->setDetectShadows(0);
}

MogDetectObject::~MogDetectObject()
{
    qDebug() << "~MogDetectObject()";
    if(!lastMat.empty())
    {
        lastMat.release();
    }
    if(!smallMat.empty())
    {
        smallMat.release();
    }
    if(!mog.empty())
    {
        mog.release();
    }
}
