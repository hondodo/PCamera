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
    saveOnlyMog = true;
    player = Q_NULLPTR;
    QPixmapCache::setCacheLimit(1024 * 10);
    menu = NULL;
    removeAction = NULL;
    checkBrighnessAction = NULL;
    fixBrighnessByTimeAction = NULL;
    checkMogAction = NULL;
    saveOnlyMogAction = NULL;
    initMenu();

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
    menu = new QMenu();
    removeAction = menu->addAction("Remove");
    removeAction->setData("remove");
    connect(removeAction, SIGNAL(triggered(bool)), this, SLOT(onMenuClickRemove()));
    checkBrighnessAction = menu->addAction("Check Brighness");
    checkBrighnessAction->setData("checkbrighness");
    checkBrighnessAction->setCheckable(true);
    checkBrighnessAction->setChecked(checkBrighness);
    connect(checkBrighnessAction, SIGNAL(triggered(bool)), this, SLOT(onMenuClickCheckBrighness()));
    fixBrighnessByTimeAction = menu->addAction("Fix Brighness By Time");
    fixBrighnessByTimeAction->setData("fixbrighnessbytime");
    fixBrighnessByTimeAction->setCheckable(true);
    fixBrighnessByTimeAction->setChecked(fixBrighnessByTime);
    connect(fixBrighnessByTimeAction, SIGNAL(triggered(bool)), this, SLOT(onMenuClickFixBrighnessbyTime()));

    checkMogAction = menu->addAction("Check MOG");
    checkMogAction->setData("fixbrighnessbytime");
    checkMogAction->setCheckable(true);
    checkMogAction->setChecked(checkMog);
    connect(checkMogAction, SIGNAL(triggered(bool)), this, SLOT(onMenuClickCheckMog()));
    saveOnlyMogAction = menu->addAction("Save Only Mog");
    saveOnlyMogAction->setData("fixbrighnessbytime");
    saveOnlyMogAction->setCheckable(true);
    saveOnlyMogAction->setChecked(saveOnlyMog);
    connect(saveOnlyMogAction, SIGNAL(triggered(bool)), this, SLOT(onMenuClickSaveOnlyMog()));
}

void CameraControl::disConnectMenu()
{
    disconnect(removeAction, SIGNAL(triggered(bool)), this, SLOT(onMenuClickRemove()));
    disconnect(checkBrighnessAction, SIGNAL(triggered(bool)), this, SLOT(onMenuClickCheckBrighness()));
    disconnect(fixBrighnessByTimeAction, SIGNAL(triggered(bool)), this, SLOT(onMenuClickFixBrighnessbyTime()));
}

CameraControl::~CameraControl()
{
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
    player = new CameraThreadMUX();
    player->setCameraType(cameraType);
    player->setCameraUrl(cameraUrl);
    player->setCameraName(cameraName);
    player->setCheckBrighness(checkBrighness);
    player->setFixBrighnessByTime(fixBrighnessByTime);
    player->setCheckMog(checkMog);
    player->setSaveOnlyMog(saveOnlyMog);
    connect(player, SIGNAL(onFrame(QImage)), this, SLOT(onImage(QImage)), Qt::DirectConnection);
    player->start();
}

void CameraControl::stop()
{
    if(player != Q_NULLPTR)
    {
        player->setStop();
        if(player->isRunning())
        {
            player->wait(3000);
            player->terminate();
        }
        player->deleteLater();
        player = Q_NULLPTR;
    }
}

void CameraControl::onImage(const QImage &image)
{
    if(image.isNull())
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
