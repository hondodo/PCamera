#include "camerathreadh264.h"

int CameraThreadH264::cameraId = 0;
CameraThreadH264::CameraThreadH264(QObject *parent) : QThread(parent)
{
    _isRunning = false;
    checkMog = true;
    saveOnlyMog = false;
    cantainaudio = false;
    cantainvideo = false;
    isTv = false;
    videoindex = 0;
    audioindex = 0;
    recdura = 0;
    lastReadInterrupTime = QDateTime::currentDateTime().toMSecsSinceEpoch();
    delayOpenCamera = false;

    ifmt_ctx = NULL;
    ofmt_ctx = NULL;
    filter_ctx = NULL;
    stream_ctx = NULL;
    stream_ctx_out = NULL;

    cameraFrame = NULL;

    cameraType = CAMERATYPE_LOCAL;
    cameraSize = CAMERASIZE_AUTO;
    currentCameraSize = CAMERASIZE_1920x1080;
#ifdef Q_OS_WIN
    //pathHelper.setRootPath("D:/");
    cameraUrl = "video=";
    outputFileNameForTemp = pathHelper.getTempFileName();
    fontFile = "D\\\\:font.ttf";
#else
    //pathHelper.setRootPath("/home/pi/");
    cameraUrl = "/dev/video0";
    outputFileNameForTemp = pathHelper.getTempFileName();
    fontFile = "/home/pi/Font/font.ttf";
#endif
    _isRunning = false;
    checkBrighness = false;
    fixBrighnessByTime = false;
    cameraName = "";
    currentCameraId = cameraId;
    cameraId++;

    pCodecCtx = NULL;
    filtedFrame = NULL;
    pOutCodecCtx = NULL;
}

CameraThreadH264::~CameraThreadH264()
{
    qDebug() << "Release thread" << cameraName;
    CameraCollectorThread::Init->removeMogRcs(currentCameraId);
    //if(ifmt_ctx) delete ifmt_ctx;
    //if(ofmt_ctx) delete ofmt_ctx;
    //if(filter_ctx) delete filter_ctx;
    //if(stream_ctx) delete[] stream_ctx;
    //ifmt_ctx = NULL;
    //ofmt_ctx = NULL;
    //filter_ctx = NULL;
    //stream_ctx = NULL;
}

void CameraThreadH264::setStop()
{
    _isRunning = false;
}

QString CameraThreadH264::getCameraUrl() const
{
    return cameraUrl;
}

void CameraThreadH264::setCameraUrl(const QString &value)
{
    cameraUrl = value;
}

CAMERATYPE CameraThreadH264::getCameraType() const
{
    return cameraType;
}

void CameraThreadH264::setCameraType(const CAMERATYPE &value)
{
    cameraType = value;
}

QString CameraThreadH264::getCameraName() const
{
    return cameraName;
}

void CameraThreadH264::setCameraName(const QString &value)
{
    cameraName = value;
    pathHelper.setCameraName(cameraName);
    pathHelper.init();
    outputFileNameForTemp = pathHelper.getTempFileName();
}

bool CameraThreadH264::getCheckBrighness() const
{
    return checkBrighness;
}

void CameraThreadH264::setCheckBrighness(bool value)
{
    checkBrighness = value;
}

bool CameraThreadH264::getFixBrighnessByTime() const
{
    return fixBrighnessByTime;
}

void CameraThreadH264::setFixBrighnessByTime(bool value)
{
    fixBrighnessByTime = value;
}

bool CameraThreadH264::getCheckMog() const
{
    return checkMog;
}

void CameraThreadH264::setCheckMog(bool value)
{
    checkMog = value;
}

bool CameraThreadH264::getSaveOnlyMog() const
{
    return saveOnlyMog;
}

void CameraThreadH264::setSaveOnlyMog(bool value)
{
    saveOnlyMog = value;
}

void CameraThreadH264::run()
{
    _isRunning = true;
    emitMessage("Thread started");
    int ret = 0;
    while (_isRunning) //LOOP
    {
        if(delayOpenCamera)
        {
            emitMessage("delay 30s open camera");
            for(int i = 0; i < 1500; i++)
            {
                if(!_isRunning)
                {
                    return;
                }
                this->msleep(20);
            }
        }
        delayOpenCamera = false;
        emitMessage("loop record");
        ret = caputuer();
        if(recdura < 5000 || ret == CANNOT_OPEN_INPUTFILE || ret == CANNOT_OPEN_OUTPUE_SAVE
                || ret == CANNOT_OPEN_OUTPUT_TEMP || ret == CANNOT_REC_UNKNOWN_ERROR)
        {
            emitMessage("[" +  QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss") + "]"
                        + "Open file error: " + ret + ";Retry 1 min later");
            for(int i = 0; i < 3000; i++)
            {
                this->msleep(20);
                if(!_isRunning)
                {
                    break;
                }
            }
        }
        else
        {
            for(int i = 0; i < 25; i++)
            {
                this->msleep(20);
                if(!_isRunning)
                {
                    break;
                }
            }
        }
    }
    emit onStopRecoding();
    //delay 2s for ui show image
    for(int i = 0; i < 100; i++)
    {
        this->msleep(20);
    }
    releaseFiltedFrame();
    emitMessage("Thread stoped");
    _isRunning = false;
}

bool CameraThreadH264::getDelayOpenCamera() const
{
    return delayOpenCamera;
}

void CameraThreadH264::setDelayOpenCamera(bool value)
{
    delayOpenCamera = value;
}

AVFrame **CameraThreadH264::getFiltedFrame()
{
    if(filtedFrame != NULL)
    {
        return &filtedFrame;
    }
}

void CameraThreadH264::releaseFiltedFrame()
{
    if(filtedFrame != NULL)
    {
        av_frame_free(&filtedFrame);
    }
}

int CameraThreadH264::AVInterruptCallBackFun(void *ctx)
{
    if(ctx == NULL)
    {
        return 0;
    }
    else
    {
        CameraThreadH264 *obj = (CameraThreadH264*)ctx;
        if(obj == NULL)
        {
            return 0;
        }
        qint64 timeout = QDateTime::currentDateTime().toMSecsSinceEpoch() - obj->lastReadInterrupTime;
        if(timeout > 5000)
        {
            obj->emitMessage("Read frame timeout");
            return 1;//超时
        }
    }
    return 0;
}

