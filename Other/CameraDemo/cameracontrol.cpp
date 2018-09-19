#include "cameracontrol.h"
#include "ui_cameracontrol.h"

CameraControl::CameraControl(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CameraControl)
{
    ui->setupUi(this);
    cameraType = CAMERATYPE_LOCAL;
    cameraUrl = "video=";
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
    ui->label->setPixmap(QPixmap::fromImage(image.scaled(400, 300, Qt::KeepAspectRatio)));
}
