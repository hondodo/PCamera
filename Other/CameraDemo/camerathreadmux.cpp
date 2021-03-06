#include "camerathreadmux.h"

using namespace std;
using namespace cv;

int CameraThreadMUX::cameraId = 0;
CameraThreadMUX::CameraThreadMUX(QObject *parent) : QThread(parent)
{
    pCodecCtx = NULL;
    filter_ctx = NULL;
    isLocalCamera = false;
    isRawVideo = false;
    _isRunning = false;
    checkMog = true;
    saveOnlyMog = false;

    cameraType = CAMERATYPE_LOCAL;
#ifdef Q_OS_WIN
    //pathHelper.setRootPath("D:/");
    cameraUrl = "video=";
    outputFileNameForTemp = pathHelper.getTempFileName();//"D:/Rec/ShowCamera_" + QDateTime::currentDateTime().toString("hhmmss") + ".avi";
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
    isSaveTurn = false;
    currentCameraId = cameraId;
    cameraId++;
}

void CameraThreadMUX::setStop()
{
    _isRunning = false;
}

void CameraThreadMUX::run()
{
    int code = caputuer();
    _isRunning = false;
    qDebug() << "Thread end at code:" << code;
}

bool CameraThreadMUX::getSaveOnlyMog() const
{
    return saveOnlyMog;
}

void CameraThreadMUX::setSaveOnlyMog(bool value)
{
    saveOnlyMog = value;
}

bool CameraThreadMUX::getCheckMog() const
{
    return checkMog;
}

void CameraThreadMUX::setCheckMog(bool value)
{
    checkMog = value;
}

QString CameraThreadMUX::getFontFile() const
{
    return fontFile;
}

void CameraThreadMUX::setFontFile(const QString &value)
{
    fontFile = value;
}

bool CameraThreadMUX::getFixBrighnessByTime() const
{
    return fixBrighnessByTime;
}

void CameraThreadMUX::setFixBrighnessByTime(bool value)
{
    fixBrighnessByTime = value;
}

bool CameraThreadMUX::getCheckBrighness() const
{
    return checkBrighness;
}

void CameraThreadMUX::setCheckBrighness(bool value)
{
    checkBrighness = value;
}

QString CameraThreadMUX::getCameraName() const
{
    return cameraName;
}

void CameraThreadMUX::setCameraName(const QString &value)
{
    cameraName = value;
    pathHelper.setCameraName(cameraName);
    pathHelper.init();
    outputFileNameForTemp = pathHelper.getTempFileName();
}

CAMERATYPE CameraThreadMUX::getCameraType() const
{
    return cameraType;
}

void CameraThreadMUX::setCameraType(const CAMERATYPE &value)
{
    cameraType = value;
}

QString CameraThreadMUX::getCameraUrl() const
{
    return cameraUrl;
}

void CameraThreadMUX::setCameraUrl(const QString &value)
{
    cameraUrl = value;
}

void CameraThreadMUX::printError(int ret)
{
    av_strerror(ret, buf, 1024);
    return;
}

int CameraThreadMUX::openInputFile(const char *filename)
{
    std::string head = "http";
    std::string input(filename);
    isLocalCamera = !(input.compare(0, head.size(), head) == 0);
    if(isLocalCamera)
    {
        avdevice_register_all();
    }

    int ret;
    unsigned int i;
    ifmt_ctx = NULL;
    AVInputFormat *inputFmt = NULL;
#ifdef Q_OS_WIN
    inputFmt = av_find_input_format("dshow");
#else
    inputFmt = av_find_input_format("video4linux2");
#endif
    AVDictionary *avdic=NULL;
    av_dict_set(&avdic, "framerate", "30", 0);
    av_dict_set(&avdic, "input_format", "mjpeg", 0);
    //av_dict_set(&avdic, "video_size", "1920x1080", 0);
    av_dict_set(&avdic, "video_size", "1280x720", 0);

    if(!isLocalCamera)
    {

        if ((ret = avformat_open_input(&ifmt_ctx,filename, NULL, NULL)) < 0)
        {
            av_log(NULL, AV_LOG_ERROR, "Cannot open input file\n");
            printError(ret);
            return ret;
        }
    }
    else
    {
        if ((ret = avformat_open_input(&ifmt_ctx,filename, inputFmt, &avdic)) < 0)
        {
            av_dict_set(&avdic, "video_size", "1280x720", 0);
            if ((ret = avformat_open_input(&ifmt_ctx,filename, inputFmt, &avdic)) < 0)
            {
                av_log(NULL, AV_LOG_ERROR, "Cannot open input file\n");
                printError(ret);
                return ret;
            }
            else
            {
                qDebug() << "Open camera @ 1280 * 720 @" << cameraName;
            }
        }
        else
        {
            qDebug() << "Open camera @ 1920 * 1080 @" << cameraName;
        }
    }
    if ((ret = avformat_find_stream_info(ifmt_ctx, NULL))< 0) {
        av_log(NULL, AV_LOG_ERROR, "Cannot find stream information\n");
        return ret;
    }
    for (i = 0; i < ifmt_ctx->nb_streams; i++)
    {
        AVStream *stream;
        AVCodecContext *codec_ctx;
        stream =ifmt_ctx->streams[i];
        codec_ctx =stream->codec;
        /* Reencode video & audio and remux subtitles etc. */
        if (codec_ctx->codec_type == AVMEDIA_TYPE_VIDEO
                ||codec_ctx->codec_type == AVMEDIA_TYPE_AUDIO)
        {
            if(codec_ctx->codec_type == AVMEDIA_TYPE_VIDEO)
            {
                pCodecCtx = codec_ctx;
            }
            /* Open decoder */
            ret =avcodec_open2(codec_ctx,
                               avcodec_find_decoder(codec_ctx->codec_id), NULL);
            if (ret < 0) {
                av_log(NULL, AV_LOG_ERROR, "Failed toopen decoder for stream #%u\n", i);
                return ret;
            }
        }
    }
    av_dump_format(ifmt_ctx, 0, filename, 0);
    return 0;
}

