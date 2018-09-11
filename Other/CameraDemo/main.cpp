#include "mainwindow.h"
#include <QApplication>
#include <QDebug>
extern "C"
{
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libswscale/swscale.h"
#include "libavdevice/avdevice.h"
}

using namespace std;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    av_register_all();
    unsigned version = avcodec_version();
    qDebug() << version;

    MainWindow w;
    w.show();

    return a.exec();
}
