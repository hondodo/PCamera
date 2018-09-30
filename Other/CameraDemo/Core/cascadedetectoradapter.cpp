#include "cascadedetectoradapter.h"

using namespace std;
using namespace cv;

CascadeDetectorAdapter::CascadeDetectorAdapter(cv::Ptr<CascadeClassifier> detector) : IDetector(),
    Detector(detector)
{
    CV_ASSERT(detector);
}
