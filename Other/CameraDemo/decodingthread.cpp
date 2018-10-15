#include "decodingthread.h"

DecodingThread::DecodingThread(QObject *parent) : QThread(parent)
{
    _isRunnig = false;

    widthOut = 100;
    heightOut = 100;
    pixOut = AV_PIX_FMT_YUV420P;
    pFrameRGB = NULL;
    imgConvertCtcRGB = NULL;
    rgbFmt = AV_PIX_FMT_BGR24;
    rgbBytes = 0;
    rgbOutBuffer = NULL;
    hasInitDecodingRecs = false;
    filtedFrame = NULL;
}

void DecodingThread::setStop()
{
    _isRunnig = false;
    resetDecodingRecs();
}

void DecodingThread::onStartRecoing(int width, int height, int pixOut)
{
    if(!hasInitDecodingRecs)
    {
        widthOut = width;
        heightOut = height;
        this->pixOut = (AVPixelFormat)pixOut;
        initDecodingRecs();
    }
}

void DecodingThread::run()
{
    _isRunnig = true;
    while (_isRunnig)
    {
        if(filtedFrame != NULL)
        {
            if(hasInitDecodingRecs)
            {
                decodeFrameAndShow(filtedFrame);
            }
            av_frame_free(&filtedFrame);
        }
        this->msleep(30);
    }
}

void DecodingThread::setFiltedFrame(const AVFrame *value)
{
    if(filtedFrame == NULL)
    {
        filtedFrame = av_frame_clone(value);
    }
}

void DecodingThread::resetDecodingRecs()
{
    widthOut = 100;
    heightOut = 100;
    pixOut = AV_PIX_FMT_YUV420P;

    if(pFrameRGB != NULL)
    {
        av_frame_free(&pFrameRGB);
    }
    pFrameRGB = NULL;
    if(imgConvertCtcRGB != NULL)
    {
        sws_freeContext(imgConvertCtcRGB);
    }
    imgConvertCtcRGB = NULL;
    rgbFmt = AV_PIX_FMT_BGR24;
    rgbBytes = 0;
    if(rgbOutBuffer != NULL)
    {
        av_free(rgbOutBuffer);
    }
    rgbOutBuffer = NULL;
    if(!mRGB.empty())
    {
        mRGB.release();
    }
    if(!temp.empty())
    {
        temp.release();
    }
    if(filtedFrame != NULL)
    {
        av_frame_free(&filtedFrame);
    }
}

void DecodingThread::initDecodingRecs()
{
    pFrameRGB = av_frame_alloc();
    imgConvertCtcRGB = sws_getContext(widthOut, heightOut,
                                      pixOut, widthOut, heightOut,
                                      rgbFmt, SWS_FAST_BILINEAR, NULL, NULL, NULL);
    rgbBytes = avpicture_get_size(rgbFmt, widthOut, heightOut);
    rgbOutBuffer = (uint8_t *) av_malloc(rgbBytes * sizeof(uint8_t));
    avpicture_fill((AVPicture *)pFrameRGB, rgbOutBuffer, rgbFmt,
                   widthOut, heightOut);

    mRGB = cv::Mat(cv::Size(widthOut, heightOut), CV_8UC3);
    hasInitDecodingRecs = true;
}

void DecodingThread::decodeFrameAndShow(AVFrame *filtedFrame)
{
    sws_scale(imgConvertCtcRGB, (uint8_t const * const *) filtedFrame->data,
              filtedFrame->linesize, 0, heightOut, pFrameRGB->data,
              pFrameRGB->linesize);

    mRGB.data =(uchar*)pFrameRGB->data[0];
    cv::cvtColor(mRGB, temp, CV_BGR2RGB);

    QImage dest((uchar*) temp.data, temp.cols, temp.rows, temp.step, QImage::Format_RGB888);
    QImage image(dest);
    image.detach();
    emit onFrameImage(image);
}
