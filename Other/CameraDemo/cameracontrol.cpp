#include "cameracontrol.h"
#include "ui_cameracontrol.h"

CameraControl::CameraControl(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CameraControl)
{
    ui->setupUi(this);
    cameraType = CAMERATYPE_LOCAL;
    cameraUrl = "video=";
    imageWidth = 640;
    imageHeight = 480;
}

CameraControl::~CameraControl()
{
    delete ui;
}

QString CameraControl::getCameraUrl() const
{
    return cameraUrl;
}

void CameraControl::setCameraUrl(const QString &value)
{
    cameraUrl = value;
}

CAMERATYPE CameraControl::getCameraType() const
{
    return cameraType;
}

void CameraControl::setCameraType(const CAMERATYPE &value)
{
    cameraType = value;
}

void CameraControl::start()
{
    VideoPlayer *player = new VideoPlayer();
    player->setCameraType(cameraType);
    player->setCameraUrl(cameraUrl);
    connect(player, SIGNAL(onFrame(QImage)), this, SLOT(onImage(QImage)));
    player->start();
}

void CameraControl::stop()
{

}

void CameraControl::onImage(QImage image)
{
    if(image.isNull())
    {
        return;
    }
//    if(this->maximumHeight() > this->height() || this->maximumWidth() > this->width())
//    {
        ui->label->setPixmap(QPixmap::fromImage(image.scaled(imageWidth, imageHeight, Qt::KeepAspectRatio)));
//    }
//    if(image.height() > this->height() || image.width() > this->width())
//    {
//        ui->label->setPixmap(QPixmap::fromImage(image.scaled(this->size(), Qt::KeepAspectRatio)));
//    }
//    else
//    {
//        ui->label->setPixmap(QPixmap::fromImage(image));
//    }
}

int CameraControl::getImageHeight() const
{
    return imageHeight;
}

void CameraControl::setImageHeight(int value)
{
    imageHeight = value;
}

int CameraControl::getImageWidth() const
{
    return imageWidth;
}

void CameraControl::setImageWidth(int value)
{
    imageWidth = value;
}

QString CameraControl::getCameraName() const
{
    return cameraName;
}

void CameraControl::setCameraName(const QString &value)
{
    cameraName = value;
    ui->label->setText("Camera:" + cameraName);
}
