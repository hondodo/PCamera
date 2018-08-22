#include "ringhelper.h"

RingHelper::RingHelper()
{
    player = new QMediaPlayer();
}

RingHelper::~RingHelper()
{
    player->stop();
    player->deleteLater();
}

void RingHelper::Ring()
{
    player->stop();
    player->setMedia(QUrl("D:/a.mp3"));
    player->setVolume(80);
    player->play();
}
