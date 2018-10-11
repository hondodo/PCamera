#include "pathhelper.h"

QString PathHelper::rootPath = "/";
PathHelper::PathHelper(QObject *parent) : QObject(parent)
{
    cameraName = "undefine";
    rootPath = "/";
    recPath = "/REC/";
    currentFileName = "/REC/undefine.avi";
    tempFileName = "/REC/temp.avi";

#ifdef Q_OS_WIN
    setRootPath("D:/");
#else
    setRootPath("/home/pi/");
#endif
}

void PathHelper::init()
{
    if(!rootPath.endsWith("/"))
    {
        rootPath += "/";
    }
    recPath = rootPath + "REC/";
    QDir dir(recPath);
    if(!dir.exists())
    {
        dir.mkdir(recPath);
    }
    tempFileName = recPath + getCameraNameForFileName() + "_Temp.avi";
    creatNewFileName();
}

QString PathHelper::getCameraName() const
{
    return cameraName;
}

QString PathHelper::getCameraNameForFileName() const
{
    QString result = cameraName;
    result = result.replace(":", "_").replace("\\", "_").replace("/", "_").replace(" ", "_");
    return result;
}

QString PathHelper::getTempFileName() const
{
    return tempFileName;
}

QString PathHelper::getRootPath() const
{
    return rootPath;
}

QString PathHelper::getRecPath() const
{
    return recPath;
}

QString PathHelper::getCurrentFileName() const
{
    return currentFileName;
}

void PathHelper::creatNewFileName()
{
    QString path = recPath + "Video/";
    QDir dir(path);
    if(!dir.exists())
    {
        dir.mkdir(path);
    }
    path = path + getCameraNameForFileName() + "/";
    dir.setPath(path);
    if(!dir.exists())
    {
        dir.mkdir(path);
    }
    path = path + QDateTime::currentDateTime().toString("yyyyMM") + "/";
    dir.setPath(path);
    if(!dir.exists())
    {
        dir.mkdir(path);
    }
    path = path + QDateTime::currentDateTime().toString("yyyyMMdd") + "/";
    dir.setPath(path);
    if(!dir.exists())
    {
        dir.mkdir(path);
    }
    currentFileName = path + getCameraNameForFileName() + "_" +
            QDateTime::currentDateTime().toString("yyyyMMddhhmmss") + ".avi";
}

void PathHelper::setCameraName(const QString &value)
{
    cameraName = value;
}

void PathHelper::setRootPath(const QString &value)
{
    rootPath = value;
}
