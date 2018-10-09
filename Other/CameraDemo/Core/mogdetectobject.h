#ifndef MOGDETECTOBJECT_H
#define MOGDETECTOBJECT_H

#include <QObject>
#include <QDebug>
#include <opencv/cv.h>
#include <opencv2/video/background_segm.hpp>

class MogDetectObject
{
public:
    MogDetectObject();
    ~MogDetectObject();

    cv::Mat lastMat, smallMat;
    cv::Ptr<cv::BackgroundSubtractorMOG2> mog;
    std::vector<std::vector<cv::Point> > cnts;

};

#endif // MOGDETECTOBJECT_H
