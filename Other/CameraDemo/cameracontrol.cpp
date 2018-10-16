#include "cameracontrol.h"
#include "ui_cameracontrol.h"

CameraControl::CameraControl(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CameraControl)
{
    ui->setupUi(this);
    cameraType = CAMERATYPE_LOCAL;
    cameraSize = CAMERASIZE_AUTO;
    cameraUrl = "video=";
    imageWidth = 640;
    imageHeight = 480;
    cameraName = "";
    checkBrighness = false;
    fixBrighnessByTime = false;
    checkMog = false;
    saveOnlyMog = false;
    fillScreen = false;
    player = Q_NULLPTR;
    QPixmapCache::setCacheLimit(1024 * 10);
    menu = NULL;
    removeAction = NULL;
    checkBrighnessAction = NULL;
    fixBrighnessByTimeAction = NULL;
    checkMogAction = NULL;
    saveOnlyMogAction = NULL;
    restartPre30MinAction = NULL;
    fillScreenAction = NULL;
    skipFrameAction = NULL;
    skipFrame = false;
    restartCameraPre30Min = false;
    lastRestart = QDateTime::currentDateTime();
    lastReceiveImageTime = QDateTime::currentDateTime();
    restartTimerId = startTimer(1000);
    restartTimeElsp = 1000 * 60 * 30;
#ifdef Q_OS_WIN
    restartTimeElsp = 1000 * 60 * 120;
#endif
    initMenu();

    restartByNoImageElsp = 2 * 1000 * 60;

    ui->label->installEventFilter(this);

#ifdef USE_OPENGL
    glWidget = NULL;
    glWidget = new Gl_widget(1280, 720, this);
    ui->label->setVisible(false);
    ui->scrollArea->setVisible(false);
#endif

    widthOut = 100;
    heightOut = 100;
    pixOut = AV_PIX_FMT_YUV420P;
    pFrameRGB = NULL;
    imgConvertCtcRGB = NULL;
    rgbFmt = AV_PIX_FMT_BGR24;
    rgbBytes = 0;
    rgbOutBuffer = NULL;
    hasNewFrame = false;
    frameTimerId = startTimer(30);
    hasStopThread = true;
    hasInitDecodingRecs = false;

    isDecodingTurn = true;
}

CameraControl::~CameraControl()
{
    if(frameTimerId > 0)
    {
        killTimer(frameTimerId);
        frameTimerId = 0;
    }
    if(restartTimerId > 0)
    {
        killTimer(restartTimerId);
        restartTimerId = 0;
    }
    stop();
    resetDecodingRecs();
#ifdef USE_OPENGL
    if(glWidget != NULL)
    {
        glWidget->deleteLater();
        glWidget = NULL;
    }
#endif
    qDebug() << "Remove camera control";
    delete ui;
}

