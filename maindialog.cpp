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

    oled.Init(0x3c);
    oled.DisplayOn();
    oled.CleanScreen();

    lastShowLed = QDateTime::currentDateTime();
    connect(ui->widgetCameraA, SIGNAL(onImage(QImage)),
            this, SLOT(onImage(QImage)));
}

MainDialog::~MainDialog()
{
    oled.DisplayOff();
    delete ui;
}

void MainDialog::onImage(const QImage &image)
{
    int els = QDateTime::currentDateTime().toMSecsSinceEpoch() - lastShowLed.toMSecsSinceEpoch();
    if(els > 1000 || els < 0)
    {
        lastShowLed = QDateTime::currentDateTime();
        oled.WriteImage(&image);
    }
}
