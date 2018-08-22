#ifndef RINGHELPER_H
#define RINGHELPER_H

#ifdef Q_OS_WIN
#include <QMultimedia>
#include <QMediaPlayer>
#else
#include <ao/ao.h>
#include <mpg123.h>
#define BITS 8
#endif

class RingHelper
{
public:
    RingHelper();
    ~RingHelper();

    void setStop();
    void Ring();

private:
#ifdef Q_OS_WIN
    QMediaPlayer *player;
#endif
    bool _isRinging;
};

#endif // RINGHELPER_H