void CameraControl::initMenu()
{
    if(menu != NULL)
    {
        menu->deleteLater();
        menu = NULL;
    }
    if(removeAction != NULL)
    {
        removeAction->deleteLater();
        removeAction = NULL;
    }
    if(checkBrighnessAction != NULL)
    {
        checkBrighnessAction->deleteLater();
        checkBrighnessAction = NULL;
    }
    if(fixBrighnessByTimeAction != NULL)
    {
        fixBrighnessByTimeAction->deleteLater();
        fixBrighnessByTimeAction = NULL;
    }
    if(checkMogAction != NULL)
    {
        checkMogAction->deleteLater();
        checkMogAction = NULL;
    }
    if(saveOnlyMogAction != NULL)
    {
        saveOnlyMogAction->deleteLater();
        saveOnlyMogAction = NULL;
    }
    if(restartPre30MinAction != NULL)
    {
        restartPre30MinAction->deleteLater();
        restartPre30MinAction = NULL;
    }
    if(fillScreenAction != NULL)
    {
        fillScreenAction->deleteLater();
        fillScreenAction = NULL;
    }
    menu = new QMenu();
    removeAction = menu->addAction("Remove");
    connect(removeAction, SIGNAL(triggered(bool)), this, SLOT(onMenuClickRemove()));
    checkBrighnessAction = menu->addAction("Check Brighness");
    checkBrighnessAction->setCheckable(true);
    checkBrighnessAction->setChecked(checkBrighness);
    connect(checkBrighnessAction, SIGNAL(triggered(bool)), this, SLOT(onMenuClickCheckBrighness()));
    fixBrighnessByTimeAction = menu->addAction("Fix Brighness By Time");
    fixBrighnessByTimeAction->setCheckable(true);
    fixBrighnessByTimeAction->setChecked(fixBrighnessByTime);
    connect(fixBrighnessByTimeAction, SIGNAL(triggered(bool)), this, SLOT(onMenuClickFixBrighnessbyTime()));

    checkMogAction = menu->addAction("Check MOG");
    checkMogAction->setCheckable(true);
    checkMogAction->setChecked(checkMog);
    connect(checkMogAction, SIGNAL(triggered(bool)), this, SLOT(onMenuClickCheckMog()));
    saveOnlyMogAction = menu->addAction("Save Only Mog");
    saveOnlyMogAction->setCheckable(true);
    saveOnlyMogAction->setChecked(saveOnlyMog);
    connect(saveOnlyMogAction, SIGNAL(triggered(bool)), this, SLOT(onMenuClickSaveOnlyMog()));

    restartPre30MinAction = menu->addAction("Restart Camera Pre 30 Min");
#ifdef Q_OS_WIN
    restartPre30MinAction->setText("Restart Camera Pre 30 Min");
#endif
    restartPre30MinAction->setCheckable(true);
    restartPre30MinAction->setChecked(restartCameraPre30Min);
    connect(restartPre30MinAction, SIGNAL(triggered(bool)), this, SLOT(onMenuClickRestartPre30Min()));

    fillScreenAction = menu->addAction("Fill Screen");
    fillScreenAction->setCheckable(true);
    fillScreenAction->setChecked(fillScreen);
    connect(fillScreenAction, SIGNAL(triggered(bool)), this, SLOT(onMenuClickFillScreen()));

    skipFrameAction = menu->addAction("Skip Frame");
    skipFrameAction->setCheckable(true);
    skipFrameAction->setChecked(skipFrame);
    connect(skipFrameAction, SIGNAL(triggered(bool)), this, SLOT(onMenuClickSkipFrame()));
}

void CameraControl::disConnectMenu()
{
    disconnect(removeAction, SIGNAL(triggered(bool)), this, SLOT(onMenuClickRemove()));
    disconnect(checkBrighnessAction, SIGNAL(triggered(bool)), this, SLOT(onMenuClickCheckBrighness()));
    disconnect(fixBrighnessByTimeAction, SIGNAL(triggered(bool)), this, SLOT(onMenuClickFixBrighnessbyTime()));
    //checkMogAction, *saveOnlyMogAction, *restartPre30MinAction, *fillScreenAction;
    disconnect(checkMogAction, SIGNAL(triggered(bool)), this, SLOT(onMenuClickCheckMog()));
    disconnect(saveOnlyMogAction, SIGNAL(triggered(bool)), this, SLOT(onMenuClickSaveOnlyMog()));
    disconnect(restartPre30MinAction, SIGNAL(triggered(bool)), this, SLOT(onMenuClickRestartPre30Min()));
    disconnect(fillScreenAction, SIGNAL(triggered(bool)), this, SLOT(onMenuClickFillScreen()));
    disconnect(skipFrameAction, SIGNAL(triggered(bool)), this, SLOT(onMenuClickSkipFrame()));
}

void CameraControl::resetDecodingRecs()
{
    hasInitDecodingRecs = false;
    widthOut = 100;
    heightOut = 100;
    pixOut = AV_PIX_FMT_YUV420P;

    if(pFrameRGB != NULL)
    {
        av_frame_free(&pFrameRGB);
    }
    pFrameRGB = NULL;
    if(imgConvertCtcRGB != NULL)
    {
        sws_freeContext(imgConvertCtcRGB);
    }
    imgConvertCtcRGB = NULL;
    rgbFmt = AV_PIX_FMT_BGR24;
    rgbBytes = 0;
    if(rgbOutBuffer != NULL)
    {
        av_free(rgbOutBuffer);
    }
    rgbOutBuffer = NULL;
    if(!mRGB.empty())
    {
        mRGB.release();
    }
    if(!temp.empty())
    {
        temp.release();
    }
}

