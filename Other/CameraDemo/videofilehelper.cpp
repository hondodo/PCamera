#include "videofilehelper.h"

VideoFileThread *VideoFileThread::Init = new VideoFileThread();
VideoFileThread::VideoFileThread()
{

}

VideoFileThread::~VideoFileThread()
{
    if(!filenameWriter.isEmpty())
    {
        while (filenameWriter.count() > 0)
        {
            QString filename = filenameWriter.firstKey();
            closeWriter(filename);
            filenameWriter.remove(filename);
        }
    }
}

void VideoFileThread::append(QString filename, cv::Mat mat)
{
    if(!filenameMatList.contains(filename))
    {
        QQueue<cv::Mat> list;
        filenameMatList[filename] = list;
    }
    if(filenameMatList[filename].count() > 100) return;
    (&filenameMatList[filename])->enqueue(mat.clone());
}

void VideoFileThread::closeWriter(QString filename)
{
    if(filenameWriter.contains(filename))
    {
        cv::VideoWriter *writer = filenameWriter[filename];
        if(writer != Q_NULLPTR)
        {
            if(writer->isOpened())
            {
                writer->release();
            }
            writer = Q_NULLPTR;
        }
    }
}

void VideoFileThread::setStop()
{
    _isRunning = false;
}

void VideoFileThread::run()
{
    return;

    _isRunning = true;
    while (_isRunning)
    {
        if(filenameMatList.isEmpty() || filenameMatList.count() == 0)
        {}
        else
        {
            int count = filenameMatList.count();
            for(int i = 0; i < count; i++)
            {
                QString filename = filenameMatList.keys().at(i);
                if(filenameMatList.value(filename).count() > 0)
                {
                    cv::Mat mat = (&filenameMatList[filename])->dequeue();
                    if(!filenameWriter.contains(filename))
                    {
                        cv::VideoWriter *writer = new cv::VideoWriter();
                        filenameWriter[filename] = writer;
                    }
                    cv::VideoWriter *writer = filenameWriter[filename];
                    if(!writer->isOpened())
                    {
                        writer->open(filename.toLocal8Bit().data(),
                                       CV_FOURCC('D', 'I', 'V', 'X'), 30,
                                       cv::Size(mat.cols, mat.rows));
                    }
                    if(writer->isOpened())
                    {
                        writer->write(mat);
                        mat.release();
                        while(filenameMatList.value(filename).count() > 0)
                        {
                            mat = (&filenameMatList[filename])->dequeue();
                            writer->write(mat);
                            mat.release();
                        }
                    }
                    else
                    {

                    }
                }
            }
        }
        for(int i = 0; i < 20; i++)
        {
            if(!_isRunning)
            {
                break;
            }
            this->msleep(20);
        }
    }
}