AVFormatContext * CameraThreadMUX::openOutputFile(const char *filename, int *ret, AVCodecContext **enc_ctx)
{
    AVStream *out_stream;
    AVStream *in_stream;
    AVCodecContext *dec_ctx;
    AVCodec *encoder;
    unsigned int i;
    AVFormatContext *ofmt_ctx = NULL;
    avformat_alloc_output_context2(&ofmt_ctx, NULL, NULL, filename);
    if (!ofmt_ctx)
    {
        av_log(NULL, AV_LOG_ERROR, "Could not create output context\n");
        *ret = AVERROR_UNKNOWN;
        return ofmt_ctx;
    }
    for (i = 0; i < ifmt_ctx->nb_streams; i++)
    {
        out_stream= avformat_new_stream(ofmt_ctx, NULL);
        if (!out_stream)
        {
            av_log(NULL, AV_LOG_ERROR, "Failed allocating output stream\n");
            *ret = AVERROR_UNKNOWN;
            return ofmt_ctx;
        }
        in_stream = ifmt_ctx->streams[i];
        dec_ctx = in_stream->codec;
        *enc_ctx = out_stream->codec;
        if ((dec_ctx)->codec_type == AVMEDIA_TYPE_VIDEO
                ||(dec_ctx)->codec_type == AVMEDIA_TYPE_AUDIO)
        {
            /* in this example, we choose transcoding to same codec */
#ifdef Q_OS_WIN
            if((dec_ctx)->codec_id == AV_CODEC_ID_MJPEG && isLocalCamera)
            {
                encoder = avcodec_find_encoder((dec_ctx)->codec_id);
            }
            if((dec_ctx)->codec_id == AV_CODEC_ID_RAWVIDEO)
            {
                encoder = avcodec_find_encoder(AV_CODEC_ID_MJPEG);
                isRawVideo = true;
            }
            else
            {
                encoder= avcodec_find_encoder((dec_ctx)->codec_id);
            }
#else
            encoder= avcodec_find_encoder((dec_ctx)->codec_id);
#endif
            /* In this example, we transcode to same properties(picture size,
            * sample rate etc.). These properties can be changed for output
            * streams easily using filters */

            if ((dec_ctx)->codec_type == AVMEDIA_TYPE_VIDEO)
            {
                (*enc_ctx)->height = (dec_ctx)->height;
                (*enc_ctx)->width = (dec_ctx)->width;
                (*enc_ctx)->sample_aspect_ratio = (dec_ctx)->sample_aspect_ratio;
                /* take first format from list of supported formats */
                if(encoder->pix_fmts != NULL)
                {
                    if((dec_ctx)->codec_id == AV_CODEC_ID_MJPEG)
                    {
                        if(isLocalCamera)
                        {
                            (*enc_ctx)->pix_fmt = AV_PIX_FMT_YUVJ422P;
                        }
                        else
                        {
                            (*enc_ctx)->pix_fmt = encoder->pix_fmts[0];
                        }
                    }
                    else if((dec_ctx)->codec_id == AV_CODEC_ID_H264)
                    {
                        (*enc_ctx)->pix_fmt = AV_PIX_FMT_YUV422P;
                    }
                    else
                    {
                        (*enc_ctx)->pix_fmt = encoder->pix_fmts[0];
                    }
                }
                else
                {
                    (*enc_ctx)->pix_fmt = AV_PIX_FMT_YUVJ422P;
                    (*enc_ctx)->codec_tag = 0;
                }
                /* video time_base can be set to whatever is handy andsupported by encoder */
                (*enc_ctx)->time_base = (dec_ctx)->time_base;
                (*enc_ctx)->bit_rate = 25000000;
                /* print output stream information*/
                av_dump_format(ofmt_ctx, 0, filename, 1);
            }
            else
            {
                (*enc_ctx)->sample_rate = (dec_ctx)->sample_rate;
                (*enc_ctx)->channel_layout = (dec_ctx)->channel_layout;
                (*enc_ctx)->channels = av_get_channel_layout_nb_channels((*enc_ctx)->channel_layout);
                /* take first format from list of supported formats */
                (*enc_ctx)->sample_fmt = encoder->sample_fmts[0];
                AVRational time_base={1, (*enc_ctx)->sample_rate};
                (*enc_ctx)->time_base = time_base;
            }
            /* Third parameter can be used to pass settings to encoder*/
            *ret = avcodec_open2((*enc_ctx), encoder, NULL);
            if (*ret < 0)
            {
                av_log(NULL, AV_LOG_ERROR, "Cannot open video encoder for stream #%u\n", i);
                return ofmt_ctx;
            }
        }
        else if((dec_ctx)->codec_type == AVMEDIA_TYPE_UNKNOWN)
        {
            av_log(NULL, AV_LOG_FATAL, "Elementary stream #%d is of unknown type, cannot proceed\n", i);
            *ret = AVERROR_INVALIDDATA;
            return ofmt_ctx;
        }
        else
        {
            /* if this stream must be remuxed */
            *ret =avcodec_copy_context(ofmt_ctx->streams[i]->codec,
                                       ifmt_ctx->streams[i]->codec);
            if (*ret < 0)
            {
                av_log(NULL, AV_LOG_ERROR, "Copying stream context failed\n");
                return ofmt_ctx;
            }
        }
        if (ofmt_ctx->oformat->flags &AVFMT_GLOBALHEADER)
            (*enc_ctx)->flags |= CODEC_FLAG_GLOBAL_HEADER;
    }
    av_dump_format(ofmt_ctx, 0, filename, 1);
    if (!(ofmt_ctx->oformat->flags &AVFMT_NOFILE))
    {
        *ret =avio_open(&ofmt_ctx->pb, filename, AVIO_FLAG_WRITE);
        if (*ret < 0)
        {
            av_log(NULL, AV_LOG_ERROR, "Could notopen output file '%s'", filename);
            return ofmt_ctx;
        }
    }
    /* init muxer, write output file header */
    *ret = avformat_write_header(ofmt_ctx, NULL);
    if (ret < 0)
    {
        av_log(NULL, AV_LOG_ERROR, "Error occurred when openingoutput file\n");
        return ofmt_ctx;
    }
    return ofmt_ctx;
}

