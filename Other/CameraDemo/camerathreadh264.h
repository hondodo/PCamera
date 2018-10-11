#ifndef CAMERATHREADH264_H
#define CAMERATHREADH264_H

#define CANNOT_OPEN_INPUTFILE -99
#define CANNOT_OPEN_OUTPUT_TEMP -100
#define CANNOT_OPEN_OUTPUE_SAVE -101

#ifndef PRId64 || PRI_MACROS_BROKEN
#undef PRId64
#define PRId64"lld"
#endif

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

class CameraThreadH264 : public QThread
{
    Q_OBJECT
public:
    explicit CameraThreadH264(QObject *parent = nullptr);
    ~CameraThreadH264();

    void setStop();

    typedef struct FilteringContext
    {
        AVFilterContext *buffersink_ctx;
        AVFilterContext *buffersrc_ctx;
        AVFilterGraph *filter_graph;
    } FilteringContext;

    typedef struct StreamContext
    {
        AVCodecContext *dec_ctx;
    } StreamContext;

    typedef struct StreamContextOutPut
    {
        AVCodecContext *enc_ctx;
    } StreamContextOutPut;

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

    bool getCheckMog() const;
    void setCheckMog(bool value);

    bool getSaveOnlyMog() const;
    void setSaveOnlyMog(bool value);

    CAMERASIZE getCameraSize() const;
    void setCameraSize(const CAMERASIZE &value);

    CAMERASIZE getCurrentCameraSize() const;

signals:
    void onFrame(const QImage &image);
#ifdef USE_OPENGL
    void onYUVFrame(const unsigned char* y_data, const unsigned char* u_data, const unsigned char* v_data);
#endif
    void onMessage(const QString text);

protected:
    void run();

private:
    bool _isRunning;
    static int cameraId;
    int currentCameraId;

    CAMERASIZE cameraSize;
    CAMERASIZE currentCameraSize;

    QString cameraUrl;
    CAMERATYPE cameraType;//0-local 1-web
    QString cameraName;
    bool checkBrighness;
    bool fixBrighnessByTime;
    bool checkMog;
    bool saveOnlyMog;

    QString outputFileNameForTemp;
    QString fontFile;
    PathHelper pathHelper;

    AVFormatContext *ifmt_ctx;
    AVFormatContext *ofmt_ctx;
    FilteringContext *filter_ctx;
    StreamContext *stream_ctx;
    StreamContextOutPut *stream_ctx_out;

    AVFrame *filtedFrame;
    AVCodecContext *pOutCodecCtx;

    int open_input_file(const char *filename);
    int open_output_file(const char *filename);
    int init_filter(FilteringContext *fctx, AVCodecContext *dec_ctx, AVCodecContext *enc_ctx, const char *filter_spec);
    int init_filters();
    int encode_write_frame(AVFrame *filt_frame, unsigned int stream_index, int *got_frame, bool cloneframe = false);
    int filter_encode_write_frame(AVFrame *frame, unsigned int stream_index, bool cloneframe = false);
    int flush_encoder(unsigned int stream_index);
    int caputuer();
    void closeContext(AVFrame **frame);
    void closeOutPut();
    void emitMessage(const QString text);

    AVCodecContext *pCodecCtx;
    bool cantainvideo, cantainaudio;
    int videoindex, audioindex;
};

#endif // CAMERATHREADH264_H
