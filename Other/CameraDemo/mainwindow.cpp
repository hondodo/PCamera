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
    camBigIndex = -1;
    timerId = 0;
    timerFrames = 0;
    timerId = startTimer(100);
    camBigShowingWidget = Q_NULLPTR;
    ui->widgetHide->setVisible(false);
    VideoFileThread::Init->start();
}

MainWindow::~MainWindow()
{
    VideoFileThread::Init->setStop();
    for(int i = 0; i < allCameraControls.count(); i++)
    {
        allCameraControls.at(i)->stop();
    }
    delete ui;
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event);
    resizeCameraControl();
}

void MainWindow::timerEvent(QTimerEvent *event)
{
    Q_UNUSED(event);
    if(event->timerId() == timerId)
    {
        if(timerFrames % 100 == 0)
        {
            if(ui->tabWidget->currentWidget() == ui->tabCamBig)
            {
                if(camBigShowingWidget != Q_NULLPTR && allCameraControls.count() > 1)
                {
                    ui->verticalLayoutHide->addWidget(camBigShowingWidget);
                }
                if(!allCameraControls.isEmpty() && allCameraControls.count() > 0)
                {
                    if(camBigIndex < 0 || camBigIndex > (allCameraControls.count() - 1))
                    {
                        camBigIndex = 0;
                    }
                    if(camBigShowingWidget != allCameraControls.at(camBigIndex))
                    {
                        camBigShowingWidget = allCameraControls.at(camBigIndex);
                        ui->verticalLayoutCamBig->addWidget(camBigShowingWidget);
                        resizeCameraControl();
                    }
                }
                else
                {
                    camBigShowingWidget = Q_NULLPTR;
                }
            }
            camBigIndex++;
        }

        timerFrames++;
        if(timerFrames > 10000)
        {
            timerFrames = 0;
        }
    }
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
                if(form->getCameraType() == CAMERATYPE_LOCAL)
                {
#ifdef Q_OS_WIN
                    control->setCheckBrighness(true);
                    control->setFixBrighnessByTime(true);
#else
                    control->setCheckBrighness(true);
                    control->setFixBrighnessByTime(true);
#endif
                }
                control->setCameraUrl(url);
                control->setCameraType(form->getCameraType());
                control->setCameraName(form->getCameraName());
                control->start();
                ui->verticalLayoutHide->removeWidget(camBigShowingWidget);
                camBigShowingWidget = control;
                ui->verticalLayoutCamBig->addWidget(camBigShowingWidget);
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
        ui->verticalLayoutCamBig->addWidget(camBigShowingWidget);
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
        if(camBigShowingWidget != Q_NULLPTR)
        {
            int tabcamwidth = ui->widgetCamBig->width() - 25;
            int tabcamheight = ui->widgetCamBig->height() - 10;
            int mw = maxwidth - 25;
            int mh = maxheight - 10;
            if(tabcamwidth > mw || tabcamheight > mh)
            {
                ui->tabCam4->setMaximumSize(mw, mh);
                ui->tabCam4->resize(mw, mh);
                tabcamwidth = mw;
                tabcamheight = mw;
            }
            camBigShowingWidget->setImageWidth(tabcamwidth);
            camBigShowingWidget->setImageHeight(tabcamheight);
        }
        //int showindex = 0;
        //if(allCameraControls.count() > showindex)
        //{
        //    allCameraControls.at(showindex)->setImageWidth(tabcamwidth);
        //    allCameraControls.at(showindex)->setImageHeight(tabcamheight);
        //}
    }
    else if(ui->tabWidget->currentWidget() == ui->tabCam4)
    {
        int tabcamwidth = ui->tabCam4->width() - 25;
        int tabcamheight = ui->tabCam4->height() - 25;
        int mw = maxwidth - 25;
        int mh = maxheight - 25;
        if(tabcamwidth > mw || tabcamheight > mh)
        {
            ui->tabCam4->setMaximumSize(mw, mh);
            ui->tabCam4->resize(mw, mh);
            tabcamwidth = mw;
            tabcamheight = mh;
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
