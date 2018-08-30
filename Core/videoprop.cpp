#include "videoprop.h"

VideoProp::VideoProp()
{
    width = 640;
    height = 480;
    fps = 25;
    faceRect.x = width - 300;
    faceRect.y = 0;
    faceRect.width = 300;
    faceRect.height = 300;
    setFileNameBuildNew();
}

int VideoProp::getWidth() const
{
    return width;
}

void VideoProp::setWidth(int value)
{
    width = value;
}

int VideoProp::getHeight() const
{
    return height;
}

void VideoProp::setHeight(int value)
{
    height = value;
}

int VideoProp::getFps() const
{
    return fps;
}

void VideoProp::setFps(int value)
{
    fps = value;
}

QString VideoProp::getFileName() const
{
    return dir + "/" + fileName + "_" + fileNameTag + ".avi";
}

void VideoProp::setFileNameBuildNew()
{
    setDirBuildNew();
    fileName = QDateTime::currentDateTime().toString("yyyyMMddhhmmss");
}

QString VideoProp::getDir() const
{
    return dir;
}

void VideoProp::setDirBuildNew()
{
    QDateTime time = QDateTime::currentDateTime();
#ifdef Q_OS_WIN
    dir = "./REC/"
            + QString("%1%2").arg(time.toString("yyyy"),
                                             time.toString("MM"),
                                             time.toString("dd"));
#else
    dir = "/media/pi/Potatokid/"       //"./REC/"
            + QString("%1%2").arg(time.toString("yyyy"),
                                             time.toString("MM"),
                                             time.toString("dd"));
#endif
    QDir bdir(dir);
    if(!bdir.exists())
    {
        bdir.mkpath(dir);
    }
}

QString VideoProp::getFileNameTag() const
{
    return fileNameTag;
}

void VideoProp::setFileNameTag(const QString &value)
{
    fileNameTag = value;
}

cv::Rect VideoProp::getFaceRect() const
{
    return faceRect;
}

void VideoProp::setFaceRect(const cv::Rect &value)
{
    faceRect = value;
}
