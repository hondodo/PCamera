#ifndef VIDEOFILEHELPER_H
#define VIDEOFILEHELPER_H

#include <QThread>
#include <QMap>
#include <QQueue>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

class VideoFileThread :public QThread
{
public:
    VideoFileThread();
    ~VideoFileThread();
    static VideoFileThread *Init;
    void append(QString filename, cv::Mat mat);
    void closeWriter(QString filename);
    void setStop();

protected:
    void run() override;

private:
    QMap<QString, QQueue<cv::Mat> > filenameMatList;
    QMap<QString, cv::VideoWriter *> filenameWriter;
    bool _isRunning;
};

#endif // VIDEOFILEHELPER_H
