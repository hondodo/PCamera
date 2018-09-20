#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    existsCameraUrls.clear();
    allCameraControls.clear();
    QRect rect = QApplication::desktop()->availableGeometry();
    maxwidth = rect.width();
    maxheight = rect.height();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event);
    resizeCameraControl();
}

void MainWindow::on_pushButtonAddCamera_clicked()
{
    AddCameraForm *form = new AddCameraForm();
    form->setExistsCameraUrls(existsCameraUrls);
    connect(form, SIGNAL(onClose(int)), this, SLOT(onAddCameraFormClose(int)));
    form->show();
}

void MainWindow::onAddCameraFormClose(int code)
{
    AddCameraForm *form = qobject_cast<AddCameraForm *>(QObject::sender());
    if(form != Q_NULLPTR)
    {
        if(code == 0)
        {
            QString url = form->getCameraUrl();
            if(!existsCameraUrls.contains(url.toLower()))
            {
                existsCameraUrls.append(url.toLower());
                CameraControl *control = new CameraControl();
                allCameraControls.append(control);
                control->setCameraUrl(url);
                control->setCameraType(form->getCameraType());
                control->setCameraName(form->getCameraName());
                control->start();
                showCamera();
            }
        }
        form->deleteLater();
        form = Q_NULLPTR;
    }
}

void MainWindow::showCamera()
{
    if(ui->tabWidget->currentWidget() == ui->tabCamBig)
    {
        if(allCameraControls.count() > 0)
        {
            ui->verticalLayoutCamBig->addWidget(allCameraControls.at(0));
        }
    }
    else if(ui->tabWidget->currentWidget() == ui->tabCam4)
    {
        for(int i = 0; i < 4; i++)
        {
            if(allCameraControls.count() > i)
            {
                if(i == 0) ui->verticalLayoutCams1->addWidget(allCameraControls.at(i));
                if(i == 1) ui->verticalLayoutCams2->addWidget(allCameraControls.at(i));
                if(i == 2) ui->verticalLayoutCams3->addWidget(allCameraControls.at(i));
                if(i == 3) ui->verticalLayoutCams4->addWidget(allCameraControls.at(i));
            }
        }
    }
    else if(ui->tabWidget->currentWidget() == ui->tabCamBigAndSmall)
    {

    }
    resizeCameraControl();
}

void MainWindow::resizeCameraControl()
{
    if(ui->tabWidget->currentWidget() == ui->tabCamBig)
    {
        int tabcamwidth = ui->tabCamBig->width();
        int tabcamheight = ui->tabCamBig->height();
        if(tabcamwidth > maxwidth || tabcamheight > maxheight)
        {
            ui->tabCam4->setMaximumSize(maxwidth, maxheight);
            ui->tabCam4->resize(maxwidth, maxheight);
            tabcamwidth = maxwidth;
            tabcamheight = maxheight;
        }
        int showindex = 0;
        if(allCameraControls.count() > showindex)
        {
            allCameraControls.at(showindex)->setImageWidth(tabcamwidth);
            allCameraControls.at(showindex)->setImageHeight(tabcamheight);
        }
    }
    else if(ui->tabWidget->currentWidget() == ui->tabCam4)
    {
        int tabcamwidth = ui->tabCam4->width();
        int tabcamheight = ui->tabCam4->height();
        if(tabcamwidth > maxwidth || tabcamheight > maxheight)
        {
            ui->tabCam4->setMaximumSize(maxwidth, maxheight);
            ui->tabCam4->resize(maxwidth, maxheight);
            tabcamwidth = maxwidth;
            tabcamheight = maxheight;
        }
        int eachwidth = tabcamwidth / 2;
        int eachheight = tabcamheight / 2;
        for(int i = 0; i < 4; i++)
        {
            if(allCameraControls.count() > i)
            {
                allCameraControls.at(i)->setImageWidth(eachwidth);
                allCameraControls.at(i)->setImageHeight(eachheight);
            }
        }
    }
    else if(ui->tabWidget->currentWidget() == ui->tabCamBigAndSmall)
    {

    }
}

void MainWindow::on_tabWidget_currentChanged(int index)
{
    Q_UNUSED(index);
    showCamera();
}