CAMERASIZE CameraThreadH264::getCurrentCameraSize() const
{
    return currentCameraSize;
}

CAMERASIZE CameraThreadH264::getCameraSize() const
{
    return cameraSize;
}

void CameraThreadH264::setCameraSize(const CAMERASIZE &value)
{
    cameraSize = value;
    currentCameraSize = value;
}

int CameraThreadH264::open_input_file(const char *filename)
{
    ifmt_ctx = NULL;
#ifdef Q_OS_WIN
#else
    QFile file(filename);
    if(!file.exists())
    {
        emitMessage(tr("camera file not exists"));
        return CANNOT_OPEN_INPUTFILE;
    }
#endif

    int ret;
    unsigned int i;
    
    avdevice_register_all();
    AVInputFormat *inputFmt = NULL;
#ifdef Q_OS_WIN
    inputFmt = av_find_input_format("dshow");
#else
    inputFmt = av_find_input_format("video4linux2");
#endif
    AVDictionary *avdic = NULL;

    if(isTv)
    {
        av_dict_set(&avdic, "video_size", "720x576", 0);
    }
    else
    {
        av_dict_set(&avdic, "max_delay", "100", 0);
        av_dict_set(&avdic, "framerate", "30", 0);
        av_dict_set(&avdic, "input_format", "mjpeg", 0);
        av_dict_set(&avdic, "timeout", "3000000", 0);//设置超时3秒
    }
    if(isTv)
    {
        ret = avformat_open_input(&ifmt_ctx, filename, inputFmt, &avdic);
    }
    else
    {
        int csize = (int)currentCameraSize;
        int minsize = (int)CAMERASIZE_640x480;
        if(csize < 0 || csize > minsize)
        {
            currentCameraSize = CAMERASIZE_AUTO;
        }
        if(currentCameraSize == CAMERASIZE_AUTO)
        {
            currentCameraSize = CAMERASIZE_1920x1080;
        }
        while (csize <= minsize)
        {
            if(currentCameraSize == CAMERASIZE_1920x1080)
            {
                av_dict_set(&avdic, "video_size", "1920x1080", 0);
            }
            else if(currentCameraSize == CAMERASIZE_1280x720)
            {
                av_dict_set(&avdic, "video_size", "1280x720", 0);
            }
            else if(currentCameraSize == CAMERASIZE_640x480)
            {
                av_dict_set(&avdic, "video_size", "640x480", 0);
            }
            if ((ret = avformat_open_input(&ifmt_ctx, filename, inputFmt, &avdic)) < 0)
            {
                qDebug() << "Cannot open for current size:" << currentCameraSize << cameraName;
                csize++;
                if(csize <= minsize)
                {
                    currentCameraSize = (CAMERASIZE)csize;
                }
            }
            else
            {
                qDebug() << "Open for current size:" << currentCameraSize << "success" << cameraName;
                break;
            }
        }
    }

    if (ret < 0)//((ret = avformat_open_input(&ifmt_ctx, filename, inputFmt, &avdic)) < 0)
    {
        av_log(NULL, AV_LOG_ERROR, "Cannot open input file\n");
        return ret;
    }

    if ((ret = avformat_find_stream_info(ifmt_ctx, NULL)) < 0) {
        av_log(NULL, AV_LOG_ERROR, "Cannot find stream information\n");
        return ret;
    }

    stream_ctx = (StreamContext *)av_mallocz_array(ifmt_ctx->nb_streams, sizeof(*stream_ctx));
    if (!stream_ctx)
        return AVERROR(ENOMEM);

    for (i = 0; i < ifmt_ctx->nb_streams; i++)
    {
        AVStream *stream = ifmt_ctx->streams[i];
        AVCodec *dec = avcodec_find_decoder(stream->codecpar->codec_id);
        AVCodecContext *codec_ctx;
        if (!dec)
        {
            av_log(NULL, AV_LOG_ERROR, "Failed to find decoder for stream #%u\n", i);
            return AVERROR_DECODER_NOT_FOUND;
        }
        codec_ctx = avcodec_alloc_context3(dec);
        if (!codec_ctx)
        {
            av_log(NULL, AV_LOG_ERROR, "Failed to allocate the decoder context for stream #%u\n", i);
            return AVERROR(ENOMEM);
        }
        ret = avcodec_parameters_to_context(codec_ctx, stream->codecpar);
        if (ret < 0)
        {
            av_log(NULL, AV_LOG_ERROR, "Failed to copy decoder parameters to input decoder context "
                                       "for stream #%u\n", i);
            return ret;
        }
        /* Reencode video & audio and remux subtitles etc. */
        if (codec_ctx->codec_type == AVMEDIA_TYPE_VIDEO
                || codec_ctx->codec_type == AVMEDIA_TYPE_AUDIO)
        {
            if (codec_ctx->codec_type == AVMEDIA_TYPE_VIDEO)
            {
                cantainvideo = true;
                videoindex = i;
                codec_ctx->framerate = av_guess_frame_rate(ifmt_ctx, stream, NULL);
            }
            else if(codec_ctx->codec_type == AVMEDIA_TYPE_AUDIO)
            {
                cantainaudio = true;
                audioindex = i;
            }
            /* Open decoder */
            ret = avcodec_open2(codec_ctx, dec, NULL);
            if (ret < 0)
            {
                av_log(NULL, AV_LOG_ERROR, "Failed to open decoder for stream #%u\n", i);
                return ret;
            }
        }
        stream_ctx[i].dec_ctx = codec_ctx;
    }

    av_dump_format(ifmt_ctx, 0, filename, 0);
    return 0;
}

