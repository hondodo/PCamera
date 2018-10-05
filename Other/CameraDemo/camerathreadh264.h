#ifndef CAMERATHREADH264_H
#define CAMERATHREADH264_H

#include <QThread>

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

class CameraThreadH264 : public QThread
{
    Q_OBJECT
public:
    explicit CameraThreadH264(QObject *parent = nullptr);

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
        AVCodecContext *enc_ctx;
    } StreamContext;

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

protected:
    void run();

private:
    bool _isRunning;
    static int cameraId;
    int currentCameraId;

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
    int open_input_file(const char *filename);
    int open_output_file(const char *filename);
    int init_filter(FilteringContext *fctx, AVCodecContext *dec_ctx, AVCodecContext *enc_ctx, const char *filter_spec);
    int init_filters();
    int encode_write_frame(AVFrame *filt_frame, unsigned int stream_index, int *got_frame);
    int filter_encode_write_frame(AVFrame *frame, unsigned int stream_index);
    int flush_encoder(unsigned int stream_index);
    int caputuer();
    void closeContext(AVFrame *frame);
};

#endif // CAMERATHREADH264_H
