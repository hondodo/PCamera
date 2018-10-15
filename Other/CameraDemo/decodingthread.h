#ifndef DECODINGTHREAD_H
#define DECODINGTHREAD_H

#include <QThread>
#include <QTime>
#include <QDebug>
#include <QImage>

extern "C"
{
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libavfilter/avfilter.h"
#include "libavfilter/buffersink.h"
#include "libavfilter/buffersrc.h"
#include "libavutil/avutil.h"
#include "libavutil/opt.h"
#include "libavutil/pixdesc.h"
#include "libavdevice/avdevice.h"
#include "libswscale/swscale.h"
#include "libavutil/time.h"
}

#include "pathhelper.h"
#include "cameratype.h"
#include "mathelper.h"
#include "cameracollectorhelper.h"
class DecodingThread : public QThread
{
    Q_OBJECT
public:
    explicit DecodingThread(QObject *parent = nullptr);
    void setStop();
    void onStartRecoing(int width, int height, int pixOut);

    void setFiltedFrame(const AVFrame *value);

protected:
    void run();

signals:
    void onFrameImage(const QImage image);

public slots:

private:
    bool _isRunnig;

    //decode
    bool hasInitDecodingRecs;
    AVFrame *filtedFrame;
    int widthOut;
    int heightOut;
    AVPixelFormat pixOut;
    AVFrame *pFrameRGB;
    struct SwsContext *imgConvertCtcRGB;
    AVPixelFormat rgbFmt;
    int rgbBytes;
    uint8_t *rgbOutBuffer;
    cv::Mat mRGB, temp;

    void resetDecodingRecs();
    void initDecodingRecs();
    void decodeFrameAndShow(AVFrame *filtedFrame);

};

#endif // DECODINGTHREAD_H
