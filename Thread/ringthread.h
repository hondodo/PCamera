#ifndef RINGTHREAD_H
#define RINGTHREAD_H

#include <QThread>
#ifdef Q_OS_WIN
#include <QMultimedia>
#include <QMediaPlayer>
#else
#include <ao/ao.h>
#include <mpg123.h>
#define BITS 8
#endif

class RingThread :public QThread
{
public:
    explicit RingThread(QObject *parent = nullptr);
    ~RingThread();

    static bool _isRunning;

    void setStop();

    std::string getFileName() const;
    void setFileName(const std::string &value);

protected:
    void run();

private:

#ifdef Q_OS_WIN
    QMediaPlayer *player;
#endif

    std::string fileName;

};

#endif // RINGTHREAD_H
