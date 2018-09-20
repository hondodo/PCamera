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
    if(ui->comboBoxType->currentIndex() == 0)
    {
        cameraType = CAMERATYPE_LOCAL;
        cameraUrl = ui->comboBoxUrl->currentData().toString();
        if(existsCameraUrls.contains(cameraUrl.toLower()))
        {
            state = 1;
        }
    }
    else
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
}

void AddCameraForm::on_comboBoxUrl_currentIndexChanged(int index)
{
    Q_UNUSED(index);
    ui->labelError->setVisible(false);
}
