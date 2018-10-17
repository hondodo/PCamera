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
    ringThread = NULL;
    showDateTimeControlToCamerasComtrol = ui->checkBoxTimeControl->isChecked();
    showWeathControl = ui->checkBoxWeatherControl->isChecked();
    ringFileName = "";
#ifdef Q_OS_LINUX
    ringFileName = "/home/pi/Music/Ring/ring.mp3";
#endif
    labelDiskInfo.setText("Free: 0G/Total: 64G");
    labelCamAInfo.setText("");
    labelCamBInfo.setText("");
    labelCamCInfo.setText("");
    labelCamDInfo.setText("");
    labelRingInfo.setText("");
    labelDark.setText("");
    labelPeople.setText("");
    labelLight.setText("");
    ui->statusBar->addWidget(&labelDiskInfo);
    ui->statusBar->addWidget(&labelCamAInfo);
    ui->statusBar->addWidget(&labelCamBInfo);
    ui->statusBar->addWidget(&labelCamCInfo);
    ui->statusBar->addWidget(&labelCamDInfo);
    ui->statusBar->addWidget(&labelRingInfo);
    ui->statusBar->addWidget(&labelPeople);
    ui->statusBar->addWidget(&labelDark);
    ui->statusBar->addWidget(&labelLight);
    qsrand(QTime(0, 0, 0).secsTo(QTime::currentTime()));

    glesWidget = new GLESWidget(ui->widgetTestYUV);
    isDark = false;
    isPeople = false;

    darkForm = new DarkForm();
    lastPeople = QDateTime::currentDateTime();
    lastTurnOnLight = QDateTime::currentDateTime().addDays(-1);
    isFromTestDark = false;

    ui->pushButtonTestDarkScreen->setVisible(false);
    ui->pushButtonClearTestMark->setVisible(false);

    setWindowTitle(tr("Potatokid Camera"));
    lastState = this->windowState();
}

MainWindow::~MainWindow()
{
    darkForm->close();
    darkForm->deleteLater();
    KeyBoardThread::Init->setStop();
    CheckDiskThread::Init->setStop();
    VideoFileThread::Init->setStop();
    for(int i = 0; i < allCameraControls.count(); i++)
    {
        allCameraControls.at(i)->stop();
    }
    deleteRingThread();
    delete ui;
}

void MainWindow::showEvent(QShowEvent *event)
{
    Q_UNUSED(event)
    if(isFirstShow)
    {
        isFirstShow = false;
        timeControl = new DateTimeControl();
#ifdef Q_OS_WIN
        ui->checkBoxShowTemperature->setChecked(false);
#endif
        timeControl->setShowTemperature(ui->checkBoxShowTemperature->isChecked());
        timeControl->setTemperatureXiaoShuWei(ui->spinBoxXiaoShuWei->value());
        ui->verticalLayoutTime->addWidget(timeControl);
        timeControl->start();
        weatherControl = new WeatherControl();
        ui->verticalLayoutWeatherReport->addWidget(weatherControl);
        weatherControl->startDateTime();
        weatherControl->setCanShowLive(true);
        weatherControl->setCanShowReport(true);
        weatherControl->updateWeather();
        weatherControl->switchView();
        KeyBoardThread::Init->start();
        connect(KeyBoardThread::Init, SIGNAL(onKey(int)), this, SLOT(onKey(int)));
        diskHelper.setPath(pathHelper.getRootPath());
        CheckDiskThread::Init->start();
        this->setWindowState(Qt::WindowFullScreen);
    }
}

void MainWindow::deleteRingThread()
{
    if(ringThread != Q_NULLPTR)
    {
        ringThread->setStop();
        ringThread->wait(1000);
        if(ringThread->isRunning())
        {
            ringThread->terminate();
        }
        ringThread->deleteLater();
        ringThread = Q_NULLPTR;
    }
}

