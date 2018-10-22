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
    diskhelper.setPath(pathhelper.getRootPath());
    qint64 kbSize = 1024;
    qint64 mbSize = kbSize * 1024;
    qint64 gbSize = mbSize * 1024;
    qint64 keepBytes = 2 * gbSize;//keep 2Gb free
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
        if(free < 0)
        {
            qDebug() << "warning: bytesAvailable error, switch to bytesFree";
            free = diskhelper.bytesFree();
        }
        if(free < 0)
        {
            qDebug() << "disk check free space error";
        }
        else
        {
            if(free < keepBytes)
            {
                qDebug() << "no more free space, free space for store: free bytes:" << free;
                int times = 0;
                QFileInfoList files = PathHelper::getAllFiles(pathhelper.getRecPath(), filters);
                while ((!files.isEmpty() && files.count() > 0) && free < keepBytes && times < 100)
                {
                    times++;
                    int index = PathHelper::getEarliestFile(&files);
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
