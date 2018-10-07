#include "cameracontrol.h"
#include "ui_cameracontrol.h"

CameraControl::CameraControl(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CameraControl)
{
    ui->setupUi(this);
    cameraType = CAMERATYPE_LOCAL;
    cameraUrl = "video=";
    imageWidth = 640;
    imageHeight = 480;
    cameraName = "";
    checkBrighness = false;
    fixBrighnessByTime = false;
    checkMog = true;
    saveOnlyMog = false;
    player = Q_NULLPTR;
    QPixmapCache::setCacheLimit(1024 * 10);
    menu = NULL;
    removeAction = NULL;
    checkBrighnessAction = NULL;
    fixBrighnessByTimeAction = NULL;
    checkMogAction = NULL;
    saveOnlyMogAction = NULL;
    restartPre30MinAction = NULL;
    restartCameraPre30Min = false;
    lastRestart = QDateTime::currentDateTime();
    lastReceiveImageTime = QDateTime::currentDateTime();
    restartTimerId = startTimer(1000);
    restartTimeElsp = 1000 * 60 * 30;
#ifdef Q_OS_WIN
    restartTimeElsp = 1000 * 60 * 120;
#endif
    initMenu();

    restartByNoImageElsp = 1000 * 60;

    ui->label->installEventFilter(this);
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
}

void CameraControl::disConnectMenu()
{
    disconnect(removeAction, SIGNAL(triggered(bool)), this, SLOT(onMenuClickRemove()));
    disconnect(checkBrighnessAction, SIGNAL(triggered(bool)), this, SLOT(onMenuClickCheckBrighness()));
    disconnect(fixBrighnessByTimeAction, SIGNAL(triggered(bool)), this, SLOT(onMenuClickFixBrighnessbyTime()));
}

CameraControl::~CameraControl()
{
    if(restartTimerId > 0)
    {
        killTimer(restartTimerId);
    }
    stop();
    qDebug() << "Remove camera control";
    delete ui;
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

void CameraControl::start()
{
    stop();
    player = new CameraThreadH264();
    player->setCameraType(cameraType);
    player->setCameraUrl(cameraUrl);
    player->setCameraName(cameraName);
    player->setCheckBrighness(checkBrighness);
    player->setFixBrighnessByTime(fixBrighnessByTime);
    player->setCheckMog(checkMog);
    player->setSaveOnlyMog(saveOnlyMog);
    connect(player, SIGNAL(onFrame(QImage)), this, SLOT(onImage(QImage)));
    player->start();
    lastReceiveImageTime = QDateTime::currentDateTime();
    lastRestart = QDateTime::currentDateTime();
}

void CameraControl::stop()
{
    if(player != Q_NULLPTR)
    {
        player->setStop();
        if(player->isRunning())
        {
            player->wait(10000);
            player->terminate();
        }
        player->deleteLater();
        player = Q_NULLPTR;
    }
}

void CameraControl::onImage(const QImage &image)
{
    lastReceiveImageTime = QDateTime::currentDateTime();
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
        ui->label->setPixmap(QPixmap::fromImage(image.scaled(imageWidth, imageHeight, Qt::KeepAspectRatio)));
    }
    else
    {
        ui->label->setPixmap(QPixmap::fromImage(image));
    }
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
                    start();
                }
            }
        }

        int imageElsp = QDateTime::currentDateTime().toMSecsSinceEpoch() - lastReceiveImageTime.toMSecsSinceEpoch();
        if(imageElsp > restartByNoImageElsp)
        {
            if(player != Q_NULLPTR)
            {
                qDebug() << "Restart camera (no image):" << player->getCameraName();
                stop();
                start();
            }
        }
    }
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
