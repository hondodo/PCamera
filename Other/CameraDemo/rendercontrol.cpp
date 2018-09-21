#include "rendercontrol.h"
#include <QPainter>

RenderControl::RenderControl(QWidget *parent) : QOpenGLWidget(parent)
{
#ifdef Q_OS_WIN
    cameraUrl = "video=";
#else
    cameraUrl = "/dev/video0";
#endif
    cameraType = CAMERATYPE_LOCAL;
    player = Q_NULLPTR;
    canUpdateImage = false;
    imageCache = QImage(10, 10, QImage::Format_ARGB32);
}

RenderControl::~RenderControl()
{
    stop();
}

void RenderControl::initializeGL()
{
    initializeOpenGLFunctions();
    glClearColor(0, 0, 0, 1);
}

void RenderControl::paintGL()
{

}

void RenderControl::resizeGL(int w, int h)
{
    Q_UNUSED(w);
    Q_UNUSED(h);
}

void RenderControl::paintEvent(QPaintEvent *e)
{
    Q_UNUSED(e);
    canUpdateImage = false;
    if(imageCache.isNull())
    {}
    else
    {
        QPainter painter;
        painter.begin(this);
        painter.drawImage(imageCache.rect(), imageCache);
        painter.end();
    }
    canUpdateImage = true;
}

void RenderControl::onImage(QImage image)
{
    if(canUpdateImage)
    {
        imageCache = image.copy();
    }
    update();
}

int RenderControl::getImageHeight() const
{
    return imageHeight;
}

void RenderControl::setImageHeight(int value)
{
    imageHeight = value;
}

int RenderControl::getImageWidth() const
{
    return imageWidth;
}

void RenderControl::setImageWidth(int value)
{
    imageWidth = value;
}

QString RenderControl::getCameraName() const
{
    return cameraName;
}

void RenderControl::setCameraName(const QString &value)
{
    cameraName = value;
}

CAMERATYPE RenderControl::getCameraType() const
{
    return cameraType;
}

void RenderControl::setCameraType(const CAMERATYPE &value)
{
    cameraType = value;
}

void RenderControl::stop()
{
    canUpdateImage = false;
    if(player != Q_NULLPTR)
    {
        player->setStop();
        player->wait(1000);
        player->terminate();
        player->deleteLater();
        player = Q_NULLPTR;
    }
}

void RenderControl::start()
{
    stop();
    player = new VideoPlayer();
    player->setCameraType(cameraType);
    player->setCameraUrl(cameraUrl);
    connect(player, SIGNAL(onFrame(QImage)), this, SLOT(onImage(QImage)));
    canUpdateImage = true;
    player->start();
}

QString RenderControl::getCameraUrl() const
{
    return cameraUrl;
}

void RenderControl::setCameraUrl(const QString &value)
{
    cameraUrl = value;
}
