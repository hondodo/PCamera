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
    checkBrighness = false;
    fixBrighnessByTime = false;
    cameraName = "";
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
    //av_dict_set(&avdic, "fflags", "nobuffer", 0);
    av_dict_set(&avdic, "max_delay", "100", 0);
    av_dict_set(&avdic, "framerate", "30", 0);
    av_dict_set(&avdic, "input_format", "mjpeg", 0);
    av_dict_set(&avdic, "video_size", "1280x720", 0);

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
        qDebug() << "can't open the file. " << ret;
        return;
    }

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
    pCodecCtx->bit_rate = 0;   //初始化为0
    pCodecCtx->time_base.num = 1;  //下面两行：一秒钟25帧
    pCodecCtx->time_base.den = 30;
    pCodecCtx->frame_number = 1;  //每包一个视频帧

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
                                     AV_PIX_FMT_BGR24, SWS_BICUBIC, NULL, NULL, NULL);

    numBytes = avpicture_get_size(AV_PIX_FMT_BGR24, pCodecCtx->width,pCodecCtx->height);

    out_buffer = (uint8_t *) av_malloc(numBytes * sizeof(uint8_t));
    avpicture_fill((AVPicture *) pFrameRGB, out_buffer, AV_PIX_FMT_BGR24,
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

    QString filename = "";
#ifdef Q_OS_WIN
    filename = "D:/test.avi";
#else
    filename = "/media/pi/Potatokid/test.avi";
#endif
    cv::Mat mRGB(cv::Size(pCodecCtx->width, pCodecCtx->height), CV_8UC3);
    cv::Mat temp, small;

    //计算出的偏差值，小于1表示比较正常，大于1表示存在亮度异常；
    //当cast异常时，da大于0表示过亮，da小于0表示过暗
    float brightnessCast = 0, brightnessDA = 0;
    double brightnessA = 2.2, brightnessB = 50;//a 1.0-3.0 b 0-100

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
            ret = avcodec_decode_video2(pCodecCtx, pFrame, &got_picture, packet);

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
                //QImage tmpImg((uchar *)out_buffer,pCodecCtx->width,pCodecCtx->height,QImage::Format_RGB32);
                //QImage image = tmpImg.copy();

                //cv::Mat tmp(image.height(),image.width(),CV_8UC3,(uchar*)image.bits(), image.bytesPerLine());
                //cv::cvtColor(tmp, tmp,CV_BGR2RGB);

                //cv::Mat mRGB(cv::Size(pCodecCtx->width, pCodecCtx->height), CV_8UC3);
                mRGB.data =(uchar*)pFrameRGB->data[0];

                //VideoFileThread::Init->append(filename, mRGB);

                if(checkBrighness)
                {
                    cv::resize(mRGB, small, cv::Size(100, 50));
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
                else
                {
                    if(fixBrighnessByTime)
                    {
                        QDateTime now = QDateTime::currentDateTime();
                        if(now.time().hour() > 6 && now.time().hour() < 18)
                        {}
                        else
                        {
                            mRGB.convertTo(mRGB, -1, 2.2, 50);
                        }
                    }
                }

                cv::cvtColor(mRGB, temp,CV_BGR2RGB);
                QImage dest((uchar*) temp.data, temp.cols, temp.rows, temp.step, QImage::Format_RGB888);
                QImage image(dest);
                image.detach();

                dealtime = othertime.elapsed();
                othertime.restart();
                emit onFrame(image);
                showtime = othertime.elapsed();
            }
        }
        av_free_packet(packet);
        int els = frametime.elapsed();
        int sleep = 1000.0 / 15 - els;
        if(sleep > 0)
        {
            msleep(sleep);
        }
        frame = 1000.0 / frametime.elapsed();
        qDebug() << "FPS:" << frame << "Read:" << readtime << "Deal:" << dealtime << "Show:" << showtime;
    }
    mRGB.release();
    temp.release();
    av_free(out_buffer);
    av_free(pFrameRGB);
    avcodec_close(pCodecCtx);
    avformat_close_input(&pFormatCtx);
}

QString VideoPlayer::getCameraName() const
{
    return cameraName;
}

void VideoPlayer::setCameraName(const QString &value)
{
    cameraName = value;
}

bool VideoPlayer::getFixBrighnessByTime() const
{
    return fixBrighnessByTime;
}

void VideoPlayer::setFixBrighnessByTime(bool value)
{
    fixBrighnessByTime = value;
}

bool VideoPlayer::getCheckBrighness() const
{
    return checkBrighness;
}

void VideoPlayer::setCheckBrighness(bool value)
{
    checkBrighness = value;
}