int CameraThreadH264::open_output_file(const char *filename)
{
    stream_ctx_out = (StreamContextOutPut *)av_mallocz_array(ifmt_ctx->nb_streams, sizeof(*stream_ctx_out));
    if (!stream_ctx_out)
        return AVERROR(ENOMEM);

    AVStream *out_stream;
    AVStream *in_stream;
    AVCodecContext *dec_ctx, *enc_ctx;
    AVCodec *encoder;
    int ret;
    unsigned int i;

    ofmt_ctx = NULL;
    avformat_alloc_output_context2(&ofmt_ctx, NULL, NULL, filename);
    if (!ofmt_ctx) {
        av_log(NULL, AV_LOG_ERROR, "Could not create output context\n");
        return AVERROR_UNKNOWN;
    }


    for (i = 0; i < ifmt_ctx->nb_streams; i++) {
        out_stream = avformat_new_stream(ofmt_ctx, NULL);
        if (!out_stream) {
            av_log(NULL, AV_LOG_ERROR, "Failed allocating output stream\n");
            return AVERROR_UNKNOWN;
        }

        in_stream = ifmt_ctx->streams[i];
        dec_ctx = stream_ctx[i].dec_ctx;

        if (dec_ctx->codec_type == AVMEDIA_TYPE_VIDEO
                || dec_ctx->codec_type == AVMEDIA_TYPE_AUDIO)
        {
            if(dec_ctx->codec_type == AVMEDIA_TYPE_VIDEO)
            {
#ifdef Q_OS_WIN
                encoder = avcodec_find_encoder(AV_CODEC_ID_H264);
#else
                encoder = avcodec_find_encoder_by_name("h264_omx");
#endif
                pCodecCtx = in_stream->codec;
            }
            else
            {
                encoder = avcodec_find_encoder(AV_CODEC_ID_MP3);//dec_ctx->codec_id);
            }
            if (!encoder)
            {
                av_log(NULL, AV_LOG_FATAL, "Necessary encoder not found\n");
                return AVERROR_INVALIDDATA;
            }
            enc_ctx = avcodec_alloc_context3(encoder);
            if (!enc_ctx) {
                av_log(NULL, AV_LOG_FATAL, "Failed to allocate the encoder context\n");
                return AVERROR(ENOMEM);
            }

            /* In this example, we transcode to same properties (picture size,
                 * sample rate etc.). These properties can be changed for output
                 * streams easily using filters */
            if (dec_ctx->codec_type == AVMEDIA_TYPE_VIDEO)
            {
                enc_ctx->height = dec_ctx->height;
                enc_ctx->width = dec_ctx->width;
                enc_ctx->sample_aspect_ratio = dec_ctx->sample_aspect_ratio;
                /* take first format from list of supported formats */
                if (encoder->pix_fmts)
                    enc_ctx->pix_fmt = encoder->pix_fmts[0];
                else
                    enc_ctx->pix_fmt = dec_ctx->pix_fmt;
                /* video time_base can be set to whatever is handy and supported by encoder */
                enc_ctx->time_base = av_inv_q(dec_ctx->framerate);
                enc_ctx->bit_rate = 2000000;
                if(currentCameraSize == CAMERASIZE_640x480)
                    enc_ctx->bit_rate = 2000000;
                else if(currentCameraSize == CAMERASIZE_1280x720)
                    enc_ctx->bit_rate = 3000000;
                else if(currentCameraSize == CAMERASIZE_1920x1080)
                    enc_ctx->bit_rate = 4000000;
                enc_ctx->gop_size = 250;
                enc_ctx->max_b_frames = 10;
                enc_ctx->qmin = 10;
                enc_ctx->qmax = 51;
                pOutCodecCtx = enc_ctx;
            }
            else
            {
                enc_ctx->sample_rate = dec_ctx->sample_rate;
                enc_ctx->channel_layout = dec_ctx->channel_layout;
                enc_ctx->channels = av_get_channel_layout_nb_channels(enc_ctx->channel_layout);
                /* take first format from list of supported formats */
                enc_ctx->sample_fmt = encoder->sample_fmts[0];
                enc_ctx->time_base.num = 1;
                enc_ctx->time_base.den = enc_ctx->sample_rate;
            }

            /* Third parameter can be used to pass settings to encoder */

            if(dec_ctx->codec_type == AVMEDIA_TYPE_VIDEO)
            {
                AVDictionary *parm = 0;
                av_dict_set(&parm, "preset", "superfast", 0);
                av_dict_set(&parm, "tune", "zerolatency", 0);
                ret = avcodec_open2(enc_ctx, encoder, &parm);
            }
            else
            {
                ret = avcodec_open2(enc_ctx, encoder, NULL);
            }
            if (ret < 0) {
                av_log(NULL, AV_LOG_ERROR, "Cannot open video encoder for stream #%u\n", i);
                return ret;
            }
            ret = avcodec_parameters_from_context(out_stream->codecpar, enc_ctx);
            if (ret < 0) {
                av_log(NULL, AV_LOG_ERROR, "Failed to copy encoder parameters to output stream #%u\n", i);
                return ret;
            }
            if (ofmt_ctx->oformat->flags & AVFMT_GLOBALHEADER)
                enc_ctx->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;

            out_stream->time_base = enc_ctx->time_base;
            stream_ctx_out[i].enc_ctx = enc_ctx;
        } else if (dec_ctx->codec_type == AVMEDIA_TYPE_UNKNOWN) {
            av_log(NULL, AV_LOG_FATAL, "Elementary stream #%d is of unknown type, cannot proceed\n", i);
            return AVERROR_INVALIDDATA;
        } else {
            /* if this stream must be remuxed */
            ret = avcodec_parameters_copy(out_stream->codecpar, in_stream->codecpar);
            if (ret < 0) {
                av_log(NULL, AV_LOG_ERROR, "Copying parameters for stream #%u failed\n", i);
                return ret;
            }
            out_stream->time_base = in_stream->time_base;
        }

    }
    av_dump_format(ofmt_ctx, 0, filename, 1);

    if (!(ofmt_ctx->oformat->flags & AVFMT_NOFILE)) {
        ret = avio_open(&ofmt_ctx->pb, filename, AVIO_FLAG_WRITE);
        if (ret < 0) {
            av_log(NULL, AV_LOG_ERROR, "Could not open output file '%s'", filename);
            return ret;
        }
    }

    /* init muxer, write output file header */
    ret = avformat_write_header(ofmt_ctx, NULL);
    if (ret < 0) {
        av_log(NULL, AV_LOG_ERROR, "Error occurred when opening output file\n");
        return ret;
    }

    return 0;
}

