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
    int sleeptime = 10 * 60 * 1000;//10min
    int eachpartsleep = sleeptime / 20;
    DiskHelper diskhelper;
    PathHelper pathhelper;
    diskhelper.setPath(pathhelper.getRecPath());
    qint64 kbSize = 1024;
    qint64 mbSize = kbSize * 1024;
    qint64 gbSize = mbSize * 1024;
    qint64 keepBytes = 4 * gbSize;//keep 4Gb free
    QStringList filters;
    filters << "*." + pathhelper.getFileExtn();
    qDebug() << "check disk thread will start after 20s";
    for(int i = 0; i < 1000; i++)
    {
        if(!_isRunning)
        {
            break;
        }
        this->msleep(20);
    }
    qDebug() << "starting check disk thread";
    while (_isRunning)
    {
        qint64 free = diskhelper.bytesAvailable();//.bytesFree();
        if(free < keepBytes)
        {
            qDebug() << "no more free space, free space for store: free bytes:" << free;
            int times = 0;
            QFileInfoList files = getAllFiles(pathhelper.getRecPath(), filters);
            while ((!files.isEmpty() && files.count() > 0) && free < keepBytes && times < 100)
            {
                times++;
                int index = getEarliestFile(&files);
                if(index < files.count())
                {
                    QFileInfo fileinfo = files.at(index);
                    QString filename = fileinfo.absoluteFilePath();
                    if(filename.toLower().contains("temp"))
                    {
                        //dot not remove it
                    }
                    else
                    {
                        QFile file(filename);
                        if(file.exists())
                        {
                            file.remove();
                        }
                        qDebug() << times << "remove file:" << filename;
                    }
                }
                files.removeAt(index);
                free = diskhelper.bytesAvailable();//.bytesFree();
                qDebug() << "now free bytes:" << free;
            }
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

QFileInfoList CheckDiskThread::getAllFiles(QString path, QStringList filters)
{
    QDir dir(path);
    if(dir.exists())
    {
        QFileInfoList allfiles = dir.entryInfoList(filters, QDir::Files | QDir::Hidden | QDir::NoSymLinks, QDir::Time);
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
        return allfiles;
    }
}

int CheckDiskThread::getEarliestFile(const QFileInfoList *files)
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