int CameraThreadMUX::initFilter(FilteringContext* fctx, AVCodecContext *dec_ctx,
                                AVCodecContext *enc_ctx, const char *filter_spec)
{
    char buf[] = "";
    char args[512];
    int ret = 0;
    const AVFilter*buffersrc = NULL;
    const AVFilter*buffersink = NULL;
    AVFilterContext*buffersrc_ctx = NULL;
    AVFilterContext*buffersink_ctx = NULL;
    AVFilterInOut*outputs = avfilter_inout_alloc();
    AVFilterInOut*inputs  = avfilter_inout_alloc();
    AVFilterGraph*filter_graph = avfilter_graph_alloc();
    if (!outputs || !inputs || !filter_graph)
    {
        ret =AVERROR(ENOMEM);
        goto end;
    }
    if (dec_ctx->codec_type == AVMEDIA_TYPE_VIDEO)
    {
        buffersrc =avfilter_get_by_name("buffer");
        buffersink= avfilter_get_by_name("buffersink");
        if (!buffersrc || !buffersink)
        {
            av_log(NULL, AV_LOG_ERROR, "filtering source or sink element not found\n");
            ret = AVERROR_UNKNOWN;
            goto end;
        }
        snprintf(args, sizeof(args),
                 "video_size=%dx%d:pix_fmt=%d:time_base=%d/%d:pixel_aspect=%d/%d",
                 dec_ctx->width, dec_ctx->height, dec_ctx->pix_fmt,
                 dec_ctx->time_base.num,dec_ctx->time_base.den,
                 dec_ctx->sample_aspect_ratio.num,
                 dec_ctx->sample_aspect_ratio.den);
        ret =avfilter_graph_create_filter(&buffersrc_ctx, buffersrc, "in",
                                          args, NULL, filter_graph);
        if (ret < 0) {
            av_strerror(ret, buf, 1024);
            av_log(NULL, AV_LOG_ERROR, "Cannot create buffer source\n");
            goto end;
        }
        ret =avfilter_graph_create_filter(&buffersink_ctx, buffersink, "out",
                                          NULL, NULL, filter_graph);
        if (ret < 0) {
            av_log(NULL, AV_LOG_ERROR, "Cannot create buffer sink\n");
            goto end;
        }
        ret = av_opt_set_bin(buffersink_ctx, "pix_fmts",
                             (uint8_t*)&enc_ctx->pix_fmt, sizeof(enc_ctx->pix_fmt),
                             AV_OPT_SEARCH_CHILDREN);
        if (ret < 0) {
            av_log(NULL, AV_LOG_ERROR, "Cannot setoutput pixel format\n");
            goto end;
        }
    }
    else if(dec_ctx->codec_type == AVMEDIA_TYPE_AUDIO)
    {
        buffersrc = avfilter_get_by_name("abuffer");
        buffersink= avfilter_get_by_name("abuffersink");
        if (!buffersrc || !buffersink) {
            av_log(NULL, AV_LOG_ERROR, "filtering source or sink element not found\n");
            ret =AVERROR_UNKNOWN;
            goto end;
        }
        if (!dec_ctx->channel_layout)
            dec_ctx->channel_layout =
                    av_get_default_channel_layout(dec_ctx->channels);
        snprintf(args, sizeof(args),
                 "time_base=%d/%d:sample_rate=%d:sample_fmt=%s:channel_layout=0x%I64x",
                 dec_ctx->time_base.num, dec_ctx->time_base.den,dec_ctx->sample_rate,
                 av_get_sample_fmt_name(dec_ctx->sample_fmt),
                 dec_ctx->channel_layout);
        ret = avfilter_graph_create_filter(&buffersrc_ctx, buffersrc, "in",
                                           args, NULL, filter_graph);
        if (ret < 0)
        {
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
        if (ret < 0)
        {
            av_log(NULL, AV_LOG_ERROR, "Cannot setoutput sample format\n");
            goto end;
        }
        ret =av_opt_set_bin(buffersink_ctx, "channel_layouts",
                            (uint8_t*)&enc_ctx->channel_layout,
                            sizeof(enc_ctx->channel_layout),AV_OPT_SEARCH_CHILDREN);
        if (ret < 0)
        {
            av_log(NULL, AV_LOG_ERROR, "Cannot setoutput channel layout\n");
            goto end;
        }
        ret =av_opt_set_bin(buffersink_ctx, "sample_rates",
                            (uint8_t*)&enc_ctx->sample_rate, sizeof(enc_ctx->sample_rate),
                            AV_OPT_SEARCH_CHILDREN);
        if (ret < 0)
        {
            av_log(NULL, AV_LOG_ERROR, "Cannot setoutput sample rate\n");
            goto end;
        }
    }
    else
    {
        ret =AVERROR_UNKNOWN;
        goto end;
    }
    /* Endpoints for the filter graph. */
    outputs->name       =av_strdup("in");
    outputs->filter_ctx = buffersrc_ctx;
    outputs->pad_idx    = 0;
    outputs->next       = NULL;
    inputs->name       = av_strdup("out");
    inputs->filter_ctx = buffersink_ctx;
    inputs->pad_idx    = 0;
    inputs->next       = NULL;
    if (!outputs->name || !inputs->name)
    {
        ret =AVERROR(ENOMEM);
        goto end;
    }
    if ((ret = avfilter_graph_parse_ptr(filter_graph,filter_spec,
                                        &inputs, &outputs, NULL)) < 0)
        goto end;
    if ((ret = avfilter_graph_config(filter_graph, NULL))< 0)
        goto end;
    /* Fill FilteringContext */
    fctx->buffersrc_ctx = buffersrc_ctx;
    fctx->buffersink_ctx = buffersink_ctx;
    fctx->filter_graph= filter_graph;
end:
    avfilter_inout_free(&inputs);
    avfilter_inout_free(&outputs);
    return ret;
}

int CameraThreadMUX::initFilters(AVFormatContext *ofmt_ctx)
{
    const char *filter_spec;
    unsigned int i;
    int ret;
    filter_ctx =(FilteringContext *)av_malloc_array(ifmt_ctx->nb_streams, sizeof(*filter_ctx));
    if (!filter_ctx)
        return AVERROR(ENOMEM);
    for (i = 0; i < ifmt_ctx->nb_streams; i++)
    {
        filter_ctx[i].buffersrc_ctx  =NULL;
        filter_ctx[i].buffersink_ctx= NULL;
        filter_ctx[i].filter_graph   =NULL;
        if(!(ifmt_ctx->streams[i]->codec->codec_type == AVMEDIA_TYPE_AUDIO
             ||ifmt_ctx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO))
            continue;
        if (ifmt_ctx->streams[i]->codec->codec_type== AVMEDIA_TYPE_VIDEO)
        {
            QString filtertext = "[in]drawtext=fontfile=" + fontFile +
                    ":fontcolor=white:fontsize=40:text='%{localtime}':"
                    "x=20:y=20:shadowcolor=black:shadowx=2:shadowy=2[a];"
                    "[a]drawtext=fontfile=" + fontFile +
                    ":fontcolor=white:fontsize=30:text='" + cameraName.replace(":", "\\:") +
                    "':x=20:y=60:shadowcolor=black:shadowx=2:shadowy=2[out]";
            filtertext = "[in]drawtext=fontfile=D\\\\:font.ttf:fontcolor=white:fontsize=40:text='%{localtime}':x=20:y=20:shadowcolor=black:shadowx=2:shadowy=2[a];[a]drawtext=fontfile=D\\\\:font.ttf:fontcolor=white:fontsize=30:text='World Facing Right':x=20:y=60:shadowcolor=black:shadowx=2:shadowy=2[out]";
            qDebug() << filtertext;
#ifdef Q_OS_WIN
            filter_spec = "drawtext=fontfile=D\\\\:font.ttf:fontcolor=white:fontsize=40:text='%{localtime}':x=20:y=20:shadowcolor=black:shadowx=2:shadowy=2";
#else
            filter_spec = "drawtext=fontfile=/home/pi/Font/font.ttf:fontcolor=white:fontsize=40:text='%{localtime}':x=20:y=20:shadowcolor=black:shadowx=2:shadowy=2";//"[in]drawtext=fontfile=/home/pi/Font/font.ttf:fontcolor=black:fontsize=30:text='%{localtime}':x=20:y=20[a];[a]drawtext=fontfile=/home/pi/Font/font.ttf:fontcolor=white:fontsize=30:text='%{localtime}':x=18:y=18[out]"; /* passthrough (dummy) filter for video */
            //filter_spec = "null";
#endif
        }
        else
            filter_spec = "anull"; /* passthrough (dummy) filter for audio */
        ret = initFilter(&filter_ctx[i], ifmt_ctx->streams[i]->codec,
                         ofmt_ctx->streams[i]->codec, filter_spec);
        if (ret)
        {
            printError(ret);
            return ret;
        }
    }
    return 0;
}

int CameraThreadMUX::encodeWriteFrame(AVFrame *filt_frame, unsigned int stream_index, int *got_frame, AVFormatContext *ofmt_ctx)
{
    int ret;
    int got_frame_local;
    AVPacket enc_pkt;
    int (*enc_func)(AVCodecContext *, AVPacket *, const AVFrame *, int*) =
            (ifmt_ctx->streams[stream_index]->codec->codec_type ==
             AVMEDIA_TYPE_VIDEO) ? avcodec_encode_video2 : avcodec_encode_audio2;
    if (!got_frame)
        got_frame =&got_frame_local;
    //av_log(NULL,AV_LOG_INFO, "Encoding frame\n");
    /* encode filtered frame */
    enc_pkt.data =NULL;
    enc_pkt.size =0;
    av_init_packet(&enc_pkt);
    if(isRawVideo)
    {
        AVCodecContext *code = ofmt_ctx->streams[stream_index]->codec;
        ret =enc_func(code, &enc_pkt,
                      filt_frame, got_frame);
    }
    else
    {
        ret =enc_func(ofmt_ctx->streams[stream_index]->codec, &enc_pkt,
                      filt_frame, got_frame);
        if(ret)
        {
            printError(ret);
        }
    }
    //av_frame_free(&filt_frame);
    if (ret < 0)
        return ret;
    if (!(*got_frame))
        return 0;
    /* prepare packet for muxing */
    enc_pkt.stream_index = stream_index;
    enc_pkt.dts =av_rescale_q_rnd(enc_pkt.dts,
                                  ofmt_ctx->streams[stream_index]->codec->time_base,
                                  ofmt_ctx->streams[stream_index]->time_base,
                                  (AVRounding)(AV_ROUND_NEAR_INF|AV_ROUND_PASS_MINMAX));
    enc_pkt.pts =av_rescale_q_rnd(enc_pkt.pts,
                                  ofmt_ctx->streams[stream_index]->codec->time_base,
                                  ofmt_ctx->streams[stream_index]->time_base,
                                  (AVRounding)(AV_ROUND_NEAR_INF|AV_ROUND_PASS_MINMAX));
    enc_pkt.duration = av_rescale_q(enc_pkt.duration,
                                    ofmt_ctx->streams[stream_index]->codec->time_base,
                                    ofmt_ctx->streams[stream_index]->time_base);
    //qDebug() << enc_pkt.dts << enc_pkt.pts << enc_pkt.duration;
    av_log(NULL,AV_LOG_DEBUG, "Muxing frame\n");
    /* mux encoded frame */
    ret = av_interleaved_write_frame(ofmt_ctx, &enc_pkt);
    return ret;
}

int CameraThreadMUX::filterEncodeNoWriteFrame(AVFrame *frame, unsigned int stream_index, AVFormatContext *ofmt_ctx)
{
    int ret;

    AVFrame *filt_frame;
    //av_log(NULL,AV_LOG_INFO, "Pushing decoded frame tofilters\n");
    /* push the decoded frame into the filtergraph */
    ret = av_buffersrc_add_frame_flags(filter_ctx[stream_index].buffersrc_ctx,
                                       frame, 0);
    ret = av_buffersink_get_frame(filter_ctx[stream_index].buffersink_ctx,
                                  frame);
    if (ret < 0)
    {
        printError(ret);
    }
    return ret;
    if (ret < 0)
    {
        printError(ret);
        av_log(NULL, AV_LOG_ERROR, "Error while feeding the filtergraph\n");
        return ret;
    }
    /* pull filtered frames from the filtergraph */
    while (1)
    {
        filt_frame = av_frame_alloc();
        if (!filt_frame)
        {
            ret =AVERROR(ENOMEM);
            break;
        }
        av_log(NULL, AV_LOG_INFO, "Pulling filtered frame from filters\n");
        ret = av_buffersink_get_frame(filter_ctx[stream_index].buffersink_ctx,
                                      filt_frame);
        if (ret < 0)
        {
            /* if nomore frames for output - returns AVERROR(EAGAIN)
            * if flushed and no more frames for output - returns AVERROR_EOF
            * rewrite retcode to 0 to show it as normal procedure completion
            */
            if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
                ret= 0;
            av_frame_free(&filt_frame);
            break;
        }
        filt_frame->pict_type = AV_PICTURE_TYPE_NONE;
        //        ret = encode_write_frame(filt_frame, stream_index, NULL);
        //        if (ret < 0)
        //            break;
        frame = filt_frame;
    }
    return ret;
}

int CameraThreadMUX::filterEncodeWriteFrame(AVFrame *frame, unsigned int stream_index, AVFormatContext *ofmt_ctx)
{
#ifdef Q_OS_WIN
#else
    isSaveTurn = !isSaveTurn;
    if(!isSaveTurn) return 0;
#endif

    int ret;
    ret = encodeWriteFrame(frame, stream_index, NULL, ofmt_ctx);
    return ret;
}

int CameraThreadMUX::flushEncoder(unsigned int stream_index, AVFormatContext *ofmt_ctx)
{
    int ret;
    int got_frame;
    if(!(ofmt_ctx->streams[stream_index]->codec->codec->capabilities&
         AV_CODEC_CAP_DELAY))
        return 0;
    while (1)
    {
        av_log(NULL, AV_LOG_INFO, "Flushing stream #%u encoder\n", stream_index);
        ret =encodeWriteFrame(NULL, stream_index, &got_frame, ofmt_ctx);
        if (ret < 0)
            break;
        if (!got_frame)
            return 0;
    }
    return ret;
}

int CameraThreadMUX::caputuer()
{
    av_register_all();
    avfilter_register_all();
    avcodec_register_all();
    bool filtercorrect = false;
    int frameindex = 0;
    int ret;
    AVFormatContext *ofmt_ctx;
    AVPacket packet;
    AVFrame *frame = NULL;
    enum AVMediaType type;
    unsigned int stream_index;
    unsigned int i;
    int got_frame;
    int (*dec_func)(AVCodecContext *, AVFrame *, int *, const AVPacket*);

    if ((ret = openInputFile(cameraUrl.toLocal8Bit().data())) < 0)
        return 1;

    pCodecCtx->bit_rate = 0;   //初始化为0
    pCodecCtx->time_base.num = 1;  //下面两行：一秒钟25帧
    pCodecCtx->time_base.den = 30;
    pCodecCtx->frame_number = 1;  //每包一个视频帧

    AVCodecContext *enc_ctx = NULL;
    ofmt_ctx = openOutputFile(outputFileNameForTemp.toLocal8Bit().data(), &ret, &enc_ctx);
    if (ret < 0)
        return 1;
    if ((ret = initFilters(ofmt_ctx)) < 0)
    {
        filtercorrect = false;
        qDebug() << "Init filter error:" << buf;
        avformat_close_input(&ifmt_ctx);
        closeOutputFile(&ofmt_ctx, &enc_ctx);
        return 1;
    }
    else
    {
        filtercorrect = true;
    }
    QTime frameControlTimer;
    double framerate = 0.0;
    int frametime = 0;
    frameControlTimer.start();
    struct SwsContext *img_convert_ctx;
    AVFrame *pFrameYUV = av_frame_alloc();
    uint8_t *out_buffer;
    int numBytes;
    if(pCodecCtx != NULL && isRawVideo)
    {
        ///这里我们改成了 将解码后的数据转换
        AVPixelFormat pixpmt = AV_PIX_FMT_YUVJ420P;
        img_convert_ctx = sws_getContext(pCodecCtx->width, pCodecCtx->height,
                                         pCodecCtx->pix_fmt, pCodecCtx->width, pCodecCtx->height,
                                         pixpmt, SWS_BICUBIC, NULL, NULL, NULL);//AV_PIX_FMT_BGR24

        numBytes = avpicture_get_size(pixpmt, pCodecCtx->width, pCodecCtx->height);

        out_buffer = (uint8_t *) av_malloc(numBytes * sizeof(uint8_t));
        avpicture_fill((AVPicture *) pFrameYUV, out_buffer, pixpmt,
                       pCodecCtx->width, pCodecCtx->height);
    }

    AVFrame *pFrameRGB = av_frame_alloc();
    struct SwsContext *imgConvertCtcRGB;
    AVPixelFormat rgbFmt = AV_PIX_FMT_BGR24;
    int rgbBytes;
    uint8_t *rgbOutBuffer;
    cv::Mat mRGB, temp, small;
    //if(pCodecCtxOut != NULL)
    //{
    imgConvertCtcRGB = sws_getContext(pCodecCtx->width, pCodecCtx->height,
                                      pCodecCtx->pix_fmt, pCodecCtx->width, pCodecCtx->height,
                                      rgbFmt, SWS_BICUBIC, NULL, NULL, NULL);
    rgbBytes = avpicture_get_size(rgbFmt, pCodecCtx->width, pCodecCtx->height);
    rgbOutBuffer = (uint8_t *) av_malloc(rgbBytes * sizeof(uint8_t));
    avpicture_fill((AVPicture *)pFrameRGB, rgbOutBuffer, rgbFmt,
                   pCodecCtx->width, pCodecCtx->height);

    mRGB = cv::Mat(cv::Size(pCodecCtx->width, pCodecCtx->height), CV_8UC3);
    std::vector<cv::Rect> mogRect;
    //}

    /* read all packets */
    _isRunning = true;
    bool savefile = false;
    //计算出的偏差值，小于1表示比较正常，大于1表示存在亮度异常；
    //当cast异常时，da大于0表示过亮，da小于0表示过暗
    float brightnessCast = 0, brightnessDA = 0;
    double brightnessA = 2.2, brightnessB = 50;//a 1.0-3.0 b 0-100
    AVFormatContext *ofmt_ctx_same_to_save = NULL;
    AVCodecContext *enc_ctx_same_to_save = NULL;
    bool isNewRecFile = true;
    QDateTime now = QDateTime::currentDateTime();
    QDateTime needRecLastTime = now;
    int minRecMS = 10 * 1000;
    int maxFrames = 30 * 30 * 60;
    int lastSecond = QDateTime::currentDateTime().time().second();
    bool isSameSecond = true;
    QDateTime nextCreatNewFile = now;
    bool isRecBySourceRate = true;

    while(_isRunning)
    {
        if((ret = av_read_frame(ifmt_ctx, &packet)) < 0)
        {
            qDebug() << "No fram, end thread.";
            break;
        }

        if(checkMog && saveOnlyMog)
        {}
        else
        {
            isNewRecFile = QDateTime::currentDateTime().toMSecsSinceEpoch() > nextCreatNewFile.toMSecsSinceEpoch();
        }
        if(isNewRecFile)
        {
            isNewRecFile = false;
            nextCreatNewFile = QDateTime::currentDateTime().addSecs(30 * 60);
            if(ofmt_ctx_same_to_save != NULL || enc_ctx_same_to_save != NULL)
            {
                closeOutputFile(&ofmt_ctx_same_to_save, &enc_ctx_same_to_save);
            }
            ofmt_ctx_same_to_save = NULL;
            frameindex = 0;
            pathHelper.creatNewFileName();
            QString savefilename = pathHelper.getCurrentFileName();//"D:/REC/Camera_" + QDateTime::currentDateTime().toString("hhmmmss") + ".avi";
            ofmt_ctx_same_to_save = openOutputFile(savefilename.toLocal8Bit().data(), &ret, &enc_ctx_same_to_save);
            if (ret < 0)
            {
                if(ofmt_ctx_same_to_save != NULL || enc_ctx_same_to_save != NULL)
                {
                    closeOutputFile(&ofmt_ctx_same_to_save, &enc_ctx_same_to_save);
                }
                ofmt_ctx_same_to_save = NULL;
            }
        }
        stream_index = packet.stream_index;
        type = ifmt_ctx->streams[packet.stream_index]->codec->codec_type;
        av_log(NULL, AV_LOG_DEBUG, "Demuxer gave frame of stream_index %u\n",
               stream_index);
        if (filter_ctx[stream_index].filter_graph)
        {
            av_log(NULL, AV_LOG_DEBUG, "Going to reencode & filter the frame\n");
            frame = av_frame_alloc();
            if (!frame)
            {
                ret = AVERROR(ENOMEM);
                if(ret)
                {
                    qDebug() << ret;
                }
                break;
            }

            packet.dts = packet.pts = frameindex;

            dec_func = (type == AVMEDIA_TYPE_VIDEO) ? avcodec_decode_video2 :
                                                      avcodec_decode_audio4;
            ret = dec_func(ifmt_ctx->streams[stream_index]->codec, frame,
                           &got_frame, &packet);
            if (ret < 0)
            {
                av_frame_free(&frame);
                //av_log(NULL, AV_LOG_ERROR, "Decoding failed\n");
                qDebug() << "Decoding failed";
                break;
            }
            int elsp = QDateTime::currentDateTime().toMSecsSinceEpoch() - needRecLastTime.toMSecsSinceEpoch();
            isSameSecond = QDateTime::currentDateTime().time().second() == lastSecond;
            if(!isSameSecond)
            {
                lastSecond = QDateTime::currentDateTime().time().second();
            }
            isRecBySourceRate = elsp < minRecMS;
            if(checkMog && saveOnlyMog)
            {
                savefile = (elsp < minRecMS) && (frameindex < maxFrames);
            }
            else
            {
                savefile = isRecBySourceRate || (!isSameSecond);
            }
            if (got_frame)
            {
                frametime = frameControlTimer.elapsed();
                frame->pts = av_frame_get_best_effort_timestamp(frame);
                frame->pict_type = AV_PICTURE_TYPE_NONE;
                if(isRawVideo)
                {
                    //filter_encode_no_write_frame(frame, stream_index);
                    sws_scale(img_convert_ctx,
                              (uint8_t const * const *) frame->data,
                              frame->linesize, 0, pCodecCtx->height, pFrameYUV->data,
                              pFrameYUV->linesize);

                    pFrameYUV->best_effort_timestamp = frame->best_effort_timestamp;
                    pFrameYUV->pts = frame->pts;
                    pFrameYUV->pkt_duration = frame->pkt_duration;
                    pFrameYUV->pkt_pts = frame->pkt_pts;
                    pFrameYUV->pkt_dts = frame->pkt_dts;
                    pFrameYUV->pkt_size = frame->pkt_size;
                    if(savefile && ofmt_ctx_same_to_save != NULL)
                    {
                        ret = filterEncodeWriteFrame(pFrameYUV, stream_index, ofmt_ctx_same_to_save);
                        if(ret)
                        {
                            qDebug() << ret;
                        }
                    }
                }
                else
                {
                    if(filtercorrect)
                    {
                        filterEncodeNoWriteFrame(frame, stream_index, ofmt_ctx);
                    }
                    if(savefile && ofmt_ctx_same_to_save != NULL)
                    {
                        ret = filterEncodeWriteFrame(frame, stream_index, ofmt_ctx_same_to_save);
                        if(ret)
                        {
                            qDebug() << ret;
                        }
                    }
                }

                sws_scale(imgConvertCtcRGB, (uint8_t const * const *) frame->data,
                          frame->linesize, 0, pCodecCtx->height, pFrameRGB->data,
                          pFrameRGB->linesize);

                mRGB.data =(uchar*)pFrameRGB->data[0];

                if(checkBrighness)
                {
                    if(fixBrighnessByTime)
                    {
                        QDateTime now = QDateTime::currentDateTime();
                        if(now.time().hour() > 6 && now.time().hour() < 18)
                        {}
                        else
                        {
                            cv::resize(mRGB, small, cv::Size(50, 50));
                            MatHelper::Init->brightnessException(small, brightnessCast, brightnessDA);
                            if(brightnessCast > 1.0)
                            {
                                if(brightnessDA < 0)
                                {
                                    mRGB.convertTo(mRGB, -1, brightnessA, brightnessB);
                                    cv::resize(mRGB, small, cv::Size(150, 100));
                                    MatHelper::Init->brightnessException(small, brightnessCast, brightnessDA);
                                    if(brightnessCast > 1.0)
                                    {
                                        if(brightnessDA < -10)
                                        {
                                            brightnessB += 1;
                                        }
                                        else if(brightnessDA > 10)
                                        {
                                            brightnessB -= 1;
                                        }
                                        if(brightnessB > 100)
                                        {
                                            brightnessB = 100;
                                        }
                                        if(brightnessB < 0)
                                        {
                                            brightnessB = 0;
                                        }
                                        qDebug() << "BRIGHTNESS:" << brightnessB;
                                    }
                                }
                            }
                        }
                    }
                    else
                    {
                        QDateTime now = QDateTime::currentDateTime();
                        if(now.time().hour() > 6 && now.time().hour() < 18)
                        {}
                        else
                        {
                            cv::resize(mRGB, small, cv::Size(50, 50));
                            MatHelper::Init->brightnessException(small, brightnessCast, brightnessDA);
                            if(brightnessCast > 1.0)
                            {
                                if(brightnessDA < 0)
                                {
                                    mRGB.convertTo(mRGB, -1, brightnessA, brightnessB);
                                    cv::resize(mRGB, small, cv::Size(150, 100));
                                    MatHelper::Init->brightnessException(small, brightnessCast, brightnessDA);
                                    if(brightnessCast > 1.0)
                                    {
                                        if(brightnessDA < -10)
                                        {
                                            brightnessB += 1;
                                        }
                                        else if(brightnessDA > 10)
                                        {
                                            brightnessB -= 1;
                                        }
                                        if(brightnessB > 100)
                                        {
                                            brightnessB = 100;
                                        }
                                        if(brightnessB < 0)
                                        {
                                            brightnessB = 0;
                                        }
                                        qDebug() << "BRIGHTNESS:" << brightnessB;
                                    }
                                }
                            }
                        }
                    }
                }

                cv::cvtColor(mRGB, temp, CV_BGR2RGB);

                if(checkMog)
                {
                    mogRect = CameraCollectorThread::Init->findMog(currentCameraId, mRGB);
                    if(!mogRect.empty() && mogRect.size() > 0)
                    {
                        needRecLastTime = QDateTime::currentDateTime();
                        if(saveOnlyMog)
                        {
                            if(!savefile)
                            {
                                isNewRecFile = true;
                            }
                        }
                    }
                    if(!savefile)
                    {
                        if(ofmt_ctx_same_to_save != NULL || enc_ctx_same_to_save != NULL)
                        {
                            closeOutputFile(&ofmt_ctx_same_to_save, &enc_ctx_same_to_save);
                        }
                        ofmt_ctx_same_to_save = NULL;
                        enc_ctx_same_to_save = NULL;
                    }
                }
                else
                {
                    needRecLastTime = QDateTime::currentDateTime();
                }

                QImage dest((uchar*) temp.data, temp.cols, temp.rows, temp.step, QImage::Format_RGB888);
                QImage image(dest);
                image.detach();
                emit onFrame(image.copy());//QIMAGE 内存问题

                frametime = frameControlTimer.elapsed();
                frameControlTimer.restart();
                framerate = 1000.0 / frametime;
                if(frameindex % 100 == 0)
                {
                    qDebug() << "FPS:" << framerate << "@" << cameraName;
                }

                av_frame_free(&frame);
                if (ret< 0)
                {
                    goto end;
                }
            }
            else
            {
                av_frame_free(&frame);
            }
            if(savefile)
            {
                frameindex++;
            }
        }
        if(frame != NULL)
        {
            qDebug() << "**************Frame*********";
            av_frame_free(&frame);
        }
        av_free_packet(&packet);

        if(ofmt_ctx_same_to_save != NULL)
        {
            /* flush filters and encoders */
            for (i = 0; i < ifmt_ctx->nb_streams; i++)
            {
                /* flush filter */
                if (!filter_ctx[i].filter_graph)
                    continue;
                ret = filterEncodeWriteFrame(NULL, i, ofmt_ctx_same_to_save);
                if (ret < 0) {
                    av_log(NULL, AV_LOG_ERROR, "Flushingfilter failed\n");
                    goto end;
                }
                /* flush encoder */
                ret = flushEncoder(i, ofmt_ctx_same_to_save);
                if (ret < 0) {
                    av_log(NULL, AV_LOG_ERROR, "Flushingencoder failed\n");
                    goto end;
                }
            }
        }

        QThread::msleep(5);
    }
    if(ofmt_ctx_same_to_save != NULL)
    {
        /* flush filters and encoders */
        for (i = 0; i < ifmt_ctx->nb_streams; i++)
        {
            /* flush filter */
            if (!filter_ctx[i].filter_graph)
                continue;
            ret = filterEncodeWriteFrame(NULL, i, ofmt_ctx_same_to_save);
            if (ret < 0) {
                av_log(NULL, AV_LOG_ERROR, "Flushingfilter failed\n");
                goto end;
            }
            /* flush encoder */
            ret = flushEncoder(i, ofmt_ctx_same_to_save);
            if (ret < 0) {
                av_log(NULL, AV_LOG_ERROR, "Flushingencoder failed\n");
                goto end;
            }
        }
    }
    av_write_trailer(ofmt_ctx);
    if(ofmt_ctx_same_to_save != NULL)
    {
        av_write_trailer(ofmt_ctx_same_to_save);
    }
end:
    mRGB.release();
    temp.release();
    av_free_packet(&packet);
    av_frame_free(&frame);
    av_frame_free(&pFrameYUV);
    av_frame_free(&pFrameRGB);
    for (i = 0; i < ifmt_ctx->nb_streams; i++)
    {
        avcodec_close(ifmt_ctx->streams[i]->codec);
        if (ofmt_ctx && ofmt_ctx->nb_streams >i && ofmt_ctx->streams[i] &&ofmt_ctx->streams[i]->codec)
        {
            avcodec_close(ofmt_ctx->streams[i]->codec);
        }
        if(filter_ctx && filter_ctx[i].filter_graph)
        {
            avfilter_graph_free(&filter_ctx[i].filter_graph);
        }
    }
    avformat_close_input(&ifmt_ctx);
    closeOutputFile(&ofmt_ctx, &enc_ctx);
    ofmt_ctx = NULL;
    closeOutputFile(&ofmt_ctx_same_to_save, &enc_ctx_same_to_save);
    ofmt_ctx_same_to_save = NULL;
    if (ret < 0)
    {
        //printError(ret);
        av_log(NULL, AV_LOG_ERROR, "Erroro ccurred\n");
    }
    _isRunning = false;
    return (ret? 1:0);
}

void CameraThreadMUX::closeOutputFile(AVFormatContext **ofmt_ctx, AVCodecContext **enc_ctx)
{
    if((*enc_ctx))
    {
        avcodec_close(*enc_ctx);
        *enc_ctx = NULL;
    }

    if ((*ofmt_ctx) && !((*ofmt_ctx)->oformat->flags & AVFMT_NOFILE))
    {
        avio_close((*ofmt_ctx)->pb);
    }
    //avformat_close_input(ofmt_ctx);
    avformat_free_context(*ofmt_ctx);
    *ofmt_ctx = NULL;
}
