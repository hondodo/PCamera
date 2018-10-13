#include "pathhelper.h"

QString PathHelper::rootPath = "/";
QString PathHelper::recPath = "/";
QString PathHelper::fileExtn = "avi";
QString PathHelper::ringPath = "/";
PathHelper::PathHelper(QObject *parent) : QObject(parent)
{
    cameraName = "undefine";
    rootPath = "/";
    recPath = "/REC/";
    currentFileName = "/REC/undefine." + fileExtn;
    tempFileName = "/REC/temp." + fileExtn;

#ifdef Q_OS_WIN
    setRootPath("D:/");
#else
    setRootPath("/home/pi/");
#endif
    recPath = rootPath + "REC/";
    ringPath = rootPath + "Ring/";
}

void PathHelper::init()
{
    if(!rootPath.endsWith("/"))
    {
        rootPath += "/";
    }
    //recPath = rootPath + "REC/";
    QDir dir(recPath);
    if(!dir.exists())
    {
        dir.mkdir(recPath);
    }
    tempFileName = recPath + getCameraNameForFileName() + "_Temp." + fileExtn;
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
            QDateTime::currentDateTime().toString("yyyyMMddhhmmss") + "." + fileExtn;
}

void PathHelper::setCameraName(const QString &value)
{
    cameraName = value;
}

QString PathHelper::getFileExtn() const
{
    return fileExtn;
}

void PathHelper::setRootPath(const QString &value)
{
    rootPath = value;
}

QFileInfoList PathHelper::getAllFiles(QString path, QStringList filters, bool childDir)
{
    QDir dir(path);
    if(dir.exists())
    {
        QFileInfoList allfiles = dir.entryInfoList(filters, QDir::Files | QDir::Hidden | QDir::NoSymLinks, QDir::Time);
        if(childDir)
        {
            QFileInfoList alldirs = dir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot, QDir::Time);
            if(alldirs.isEmpty() || alldirs.count() <= 0)
            {}
            else
            {
                int count = alldirs.count();
                for(int i = 0; i < count; i++)
                {
                    QString dirpath = alldirs.at(i).absoluteFilePath();
                    QFileInfoList childfiles = getAllFiles(dirpath, filters);
                    if(childfiles.isEmpty() || childfiles.count() <= 0)
                    {}
                    else
                    {
                        allfiles.append(childfiles);
                    }
                }
            }
        }
        return allfiles;
    }
}

int PathHelper::getEarliestFile(const QFileInfoList *files)
{
    int index = 0;
    if(files != NULL && !files->isEmpty() && files->count() > 0)
    {
        int count = files->count();
        qint64 earliestmodify = files->at(0).lastModified().toMSecsSinceEpoch();
        for(int i = 1; i < count; i++)
        {
            qint64 modify = files->at(i).lastModified().toMSecsSinceEpoch();
            if(modify < earliestmodify)
            {
                earliestmodify = modify;
                index = i;
            }
        }
    }
    return index;
}

QString PathHelper::getRingPath()
{
    return ringPath;
}

