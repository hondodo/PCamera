#ifndef MATHELPER_H
#define MATHELPER_H

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

class MatHelper
{
public:
    MatHelper();
    static MatHelper *Init;
    void brightnessException(cv::Mat InputImg, float &cast, float &da);
    void brightnessException(cv::Mat InputImg, float &cast, float &da, cv::Rect rect);
};

#endif // MATHELPER_H
