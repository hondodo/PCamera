#include "mogdetectobject.h"

MogDetectObject::MogDetectObject()
{
    mog = cv::createBackgroundSubtractorMOG2(100, 25, false);
    mog->setNMixtures(2);
    mog->setDetectShadows(0);
}
