#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    isFirstShow = true;
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
    timeControl = NULL;
    weatherControl = NULL;
    showDateTimeControlToCamerasComtrol = ui->checkBoxTimeControl->isChecked();
    showWeathControl = ui->checkBoxWeatherControl->isChecked();
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

void MainWindow::showEvent(QShowEvent *event)
{
    Q_UNUSED(event)
    if(isFirstShow)
    {
        isFirstShow = false;
        timeControl = new DateTimeControl();
        ui->verticalLayoutTime->addWidget(timeControl);
        timeControl->start();
        weatherControl = new WeatherControl();
        ui->verticalLayoutWeatherReport->addWidget(weatherControl);
        weatherControl->startDateTime();
        weatherControl->setCanShowLive(true);
        weatherControl->setCanShowReport(true);
        weatherControl->updateWeather();
        weatherControl->switchView();
    }
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

        if(timerFrames % 100 == 0)
        {
            weatherControl->switchView();
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
            if(form->getCameraType() == CAMERATYPE_LOCAL || form->getCameraType() == CAMERATYPE_WEB)
            {
                QString url = form->getCameraUrl();
                if(!existsCameraUrls.contains(url.toLower()))
                {
                    existsCameraUrls.append(url.toLower());
                    CameraControl *control = new CameraControl();
                    connect(control, SIGNAL(onRemoveRequest()), this, SLOT(onCameraControlRequestRemove()));
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
        bool isshowedtimecontrol = false;
        bool isshowedweathercontrol = false;
        for(int i = 0; i < 4; i++)
        {
            if(allCameraControls.count() > i)
            {
                if(i == 0) ui->verticalLayoutCams1->addWidget(allCameraControls.at(i));
                if(i == 1) ui->verticalLayoutCams2->addWidget(allCameraControls.at(i));
                if(i == 2) ui->verticalLayoutCams3->addWidget(allCameraControls.at(i));
                if(i == 3) ui->verticalLayoutCams4->addWidget(allCameraControls.at(i));
            }
            else
            {
                if(!isshowedtimecontrol && showDateTimeControlToCamerasComtrol)
                {
                    isshowedtimecontrol = true;
                    if(i == 0) ui->verticalLayoutCams1->addWidget(timeControl);
                    if(i == 1) ui->verticalLayoutCams2->addWidget(timeControl);
                    if(i == 2) ui->verticalLayoutCams3->addWidget(timeControl);
                    if(i == 3) ui->verticalLayoutCams4->addWidget(timeControl);
                    continue;
                }
                if(!isshowedweathercontrol && showWeathControl)
                {
                    isshowedweathercontrol = true;
                    if(i == 0) ui->verticalLayoutCams1->addWidget(weatherControl);
                    if(i == 1) ui->verticalLayoutCams2->addWidget(weatherControl);
                    if(i == 2) ui->verticalLayoutCams3->addWidget(weatherControl);
                    if(i == 3) ui->verticalLayoutCams4->addWidget(weatherControl);
                    continue;
                }
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
            int tabcamwidth = ui->widgetCamBig->width() - 4;
            int tabcamheight = ui->widgetCamBig->height() - 4;
            int mw = maxwidth - 4;
            int mh = maxheight - 4;
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
        int tabcamwidth = ui->tabCam4->width() - 4;
        int tabcamheight = ui->tabCam4->height() - 4;
        int mw = maxwidth - 4;
        int mh = maxheight - 4;
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
        timeControl->setMaximumSize(QSize(eachwidth, eachheight));
        weatherControl->setMaximumSize(QSize(eachwidth, eachheight));
    }
    else if(ui->tabWidget->currentWidget() == ui->tabCamBigAndSmall)
    {

    }
    else if(ui->tabWidget->currentWidget() == ui->tabTime)
    {
        ui->verticalLayoutTime->addWidget(timeControl);
        timeControl->setMaximumSize(ui->tabTime->size());
    }
    else if(ui->tabWidget->currentWidget() == ui->tabWeatherReport)
    {
        ui->verticalLayoutWeatherReport->addWidget(weatherControl);
        weatherControl->setMaximumSize(ui->tabWeatherReport->size());
    }
}

void MainWindow::on_tabWidget_currentChanged(int index)
{
    Q_UNUSED(index);
    showCamera();
}

void MainWindow::on_checkBoxTimeControl_stateChanged(int arg1)
{
    Q_UNUSED(arg1);
    showDateTimeControlToCamerasComtrol = ui->checkBoxTimeControl->isChecked();
}

void MainWindow::on_checkBoxWeatherControl_stateChanged(int arg1)
{
    Q_UNUSED(arg1);
    showWeathControl = ui->checkBoxWeatherControl->isChecked();
}

void MainWindow::onCameraControlRequestRemove()
{
    CameraControl *control = qobject_cast<CameraControl *>(QObject::sender());
    if(control != NULL)
    {
        allCameraControls.removeAll(control);
        ui->verticalLayoutHide->removeWidget(control);
        ui->verticalLayoutCamBig->removeWidget(control);
        ui->verticalLayoutCams1->removeWidget(control);
        ui->verticalLayoutCams2->removeWidget(control);
        ui->verticalLayoutCams3->removeWidget(control);
        ui->verticalLayoutCams4->removeWidget(control);
        existsCameraUrls.removeAll(control->getCameraUrl());
        control->stop();
        control->deleteLater();
        control = Q_NULLPTR;
        camBigIndex = 0;
    }
}
