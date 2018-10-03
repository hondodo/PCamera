//#include "zhibo.h"

//ZhiBo::ZhiBo()
//{

//}
///**
// * 最简单的基于FFmpeg的推流器-安卓
// * Simplest FFmpeg Android Streamer
// *
// * 雷霄骅 Lei Xiaohua
// * leixiaohua1020@126.com
// * 中国传媒大学/数字电视技术
// * Communication University of China / Digital TV Technology
// * http://blog.csdn.net/leixiaohua1020
// *
// * 本程序是安卓平台下最简单的基于FFmpeg的流媒体推送器。
// * 它可以将视频数据以流媒体的形式发送出去。
// *
// * This software is the simplest streamer based on FFmpeg in Android.
// * It can stream local media file to streaming media server (in RTMP).
// */
//#define AV_CODEC_FLAG_GLOBAL_HEADER (1 << 22)
//#define CODEC_FLAG_GLOBAL_HEADER AV_CODEC_FLAG_GLOBAL_HEADER
//#define AVFMT_RAWPICTURE 0x0020

//#include <stdio.h>
//#include <time.h>

////extern "C"
////{
//#include "libavcodec/avcodec.h"
//#include "libavformat/avformat.h"
//#include "libavutil/log.h"
//#include "libavdevice/avdevice.h"
////}
//#ifdef ANDROID
//#include <jni.h>
//#include <android/log.h>
//#define LOGE(format, ...)  __android_log_print(ANDROID_LOG_ERROR, "(>_<)", format, ##__VA_ARGS__)
//#define LOGI(format, ...)  __android_log_print(ANDROID_LOG_INFO,  "(^_^)", format, ##__VA_ARGS__)
//#else
//#define LOGE(format, ...)  printf("(>_<) " format "\n", ##__VA_ARGS__)
//#define LOGI(format, ...)  printf("(^_^) " format "\n", ##__VA_ARGS__)
//#endif


////Output FFmpeg's av_log()
//void custom_log(void *ptr, int level, const char* fmt, va_list vl){

//    //To TXT file
//    FILE *fp=fopen("/storage/emulated/0/av_log.txt","a+");
//    if(fp){
//        vfprintf(fp,fmt,vl);
//        fflush(fp);
//        fclose(fp);
//    }

//    //To Logcat
//    //LOGE(fmt, vl);
//}

//int main()
//{
//    AVOutputFormat *ofmt = NULL;
//    AVFormatContext *ifmt_ctx = NULL, *ofmt_ctx = NULL;
//    AVPacket pkt;

//    int ret, i;
//    char input_str[]="D:/1.mkv";//"video=EasyCamera";//"D:/1.mkv";
//    char output_str[]="rtmp://localhost:1935";//"D:/2.flv";//"rtmp://localhost/publishlive/livestream";
//    char info[1000]={0};
//    //sprintf(input_str,"%s",(*env)->GetStringUTFChars(env,input_jstr, NULL));
//    //sprintf(output_str,"%s",(*env)->GetStringUTFChars(env,output_jstr, NULL));

//    //input_str  = "cuc_ieschool.flv";
//    //output_str = "rtmp://localhost/publishlive/livestream";
//    //output_str = "rtp://233.233.233.233:6666";

//    //FFmpeg av_log() callback
//    av_log_set_callback(custom_log);

//    av_register_all();
//    //Network
//    avformat_network_init();

//    avcodec_register_all();
//    avdevice_register_all();

//    AVInputFormat *inputFmt = NULL;
//    inputFmt = av_find_input_format("dshow");
//    AVDictionary *avdic=NULL;
//    char option_key[]="rtsp_transport";
//    char option_value[]="tcp";
//    av_dict_set(&avdic,option_key,option_value,0);
//    av_dict_set(&avdic, "max_delay", "100", 0);
//    av_dict_set(&avdic, "framerate", "30", 0);
//    av_dict_set(&avdic, "input_format", "mjpeg", 0);
//    av_dict_set(&avdic, "video_size", "1280x720", 0);

//    //Input
//    if ((ret = avformat_open_input(&ifmt_ctx, input_str, NULL, NULL)) < 0) {
//        LOGE( "Could not open input file.");
//        goto end;
//    }
//    if ((ret = avformat_find_stream_info(ifmt_ctx, 0)) < 0) {
//        LOGE( "Failed to retrieve input stream information");
//        goto end;
//    }

//    int videoindex=-1;
//    for(i=0; i<ifmt_ctx->nb_streams; i++)
//        if(ifmt_ctx->streams[i]->codec->codec_type==AVMEDIA_TYPE_VIDEO){
//            videoindex=i;
//            break;
//        }
//    //Output
//    avformat_alloc_output_context2(&ofmt_ctx, NULL, "flv",output_str); //RTMP
//    //avformat_alloc_output_context2(&ofmt_ctx, NULL, "mpegts", output_str);//UDP

