#ifndef FACEDETECTHELPER_H
#define FACEDETECTHELPER_H


/*
 * Author: Samyak Datta (datta[dot]samyak[at]gmail.com)
 *
 * A program to detect facial feature points using
 * Haarcascade classifiers for face, eyes, nose and mouth
 *
 */

#include "opencv2/objdetect.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"

#include <iostream>
#include <cstdio>
#include <vector>
#include <algorithm>

class FaceDetectHelper
{
public:
    FaceDetectHelper();

    bool init(std::string face_cascade_path, std::string eye_cascade_path);
    // Functions for facial feature detection
    void help();
    void detectFaces(cv::Mat&, std::vector<cv::Rect_<int> >&);
    void detectEyes(cv::Mat&, std::vector<cv::Rect_<int> >&);
    void detectFacialFeaures(cv::Mat&, const std::vector<cv::Rect_<int> >, std::vector<cv::Rect_<int> > eyes);

private:
    cv::CascadeClassifier face_cascade, eyes_cascade;
    bool canDetectFace, canDeteceEye;

};

#endif // FACEDETECTHELPER_H
