#ifndef CHECKDISKTHREAD_H
#define CHECKDISKTHREAD_H

#include <QThread>
#include <QDir>
#include <QFileInfo>
#include <QFileInfoList>
#include <QDebug>
#include "pathhelper.h"
#include "Core/diskhelper.h"

class CheckDiskThread : public QThread
{
    Q_OBJECT
public:
    explicit CheckDiskThread(QObject *parent = nullptr);
    static CheckDiskThread *Init;

    void setStop();

protected:
    void run();

signals:

public slots:

private:
    bool _isRunning;
    QFileInfoList getAllFiles(QString path, QStringList filters);
};

#endif // CHECKDISKTHREAD_H
