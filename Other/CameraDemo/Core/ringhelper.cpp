#include "ringhelper.h"

RingHelper::RingHelper()
{
#ifdef Q_OS_WIN
    player = Q_NULLPTR;
#endif
    _isRinging = false;
}

RingHelper::~RingHelper()
{
    setStop();
}

void RingHelper::setStop()
{
    _isRinging = false;
#ifdef Q_OS_WIN
    if(player != Q_NULLPTR)
    {
        player->stop();
        player->deleteLater();
        player = Q_NULLPTR;
    }
#endif
}

void RingHelper::Ring()
{
#ifdef Q_OS_WIN
    player = new QMediaPlayer();
    player->setMedia(QUrl("D:/a.mp3"));
    player->setVolume(80);
    player->play();
#else

    for(int i = 0; i < 2; i++)
    {
        if(!_isRinging)
        {
            continue;
        }
        mpg123_handle *mh;
        unsigned char *buffer;
        size_t buffer_size;
        size_t done;
        int err;

        int driver;
        ao_device *dev;

        ao_sample_format format;
        int channels, encoding;
        long rate;

        /* initializations */
        ao_initialize();
        driver = ao_default_driver_id();
        mpg123_init();
        mh = mpg123_new(NULL, &err);
        buffer_size = mpg123_outblock(mh);
        buffer = (unsigned char*) malloc(buffer_size * sizeof(unsigned char));

        /* open the file and get the decoding format */
        mpg123_open(mh, "/home/pi/Music/dingdong.mp3");
        mpg123_getformat(mh, &rate, &channels, &encoding);

        /* set the output format and open the output device */
        format.bits = mpg123_encsize(encoding) * BITS;
        format.rate = rate;
        format.channels = channels;
        format.byte_format = AO_FMT_NATIVE;
        format.matrix = 0;
        dev = ao_open_live(driver, &format, NULL);

        /* decode and play */
        while (mpg123_read(mh, buffer, buffer_size, &done) == MPG123_OK)
        {
            ao_play(dev, (char *)buffer, done);
            if(!_isRinging)
            {
                break;
            }
        }

        /* clean up */
        free(buffer);
        ao_close(dev);
        mpg123_close(mh);
        mpg123_delete(mh);
        mpg123_exit();
        ao_shutdown();
    }
#endif
}
