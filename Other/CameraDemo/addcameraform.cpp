#include "addcameraform.h"
#include "ui_addcameraform.h"

AddCameraForm::AddCameraForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AddCameraForm)
{
    ui->setupUi(this);
    isFirstShow = true;
}

AddCameraForm::~AddCameraForm()
{
    delete ui;
}

void AddCameraForm::showEvent(QShowEvent *event)
{
    Q_UNUSED(event);
    if(isFirstShow)
    {
        isFirstShow = false;
        on_comboBoxType_currentIndexChanged(ui->comboBoxType->currentIndex());
        state = -1;
        cameraUrl = "";
        cameraName = "";
        cameraType = CAMERATYPE_LOCAL;
        existsCameraUrls.clear();
        ui->labelError->setText("");
        ui->labelError->setVisible(false);
    }
}

void AddCameraForm::on_comboBoxType_currentIndexChanged(int index)
{
    Q_UNUSED(index);
    ui->labelError->setVisible(false);
    if(index == 0)
    {
        ui->comboBoxUrl->setVisible(true);
        ui->lineEditUrl->setVisible(false);
        initCameraComboBox();
    }
    else
    {
        ui->comboBoxUrl->setVisible(false);
        ui->lineEditUrl->setVisible(true);
    }
    initCameraName();
}

void AddCameraForm::initCameraComboBox()
{
    ui->comboBoxUrl->clear();
#ifdef Q_OS_WIN
    QList<QByteArray> all = QCamera::availableDevices();
    for(int i = 0; i < all.count(); i++)
    {
        QString desc = QCamera::deviceDescription(all.at(i));
        QString url = "video=" + desc;

//        QList<QAudioDeviceInfo> devInfo = QAudioDeviceInfo::availableDevices(QAudio::AudioInput);
//        for(int i = 0; i < devInfo.count(); i++)
//        {
//            QString audioname = devInfo.at(i).deviceName();
//            qDebug() << audioname;
//            url = "audio=" + audioname;
//        }

        if(!existsCameraUrls.contains(url.toLower()))
        {
            ui->comboBoxUrl->addItem(desc, url);
        }
    }
#else
    for(int i = 0; i < 16; i++)
    {
        QString desc = QString::number(i, 'f', 0);
        QString url = "/dev/video" + desc;
        QFile file(url);
        if(file.exists())
        {
            if(!existsCameraUrls.contains(url.toLower()))
            {
                ui->comboBoxUrl->addItem(desc, url);
            }
        }
    }
#endif
}

void AddCameraForm::initCameraName()
{
    if(ui->comboBoxType->currentIndex() == 0)
    {
        ui->lineEditName->setText(ui->comboBoxUrl->currentText());
    }
    else
    {
        ui->lineEditName->setText(ui->lineEditUrl->text().trimmed());
    }
}

QString AddCameraForm::getCameraName() const
{
    return cameraName;
}

void AddCameraForm::setExistsCameraUrls(const QList<QString> &value)
{
    existsCameraUrls = value;
}

int AddCameraForm::getState() const
{
    return state;
}

CAMERATYPE AddCameraForm::getCameraType() const
{
    return cameraType;
}

QString AddCameraForm::getCameraUrl() const
{
    return cameraUrl;
}

void AddCameraForm::on_pushButtonOK_clicked()
{
    state = 0;
    cameraName = ui->lineEditName->text().trimmed();
    if(ui->comboBoxType->currentIndex() == 0)
    {
        cameraType = CAMERATYPE_LOCAL;
        cameraUrl = ui->comboBoxUrl->currentData().toString();
        if(existsCameraUrls.contains(cameraUrl.toLower()))
        {
            state = 1;
        }
    }
    else if(ui->comboBoxType->currentIndex() == 1)
    {
        cameraType = CAMERATYPE_WEB;
        cameraUrl = ui->lineEditUrl->text().trimmed();
        if(existsCameraUrls.contains(cameraUrl.toLower()))
        {
            state = 1;
        }
    }

    if(state == 1)
    {
        ui->labelError->setText("Camera exist");
        ui->labelError->setVisible(true);
        return;
    }
    this->close();
    emit onClose(state);
}

void AddCameraForm::on_lineEditUrl_textChanged(const QString &arg1)
{
    Q_UNUSED(arg1);
    ui->labelError->setVisible(false);
    initCameraName();
}

void AddCameraForm::on_comboBoxUrl_currentIndexChanged(int index)
{
    Q_UNUSED(index);
    ui->labelError->setVisible(false);
    initCameraName();
}
