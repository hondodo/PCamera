/**
 * 李震
 * 我的码云：https://git.oschina.net/git-lizhen
 * 我的CSDN博客：http://blog.csdn.net/weixin_38215395
 * 联系：QQ1039953685
 */

#include "videoplayer.h"

extern "C"
{
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libavutil/pixfmt.h"
#include "libswscale/swscale.h"
#include "libavutil/time.h"
#include "libavutil/mathematics.h"
}

#include <stdio.h>
#include<iostream>
using namespace std;
VideoPlayer::VideoPlayer()
{
    cameraType = CAMERATYPE_LOCAL;
#ifdef Q_OS_WIN
    cameraUrl = "video=";
#else
    cameraUrl = "/dev/video0";
#endif
    _isRunning = false;
}

VideoPlayer::~VideoPlayer()
{
    setStop();
}

void VideoPlayer::setStop()
{
    _isRunning = false;
}

CAMERATYPE VideoPlayer::getCameraType() const
{
    return cameraType;
}

void VideoPlayer::setCameraType(const CAMERATYPE &value)
{
    cameraType = value;
}

QString VideoPlayer::getCameraUrl() const
{
    return cameraUrl;
}

void VideoPlayer::setCameraUrl(const QString &value)
{
    cameraUrl = value;
}

void VideoPlayer::run()
{
    //char *file_path = mFileName.toUtf8().data();
    //cout<<file_path<<endl;
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
    //    char option_key2[]="max_delay";
    //    char option_value2[]="100";
    //    av_dict_set(&avdic,option_key2,option_value2,0);
    av_dict_set(&avdic, "fflags", "nobuffer", 0);
    av_dict_set(&avdic, "max_delay", "100000", 0);
    av_dict_set(&avdic, "framerate", "30", 0);
    av_dict_set(&avdic, "input_format", "mjpeg", 0);
    av_dict_set(&avdic, "video_size", "1280x720", 0);

    //#ifdef Q_OS_WIN
    //    QList<QByteArray> all = QCamera::availableDevices();
    //    QString desc = QCamera::deviceDescription(all.at(0));
    //    desc = "video=" + desc;
    //char url[]="video=World Facing Right";
    //"http://admin:12345@192.168.31.87:8081";//"rtsp://admin:admin@192.168.1.18:554/h264/ch1/main/av_stream";
    //ret = avformat_open_input(&pFormatCtx, url, NULL, &avdic);
    if(cameraType == CAMERATYPE_LOCAL)
    {
        ret = avformat_open_input(&pFormatCtx, cameraUrl.toLocal8Bit().data(), inputFmt, &avdic);
    }
    else if(cameraType == CAMERATYPE_WEB)
    {
        ret = avformat_open_input(&pFormatCtx, cameraUrl.toLocal8Bit().data(), NULL, &avdic);
    }

    if (ret != 0)
    {
        printf("can't open the file. \n");
        return;
    }
    //#else
    //    ret = avformat_open_input(&pFormatCtx, "/dev/video0", inputFmt, &avdic);
    //    if (ret != 0)
    //    {
    //        printf("can't open the file. \n");
    //        return;
    //    }
    //#endif

    if (avformat_find_stream_info(pFormatCtx, NULL) < 0) {
        printf("Could't find stream infomation.\n");
        return;
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
        return;
    }

    ///查找解码器
    pCodecCtx = pFormatCtx->streams[videoStream]->codec;
    pCodec = avcodec_find_decoder(pCodecCtx->codec_id);
    //2017.8.9---lizhen
    pCodecCtx->bit_rate =0;   //初始化为0
    pCodecCtx->time_base.num=1;  //下面两行：一秒钟25帧
    pCodecCtx->time_base.den=10;
    pCodecCtx->frame_number=1;  //每包一个视频帧

    if (pCodec == NULL) {
        printf("Codec not found.\n");
        return;
    }

    ///打开解码器
    if (avcodec_open2(pCodecCtx, pCodec, NULL) < 0) {
        printf("Could not open codec.\n");
        return;
    }

    pFrame = av_frame_alloc();
    pFrameRGB = av_frame_alloc();

    ///这里我们改成了 将解码后的YUV数据转换成RGB32
    img_convert_ctx = sws_getContext(pCodecCtx->width, pCodecCtx->height,
                                     pCodecCtx->pix_fmt, pCodecCtx->width, pCodecCtx->height,
                                     AV_PIX_FMT_RGB32, SWS_BICUBIC, NULL, NULL, NULL);

    numBytes = avpicture_get_size(AV_PIX_FMT_RGB32, pCodecCtx->width,pCodecCtx->height);

    out_buffer = (uint8_t *) av_malloc(numBytes * sizeof(uint8_t));
    avpicture_fill((AVPicture *) pFrameRGB, out_buffer, AV_PIX_FMT_RGB32,
                   pCodecCtx->width, pCodecCtx->height);

    int y_size = pCodecCtx->width * pCodecCtx->height;

    packet = (AVPacket *) malloc(sizeof(AVPacket)); //分配一个packet
    av_new_packet(packet, y_size); //分配packet的数据

    _isRunning = true;
    QTime frametime, othertime;
    double  frame = 0.0;
    int readtime = 0, dealtime = 0, showtime = 0;
    frametime.start();
    othertime.start();
    while (_isRunning)
    {
        if (av_read_frame(pFormatCtx, packet) < 0)
        {
            break; //这里认为视频读取完了
        }
        frametime.restart();
        if (packet->stream_index == videoStream)
        {

            othertime.restart();
            ret = avcodec_decode_video2(pCodecCtx, pFrame, &got_picture,packet);

            if (ret < 0)
            {
                printf("decode error.\n");
                return;
            }

            if (got_picture)
            {
                readtime = othertime.elapsed();
                othertime.restart();
                sws_scale(img_convert_ctx,
                          (uint8_t const * const *) pFrame->data,
                          pFrame->linesize, 0, pCodecCtx->height, pFrameRGB->data,
                          pFrameRGB->linesize);

                //把这个RGB数据 用QImage加载
                QImage tmpImg((uchar *)out_buffer,pCodecCtx->width,pCodecCtx->height,QImage::Format_RGB32);
                QImage image = tmpImg.copy();
                dealtime = othertime.elapsed();
                othertime.restart();
                emit onFrame(image);
                showtime = othertime.elapsed();
            }
        }
        av_free_packet(packet);
        int els = frametime.elapsed();
        int sleep = 1000.0 / 30 - els;
        if(sleep > 0)
        {
            msleep(sleep);
        }
        frame = 1000.0 / frametime.elapsed();
        qDebug() << "FPS:" << frame << "Read:" << readtime << "Deal:" << dealtime << "Show:" << showtime;
    }
    av_free(out_buffer);
    av_free(pFrameRGB);
    avcodec_close(pCodecCtx);
    avformat_close_input(&pFormatCtx);
}