int CameraThreadH264::init_filter(CameraThreadH264::FilteringContext *fctx, AVCodecContext *dec_ctx, AVCodecContext *enc_ctx, const char *filter_spec)
{
    char args[512];
    int ret = 0;
    const AVFilter *buffersrc = NULL;
    const AVFilter *buffersink = NULL;
    AVFilterContext *buffersrc_ctx = NULL;
    AVFilterContext *buffersink_ctx = NULL;
    AVFilterInOut *outputs = avfilter_inout_alloc();
    AVFilterInOut *inputs  = avfilter_inout_alloc();
    AVFilterGraph *filter_graph = avfilter_graph_alloc();

    if (!outputs || !inputs || !filter_graph) {
        ret = AVERROR(ENOMEM);
        goto end;
    }

    if (dec_ctx->codec_type == AVMEDIA_TYPE_VIDEO) {
        buffersrc = avfilter_get_by_name("buffer");
        buffersink = avfilter_get_by_name("buffersink");
        if (!buffersrc || !buffersink) {
            av_log(NULL, AV_LOG_ERROR, "filtering source or sink element not found\n");
            ret = AVERROR_UNKNOWN;
            goto end;
        }

        snprintf(args, sizeof(args),
                 "video_size=%dx%d:pix_fmt=%d:time_base=%d/%d:pixel_aspect=%d/%d",
                 dec_ctx->width, dec_ctx->height, dec_ctx->pix_fmt,
                 dec_ctx->time_base.num, dec_ctx->time_base.den,
                 dec_ctx->sample_aspect_ratio.num,
                 dec_ctx->sample_aspect_ratio.den);

        ret = avfilter_graph_create_filter(&buffersrc_ctx, buffersrc, "in",
                                           args, NULL, filter_graph);
        if (ret < 0) {
            av_log(NULL, AV_LOG_ERROR, "Cannot create buffer source\n");
            goto end;
        }

        ret = avfilter_graph_create_filter(&buffersink_ctx, buffersink, "out",
                                           NULL, NULL, filter_graph);
        if (ret < 0) {
            av_log(NULL, AV_LOG_ERROR, "Cannot create buffer sink\n");
            goto end;
        }

        ret = av_opt_set_bin(buffersink_ctx, "pix_fmts",
                             (uint8_t*)&enc_ctx->pix_fmt, sizeof(enc_ctx->pix_fmt),
                             AV_OPT_SEARCH_CHILDREN);
        if (ret < 0) {
            av_log(NULL, AV_LOG_ERROR, "Cannot set output pixel format\n");
            goto end;
        }
    } else if (dec_ctx->codec_type == AVMEDIA_TYPE_AUDIO)
    {
        buffersrc = avfilter_get_by_name("abuffer");
        buffersink = avfilter_get_by_name("abuffersink");
        if (!buffersrc || !buffersink)
        {
            av_log(NULL, AV_LOG_ERROR, "filtering source or sink element not found\n");
            ret = AVERROR_UNKNOWN;
            goto end;
        }

        if (!dec_ctx->channel_layout)
            dec_ctx->channel_layout =
                    av_get_default_channel_layout(dec_ctx->channels);
        snprintf(args, sizeof(args),
                 "time_base=%d/%d:sample_rate=%d:sample_fmt=%s:channel_layout=0x%"PRIx64,
                 dec_ctx->time_base.num, dec_ctx->time_base.den, dec_ctx->sample_rate,
                 av_get_sample_fmt_name(dec_ctx->sample_fmt),
                 dec_ctx->channel_layout);
        qDebug() << args;
        ret = avfilter_graph_create_filter(&buffersrc_ctx, buffersrc, "in",
                                           args, NULL, filter_graph);
        if (ret < 0) {
            av_log(NULL, AV_LOG_ERROR, "Cannot create audio buffer source\n");
            goto end;
        }

        ret = avfilter_graph_create_filter(&buffersink_ctx, buffersink, "out",
                                           NULL, NULL, filter_graph);
        if (ret < 0) {
            av_log(NULL, AV_LOG_ERROR, "Cannot create audio buffer sink\n");
            goto end;
        }

        ret = av_opt_set_bin(buffersink_ctx, "sample_fmts",
                             (uint8_t*)&enc_ctx->sample_fmt, sizeof(enc_ctx->sample_fmt),
                             AV_OPT_SEARCH_CHILDREN);
        if (ret < 0) {
            av_log(NULL, AV_LOG_ERROR, "Cannot set output sample format\n");
            goto end;
        }

        ret = av_opt_set_bin(buffersink_ctx, "channel_layouts",
                             (uint8_t*)&enc_ctx->channel_layout,
                             sizeof(enc_ctx->channel_layout), AV_OPT_SEARCH_CHILDREN);
        if (ret < 0) {
            av_log(NULL, AV_LOG_ERROR, "Cannot set output channel layout\n");
            goto end;
        }

        ret = av_opt_set_bin(buffersink_ctx, "sample_rates",
                             (uint8_t*)&enc_ctx->sample_rate, sizeof(enc_ctx->sample_rate),
                             AV_OPT_SEARCH_CHILDREN);
        if (ret < 0) {
            av_log(NULL, AV_LOG_ERROR, "Cannot set output sample rate\n");
            goto end;
        }
    } else
    {
        ret = AVERROR_UNKNOWN;
        goto end;
    }

    /* Endpoints for the filter graph. */
    outputs->name       = av_strdup("in");
    outputs->filter_ctx = buffersrc_ctx;
    outputs->pad_idx    = 0;
    outputs->next       = NULL;

    inputs->name       = av_strdup("out");
    inputs->filter_ctx = buffersink_ctx;
    inputs->pad_idx    = 0;
    inputs->next       = NULL;

    if (!outputs->name || !inputs->name) {
        ret = AVERROR(ENOMEM);
        goto end;
    }

    if ((ret = avfilter_graph_parse_ptr(filter_graph, filter_spec,
                                        &inputs, &outputs, NULL)) < 0)
        goto end;

    if ((ret = avfilter_graph_config(filter_graph, NULL)) < 0)
        goto end;

    /* Fill FilteringContext */
    fctx->buffersrc_ctx = buffersrc_ctx;
    fctx->buffersink_ctx = buffersink_ctx;
    fctx->filter_graph = filter_graph;

end:
    avfilter_inout_free(&inputs);
    avfilter_inout_free(&outputs);

    return ret;
}

