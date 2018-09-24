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
    cameraName = "";
    checkBrighness = false;
    fixBrighnessByTime = false;
    player = Q_NULLPTR;
    glcontrol = Q_NULLPTR;
}

CameraControl::~CameraControl()
{
    stop();
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
    stop();
    player = new VideoPlayer();
    player->setCameraType(cameraType);
    player->setCameraUrl(cameraUrl);
    player->setCameraName(cameraName);
    player->setCheckBrighness(checkBrighness);
    player->setFixBrighnessByTime(fixBrighnessByTime);
    connect(player, SIGNAL(onFrame(QImage)), this, SLOT(onImage(QImage)));
    connect(player, SIGNAL(onFrameSize(int,int)), this, SLOT(onFreamSize(int,int)));
    connect(player, SIGNAL(onFrame(unsigned char*)), this, SLOT(onFream(unsigned char*)));
    player->start();
}

void CameraControl::stop()
{
    if(player != Q_NULLPTR)
    {
        player->setStop();
        player->wait(3000);
        player->terminate();
        player->deleteLater();
        player = Q_NULLPTR;
    }
}

void CameraControl::onImage(QImage image)
{
    if(image.isNull())
    {
        return;
    }
    if(image.width() > imageWidth || image.height() > imageHeight)
    {
        ui->label->setPixmap(QPixmap::fromImage(image.scaled(imageWidth, imageHeight, Qt::KeepAspectRatio)));
    }
    else
    {
        ui->label->setPixmap(QPixmap::fromImage(image));
    }
}

void CameraControl::onFreamSize(int width, int height)
{
    glcontrol = new CameraControlGL(width, height);
    glcontrol->show();
}

void CameraControl::onFream(unsigned char *yuvData)
{
    if(glcontrol != Q_NULLPTR)
    {
        glcontrol->onFrame(yuvData);
    }
}

bool CameraControl::getFixBrighnessByTime() const
{
    return fixBrighnessByTime;
}

void CameraControl::setFixBrighnessByTime(bool value)
{
    fixBrighnessByTime = value;
}

bool CameraControl::getCheckBrighness() const
{
    return checkBrighness;
}

void CameraControl::setCheckBrighness(bool value)
{
    checkBrighness = value;
}

int CameraControl::getImageHeight() const
{
    return imageHeight;
}

void CameraControl::setImageHeight(int value)
{
    imageHeight = value > 720? 720 : value;
}

int CameraControl::getImageWidth() const
{
    return imageWidth;
}

void CameraControl::setImageWidth(int value)
{
    imageWidth = value > 1280? 1280 : value;
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