void CameraControl::initDecodingRecs()
{
    pFrameRGB = av_frame_alloc();
    imgConvertCtcRGB = sws_getContext(widthOut, heightOut,
                                      pixOut, widthOut, heightOut,
                                      rgbFmt, SWS_FAST_BILINEAR, NULL, NULL, NULL);
    rgbBytes = avpicture_get_size(rgbFmt, widthOut, heightOut);
    rgbOutBuffer = (uint8_t *) av_malloc(rgbBytes * sizeof(uint8_t));
    avpicture_fill((AVPicture *)pFrameRGB, rgbOutBuffer, rgbFmt,
                   widthOut, heightOut);

    mRGB = cv::Mat(cv::Size(widthOut, heightOut), CV_8UC3);
    hasInitDecodingRecs = true;
}

void CameraControl::decodeFrameAndShow(AVFrame **filtedFrame)
{
    if(!KeyBoardThread::Init->isShowingDarkForm())
    {
        if(!skipFrame || isDecodingTurn)
        {
            sws_scale(imgConvertCtcRGB, (uint8_t const * const *) (*filtedFrame)->data,
                      (*filtedFrame)->linesize, 0, heightOut, pFrameRGB->data,
                      pFrameRGB->linesize);

            mRGB.data =(uchar*)pFrameRGB->data[0];
            cv::cvtColor(mRGB, temp, CV_BGR2RGB);

            QImage dest((uchar*) temp.data, temp.cols, temp.rows, temp.step, QImage::Format_RGB888);
            QImage image(dest);
            image.detach();
            onImage(image);
        }
        isDecodingTurn = !isDecodingTurn;
    }
    av_frame_free(filtedFrame);
}

QString CameraControl::getCameraUrl() const
{
    return cameraUrl;
}

void CameraControl::setCameraUrl(const QString &value)
{
    cameraUrl = value;
}

CAMERATYPE CameraControl::getCameraType() const
{
    return cameraType;
}

void CameraControl::setCameraType(const CAMERATYPE &value)
{
    cameraType = value;
}

void CameraControl::start(bool delayOpenCamera)
{
    stop();
#ifdef USE_H264
    player = new CameraThreadH264();
#else
    player = new CameraThreadMUX();
#endif
    player->setDelayOpenCamera(delayOpenCamera);
    connect(player, SIGNAL(onMessage(QString)), this, SLOT(onMessage(QString)));
    player->setCameraType(cameraType);
    player->setCameraSize(cameraSize);
    player->setCameraUrl(cameraUrl);
    player->setCameraName(cameraName);
    player->setCheckBrighness(checkBrighness);
    player->setFixBrighnessByTime(fixBrighnessByTime);
    player->setCheckMog(checkMog);
    player->setSaveOnlyMog(saveOnlyMog);
    //connect(player, SIGNAL(onFrame(QImage)), this, SLOT(onImage(QImage)));
    connect(player, SIGNAL(onFrame()), this, SLOT(onFrame()));
    connect(player, SIGNAL(onStartRecoing(int,int,int)), this, SLOT(onStartRecoing(int,int,int)));
    connect(player, SIGNAL(onStopRecoding()), this, SLOT(onStopRecoding()));
    player->start();
    lastReceiveImageTime = QDateTime::currentDateTime();
    lastRestart = QDateTime::currentDateTime();
#ifdef USE_OPENGL
    if(glWidget != NULL)
    {
        connect(player, SIGNAL(onYUVFrame(const unsigned char*,const unsigned char*,const unsigned char*)),
                this, SLOT(onYUVFrame(const unsigned char*,const unsigned char*,const unsigned char*)));
    }
#endif
    hasStopThread = false;
}