int CameraThreadH264::init_filters()
{
    const char *filter_spec;
    unsigned int i;
    int ret;
    filter_ctx = (FilteringContext *)av_malloc_array(ifmt_ctx->nb_streams, sizeof(*filter_ctx));
    if (!filter_ctx)
        return AVERROR(ENOMEM);

    for (i = 0; i < ifmt_ctx->nb_streams; i++) {
        filter_ctx[i].buffersrc_ctx  = NULL;
        filter_ctx[i].buffersink_ctx = NULL;
        filter_ctx[i].filter_graph   = NULL;
        if (!(ifmt_ctx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO
              || ifmt_ctx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO))
            continue;


        if (ifmt_ctx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO)
        {
            if(isTv)
            {
                filter_spec = "null";
            }
            else
            {
#ifdef Q_OS_WIN
                //filter_spec = "[in]drawtext=fontfile=D\\\\:font.ttf:fontcolor=white:fontsize=40:text='%{localtime}':x=20:y=20:shadowcolor=black:shadowx=2:shadowy=2[a];[a]hqdn3d[a];[a]unsharp=5:5:2[out]";
                filter_spec = "[in]drawtext=fontfile=D\\\\:font.ttf:fontcolor=white:fontsize=40:text='%{localtime}':x=20:y=20:shadowcolor=black:shadowx=2:shadowy=2[a];[a]hue=h=360:s=2[out]";
#else
                filter_spec = "[in]drawtext=fontfile=/home/pi/Font/font.ttf:fontcolor=white:fontsize=20:text='%{localtime}':x=20:y=20:shadowcolor=black:shadowx=2:shadowy=2[a];[a]hue=h=360:s=2[out]"; /* passthrough (dummy) filter for video */
                //filter_spec = "[in]drawtext=fontfile=/home/pi/Font/font.ttf:fontcolor=white:fontsize=20:text='%{localtime}':x=20:y=20:shadowcolor=black:shadowx=2:shadowy=2[a];[a]hqdn3d[a];[a]unsharp=5:5:2[out]";
#endif
            }
        }
        else
            filter_spec = "anull"; /* passthrough (dummy) filter for audio */
        ret = init_filter(&filter_ctx[i], stream_ctx[i].dec_ctx,
                          stream_ctx_out[i].enc_ctx, filter_spec);
        if (ret)
            return ret;
    }
    return 0;
}

int CameraThreadH264::encode_write_frame(AVFrame *filt_frame, unsigned int stream_index, int *got_frame, bool cloneframe)
{
    int ret;
    int got_frame_local;
    AVPacket enc_pkt;
    int (*enc_func)(AVCodecContext *, AVPacket *, const AVFrame *, int *) =
            (ifmt_ctx->streams[stream_index]->codecpar->codec_type ==
             AVMEDIA_TYPE_VIDEO) ? avcodec_encode_video2 : avcodec_encode_audio2;

    if (!got_frame)
        got_frame = &got_frame_local;

    //av_log(NULL, AV_LOG_INFO, "Encoding frame\n");
    /* encode filtered frame */
    enc_pkt.data = NULL;
    enc_pkt.size = 0;
    av_init_packet(&enc_pkt);
    ret = enc_func(stream_ctx_out[stream_index].enc_ctx, &enc_pkt,
                   filt_frame, got_frame);

    //-----------
    if(cloneframe)
    {
        //if(filtedFrame != NULL)
        //{
        //    av_frame_free(&filtedFrame);
        //}
        //if(filt_frame != NULL) //2018/10/14
        if(filtedFrame == NULL && filt_frame != NULL)
        {
            filtedFrame = av_frame_clone(filt_frame);
        }
    }
    //------------

    av_frame_free(&filt_frame);
    if (ret < 0)
        return ret;
    if (!(*got_frame))
        return 0;

    /* prepare packet for muxing */
    enc_pkt.stream_index = stream_index;
    av_packet_rescale_ts(&enc_pkt,
                         stream_ctx_out[stream_index].enc_ctx->time_base,
                         ofmt_ctx->streams[stream_index]->time_base);

    av_log(NULL, AV_LOG_DEBUG, "Muxing frame\n");
    /* mux encoded frame */
    if(!isTv)
    {
        ret = av_interleaved_write_frame(ofmt_ctx, &enc_pkt);
    }
    av_packet_unref(&enc_pkt);
    return ret;
}

int CameraThreadH264::filter_encode_write_frame(AVFrame *frame, unsigned int stream_index, bool cloneframe)
{
    int ret;
    AVFrame *filt_frame;

    //av_log(NULL, AV_LOG_INFO, "Pushing decoded frame to filters\n");
    /* push the decoded frame into the filtergraph */
    ret = av_buffersrc_add_frame_flags(filter_ctx[stream_index].buffersrc_ctx,
                                       frame, 0);
    if (ret < 0) {
        av_log(NULL, AV_LOG_ERROR, "Error while feeding the filtergraph\n");
        return ret;
    }

    /* pull filtered frames from the filtergraph */
    while (1) {
        filt_frame = av_frame_alloc();
        if (!filt_frame)
        {
            ret = AVERROR(ENOMEM);
            break;
        }
        //av_log(NULL, AV_LOG_INFO, "Pulling filtered frame from filters\n");
        ret = av_buffersink_get_frame(filter_ctx[stream_index].buffersink_ctx,
                                      filt_frame);
        if (ret < 0) {
            /* if no more frames for output - returns AVERROR(EAGAIN)
                 * if flushed and no more frames for output - returns AVERROR_EOF
                 * rewrite retcode to 0 to show it as normal procedure completion
                 */
            if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
                ret = 0;
            av_frame_free(&filt_frame);
            break;
        }

        filt_frame->pict_type = AV_PICTURE_TYPE_NONE;
        ret = encode_write_frame(filt_frame, stream_index, NULL, cloneframe);
        if (ret < 0)
            break;
    }

    return ret;
}

