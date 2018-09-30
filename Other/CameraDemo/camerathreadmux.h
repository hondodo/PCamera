#ifndef CAMERATHREADMUX_H
#define CAMERATHREADMUX_H

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
#include "pathhelper.h"
#include "cameracollectorhelper.h"

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
#ifndef AV_CODEC_FLAG_GLOBAL_HEADER
#define AV_CODEC_FLAG_GLOBAL_HEADER (1 << 22)
#endif
#ifndef CODEC_FLAG_GLOBAL_HEADER
#define CODEC_FLAG_GLOBAL_HEADER AV_CODEC_FLAG_GLOBAL_HEADER
#endif
#ifndef AVFMT_RAWPICTURE
#define AVFMT_RAWPICTURE 0x0020
#endif

typedef struct FilteringContext
{
    AVFilterContext *buffersink_ctx;
    AVFilterContext *buffersrc_ctx;
    AVFilterGraph *filter_graph;
} FilteringContext;

class CameraThreadMUX : public QThread
{
    Q_OBJECT
public:
    explicit CameraThreadMUX(QObject *parent = nullptr);
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

    QString getFontFile() const;
    void setFontFile(const QString &value);

protected:
    void run();

signals:signals:
    void onFrame(QImage);
    void onFrameSize(int width, int height);
    void onFrame(unsigned char *yuvData);

protected slots:

private:
    bool _isRunning;
    static int cameraId;
    int currentCameraId;

    QString cameraUrl;
    CAMERATYPE cameraType;//0-local 1-web
    QString cameraName;
    bool checkBrighness;
    bool fixBrighnessByTime;

    QString outputFileNameForTemp;
    QString fontFile;
    PathHelper pathHelper;

    AVFormatContext *ifmt_ctx;
    //AVFormatContext *ofmt_ctx;
    AVCodecContext *pCodecCtx;
    FilteringContext *filter_ctx;
    bool isLocalCamera = false;
    bool isRawVideo = false;
    bool isSaveTurn;
    char buf[1024];

    void printError(int ret);
    int openInputFile(const char *filename);
    AVFormatContext *openOutputFile(const char *filename, int *ret, AVCodecContext **dec_ctx);
    int initFilter(FilteringContext *fctx, AVCodecContext *dec_ctx, AVCodecContext *enc_ctx, const char *filter_spec);
    int initFilters(AVFormatContext *ofmt_ctx);
    int encodeWriteFrame(AVFrame *filt_frame, unsigned int stream_index, int *got_frame, AVFormatContext *ofmt_ctx);
    int filterEncodeNoWriteFrame(AVFrame *frame, unsigned int stream_index, AVFormatContext *ofmt_ctx);
    int filterEncodeWriteFrame(AVFrame *frame, unsigned int stream_index, AVFormatContext *ofmt_ctx);
    int flushEncoder(unsigned int stream_index, AVFormatContext *ofmt_ctx);
    int caputuer();
    void closeOutputFile(AVFormatContext **ofmt_ctx, AVCodecContext **enc_ctx);
};

#endif // CAMERATHREADMUX_H
