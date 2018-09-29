#ifndef CAMERATHREAD_H
#define CAMERATHREAD_H

#include <QSysInfo>
#include <stdio.h>
#include <string.h>
#include <QThread>
#include <QDebug>
#include <QTime>
#include <QImage>
#include <QDateTime>

#include "cameratype.h"
#include "mathelper.h"

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
}

#define AV_CODEC_FLAG_GLOBAL_HEADER (1 << 22)
#define CODEC_FLAG_GLOBAL_HEADER AV_CODEC_FLAG_GLOBAL_HEADER
#define AVFMT_RAWPICTURE 0x0020

typedef struct FilteringContext
{
    AVFilterContext*buffersink_ctx;
    AVFilterContext*buffersrc_ctx;
    AVFilterGraph*filter_graph;
} FilteringContext;

class CameraThread : public QThread
{
    Q_OBJECT
public:
    explicit CameraThread(QObject *parent = nullptr);
    void setStop();

    QString getCameraUrl() const;
    void setCameraUrl(const QString &value);

    CAMERATYPE getCameraType() const;
    void setCameraType(const CAMERATYPE &value);

    QString getCameraName() const;
    void setCameraName(const QString &value);

    bool getCheckBrighness() const;
    void setCheckBrighness(bool value);

    bool getFixBrighnessByTime() const;
    void setFixBrighnessByTime(bool value);

protected:
    void run();

signals:signals:
    void onFrame(QImage);
    void onFrameSize(int width, int height);
    void onFrame(unsigned char *yuvData);

protected slots:

private:
    bool _isRunning;

    QString cameraUrl;
    CAMERATYPE cameraType;//0-local 1-web
    QString cameraName;
    bool checkBrighness;
    bool fixBrighnessByTime;

    QString outputFileName;

    AVFormatContext *ifmt_ctx;
    AVFormatContext *ofmt_ctx;
    AVCodecContext *pCodecCtx;
    AVCodecContext *pCodecCtxOut;
    FilteringContext *filter_ctx;
    bool isLocalCamera = false;
    bool isRawVideo = false;
    bool isSaveTurn;
    char buf[1024];

    void printError(int ret);
    int open_input_file(const char *filename);
    int open_output_file(const char *filename);
    int init_filter(FilteringContext *fctx, AVCodecContext *dec_ctx, AVCodecContext *enc_ctx, const char *filter_spec);
    int init_filters();
    int encode_write_frame(AVFrame *filt_frame, unsigned int stream_index, int *got_frame);
    int filter_encode_no_write_frame(AVFrame *frame, unsigned int stream_index);
    int filter_encode_write_frame(AVFrame *frame, unsigned int stream_index);
    int flush_encoder(unsigned int stream_index);
    int caputuer();
};

#endif // CAMERATHREAD_H