void CameraControl::stop()
{
    hasStopThread = true;
    if(player != Q_NULLPTR)
    {
        qDebug() << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss")
                 << "stop thread:" << cameraName;
        player->setStop();
        if(player->isRunning())
        {
            qDebug() << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss")
                     << "thread is running:" << cameraName;
            player->wait(10000);
            qDebug() << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss")
                     << "terminate thread:" << cameraName;
            player->terminate();
        }
         qDebug() << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss")
                  << "thread delete later:" << cameraName;
        player->deleteLater();
        player = Q_NULLPTR;
    }
}

void CameraControl::onImage(const QImage &image)
{
    lastReceiveImageTime = QDateTime::currentDateTime();

    if(KeyBoardThread::Init->isShowingDarkForm())
    {
        return;
    }

    if(image.isNull())
    {
        return;
    }
    if(!this->isVisible() || imageWidth < 10 || imageHeight < 10 || imageWidth > 2000 || imageHeight > 2000)
    {
        return;
    }
    if(image.width() > imageWidth || image.height() > imageHeight)
    {
        if(!fillScreen)
        {
            ui->label->setPixmap(QPixmap::fromImage(image.scaled(imageWidth, imageHeight, Qt::KeepAspectRatio)));
        }
        else
        {
            ui->label->setPixmap(QPixmap::fromImage(image.scaled(imageWidth, imageHeight, Qt::IgnoreAspectRatio)));
        }
    }
    else
    {
        ui->label->setPixmap(QPixmap::fromImage(image));
    }
}

void CameraControl::onFrame()
{
    lastReceiveImageTime = QDateTime::currentDateTime();
    hasNewFrame = true;
}

bool CameraControl::getFixBrighnessByTime() const
{
    return fixBrighnessByTime;
}

void CameraControl::setFixBrighnessByTime(bool value)
{
    fixBrighnessByTime = value;
}

bool CameraControl::eventFilter(QObject *watched, QEvent *event)
{
    if(event->type() == QEvent::ContextMenu)
    {
        if(watched == ui->label)
        {
            on_label_customContextMenuRequested(QCursor::pos());
            return true;
        }
    }
    return QWidget::eventFilter(watched, event);
}

void CameraControl::timerEvent(QTimerEvent *event)
{
    if(event->timerId() == restartTimerId)
    {
        int mels = QDateTime::currentDateTime().toMSecsSinceEpoch() - lastRestart.toMSecsSinceEpoch();
        if(mels > restartTimeElsp)
        {
            lastRestart = QDateTime::currentDateTime();
            if(restartCameraPre30Min)
            {
                if(player != Q_NULLPTR && player->isRunning())
                {
                    qDebug() << "Restart camera (timerly):" << player->getCameraName();
                    stop();
                    start(true);
                }
            }
        }

        int imageElsp = QDateTime::currentDateTime().toMSecsSinceEpoch() - lastReceiveImageTime.toMSecsSinceEpoch();
        if(imageElsp > restartByNoImageElsp)
        {
            lastReceiveImageTime = QDateTime::currentDateTime();
            if(player != Q_NULLPTR)
            {
                qDebug() << "Restart camera (no image):" << player->getCameraName();
                onMessage("Restart camera (no image) @" + player->getCameraName());
                stop();
                start(true);
            }
        }
    }
    if(event->timerId() == frameTimerId)
    {
        if(hasInitDecodingRecs)
        {
            if(hasNewFrame && !hasStopThread && player != NULL && player->isRunning())
            {
                AVFrame **filtedFrame = player->getFiltedFrame();
                if(filtedFrame != NULL && (*filtedFrame) != NULL)
                {
                    decodeFrameAndShow(filtedFrame);
                }
                hasNewFrame = false;
            }
        }
    }
}

void CameraControl::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event);
#ifdef USE_OPENGL
    if(glWidget != NULL)
    {
        glWidget->setGeometry(0, 0, videoWidth(), videoHeight());
    }
#endif
}

bool CameraControl::getCheckBrighness() const
{
    return checkBrighness;
}

void CameraControl::setCheckBrighness(bool value)
{
    checkBrighness = value;
}

int CameraControl::getImageHeight() const
{
    return imageHeight;
}

