#include "diskhelper.h"

DiskHelper::DiskHelper(QObject *parent) : QObject(parent)
{
    storageInfo = new QStorageInfo();
    kbSize = 1024;
    mbSize = kbSize * 1024;
    gbSize = mbSize * 1024;
    tbSize = gbSize * 1024;
}

DiskHelper::~DiskHelper()
{
    if(storageInfo != Q_NULLPTR)
    {
        delete storageInfo;
        storageInfo = Q_NULLPTR;
    }
}

QString DiskHelper::getPath() const
{
    return path;
}

void DiskHelper::setPath(const QString &value)
{
    path = value;
    storageInfo->setPath(value);
}

qint64 DiskHelper::bytesTotal()
{
    storageInfo->refresh();
    return storageInfo->bytesTotal();;
}

qint64 DiskHelper::bytesFree()
{
    storageInfo->refresh();
    return storageInfo->bytesFree();
}

qint64 DiskHelper::bytesAvailable()
{
    storageInfo->refresh();
    return storageInfo->bytesAvailable();
}

QString DiskHelper::toSizeInfo(quint64 size)
{
    QString text = "";
    double tosize = 0;
    if(size > tbSize)
    {
        tosize = (double)size / tbSize;
        if((size % tbSize == 0))
        {
            text = QString::number(tosize, 'f', 0) + tr("TB");
        }
        else
        {
            text = QString::number(tosize, 'f', 2) + tr("TB");
        }
    }
    else if(size > gbSize)
    {
        tosize = (double)size / gbSize;
        if((size % gbSize == 0))
        {
            text = QString::number(tosize, 'f', 0) + tr("GB");
        }
        else
        {
            text = QString::number(tosize, 'f', 2) + tr("GB");
        }
    }
    else if(size > mbSize)
    {
        tosize = (double)size / mbSize;
        if((size % mbSize == 0))
        {
            text = QString::number(tosize, 'f', 0) + tr("MB");
        }
        else
        {
            text = QString::number(tosize, 'f', 2) + tr("MB");
        }
    }
    else if(size > kbSize)
    {
        tosize = (double)size / kbSize;
        if((size % kbSize == 0))
        {
            text = QString::number(tosize, 'f', 0) + tr("KB");
        }
        else
        {
            text = QString::number(tosize, 'f', 2) + tr("KB");
        }
    }
    else
    {
        text = QString::number(size, 'f', 0) + tr("B");
    }
    return text;
}

QStorageInfo *DiskHelper::getStorageInfo() const
{
    return storageInfo;
}
