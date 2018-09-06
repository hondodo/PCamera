#include "maindialog.h"
#include "ui_maindialog.h"

MainDialog::MainDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MainDialog)
{
    ui->setupUi(this);
    isFirstShow = true;

    ui->widgetCameraA->setCameraId(0);
    ui->widgetCameraB->setCameraId(1);
    ui->widgetCameraC->setCameraId(2);
    ui->widgetCameraD->setCameraId(3);

    ui->widgetCameraA->setDefaultText();
    ui->widgetCameraA->startNewCameraThread(0);
    ui->widgetCameraA->setIsFacing(false);
    ui->widgetCameraA->setIsRinging(true);

    ui->widgetCameraB->setDefaultText();

    ui->widgetBtm->setVisible(false);

    connect(ui->widgetCameraA, SIGNAL(onImage(QImage)),
            this, SLOT(onImage(QImage)));
    connect(ui->widgetCameraA, SIGNAL(onTip(QString)),
            this, SLOT(onTip(QString)));

    this->setWindowFlags(Qt::Window);
    this->showMaximized();

    textHeader.clear();
    imageHeader.clear();
    textHeader.append('1');
    textHeader.append('0');
    textHeader.append('0');
    textHeader.append('8');
    textHeader.append('6');
    imageHeader.append('1');
    imageHeader.append('0');
    imageHeader.append('0');
    imageHeader.append('1');
    imageHeader.append('0');
    textTag.append('6');
    textTag.append('8');
    textTag.append('0');
    textTag.append('0');
    textTag.append('1');
    imageTag.append('0');
    imageTag.append('1');
    imageTag.append('0');
    imageTag.append('0');
    imageTag.append('1');

    utf8Code = QTextCodec::codecForName("UTF-8");

    oledThread = new OLedThread();
    oledThread->start();
    //CameraCollectorThread::Init->start();
    server = new QTcpServer();
    connect(server, SIGNAL(newConnection()), this, SLOT(newConnection()));
    connect(server, SIGNAL(acceptError(QAbstractSocket::SocketError)),
            this, SLOT(acceptError(QAbstractSocket::SocketError)));
    if(!server->listen(QHostAddress::Any, 10010))
    {
        qDebug() << server->errorString();
    }
    else
    {
        qDebug() << "listen on port: 10010";
    }
    lastSendImage = QDateTime::currentDateTime();
    isConverImage = false;
    isSending = false;

    udpPort = 12345;
    udpServer = new QUdpSocket();
    //connect(udpServer, SIGNAL(readyRead()), this, SLOT(onReadyRead()));
    udpServer->bind(udpPort, QUdpSocket::ShareAddress);
    clientBeatTime.clear();
    isTcpTurn.clear();
    isRequestText.clear();
    showIndex = 0;
    isTimeTurn = false;
    isFirstShowTime = true;
    ui->widgetTime->setVisible(false);
    ui->widgetWeather->setVisible(false);
    nextUpdateWeatherTime = QDateTime::currentDateTime();
    nextTipTime = QDateTime::currentDateTime().addMSecs(1000 * 60 * 60).time().hour();
    ringThread = Q_NULLPTR;
}

MainDialog::~MainDialog()
{
    deleteRingThread();
    if(switchTimeId > 0)
    {
        killTimer(switchTimeId);
        switchTimeId = 0;
    }
    server->close();
    server->deleteLater();
    if(oledThread != NULL)
    {
        oledThread->setStop();
        oledThread->wait(1000);
        oledThread->terminate();
        oledThread = NULL;
    }
    delete ui;
}

void MainDialog::resizeEvent(QResizeEvent *)
{
    return;
    int totalhight = this->height();
    int bigheight = totalhight / 3 * 2;
    ui->widgetCameraA->setMinimumHeight(bigheight);
    ui->widgetCameraA->setMaximumHeight(bigheight);
}

void MainDialog::showEvent(QShowEvent *)
{
    if(isFirstShow)
    {
        isFirstShow = false;
        ui->widgetTime->start();
        ui->widgetWeather->startDateTime();
        switchTimeId = startTimer(500);
    }
}

void MainDialog::deleteRingThread()
{
    if(ringThread != Q_NULLPTR)
    {
        ringThread->setStop();
        ringThread->wait(1000);
        ringThread->terminate();
        ringThread = Q_NULLPTR;
    }
}

