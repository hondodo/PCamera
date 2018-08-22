#ifndef RINGHELPER_H
#define RINGHELPER_H

#include <QMultimedia>
#include <QMediaPlayer>

class RingHelper
{
public:
    RingHelper();
    ~RingHelper();

    void Ring();

private:
    QMediaPlayer *player;
};

#endif // RINGHELPER_H
