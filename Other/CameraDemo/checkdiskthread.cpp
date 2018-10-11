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
    while (_isRunning)
    {


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