int CameraThreadH264::flush_encoder(unsigned int stream_index)
{
    int ret;
    int got_frame;

    if (!(stream_ctx_out[stream_index].enc_ctx->codec->capabilities &
          AV_CODEC_CAP_DELAY))
        return 0;

    while (1) {
        av_log(NULL, AV_LOG_INFO, "Flushing stream #%u encoder\n", stream_index);
        ret = encode_write_frame(NULL, stream_index, &got_frame);
        if (ret < 0)
            break;
        if (!got_frame)
            return 0;
    }
    return ret;
}

void CameraThreadH264::freeContext()
{
    emitMessage("free context");
    if(cameraPacket.data)
    {
        av_packet_unref(&cameraPacket);
    }

    if((cameraFrame) != NULL)
    {
        av_frame_free(&cameraFrame);
    }

    closeOutPut();

    if(ifmt_ctx != NULL)
    {
        for (unsigned int i = 0; i < ifmt_ctx->nb_streams; i++)
        {
            avcodec_free_context(&stream_ctx[i].dec_ctx);
        }
    }
    if(stream_ctx != NULL)
    {
        av_free(stream_ctx);
        stream_ctx = NULL;
    }
    if(ifmt_ctx != NULL)
    {
        avformat_close_input(&ifmt_ctx);
        ifmt_ctx = NULL;
    }
    emitMessage("free context finish");
}

