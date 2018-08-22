#include "cameracontrol.h"
#include "ui_cameracontrol.h"

int CameraControl::Width = 400;
int CameraControl::Height = 300;
CameraControl::CameraControl(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CameraControl)
{
    ui->setupUi(this);
    this->lastDetectFaceTime = QDateTime::currentDateTime().addDays(-1);
    this->_isRing = ui->checkBoxRing->isChecked();
    this->CameraId = -1;
    this->camThread = Q_NULLPTR;
    this->resize(Width, Height);
    this->timeLabel = new QLabel("Time", ui->labelCamara);
    this->timeLabel->setObjectName("timeLabel");
    this->timeLabel->move(5, 5);
    this->timeLabel->show();
    setDefaultText();
}

CameraControl::CameraControl(int id, QWidget *parent) : CameraControl(parent)
{
    this->CameraId = id;
    setDefaultText();
    startNewCameraThread(id);
}

void CameraControl::startNewCameraThread(int id)
{
    camThread = new CamaraThread();
    camThread->setCamaraId(id);

    camThread->setIsDetectFace(ui->checkBoxFace->isChecked());

    connect(camThread, SIGNAL(onTip(QString)), this, SLOT(setTip(QString)));
    connect(camThread, SIGNAL(onImage(QImage)), this, SLOT(setImage(QImage)));
    connect(camThread, SIGNAL(onConnectChanged(bool)), this, SLOT(onConnectChanged(bool)));
    connect(camThread, SIGNAL(onFaceDetected(int)), this, SLOT(onFaceDetected(int)));

    camThread->start();
}


void CameraControl::deleteCameraThread()
{
    if(camThread != Q_NULLPTR)
    {
        camThread->setStop();
        camThread->wait(1000);
        camThread->terminate();
        camThread = Q_NULLPTR;
    }
}

CameraControl::~CameraControl()
{
    this->timeLabel->deleteLater();
    deleteCameraThread();
    delete ui;
}

void CameraControl::setDefaultText()
{
    ui->labelCamara->setText(QString("(%1)").arg(QString::number(this->CameraId)) +
                             tr("No Signal"));
    updateTime(QDateTime::currentDateTime());

}

int CameraControl::getCameraId() const
{
    return CameraId;
}

void CameraControl::setCameraId(int value)
{
    CameraId = value;
}

void CameraControl::updateTime(QDateTime time)
{
    this->timeLabel->setText(time.toString("yyyy-MM-dd hh:mm:ss"));
    this->timeLabel->adjustSize();
}

void CameraControl::setImage(const QImage &image)
{
    ui->labelCamara->setPixmap(QPixmap::fromImage(image));
    updateTime(QDateTime::currentDateTime());
}

void CameraControl::setTip(const QString &tip)
{
    ui->labelCamara->setText(tip);
}

void CameraControl::onConnectChanged(bool connected)
{
    ui->pushButtonConnect->setVisible(!connected);
}

void CameraControl::onFaceDetected(int faceCount)
{
    Q_UNUSED(faceCount);
    if(QDateTime::currentDateTime().toSecsSinceEpoch() - lastDetectFaceTime.toSecsSinceEpoch() > 30)
    {
        lastDetectFaceTime = QDateTime::currentDateTime();
        if(_isRing)
        {
            ringHelper.Ring();
        }
    }
}

void CameraControl::reConnect()
{
    deleteCameraThread();
    startNewCameraThread(this->CameraId);
}

void CameraControl::on_pushButtonConnect_clicked()
{
    reConnect();
}

void CameraControl::on_checkBoxFace_toggled(bool checked)
{
    if(camThread != Q_NULLPTR)
    {
        camThread->setIsDetectFace(checked);
    }
}

void CameraControl::on_checkBoxRing_toggled(bool checked)
{
    _isRing = checked;
}
