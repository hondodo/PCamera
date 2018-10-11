#ifndef CHECKDISKTHREAD_H
#define CHECKDISKTHREAD_H

#include <QThread>

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
};

#endif // CHECKDISKTHREAD_H
