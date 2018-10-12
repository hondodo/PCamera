#include "checkdiskthread.h"

CheckDiskThread *CheckDiskThread::Init = new CheckDiskThread();
CheckDiskThread::CheckDiskThread(QObject *parent) : QThread(parent)
{
    _isRunning = false;
}

void CheckDiskThread::setStop()
{
    _isRunning = false;
}

void CheckDiskThread::run()
{
    _isRunning = true;
    int sleeptime = 60 * 1000;
    int eachpartsleep = sleeptime / 20;
    DiskHelper diskhelper;
    PathHelper pathhelper;
    diskhelper.setPath(pathhelper.getRecPath());
    qint64 kbSize = 1024;
    qint64 mbSize = kbSize * 1024;
    qint64 gbSize = mbSize * 1024;
    qint64 keepBytes = 2 * gbSize;
    while (_isRunning)
    {
        qint64 free = diskhelper.bytesFree();
        if(free < keepBytes)
        {
            qDebug() << "no more free space, free space for store";
        }
        for(int i = 0; i < eachpartsleep; i++)
        {
            if(!_isRunning)
            {
                break;
            }
            this->msleep(20);
        }
    }
}

QFileInfoList CheckDiskThread::getAllFiles(QString path)
{
    QDir dir(path);
    if(dir.exists())
    {
        QFileInfoList allfiles = dir.entryInfoList(QDir::Files | QDir::Hidden | QDir::NoSymLinks);
        QFileInfoList alldirs = dir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);
        if(alldirs.isEmpty() || alldirs.count() <= 0)
        {}
        else
        {
            int count = alldirs.count();
            for(int i = 0; i < count; i++)
            {
                QString dirpath = alldirs.at(i).absoluteFilePath();
                QFileInfoList childfiles = getAllFiles(dirpath);
                if(childfiles.isEmpty() || childfiles.count() <= 0)
                {}
                else
                {
                    allfiles.append(childfiles);
                }
            }
        }
        return allfiles;
    }
}
