#include <stdio.h>
#include <QThread>
#include <QDebug>
#include <QTime>


#define STREAM_DURATION   10.0
#define STREAM_FRAME_RATE 25 /* 25 images/s */
#define STREAM_PIX_FMT    AV_PIX_FMT_YUV420P /* default pix_fmt */
#define SCALE_FLAGS SWS_BICUBIC

extern "C"
{
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libswscale/swscale.h"
#include "libavdevice/avdevice.h"
}

AVFormatContext *i_fmt_ctx;
AVStream *i_video_stream;

AVFormatContext *o_fmt_ctx;
AVStream *o_video_stream;

typedef struct OutputStream {
    AVStream *st;
    AVCodecContext *enc;
    /* pts of the next frame that will be generated */
    int64_t next_pts;
    int samples_count;
    AVFrame *frame;
    AVFrame *tmp_frame;
    float t, tincr, tincr2;
    struct SwsContext *sws_ctx;
    struct SwrContext *swr_ctx;
} OutputStream;

static void log_packet(const AVFormatContext *fmt_ctx, const AVPacket *pkt)
{
//    AVRational *time_base = &fmt_ctx->streams[pkt->stream_index]->time_base;
//    printf("pts:%s pts_time:%s dts:%s dts_time:%s duration:%s duration_time:%s stream_index:%d\n",
//           av_ts2str(pkt->pts), av_ts2timestr(pkt->pts, time_base),
//           av_ts2str(pkt->dts), av_ts2timestr(pkt->dts, time_base),
//           av_ts2str(pkt->duration), av_ts2timestr(pkt->duration, time_base),
//           pkt->stream_index);
}

static int write_frame(AVFormatContext *fmt_ctx, const AVRational *time_base, AVStream *st, AVPacket *pkt)
{
    /* rescale output packet timestamp values from codec to stream timebase */
    av_packet_rescale_ts(pkt, *time_base, st->time_base);
    pkt->stream_index = st->index;

    /* Write the compressed frame to the media file. */
    log_packet(fmt_ctx, pkt);
    return av_interleaved_write_frame(fmt_ctx, pkt);
}

/* Prepare a dummy image. */
static void fill_yuv_image(AVFrame *pict, int frame_index,
                           int width, int height)
{
    int x, y, i;
    i = frame_index;
    /* Y */
    for (y = 0; y < height; y++)
        for (x = 0; x < width; x++)
            pict->data[0][y * pict->linesize[0] + x] = x + y + i * 3;
    /* Cb and Cr */
    for (y = 0; y < height / 2; y++) {
        for (x = 0; x < width / 2; x++) {
            pict->data[1][y * pict->linesize[1] + x] = 128 + y + i * 2;
            pict->data[2][y * pict->linesize[2] + x] = 64 + x + i * 5;
        }
    }
}

static AVFrame *get_video_frame(OutputStream *ost)
{
    AVCodecContext *c = ost->enc;
    /* check if we want to generate more frames */
    AVRational ar;
    ar.den = 1;
    ar.num = 1;
    if (av_compare_ts(ost->next_pts, c->time_base,
                      STREAM_DURATION, ar) >= 0)
        return NULL;
    /* when we pass a frame to the encoder, it may keep a reference to it
     * internally; make sure we do not overwrite it here */
    if (av_frame_make_writable(ost->frame) < 0)
        exit(1);
    if (c->pix_fmt != AV_PIX_FMT_YUV420P) {
        /* as we only generate a YUV420P picture, we must convert it
         * to the codec pixel format if needed */
        if (!ost->sws_ctx) {
            ost->sws_ctx = sws_getContext(c->width, c->height,
                                          AV_PIX_FMT_YUV420P,
                                          c->width, c->height,
                                          c->pix_fmt,
                                          SCALE_FLAGS, NULL, NULL, NULL);
            if (!ost->sws_ctx) {
                fprintf(stderr,
                        "Could not initialize the conversion context\n");
                exit(1);
            }
        }
        fill_yuv_image(ost->tmp_frame, ost->next_pts, c->width, c->height);
        sws_scale(ost->sws_ctx, (const uint8_t * const *) ost->tmp_frame->data,
                  ost->tmp_frame->linesize, 0, c->height, ost->frame->data,
                  ost->frame->linesize);
    } else {
        fill_yuv_image(ost->frame, ost->next_pts, c->width, c->height);
    }
    ost->frame->pts = ost->next_pts++;
    return ost->frame;
}

