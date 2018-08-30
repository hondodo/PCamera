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
    ringThread = Q_NULLPTR;

    ui->checkBoxFace->setChecked(false);
    ui->checkBoxRing->setChecked(false);
    ui->checkBoxFace->setVisible(false);
    ui->checkBoxRing->setVisible(false);

    timeLabel->setVisible(false);
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
    camThread->setTargetSize(this->size());
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

void CameraControl::deleteRingThread()
{
    if(ringThread != Q_NULLPTR)
    {
        ringThread->setStop();
        ringThread->wait(1000);
        ringThread->terminate();
        ringThread = Q_NULLPTR;
    }
}

void CameraControl::startNewRingThread()
{
    ringThread = new RingThread();
    ringThread->start();
}

bool CameraControl::getIsRinging() const
{
    return ui->checkBoxRing->isChecked();
}

void CameraControl::setIsRinging(bool value)
{
    ui->checkBoxRing->setChecked(value);
}

void CameraControl::resizeEvent(QResizeEvent *)
{
    if(camThread != Q_NULLPTR)
    {
        camThread->setTargetSize(this->size());
    }
}

void CameraControl::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    if(!imageCache.isNull())
    {
        QPainter painter(this);
        painter.fillRect(this->rect(), Qt::black);
        int x = (this->width() - imageCache.width()) / 2;
        int y = (this->height() - imageCache.height()) / 2;
        painter.drawImage(x, y, imageCache);
    }
}

bool CameraControl::getIsFacing() const
{
    return ui->checkBoxFace->isChecked();
}

void CameraControl::setIsFacing(bool value)
{
    ui->checkBoxFace->setChecked(value);
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
    return;
    this->timeLabel->setText(time.toString("yyyy-MM-dd hh:mm:ss"));
    this->timeLabel->adjustSize();
}

void CameraControl::setImage(const QImage &image)
{
    imageCache = image.copy();
    this->update();
    emit onImage(image);
    return;

    ui->labelCamara->setPixmap(QPixmap::fromImage(image));
    updateTime(QDateTime::currentDateTime());
    emit onImage(image);
}

void CameraControl::setTip(const QString &tip)
{
    ui->labelCamara->setText(tip);
    emit onTip(tip);
}

void CameraControl::onConnectChanged(bool connected)
{
    ui->pushButtonConnect->setVisible(!connected);
    ui->labelCamara->setVisible(!connected);
}

void CameraControl::onFaceDetected(int faceCount)
{
    Q_UNUSED(faceCount);
    if(QDateTime::currentDateTime().toMSecsSinceEpoch() - lastDetectFaceTime.toMSecsSinceEpoch() > 30 * 1000)
    {
        lastDetectFaceTime = QDateTime::currentDateTime();
        if(_isRing)
        {
            deleteRingThread();
            startNewRingThread();
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