int CameraThreadH264::caputuer()
{
    int ret;
    cameraFrame = NULL;
    enum AVMediaType type;
    unsigned int stream_index;
    unsigned int i;
    int got_frame;
    int (*dec_func)(AVCodecContext *, AVFrame *, int *, const AVPacket *);

    QTime frameTimer;
    int frametime = 0;
    frameTimer.start();
    int frameindex = 0;

    //int maxFrame = 30 * 30;//30 * 30 * 60;//30min * 30fp/s * 60s
    int currentFrame = 0;
    int maxDuraMS = 60 * 1000 * 60;//

    bool hadwriteheader = false;

    int ignoreEncodingFrameTime = 0;

#ifdef USE_FIX_30FPS
    int64_t eachframetime = 33333;
    int64_t start_time = av_gettime();
    int64_t lasttime = start_time;
    int64_t encodewritetime = 0;
    int64_t sleeptimeus = 5000;
    int64_t now = start_time;
    int64_t now_time = 0;
    int64_t frame_index = 0;
    int64_t duration = 0;
#endif
    int loopindex = 0;
    int maxloop = 5;//5;//5 * 30 min
    QDateTime lastRecTime = QDateTime::currentDateTime();

    av_register_all();
    avfilter_register_all();

    emitMessage("opening camera");
    if ((ret = open_input_file(cameraUrl.toUtf8().data())) < 0)
    {
        emitMessage("open camera failed");
        //closeContext(&frame);
        ret = CANNOT_OPEN_INPUTFILE;
        //return ret;
        goto end;
    }
    //注册超时回调
    ifmt_ctx->interrupt_callback.callback = AVInterruptCallBackFun;
    ifmt_ctx->interrupt_callback.opaque = this;

    emitMessage("open camera success");
    pathHelper.creatNewFileName();
    emitMessage("open optput file");
    hadwriteheader = false;
    if ((ret = open_output_file(pathHelper.getCurrentFileName().toUtf8().data())) < 0)
    {
        emitMessage("open output file failed");
        //closeContext(&frame);
        ret = CANNOT_OPEN_OUTPUT_TEMP;
        //return ret;
        goto end;
    }
    hadwriteheader = true;
    emitMessage("open output file sucess");
    emitMessage("init filters");
    if ((ret = init_filters()) < 0)
    {
        emitMessage("init filters failed");
        //closeContext(&frame);
        //return ret;
        goto end;
    }
    emitMessage("init filters success");

    if(cantainvideo)
    {
        //widthOut = pOutCodecCtx->width;
        //heightOut = pOutCodecCtx->height;
        //pixOut = pOutCodecCtx->pix_fmt;
        emit onStartRecoing(pOutCodecCtx->width, pOutCodecCtx->height, (int)pOutCodecCtx->pix_fmt);
    }
#ifdef USE_OPENGL
    struct SwsContext *img_convert_ctx;
    AVFrame *pFrameYUV = av_frame_alloc();
    uint8_t *out_buffer;
    int numBytes;
    AVPixelFormat pixpmt = AV_PIX_FMT_YUV420P;
    img_convert_ctx = sws_getContext(widthOut, heightOut,
                                     pixOut, widthOut, heightOut,
                                     pixpmt, SWS_BICUBIC, NULL, NULL, NULL);

    numBytes = avpicture_get_size(pixpmt, widthOut, heightOut);

    out_buffer = (uint8_t *) av_malloc(numBytes * sizeof(uint8_t));
    avpicture_fill((AVPicture *) pFrameYUV, out_buffer, pixpmt,
                   widthOut, heightOut);
#endif

    emitMessage("Recording");
    /* read all packets */
    recdura = 0;
#ifdef USE_FIX_30FPS
    eachframetime = 33333;
    start_time = av_gettime();
    lasttime = start_time;
    encodewritetime = 0;
    sleeptimeus = 5000;
    now = start_time;
    now_time = 0;
    frame_index = 0;
    duration = 0;
#endif
    frameTimer.restart();
    while (_isRunning && (recdura >= 0 && recdura < maxDuraMS))//loopindex < maxloop)
    {
        //Fix too fast
#ifdef USE_FIX_30FPS
        encodewritetime = av_gettime() - now;
        sleeptimeus = eachframetime - encodewritetime + 5000;
        if(sleeptimeus > 0 && sleeptimeus < eachframetime)
        {
            av_usleep(sleeptimeus);
        }
#else
#endif
        //recdura = QDateTime::currentDateTime().toMSecsSinceEpoch() - lastRecTime.toMSecsSinceEpoch();
        if(recdura < 0 || recdura > maxDuraMS)
        //if(currentFrame >= maxFrame)
        {
            lastRecTime = QDateTime::currentDateTime();
            if(loopindex > maxloop)
            {
                emitMessage("close camera to reset");
                break;
            }
            loopindex++;
            emitMessage("close output file");
            //break;
            for (i = 0; i < ifmt_ctx->nb_streams; i++)
            {
                /* flush filter */
                if (!filter_ctx[i].filter_graph)
                    continue;
                ret = filter_encode_write_frame(NULL, i);
                if (ret < 0) {
                    av_log(NULL, AV_LOG_ERROR, "Flushing filter failed\n");
                    //closeContext(&frame);
                    //return ret;
                    goto end;
                }

                /* flush encoder */
                ret = flush_encoder(i);
                if (ret < 0)
                {
                    av_log(NULL, AV_LOG_ERROR, "Flushing encoder failed\n");
                    //closeContext(&frame);
                    //return ret;
                    goto end;
                }
            }
            av_write_trailer(ofmt_ctx);
            closeOutPut();
            emitMessage("open optput file");
            pathHelper.creatNewFileName();
            hadwriteheader = false;
            if ((ret = open_output_file(pathHelper.getCurrentFileName().toLocal8Bit().data())) < 0)
            {
                emitMessage("open optput file failed");
                //closeContext(&frame);
                ret = CANNOT_OPEN_OUTPUT_TEMP;
                //return ret;
                goto end;
            }
            hadwriteheader = true;
            emitMessage("open optput file success");
            emitMessage("init filters");
            if ((ret = init_filters()) < 0)
            {
                emitMessage("init filters failed");
                //closeContext(&frame);
                //return ret;
                goto end;
            }
            emitMessage("init filters success");
            currentFrame = 0;

#ifdef USE_FIX_30FPS
            start_time = av_gettime();
            lasttime = start_time;
            encodewritetime = 0;
            now = start_time;
            now_time = 0;
            frame_index = 0;
            duration = 0;
#endif
            emitMessage("Recording");
        }
        currentFrame++;

        if(frameindex > 2000000000)
        {
            frameindex = 0;
        }
        frameindex++;
        lastReadInterrupTime = QDateTime::currentDateTime().toMSecsSinceEpoch();
        if ((ret = av_read_frame(ifmt_ctx, &cameraPacket)) < 0)
            break;

#ifdef USE_FIX_30FPS
        now = av_gettime();
        now_time = now - start_time;
        frame_index = now_time / eachframetime;
        duration = now - lasttime;
        if(duration > 1000)
        {
            cameraPacket.duration = duration;
        }
        else
        {
            cameraPacket.duration = eachframetime;
        }
        lasttime = now;
        cameraPacket.dts = cameraPacket.pts = frame_index;
        //qDebug() << frame_index << packet.duration;
#else
        packet.pts = packet.dts = 0;
#endif

        stream_index = cameraPacket.stream_index;
        type = ifmt_ctx->streams[cameraPacket.stream_index]->codecpar->codec_type;
        av_log(NULL, AV_LOG_DEBUG, "Demuxer gave frame of stream_index %u\n",
               stream_index);

        if (filter_ctx[stream_index].filter_graph) {
            av_log(NULL, AV_LOG_DEBUG, "Going to reencode&filter the frame\n");
            cameraFrame = av_frame_alloc();
            if (!cameraFrame) {
                ret = AVERROR(ENOMEM);
                break;
            }
#ifdef USE_FIX_30FPS
            //av_packet_rescale_ts(&packet,
            //                     ifmt_ctx->streams[stream_index]->time_base,
            //                     stream_ctx[stream_index].dec_ctx->time_base);
            //qDebug() << packet.pts << packet.dts << packet.duration;
#else
            av_packet_rescale_ts(&packet,
                                 ifmt_ctx->streams[stream_index]->time_base,
                                 stream_ctx[stream_index].dec_ctx->time_base);
#endif
            dec_func = (type == AVMEDIA_TYPE_VIDEO) ? avcodec_decode_video2 :
                                                      avcodec_decode_audio4;
            ret = dec_func(stream_ctx[stream_index].dec_ctx, cameraFrame,
                           &got_frame, &cameraPacket);
            if (ret < 0) {
                av_frame_free(&cameraFrame);
                av_log(NULL, AV_LOG_ERROR, "Decoding failed\n");
                break;
            }

            if (got_frame) {
                cameraFrame->pts = cameraFrame->best_effort_timestamp;
                ret = filter_encode_write_frame(cameraFrame, stream_index, true);
                if(ret < 0)
                {
                    if(ignoreEncodingFrameTime < 2)
                    {
                        emitMessage("Waring: encoding frame error, retry now");
                        ignoreEncodingFrameTime++;
                        av_frame_free(&cameraFrame);
                        av_packet_unref(&cameraPacket);
                        ret = 0;
                    }
                    else
                    {
                        emitMessage("Error: cannot encoding frame or write");
                    }
                }

#ifdef USE_OPENGL
                sws_scale(img_convert_ctx,
                          (uint8_t const * const *) frame->data,
                          frame->linesize, 0, heightOut, pFrameYUV->data,
                          pFrameYUV->linesize);

                emit onYUVFrame((unsigned char*)(pFrameYUV->data[0]), 0, 0);
#endif

//                if(filtedFrame != NULL)
//                {
//#ifdef Q_OS_WIN
//#else
//                    if(frameindex % 3 == 0)
//#endif
//                    {
//                        sws_scale(imgConvertCtcRGB, (uint8_t const * const *) filtedFrame->data,
//                                  filtedFrame->linesize, 0, heightOut, pFrameRGB->data,
//                                  pFrameRGB->linesize);

//                        mRGB.data =(uchar*)pFrameRGB->data[0];
//                        cv::cvtColor(mRGB, temp, CV_BGR2RGB);

//                        if(checkMog)
//                        {
//                            isMoving = CameraCollectorThread::Init->findMogBOOL(currentCameraId, mRGB);
//                        }
//                        else
//                        {
//                            isMoving = true;
//                        }

//                        QImage dest((uchar*) temp.data, temp.cols, temp.rows, temp.step, QImage::Format_RGB888);
//                        QImage image(dest);
//                        image.detach();
//                        emit onFrame(image.copy());//QIMAGE 内存问题
//                    }
//                    av_frame_free(&filtedFrame);
//                }
                emit onFrame();
                av_frame_free(&cameraFrame);
                if (ret < 0)
                {
                    //closeContext(&frame);
                    //return ret;
                    goto end;
                }
            } else {
                av_frame_free(&cameraFrame);
            }
        } else {
            /* remux this frame without reencoding */
            av_packet_rescale_ts(&cameraPacket,
                                 ifmt_ctx->streams[stream_index]->time_base,
                                 ofmt_ctx->streams[stream_index]->time_base);

            ret = av_interleaved_write_frame(ofmt_ctx, &cameraPacket);
            if (ret < 0)
            {
                //closeContext(&frame);
                //return ret;
                goto end;
            }
        }
        av_packet_unref(&cameraPacket);
        recdura = QDateTime::currentDateTime().toMSecsSinceEpoch() - lastRecTime.toMSecsSinceEpoch();
#ifdef USE_FIX_30FPS
        encodewritetime = av_gettime() - now;
        sleeptimeus = eachframetime - encodewritetime + 5000;
        if(sleeptimeus > 0 && sleeptimeus < eachframetime)
        {
            av_usleep(sleeptimeus);
        }
        else
        {
            av_usleep(5000);
        }
#else
        av_usleep(5000);
#endif
        if(frameindex % 100 == 0)
        {
            frametime = frameTimer.elapsed();
            frameTimer.restart();
            if(frametime > 0)
            {
                double fps = 1000.0 / (frametime / 100.0);
                QString text = "FPS:" + QString::number(fps);
                emitMessage(text);
            }
        }
    }

    /* flush filters and encoders */
    for (i = 0; i < ifmt_ctx->nb_streams; i++)
    {
        /* flush filter */
        if (!filter_ctx[i].filter_graph)
            continue;
        ret = filter_encode_write_frame(NULL, i);
        if (ret < 0) {
            av_log(NULL, AV_LOG_ERROR, "Flushing filter failed\n");
            //closeContext(&frame);
            //return ret;
            goto end;
        }

        /* flush encoder */
        ret = flush_encoder(i);
        if (ret < 0)
        {
            av_log(NULL, AV_LOG_ERROR, "Flushing encoder failed\n");
            //closeContext(&frame);
            //return ret;
            goto end;
        }
    }

end:
    emitMessage("stop record and free resources");

    if(hadwriteheader)
        av_write_trailer(ofmt_ctx);
#ifdef USE_OPENGL
    sws_freeContext(img_convert_ctx);
#endif

    freeContext();

    if(recdura < 5000)
    {
        return CANNOT_REC_UNKNOWN_ERROR;
    }
    return (ret? 1:0);
}