void CameraControl::setImageHeight(int value)
{
    imageHeight = value > 900? 900 : value;
}

int CameraControl::getImageWidth() const
{
    return imageWidth;
}

void CameraControl::setImageWidth(int value)
{
    imageWidth = value > 1400? 1400 : value;
}

QString CameraControl::getCameraName() const
{
    return cameraName;
}

void CameraControl::setCameraName(const QString &value)
{
    cameraName = value;
    ui->label->setText("Camera:" + cameraName);
}

void CameraControl::on_label_customContextMenuRequested(const QPoint &pos)
{
    Q_UNUSED(pos);
    initMenu();
    menu->exec(QCursor::pos());
}

void CameraControl::onMenuClickRemove()
{
    disConnectMenu();
    emit onRemoveRequest();
}

void CameraControl::onMenuClickCheckBrighness()
{
    disConnectMenu();
    checkBrighness = !checkBrighness;
    checkBrighnessAction->setChecked(checkBrighness);
    if(player != NULL)
    {
        player->setCheckBrighness(checkBrighness);
    }
}

void CameraControl::onMenuClickFixBrighnessbyTime()
{
    disConnectMenu();
    fixBrighnessByTime = !fixBrighnessByTime;
    fixBrighnessByTimeAction->setChecked(fixBrighnessByTime);
    if(player != NULL)
    {
        player->setFixBrighnessByTime(fixBrighnessByTime);
    }
}

void CameraControl::onMenuClickCheckMog()
{
    disConnectMenu();
    checkMog = !checkMog;
    checkMogAction->setChecked(checkMog);
    if(player != NULL)
    {
        player->setCheckMog(checkMog);
    }
}

void CameraControl::onMenuClickSaveOnlyMog()
{
    disConnectMenu();
    saveOnlyMog = !saveOnlyMog;
    saveOnlyMogAction->setChecked(saveOnlyMog);
    if(player != NULL)
    {
        player->setSaveOnlyMog(saveOnlyMog);
    }
}

void CameraControl::onMenuClickRestartPre30Min()
{
    disConnectMenu();
    restartCameraPre30Min = !restartCameraPre30Min;
}

void CameraControl::onMenuClickFillScreen()
{
    disConnectMenu();
    fillScreen = !fillScreen;
}

void CameraControl::onMenuClickSkipFrame()
{
    disConnectMenu();
    skipFrame = !skipFrame;
}

void CameraControl::onStartRecoing(int width, int height, int pixOut)
{
    if(!hasInitDecodingRecs)//only init once, so cannot change thread's width height and pix-format
    {
        widthOut = width;
        heightOut = height;
        this->pixOut = (AVPixelFormat)pixOut;
        initDecodingRecs();
    }
}

void CameraControl::onStopRecoding()
{
    hasStopThread = true;
}

void CameraControl::onMessage(const QString text)
{
    setMessage(text);
}

QString CameraControl::getMessage() const
{
    return message;
}

void CameraControl::setMessage(const QString &value)
{
    message = value;
}

CAMERASIZE CameraControl::getCameraSize() const
{
    return cameraSize;
}

void CameraControl::setCameraSize(const CAMERASIZE &value)
{
    cameraSize = value;
}

#ifdef USE_OPENGL
void CameraControl::onYUVFrame(const unsigned char *y_data, const unsigned char *u_data, const unsigned char *v_data)
{
    if(glWidget != NULL)
    {
        glWidget->onYUVFrame(y_data, u_data, v_data);
    }

}
#endif

bool CameraControl::getRestartCameraPre30Min() const
{
    return restartCameraPre30Min;
}

void CameraControl::setRestartCameraPre30Min(bool value)
{
    restartCameraPre30Min = value;
}

bool CameraControl::getSaveOnlyMog() const
{
    return saveOnlyMog;
}

void CameraControl::setSaveOnlyMog(bool value)
{
    saveOnlyMog = value;
}

bool CameraControl::getCheckMog() const
{
    return checkMog;
}

void CameraControl::setCheckMog(bool value)
{
    checkMog = value;
}