static int write_video_frame(AVFormatContext *oc, AVCodecContext *c, AVStream *st, AVFrame *frame)
{
    int ret;
    //AVCodecContext *c;
    //AVFrame *frame;
    int got_packet = 0;
    AVPacket pkt = { 0 };

    //c = ost->enc;

    //frame = get_video_frame(ost);

    av_init_packet(&pkt);

    /* encode the image */
    ret = avcodec_encode_video2(c, &pkt, frame, &got_packet);
    if (ret < 0)
    {
        //fprintf(stderr, "Error encoding video frame: %s\n", av_err2str(ret));
        return 1;
    }

    if (got_packet) {
        ret = write_frame(oc, &c->time_base, st, &pkt);
    } else {
        ret = 0;
    }

    if (ret < 0) {
        //fprintf(stderr, "Error while writing video frame: %s\n", av_err2str(ret));
        //exit(1);
        return 1;
    }

    return (frame || got_packet) ? 0 : 1;
}

int main(int argc, char **argv)
{
    AVFormatContext *pFormatCtx;
    AVCodecContext *pCodecCtx;
    AVCodec *pCodec;
    AVFrame *pFrame, *pFrameRGB;
    AVPacket *packet;
    uint8_t *out_buffer;

    struct SwsContext *img_convert_ctx;

    int videoStream, i, numBytes;
    int ret = -1, got_picture;

    avformat_network_init();   //初始化FFmpeg网络模块，2017.8.5---lizhen
    av_register_all();         //初始化FFMPEG  调用了这个才能正常适用编码器和解码器
    avdevice_register_all();

    //Allocate an AVFormatContext.
    pFormatCtx = avformat_alloc_context();

    AVInputFormat *inputFmt = NULL;
#ifdef Q_OS_WIN
    inputFmt = av_find_input_format("dshow");
#else
    inputFmt = av_find_input_format("video4linux2");
#endif

    //2017.8.5---lizhen
    AVDictionary *avdic=NULL;
    char option_key[]="rtsp_transport";
    char option_value[]="tcp";
    av_dict_set(&avdic,option_key,option_value,0);
    //av_dict_set(&avdic, "fflags", "nobuffer", 0);
    av_dict_set(&avdic, "max_delay", "100", 0);
    av_dict_set(&avdic, "framerate", "30", 0);
    av_dict_set(&avdic, "input_format", "mjpeg", 0);
    av_dict_set(&avdic, "video_size", "1280x720", 0);

    //if(cameraType == CAMERATYPE_LOCAL)
    //{
        ret = avformat_open_input(&pFormatCtx, "video=World Facing Right", inputFmt, &avdic);
    //}
    //else if(cameraType == CAMERATYPE_WEB)
    //{
    //    ret = avformat_open_input(&pFormatCtx, cameraUrl.toLocal8Bit().data(), NULL, &avdic);
    //}

#ifdef Q_OS_WIN
        ret = avformat_open_input(&pFormatCtx, "video=World Facing Right", inputFmt, &avdic);
#else
        ret = avformat_open_input(&pFormatCtx, "/dev/video0", inputFmt, &avdic);
#endif

    if (ret != 0)
    {
        printf("can't open the file. \n");
        qDebug() << "can't open the file. " << ret;
        return 1;
    }

    if (avformat_find_stream_info(pFormatCtx, NULL) < 0) {
        printf("Could't find stream infomation.\n");
        return 1;
    }

    videoStream = -1;

    ///循环查找视频中包含的流信息，直到找到视频类型的流
    ///便将其记录下来 保存到videoStream变量中
    ///这里我们现在只处理视频流  音频流先不管他
    for (i = 0; i < pFormatCtx->nb_streams; i++) {
        if (pFormatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO) {
            videoStream = i;
        }
    }

    ///如果videoStream为-1 说明没有找到视频流
    if (videoStream == -1) {
        printf("Didn't find a video stream.\n");
        return 1;
    }

    ///查找解码器
    pCodecCtx = pFormatCtx->streams[videoStream]->codec;
    pCodec = avcodec_find_decoder(pCodecCtx->codec_id);
    //2017.8.9---lizhen
    pCodecCtx->bit_rate = 0;   //初始化为0
    pCodecCtx->time_base.num = 1;  //下面两行：一秒钟25帧
    pCodecCtx->time_base.den = 30;
    pCodecCtx->frame_number = 1;  //每包一个视频帧

    if (pCodec == NULL) {
        printf("Codec not found.\n");
        return 1;
    }

    ///打开解码器
    if (avcodec_open2(pCodecCtx, pCodec, NULL) < 0) {
        printf("Could not open codec.\n");
        return 1;
    }

    pFrame = av_frame_alloc();
    pFrameRGB = av_frame_alloc();

    ///这里我们改成了 将解码后的YUV数据转换成RGB32
    img_convert_ctx = sws_getContext(pCodecCtx->width, pCodecCtx->height,
                                     pCodecCtx->pix_fmt, pCodecCtx->width, pCodecCtx->height,
                                     AV_PIX_FMT_YUV420P, SWS_BICUBIC, NULL, NULL, NULL);//AV_PIX_FMT_BGR24

    numBytes = avpicture_get_size(AV_PIX_FMT_YUV420P, pCodecCtx->width,pCodecCtx->height);

    out_buffer = (uint8_t *) av_malloc(numBytes * sizeof(uint8_t));
    avpicture_fill((AVPicture *) pFrameRGB, out_buffer, AV_PIX_FMT_YUV420P,
                   pCodecCtx->width, pCodecCtx->height);

    int y_size = pCodecCtx->width * pCodecCtx->height;

    packet = (AVPacket *) malloc(sizeof(AVPacket)); //分配一个packet
    av_new_packet(packet, y_size); //分配packet的数据

    QString filename = "";
#ifdef Q_OS_WIN
    filename = "D:/test.flv";
#else
    filename = "/media/pi/Disk0/test.avi";
#endif

    QTime frameTimer;
    frameTimer.start();
    double fps = 0.0;

    AVOutputFormat *fmt;
    AVFormatContext *oc;
    avformat_alloc_output_context2(&oc, NULL, "flv", filename.toLocal8Bit().data());
    if(!oc)
    {
        printf("cannot alloc flv format");
        return 1;
    }
    fmt = oc->oformat;

    OutputStream ost = { 0 };
    AVStream *st;
    AVCodecContext *c;
    st = avformat_new_stream(oc, NULL);
    if(!st)
    {
        printf("cannot alloc stream");
    }

    AVCodec *video_codec = avcodec_find_encoder(fmt->video_codec);
    c = avcodec_alloc_context3(video_codec);
    AVRational ar;
    ar.num = 1;
    ar.den = 25;
    st->id = oc->nb_streams - 1;
    c->codec_id = fmt->video_codec;
    c->bit_rate = 400000;
    c->width = 1280;
    c->height = 720;
    st->time_base.num = 1;
    st->time_base.den = 25;// = ar;
    c->gop_size = 12;
    c->pix_fmt = AV_PIX_FMT_YUV420P;
    ret = avcodec_parameters_from_context(st->codecpar, c);
    if(ret < 0)
    {
        printf("cannot copy the stream parmeters");
        return 1;
    }
    AVDictionary *opt = NULL;
    ret = avio_open(&oc->pb, filename.toLocal8Bit().data(), AVIO_FLAG_READ_WRITE);
    if(ret < 0)
    {
        printf("cannot open output file");
        return 1;
    }
    ret = avformat_write_header(oc, &opt);
    if(ret < 0)
    {
        printf("cannot write header");
        return 1;
    }

    while (1)
    {
        frameTimer.restart();
        if (av_read_frame(pFormatCtx, packet) < 0)
        {
            break; //这里认为视频读取完了
        }
        if (packet->stream_index == videoStream)
        {
            ret = avcodec_decode_video2(pCodecCtx, pFrame, &got_picture, packet);

            if (ret < 0)
            {
                printf("decode error.\n");
                return 1;
            }

            if (got_picture)
            {
                sws_scale(img_convert_ctx,
                          (uint8_t const * const *) pFrame->data,
                          pFrame->linesize, 0, pCodecCtx->height, pFrameRGB->data,
                          pFrameRGB->linesize);

                //av_packet_rescale_ts(packet, c->time_base, st->time_base);
                //packet->stream_index = st->index;
                //av_interleaved_write_frame(oc, packet);
                write_video_frame(oc, c, st, pFrame);
            }
        }
        av_free_packet(packet);
        QThread::msleep(50);
        fps = 1000.0 / frameTimer.elapsed();
        qDebug() << "FPS:" << fps;
    }

    av_write_trailer(oc);
    avio_close(oc->pb);

    av_free(out_buffer);
    av_free(pFrameRGB);
    avcodec_close(pCodecCtx);
    avformat_close_input(&pFormatCtx);

    return 0;
}
