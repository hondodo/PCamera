#include "maindialog.h"
#include "ui_maindialog.h"

MainDialog::MainDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MainDialog)
{
    ui->setupUi(this);
    ui->widgetCameraA->setCameraId(0);
    ui->widgetCameraB->setCameraId(1);
    ui->widgetCameraC->setCameraId(2);
    ui->widgetCameraD->setCameraId(3);

    ui->widgetCameraA->setDefaultText();
    ui->widgetCameraA->startNewCameraThread(0);
    ui->widgetCameraA->setIsFacing(true);
    ui->widgetCameraA->setIsRinging(true);

    ui->widgetCameraB->setDefaultText();

    connect(ui->widgetCameraA, SIGNAL(onImage(QImage)),
            this, SLOT(onImage(QImage)));
    connect(ui->widgetCameraA, SIGNAL(onTip(QString)),
            this, SLOT(onTip(QString)));

    this->setWindowFlags(Qt::Window);
    this->showMaximized();

    oledThread = new OLedThread();
    oledThread->start();
    CameraCollectorThread::Init->start();
}

MainDialog::~MainDialog()
{
    if(oledThread != NULL)
    {
        oledThread->setStop();
        oledThread->wait(1000);
        oledThread->terminate();
    }
    delete ui;
}

void MainDialog::resizeEvent(QResizeEvent *)
{
    int totalhight = ui->widgetMain->height();
    int bigheight = totalhight / 3 * 2;
    ui->widgetCameraA->setMinimumHeight(bigheight);
    ui->widgetCameraA->setMaximumHeight(bigheight);
}

void MainDialog::onImage(const QImage &image)
{
    //oledThread->setImage(image);
}

void MainDialog::onTip(QString message)
{
    oledThread->setMessage(message);
    qDebug() << message;
}