void MainWindow::startNewRingThread()
{
    //deleteRingThread();
    if(ringThread != NULL)
    {
        if(ringThread->isRunning())
        {
            return;
        }
        if(ringThread->isFinished())
        {
            delete ringThread;
            ringThread = NULL;
        }
        ringThread = NULL;
    }

    labelRingInfo.setText("");
    PathHelper pathhelper;
    QStringList filters;
    filters << "*.mp3" << "*.wav";
    QFileInfoList files = PathHelper::getAllFiles(pathhelper.getRingPath(), filters, false);
    if(!files.isEmpty() && files.count() > 0)
    {
        int index = qrand() % files.count();
        if(index < files.count())
        {
            ringFileName = files.at(index).absoluteFilePath();
            labelRingInfo.setText("ring file: " + ringFileName);

            ringThread = new RingThread();
            connect(ringThread, SIGNAL(finished()), this, SLOT(onRingThreadFinish()));
            ringThread->setFileName(ringFileName);
            ringThread->start();
        }
        else
        {
            labelRingInfo.setText("error ring index");
        }
    }
    else
    {
        labelRingInfo.setText("no ring files");
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
        if(timerFrames % 10 == 0)
        {
            qint64 free = diskHelper.bytesAvailable();//.bytesFree();
            qint64 total = diskHelper.bytesTotal();
            QString diskinfo = QString("Free: %0 / Total: %1").arg(diskHelper.toSizeInfo(free), diskHelper.toSizeInfo(total));
            labelDiskInfo.setText(diskinfo);

            for(int i = 0; i < 4; i++)
            {
                if(i == 0)
                {
                    if(allCameraControls.count() > i)
                        labelCamAInfo.setText(allCameraControls.at(i)->getMessage());
                    else
                        labelCamAInfo.setText("");
                }
                if(i == 1)
                {
                    if(allCameraControls.count() > i)
                        labelCamBInfo.setText(allCameraControls.at(i)->getMessage());
                    else
                        labelCamBInfo.setText("");
                }
                if(i == 2)
                {
                    if(allCameraControls.count() > i)
                        labelCamCInfo.setText(allCameraControls.at(i)->getMessage());
                    else
                        labelCamCInfo.setText("");
                }
                if(i == 3)
                {
                    if(allCameraControls.count() > i)
                        labelCamDInfo.setText(allCameraControls.at(i)->getMessage());
                    else
                        labelCamDInfo.setText("");
                }
            }

            isDark = KeyBoardThread::Init->isDark();
            isPeople = KeyBoardThread::Init->isPeople();
            labelDark.setText(isDark? "Dark" : "Brightness");
            labelPeople.setText(isPeople? "People" : "No People");
            bool lightisturnon = KeyBoardThread::Init->lightIsTurnOn();

            if(isPeople)
            {
                lastPeople = QDateTime::currentDateTime();
            }

            qint64 nopeopleels = QDateTime::currentDateTime().toMSecsSinceEpoch() - lastPeople.toMSecsSinceEpoch();

            if(!isFromTestDark && ui->checkBoxAutoDarkForm->isChecked())
            {
                if((isDark && !isPeople && nopeopleels > 60000)//无光照，超过60秒无人
                        || (!isDark && !isPeople && nopeopleels > 600000))//有光照，超过10分钟无人
                {
                    if(darkForm != NULL && darkForm->isHidden())
                    {
                        darkForm->show();
                        KeyBoardThread::Init->setIsShowingDarkForm(true);
                    }
                }
                else
                {
                    if(darkForm != NULL && (!darkForm->isHidden() && !darkForm->getRequestHide()))
                    {
                        darkForm->hideForm();
                        KeyBoardThread::Init->setIsShowingDarkForm(false);
                    }
                }
            }

            qint64 turnonlight = QDateTime::currentDateTime().toMSecsSinceEpoch() - lastTurnOnLight.toMSecsSinceEpoch();
            if(ui->checkBoxAutoLight->isChecked())
            {
                if((isDark && nopeopleels < 60000) || turnonlight < 20000)//无光，60秒无人，开继电器，继电器至少开启20秒
                {
                    if(!lightisturnon)
                    {
                        KeyBoardThread::Init->setLightIsTurnOn(true);
                        lastTurnOnLight = QDateTime::currentDateTime();
                    }
                }
                else
                {
                    if(lightisturnon)
                    {
                        KeyBoardThread::Init->setLightIsTurnOn(false);
                    }
                }
            }

            if(ui->checkBoxShowTemperature->isChecked())
            {
                if(timeControl != NULL)
                {
                    timeControl->setTemperature(KeyBoardThread::Init->getTemperature());
                }
            }
            ui->pushButtonTurnOnLight->setText(lightisturnon? tr("Turn Off Light") : tr("Turn On Light"));
            labelLight.setText(lightisturnon? tr("Light On") : tr("Light Off"));
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
    lastState = this->windowState();
    if(lastState == Qt::WindowFullScreen)
    {
        this->showMaximized();
    }
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
                    control->setCameraUrl(url);
                    control->setCameraType(form->getCameraType());
                    control->setCameraName(form->getCameraName());
                    control->setCameraSize(form->getCameraSize());
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
    this->setWindowState(lastState);
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
        existsCameraUrls.removeAll(control->getCameraUrl().toLower());
        control->stop();
        control->deleteLater();
        control = Q_NULLPTR;
        camBigIndex = 0;
    }
}

void MainWindow::onKey(int key)
{
    Q_UNUSED(key);
    lastPeople = QDateTime::currentDateTime();//也当做有人
    startNewRingThread();
}

void MainWindow::onRingThreadFinish()
{
    labelRingInfo.setText("");
    if(ringThread != NULL)
    {
        disconnect(ringThread, SIGNAL(finished()), this, SLOT(onRingThreadFinish()));
        ringThread->deleteLater();
        ringThread = NULL;
    }
}

void MainWindow::on_pushButtonTestYUV_clicked()
{
    glesWidget->resize(ui->widgetTestYUV->size());
    glesWidget->PlayOneFrame();
}

void MainWindow::on_pushButtonTestDarkScreen_clicked()
{
    isFromTestDark = true;
    if(darkForm != NULL)
    {
        if(!darkForm->isHidden())
        {
            darkForm->hide();
        }
        else
        {
            darkForm->showNormal();
        }
    }
}

void MainWindow::on_pushButtonClearTestMark_clicked()
{
    isFromTestDark = false;
}

void MainWindow::on_pushButtonTurnOnLight_clicked()
{
    bool lightisturnon = KeyBoardThread::Init->lightIsTurnOn();
    KeyBoardThread::Init->setLightIsTurnOn(!lightisturnon);
}

void MainWindow::on_pushButtonMiniForm_clicked()
{
    this->showMinimized();
}

void MainWindow::on_pushButtonNormalForm_clicked()
{
    this->showNormal();
}

void MainWindow::on_pushButtonFullScreen_clicked()
{
    this->showFullScreen();
}

void MainWindow::on_pushButtonClose_clicked()
{
    this->close();
}

void MainWindow::on_checkBoxShowTemperature_stateChanged(int arg1)
{
    Q_UNUSED(arg1);
    if(timeControl != NULL)
    {
        timeControl->setShowTemperature(ui->checkBoxShowTemperature->isChecked());
    }
}

void MainWindow::on_spinBoxXiaoShuWei_valueChanged(int arg1)
{
    if(timeControl != NULL)
    {
        timeControl->setTemperatureXiaoShuWei(arg1);
    }
}