void MainDialog::startNewRingThread(QString filename)
{
    if(QDateTime::currentDateTime().time().hour() > 6 &&
            QDateTime::currentDateTime().time().hour() < 19)
    {
        ringThread = new RingThread();
        ringThread->setFileName(filename);
        ringThread->start();
    }
}

void MainDialog::timerEvent(QTimerEvent *event)
{
    //TipTime
    {
        if(nextTipTime == QDateTime::currentDateTime().time().hour())
        {
            QString filename = QString::number(nextTipTime, 'f', 0) + ".mp3";
            nextTipTime = QDateTime::currentDateTime().addMSecs(1000 * 60 * 60).time().hour();
#ifdef Q_OS_WIN
            filename = "D:/Potatokid/Source/PCamera/TimeMp3/" + filename;
#else
            filename = "/home/pi/Music/TimeMp3/" + filename;
#endif
            if(nextTipTime >= 6 && nextTipTime <= 21)
            {
                startNewRingThread(filename);
            }
        }
    }
    if(event->timerId() == switchTimeId)
    {
        if((QDateTime::currentDateTime().toMSecsSinceEpoch()
                - nextUpdateWeatherTime.toMSecsSinceEpoch()) > 0)
        {
            QDateTime time = QDateTime::currentDateTime();
            nextUpdateWeatherTime = time.addMSecs(60 * 30 * 1000);
            ui->widgetWeather->updateWeather();
        }
        if(ui->widgetCameraA->getIsRecording())
        {
            ui->widgetTime->setVisible(false);
            ui->widgetWeather->setVisible(false);
            ui->widgetCameraA->setVisible(true);
        }
        else
        {
            if(showIndex % 10 == 0)
            {
                if(isTimeTurn && isFirstShowTime)
                {
                    isFirstShowTime = false;
                }
                else
                {
                    isTimeTurn = !isTimeTurn;
                    if(isTimeTurn)
                    {
                        isFirstShowTime = true;
                    }
                }
            }
            if(ui->widgetWeather->getCanShowLive() || ui->widgetWeather->getCanShowReport())
            {
                if(isTimeTurn)
                {
                    if(isFirstShowTime)
                    {
                        ui->widgetWeather->setVisible(false);
                        ui->widgetTime->setVisible(true);
                    }
                    else
                    {
                        if(!ui->widgetWeather->isVisible())
                        {
                            ui->widgetWeather->switchView();
                        }
                        ui->widgetWeather->setVisible(true);
                        ui->widgetTime->setVisible(false);
                    }
                }
                else
                {
                    ui->widgetWeather->setVisible(false);
                    ui->widgetTime->setVisible(false);
                }
            }
            else
            {
                ui->widgetTime->setVisible(isTimeTurn);
            }
            ui->widgetCameraA->setVisible(!isTimeTurn);
            showIndex++;
            if(showIndex > 100000)
            {
                showIndex = 0;
            }
        }
    }
}

void MainDialog::onImage(const QImage &image)
{
    if(image.isNull() || isConverImage)
    {
        return;
    }
    isConverImage = true;
    int els = QDateTime::currentDateTime().toMSecsSinceEpoch() - lastSendImage.toMSecsSinceEpoch();
    if(els < 0 || els > 100)
    {
        QBuffer buffer;
        buffer.open(QIODevice::ReadWrite);
        int quality = 60;
        if(image.width() > 640 || image.height() > 480)
        {
            QImage tmpimg = image.scaled(640, 480, Qt::KeepAspectRatio);
            tmpimg.save(&buffer, "jpg", quality);
        }
        else
        {
            image.save(&buffer, "jpg", quality);
        }
        QByteArray pixArray;
        pixArray.append(imageHeader);
        pixArray.append(buffer.data());
        pixArray.append(imageTag);
        sendMessage(pixArray);
        lastSendImage = QDateTime::currentDateTime();
    }
    isConverImage = false;
}

void MainDialog::onTip(QString message)
{
    if(message.isNull() || message.isEmpty())
    {
        return;
    }
    oledThread->setMessage(message);
    //qDebug() << message;
    QByteArray array;
    if(utf8Code != 0)
    {
        array = utf8Code->fromUnicode(message);
    }
    else
    {
        array = message.toUtf8();
    }
    array = textHeader + array + textTag;
    sendMessage(array);
}