//    if (!ofmt_ctx) {
//        LOGE( "Could not create output context\n");
//        ret = AVERROR_UNKNOWN;
//        goto end;
//    }
//    ofmt = ofmt_ctx->oformat;
//    for (i = 0; i < ifmt_ctx->nb_streams; i++) {
//        //Create output AVStream according to input AVStream
//        AVStream *in_stream = ifmt_ctx->streams[i];
//        AVStream *out_stream = avformat_new_stream(ofmt_ctx, in_stream->codec->codec);
//        if (!out_stream) {
//            LOGE( "Failed allocating output stream\n");
//            ret = AVERROR_UNKNOWN;
//            goto end;
//        }
//        //Copy the settings of AVCodecContext
//        ret = avcodec_copy_context(out_stream->codec, in_stream->codec);
//        if (ret < 0) {
//            LOGE( "Failed to copy context from input to output stream codec context\n");
//            goto end;
//        }
//        out_stream->codec->codec_tag = 0;
//        if (ofmt_ctx->oformat->flags & AVFMT_GLOBALHEADER)
//            out_stream->codec->flags |= CODEC_FLAG_GLOBAL_HEADER;
//    }

//    //Open output URL
//    if (!(ofmt->flags & AVFMT_NOFILE)) {
//        ret = avio_open(&ofmt_ctx->pb, output_str, AVIO_FLAG_WRITE);
//        if (ret < 0) {
//            LOGE( "Could not open output URL '%s'", output_str);
//            goto end;
//        }
//    }
//    //Write file header
//    ret = avformat_write_header(ofmt_ctx, NULL);
//    if (ret < 0) {
//        LOGE( "Error occurred when opening output URL\n");
//        char buf[] = "";
//        av_strerror(ret, buf, 1024);
//        goto end;
//    }

//    int frame_index=0;

//    int64_t start_time=av_gettime();
//    while (1) {
//        AVStream *in_stream, *out_stream;
//        //Get an AVPacket
//        ret = av_read_frame(ifmt_ctx, &pkt);
//        if (ret < 0)
//            break;
//        //FIX：No PTS (Example: Raw H.264)
//        //Simple Write PTS
//        if(pkt.pts==AV_NOPTS_VALUE){
//            //Write PTS
//            AVRational time_base1=ifmt_ctx->streams[videoindex]->time_base;
//            //Duration between 2 frames (us)
//            int64_t calc_duration=(double)AV_TIME_BASE/av_q2d(ifmt_ctx->streams[videoindex]->r_frame_rate);
//            //Parameters
//            pkt.pts=(double)(frame_index*calc_duration)/(double)(av_q2d(time_base1)*AV_TIME_BASE);
//            pkt.dts=pkt.pts;
//            pkt.duration=(double)calc_duration/(double)(av_q2d(time_base1)*AV_TIME_BASE);
//        }
//        //Important:Delay
//        if(pkt.stream_index==videoindex){
//            AVRational time_base=ifmt_ctx->streams[videoindex]->time_base;
//            AVRational time_base_q={1,AV_TIME_BASE};
//            int64_t pts_time = av_rescale_q(pkt.dts, time_base, time_base_q);
//            int64_t now_time = av_gettime() - start_time;
//            if (pts_time > now_time)
//                av_usleep(pts_time - now_time);

//        }

//        in_stream  = ifmt_ctx->streams[pkt.stream_index];
//        out_stream = ofmt_ctx->streams[pkt.stream_index];
//        /* copy packet */
//        //Convert PTS/DTS
//        pkt.pts = av_rescale_q_rnd(pkt.pts, in_stream->time_base, out_stream->time_base, AV_ROUND_NEAR_INF|AV_ROUND_PASS_MINMAX);
//        pkt.dts = av_rescale_q_rnd(pkt.dts, in_stream->time_base, out_stream->time_base, AV_ROUND_NEAR_INF|AV_ROUND_PASS_MINMAX);
//        pkt.duration = av_rescale_q(pkt.duration, in_stream->time_base, out_stream->time_base);
//        pkt.pos = -1;
//        //Print to Screen
//        if(pkt.stream_index==videoindex){
//            LOGE("Send %8d video frames to output URL\n",frame_index);
//            frame_index++;
//        }
//        //ret = av_write_frame(ofmt_ctx, &pkt);
//        ret = av_interleaved_write_frame(ofmt_ctx, &pkt);

//        if (ret < 0) {
//            LOGE( "Error muxing packet\n");
//            break;
//        }
//        av_free_packet(&pkt);

//    }
//    //Write file trailer
//    av_write_trailer(ofmt_ctx);
//end:
//    avformat_close_input(&ifmt_ctx);
//    /* close output */
//    if (ofmt_ctx && !(ofmt->flags & AVFMT_NOFILE))
//        avio_close(ofmt_ctx->pb);
//    avformat_free_context(ofmt_ctx);
//    if (ret < 0 && ret != AVERROR_EOF) {
//        LOGE( "Error occurred.\n");
//        return -1;
//    }
//    return 0;
//}
