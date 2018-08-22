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
}

Dialog::~Dialog()
{
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
    ring.setStop();
    ring.Ring();
}