void MainDialog::newConnection()
{
    qDebug() << "new client";
    QTcpSocket *client = server->nextPendingConnection();
    allClient.append(client);
    connect(client, SIGNAL(readyRead()), this, SLOT(readyRead()));
    connect(client, SIGNAL(disconnected()), this, SLOT(disconnected()));
    QString ip = QString::number(client->peerAddress().toIPv4Address(), 10);
    ip = "IP@" + ip + "@IP";
    client->write(ip.toUtf8());
    client->flush();
    qDebug() << client->peerAddress() << client->peerPort();
    clientBeatTime[client] = QDateTime::currentDateTime();
}

void MainDialog::acceptError(QAbstractSocket::SocketError socketError)
{
    Q_UNUSED(socketError);
}

void MainDialog::readyRead()
{
    QTcpSocket *client = qobject_cast<QTcpSocket *>(QObject::sender());
    if(client != Q_NULLPTR)
    {
        clientBeatTime[client] = QDateTime::currentDateTime();
        QByteArray array = client->readAll();
        if(array.isNull() || array.isEmpty())
        {
            return;
        }
        QString text = "";
        if(utf8Code != 0)
        {
            text = utf8Code->toUnicode(array);
        }
        else
        {
            text = QString(array);
        }
        if(text.contains("ISTCPTURNTEXT"))
        {
            isTcpTurn[client] = true;
            isRequestText[client] = true;
        }
        else if(text.contains("ISTCPTURNIMAGE"))
        {
            isTcpTurn[client] = true;
            isRequestText[client] = false;
        }
        else
        {
        }
    }
}

void MainDialog::disconnected()
{
    QTcpSocket *client = qobject_cast<QTcpSocket *>(QObject::sender());
    if(client != Q_NULLPTR)
    {
        allClient.removeAll(client);
        client->deleteLater();
    }
    qDebug() << "a client disconnected";
}

void MainDialog::onReadyRead()
{
    while(udpServer->hasPendingDatagrams())
    {
        QByteArray datagram;
        QHostAddress add;
        quint16 port;
        datagram.resize(udpServer->pendingDatagramSize());
        udpServer->readDatagram(datagram.data(), datagram.size(), &add, &port);
        qDebug() << datagram << add << port;
    }
}

void MainDialog::sendMessage(QByteArray array)
{
    if(isSending)
    {
        return;
    }
    isSending = true;
    QList<QTcpSocket *> nullclient;
    if(!allClient.isEmpty())
    {
        int count = allClient.count();
        for(int i = 0; i < count; i++)
        {
            QTcpSocket *client = allClient.at(i);

            if(!clientBeatTime.contains(client))
            {
                nullclient.append(client);
                continue;
            }
            QDateTime lasttime = clientBeatTime[client];
            int beatels = QDateTime::currentDateTime().toMSecsSinceEpoch() - lasttime.toMSecsSinceEpoch();
            if(beatels > 60000 || beatels < 0)
            {
                nullclient.append(client);
                continue;
            }

            if(client->isValid() && client->isOpen())
            {
                udpServer->writeDatagram(array, QHostAddress(client->peerAddress().toIPv4Address()), udpPort);

                if(isTcpTurn.contains(client) && isTcpTurn[client])
                {
                    if(isRequestText[client] && array.startsWith(textHeader))
                    {
                        client->write(array);
                        client->flush();
                    }
                    else if(!isRequestText[client] && array.startsWith(imageHeader))
                    {
                        client->write(array);
                        client->flush();
                    }
                    isTcpTurn[client] = false;
                }
            }
            else
            {
                nullclient.append(client);
            }
        }
    }
    if(!nullclient.isEmpty())
    {
        int count = nullclient.count();
        for(int i = 0; i < count; i++)
        {
            QTcpSocket *client = allClient.at(i);
            clientBeatTime.remove(client);
            allClient.removeAll(client);
            isTcpTurn.remove(client);
            isRequestText.remove(client);
            if(client->isValid())
            {
                client->close();
            }
            client->abort();
            client = Q_NULLPTR;
        }
    }
    isSending = false;
}
