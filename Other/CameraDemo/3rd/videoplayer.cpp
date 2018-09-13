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
    //2017.8.9---lizhen
    #include "libavutil/time.h"
    #include "libavutil/mathematics.h"
}

#include <stdio.h>
#include<iostream>
using namespace std;
VideoPlayer::VideoPlayer()
{

}

VideoPlayer::~VideoPlayer()
{

}

void VideoPlayer::startPlay()
{
    ///调用 QThread 的start函数 将会自动执行下面的run函数 run函数是一个新的线程
    this->start();
}

//Show Dshow Device
void VideoPlayer::show_dshow_device(){
    AVFormatContext *pFormatCtx = avformat_alloc_context();
    AVDictionary* options = NULL;
    av_dict_set(&options,"list_devices","true",0);
    AVInputFormat *iformat = av_find_input_format("dshow");
    printf("========Device Info=============\n");
    avformat_open_input(&pFormatCtx,"video=dummy",iformat,&options);
    printf("================================\n");
}

//Show Dshow Device Option
void VideoPlayer::show_dshow_device_option(){
    AVFormatContext *pFormatCtx = avformat_alloc_context();
    AVDictionary* options = NULL;
    av_dict_set(&options,"list_options","true",0);
    AVInputFormat *iformat = av_find_input_format("dshow");
    printf("========Device Option Info======\n");
    avformat_open_input(&pFormatCtx,"video=Integrated Camera",iformat,&options);
    printf("================================\n");
}

//Show VFW Device
void VideoPlayer::show_vfw_device(){
    AVFormatContext *pFormatCtx = avformat_alloc_context();
    AVInputFormat *iformat = av_find_input_format("vfwcap");
    printf("========VFW Device Info======\n");
    avformat_open_input(&pFormatCtx,"list",iformat,NULL);
    printf("=============================\n");
}

//Show AVFoundation Device
void VideoPlayer::show_avfoundation_device(){
    AVFormatContext *pFormatCtx = avformat_alloc_context();
    AVDictionary* options = NULL;
    av_dict_set(&options,"list_devices","true",0);
    AVInputFormat *iformat = av_find_input_format("avfoundation");
    printf("==AVFoundation Device Info===\n");
    avformat_open_input(&pFormatCtx,"",iformat,&options);
    printf("=============================\n");
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

    static struct SwsContext *img_convert_ctx;

    int videoStream, i, numBytes;
    int ret, got_picture;

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
    char option_key2[]="max_delay";
    char option_value2[]="100";
    av_dict_set(&avdic,option_key2,option_value2,0);

#ifdef Q_OS_WIN
    char url[]="video=World Facing Right";
    //"http://admin:12345@192.168.31.55:8081";//"rtsp://admin:admin@192.168.1.18:554/h264/ch1/main/av_stream";
    //ret = avformat_open_input(&pFormatCtx, url, NULL, &avdic);
    ret = avformat_open_input(&pFormatCtx, url, inputFmt, &avdic);
    if (ret != 0)
    {
        printf("can't open the file. \n");
        return;
    }
#else
    ret = avformat_open_input(&pFormatCtx, "/dev/video0", inputFmt, &avdic);
    if (ret != 0)
    {
        printf("can't open the file. \n");
        return;
    }
#endif

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

    //2017.8.7---lizhen
    //cout<<pCodecCtx->width<<endl;

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

    //2017.8.1---lizhen
    //av_dump_format(pFormatCtx, 0, url, 0); //输出视频信息

    QTime frameTimer, otherTimer;
    frameTimer.start();
    otherTimer.start();
    int readtime = 0, showtime = 0;

    while (1)
    {
        if (av_read_frame(pFormatCtx, packet) < 0)
        {
            break; //这里认为视频读取完了
        }
        frameTimer.restart();
        otherTimer.restart();

        if (packet->stream_index == videoStream) {
            ret = avcodec_decode_video2(pCodecCtx, pFrame, &got_picture,packet);

            if (ret < 0) {
                printf("decode error.\n");
                return;
            }

            if (got_picture) {
                sws_scale(img_convert_ctx,
                        (uint8_t const * const *) pFrame->data,
                        pFrame->linesize, 0, pCodecCtx->height, pFrameRGB->data,
                        pFrameRGB->linesize);
                readtime = otherTimer.elapsed();
                otherTimer.restart();
                //把这个RGB数据 用QImage加载
                QImage tmpImg((uchar *)out_buffer,pCodecCtx->width,pCodecCtx->height,QImage::Format_RGB32);
                QImage image = tmpImg.copy(); //把图像复制一份 传递给界面显示
                image = image.scaled(1024, 720);
                emit sig_GetOneFrame(image);  //发送信号
                showtime = otherTimer.elapsed();
                double frame = 1000.0 / frameTimer.elapsed();
                QString text = QString("%1FPS, R:%2, S:%3").arg(
                            QString::number(frame, 'f', 2),
                            QString::number(readtime, 'f', 0),
                            QString::number(showtime, 'f', 0)
                            );
                emit onMessage(text);
            }
        }
        av_free_packet(packet); //释放资源,否则内存会一直上升

        //2017.8.7---lizhen
        msleep(5); //停一停  不然放的太快了

        //2017.8.9---lizhen
        /*int64_t start_time=av_gettime();
        AVRational time_base=pFormatCtx->streams[videoStream]->time_base;
        AVRational time_base_q={1,AV_TIME_BASE};
        int64_t pts_time = av_rescale_q(packet->dts, time_base, time_base_q);
        int64_t now_time = av_gettime() - start_time;
        if (pts_time > now_time)
             av_usleep(pts_time - now_time);*/
    }
    av_free(out_buffer);
    av_free(pFrameRGB);
    avcodec_close(pCodecCtx);
    avformat_close_input(&pFormatCtx);
}
