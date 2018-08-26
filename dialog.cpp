#include "dialog.h"
#include "ui_dialog.h"

Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    cameraIdIndex(0),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);
    ui->widgetCamara->setStyleSheet(QString("QWiget#widgetCamara"
                                       "{"
                                       "background-color: rgb(255, 255, 255);"
                                       "}"));
    this->setWindowFlags(Qt::Window);
    th = Q_NULLPTR;
    oledVol = 0x20;
    oled.Init(0x3c);
}

void Dialog::stopCameraCollectThread()
{
    if(th != Q_NULLPTR)
    {
        th->setStop();
        th->wait(1000);
        th->terminate();
    }
}

Dialog::~Dialog()
{
    stopCameraCollectThread();
    delete ui;
}

void Dialog::resizeEvent(QResizeEvent *)
{
    on_restoreCamaras();
}

void Dialog::on_pushButtonStart_clicked()
{
    CameraControl *control = new CameraControl(cameraIdIndex, ui->widgetCamara);
    control->move(0, 0);
    on_restoreCamaras();
    control->show();
    cameraIdIndex++;
}

void Dialog::on_restoreCamaras()
{
    QSize containerSize = ui->scrollAreaWidgetContents->size();
    int controlCount = getShowingCameraCount();
    if(controlCount > 0)
    {
        int maxWidth = containerSize.width();
        int itemWhith = CameraControl::Width;
        int itemHeight = CameraControl::Height;
        int cols = maxWidth / itemWhith;
        int rows = controlCount / cols + (controlCount % cols == 0? 0 : 1);
        ui->widgetCamara->setMaximumSize(itemWhith * cols, itemHeight * rows);
        ui->widgetCamara->setMinimumSize(itemWhith * cols, itemHeight * rows);
        //
        int index = 0;
        QObjectList list = ui->widgetCamara->children();
        if(!list.isEmpty())
        {
            int tmpcount = list.count();
            for(int i = 0; i < tmpcount; i++)
            {
                CameraControl *control = qobject_cast<CameraControl *>(list.at(i));
                if(control != Q_NULLPTR)
                {
                    int row = index / cols;
                    int col = index % cols;
                    control->move(col * itemWhith, row * itemHeight);
                    index++;
                }
            }
            ui->widgetCamara->update();
        }
    }
}

int Dialog::getCameraCount() const
{
    return 0;
}

int Dialog::getShowingCameraCount() const
{
    QObjectList list = ui->widgetCamara->children();
    int count = 0;
    if(!list.isEmpty())
    {
        int tmpcount = list.count();
        for(int i = 0; i < tmpcount; i++)
        {
            CameraControl *control = qobject_cast<CameraControl *>(list.at(i));
            if(control != Q_NULLPTR)
            {
                count++;
            }
        }
    }
    return count;
}

void Dialog::on_pushButtonStop_clicked()
{
    stopCameraCollectThread();
    th = new CameraCollectorThread();
    th->start();
}

void Dialog::on_pushButtonOLedOn_clicked()
{
    oled.DisplayOn();
}

void Dialog::on_pushButtonOLedOff_clicked()
{
    oled.DisplayOff();
}

void Dialog::on_pushButtonOLedAdd_clicked()
{
    oledVol++;
    if(oledVol > 0xff)
    {
        oledVol = 0x00;
    }
    oled.SetVcomh(oledVol);
    qDebug() << oledVol;
}

void Dialog::on_pushButtonOLedMut_clicked()
{
    oledVol--;
    if(oledVol < 0x00)
    {
        oledVol = 0xff;
    }
    oled.SetVcomh(oledVol);
    qDebug() << oledVol;
}

void Dialog::on_pushButtonCleanOLed_clicked()
{
    oled.CleanScreen();
    oled.WriteCMD(0x00, 0xb0);
}

void Dialog::on_pushButtonTes_clicked()
{
    //QImage image(128, 64, QImage::Format_ARGB32);
    //QPainter painter(&image);
    //painter.fillRect(QRect(0, 0, 128, 64), Qt::white);
    //QPen pen(Qt::black);
    //pen.setWidth(2);
    //painter.setPen(pen);
    //painter.drawLine(0, 0, 128, 64);
    //image.save("/home/pi/Pictures/hi.png");
    //oled.WriteImage(&image);
    //return;

    QImage img;
    if(img.load("/home/pi/Pictures/bdlogo.png"))
    {
        oled.WriteImage(&img);
    }
    return;
    //for(int i = 0; i < 8; i++)
    //{
        oled.WriteCMD(0x00, 0xb0);
        //oled.WriteCMD(0x00, 0x00);
        //oled.WriteCMD(0x00, 0x10);
        //for(int j = 0; j < 16; j++)
       // {
    oled.WriteCMD(0x40, 0x08);
    oled.WriteCMD(0x40, 0xf8);
    oled.WriteCMD(0x40, 0x88);
    oled.WriteCMD(0x40, 0x88);
    oled.WriteCMD(0x40, 0x88);
    oled.WriteCMD(0x40, 0x70);
    oled.WriteCMD(0x40, 0x00);
    oled.WriteCMD(0x40, 0x00);

    for(int j = 0; j < (128 - 8); j++)
    {
        oled.WriteCMD(0x40, 0x00);
    }

    oled.WriteCMD(0x00, 0xb0 + 1);
    //oled.WriteCMD(0x00, 0x00);
    //oled.WriteCMD(0x00, 0x10);

    oled.WriteCMD(0x40, 0x20);
    oled.WriteCMD(0x40, 0x3f);
    oled.WriteCMD(0x40, 0x20);
    oled.WriteCMD(0x40, 0x20);
    oled.WriteCMD(0x40, 0x20);
    oled.WriteCMD(0x40, 0x11);
    oled.WriteCMD(0x40, 0x0e);
    oled.WriteCMD(0x40, 0x00);
        //}
    //}
}
