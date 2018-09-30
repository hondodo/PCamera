#include "facedetecthelper.h"

using namespace std;
using namespace cv;

FaceDetectHelper::FaceDetectHelper()
{
    canDetectFace = false;
    canDeteceEye = false;
}

bool FaceDetectHelper::init(string face_cascade_path, string eye_cascade_path)
{
    if(face_cascade_path.empty())
    {
        return false;
    }
    if(face_cascade.load(face_cascade_path))
    {
        canDetectFace = true;
        if(!eye_cascade_path.empty())
        {
            canDeteceEye = eyes_cascade.load(eye_cascade_path);
            return canDeteceEye;
        }
        return true;
    }
    return false;
}

void FaceDetectHelper::detectFaces(Mat& img, vector<Rect_<int> >& faces)
{
    if(canDetectFace)
    {
        face_cascade.detectMultiScale(img, faces, 1.15, 3, 0|CASCADE_SCALE_IMAGE, Size(30, 30));
    }
}

void FaceDetectHelper::detectFacialFeaures(Mat& img, const vector<Rect_<int> > faces, vector<Rect_<int> > eyes)
{
    for(unsigned int i = 0; i < faces.size(); ++i)
    {
        // Mark the bounding box enclosing the face
        Rect face = faces[i];
        rectangle(img, Point(face.x, face.y), Point(face.x+face.width, face.y+face.height),
                  Scalar(0, 0, 255), 2, 4);

        // Eyes, nose and mouth will be detected inside the face (region of interest)
        Mat ROI = img(Rect(face.x, face.y, face.width, face.height));
        // Detect eyes if classifier provided by the user
        detectEyes(ROI, eyes);
        // Mark points corresponding to the centre of the eyes
        for(unsigned int j = 0; j < eyes.size(); ++j)
        {
            Rect e = eyes[j];
            circle(ROI, Point(e.x+e.width/2, e.y+e.height/2), 3, Scalar(0, 255, 0), -1, 8);
            /* rectangle(ROI, Point(e.x, e.y), Point(e.x+e.width, e.y+e.height),
                    Scalar(0, 255, 0), 1, 4); */
        }
    }
    return;
}

void FaceDetectHelper::detectEyes(Mat& img, vector<Rect_<int> >& eyes)
{
    if(canDeteceEye)
    {
        eyes_cascade.detectMultiScale(img, eyes, 1.20, 5, 0|CASCADE_SCALE_IMAGE, Size(30, 30));
    }
}
