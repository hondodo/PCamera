#ifndef VIDEOPROP_H
#define VIDEOPROP_H

#include <QtCore>
#include <QDateTime>
#include "opencv/cv.h"

class VideoProp
{
public:
    VideoProp();

    int getWidth() const;
    void setWidth(int value);

    int getHeight() const;
    void setHeight(int value);

    int getFps() const;
    void setFps(int value);

    QString getFileName() const;
    void setFileNameBuildNew();

    QString getDir() const;
    void setDirBuildNew();

    QString getFileNameTag() const;
    void setFileNameTag(const QString &value);

    cv::Rect getFaceRect() const;
    void setFaceRect(const cv::Rect &value);

    QString getBaseDir() const;
    void setBaseDir(const QString &value);

private:
    int width;
    int height;
    int fps;
    QString fileName;
    QString fileNameTag;
    QString baseDir, dir;
    cv::Rect faceRect;
};

#endif // VIDEOPROP_H