//void CameraThreadH264::closeContext(AVFrame **frame)
//{
//    if((*frame) != NULL)
//    {
//        av_frame_free(frame);
//    }

//    closeOutPut();

//    if(ifmt_ctx != NULL)
//    {
//        for (int i = 0; i < ifmt_ctx->nb_streams; i++)
//        {
//            avcodec_free_context(&stream_ctx[i].dec_ctx);
//            //if (ofmt_ctx && ofmt_ctx->nb_streams > i && ofmt_ctx->streams[i])
//            //{
//            //    if(stream_ctx_out != NULL &&  stream_ctx_out[i].enc_ctx)
//            //    avcodec_free_context(&stream_ctx_out[i].enc_ctx);
//            //}
//            //if (filter_ctx && filter_ctx[i].filter_graph)
//            //{
//            //    avfilter_graph_free(&filter_ctx[i].filter_graph);
//            //}
//        }
//    }
//    //if(filter_ctx != NULL) av_free(filter_ctx);
//    if(stream_ctx != NULL) av_free(stream_ctx);
//    //if(stream_ctx_out != NULL) av_free(stream_ctx_out);
//    if(ifmt_ctx != NULL) avformat_close_input(&ifmt_ctx);
//    //if (ofmt_ctx && !(ofmt_ctx->oformat->flags & AVFMT_NOFILE))
//    //    avio_closep(&ofmt_ctx->pb);
//    //if(ofmt_ctx != NULL) avformat_free_context(ofmt_ctx);
//}

void CameraThreadH264::closeOutPut()
{
    if(ofmt_ctx && ofmt_ctx->nb_streams)
    {
        for(int i = 0; i < ofmt_ctx->nb_streams; i++)
        {
            if (ofmt_ctx->streams[i])
            {
                if(stream_ctx_out != NULL &&  stream_ctx_out[i].enc_ctx)
                avcodec_free_context(&stream_ctx_out[i].enc_ctx);
            }
            if (filter_ctx && filter_ctx[i].filter_graph)
            {
                avfilter_graph_free(&filter_ctx[i].filter_graph);
            }
        }
    }
    if(filter_ctx != NULL) av_free(filter_ctx);
    if(stream_ctx_out != NULL) av_free(stream_ctx_out);
    if (ofmt_ctx && !(ofmt_ctx->oformat->flags & AVFMT_NOFILE))
        avio_closep(&ofmt_ctx->pb);
    if(ofmt_ctx != NULL) avformat_free_context(ofmt_ctx);
    filter_ctx = NULL;
    stream_ctx_out = NULL;
    ofmt_ctx = NULL;
}

bool CameraThreadH264::getIsTv() const
{
    return isTv;
}

void CameraThreadH264::setIsTv(bool value)
{
    isTv = value;
}

void CameraThreadH264::emitMessage(const QString text)
{
    qDebug() << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss") << text + "@" + cameraName;
    emit onMessage(text + "@" + cameraName);
}
