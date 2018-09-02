#include "dialog.h"
#include "ui_dialog.h"

Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);
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

    tcpThread = new TcpClientThread();
    connect(tcpThread, SIGNAL(onConnected()), this, SLOT(onConnected()));
    connect(tcpThread, SIGNAL(onDisconnected()), this, SLOT(onDisconnected()));
    connect(tcpThread, SIGNAL(onError(QAbstractSocket::SocketError)),
            this, SLOT(onError(QAbstractSocket::SocketError)));
    connect(tcpThread, SIGNAL(onReadyRead(QString)), this, SLOT(onTcpReadyRead(QString)));
    connect(tcpThread, SIGNAL(onStateChanged(QAbstractSocket::SocketState)),
            this, SLOT(onStateChanged(QAbstractSocket::SocketState)));
    connect(tcpThread, SIGNAL(onReadyReadArrray(QByteArray)),
            this, SLOT(onReadyReadArrray(QByteArray)));
    utf8Code = QTextCodec::codecForName("UTF-8");

    this->setWindowFlags(Qt::Window);
    this->showMaximized();
    isPainting = false;
    isCoverMessage = false;

    udpClient = Q_NULLPTR;
    isHideControls = false;
    switchControlVisible();
    connect(ui->label, SIGNAL(clicked()), this, SLOT(onImageLabelClicked()));

    blackBgStyleSheet = QString("QDialog#Dialog{background-color: rgb(0, 0, 0);}");
    whiteBgStyleSheet = QString("QDialog#Dialog{background-color: rgb(255, 255, 255);}");
    ui->label->setText("");
    ui->label->setAlignment(Qt::AlignCenter);
    isClickedImage = false;

    isHengPin = false;
    hengPinSize = QSize(0, 0);
    hengPinBigSize = QSize(0, 0);
    setButtonConnectText();

    checkUdpTimerId = 0;
    lastReceiveUdpData = QDateTime::currentDateTime().addDays(-1);
    isConnected = false;
    canRequestTcp = false;
    isRequestText = true;
    tcpArrayCache.clear();
    udpArrayCache.clear();
}

Dialog::~Dialog()
{
    if(checkUdpTimerId > 0)
    {
        killTimer(checkUdpTimerId);
        checkUdpTimerId = 0;
    }
    if(tcpThread != Q_NULLPTR)
    {
        tcpThread->setStop();
    }
    delete ui;
}

void Dialog::paintEvent(QPaintEvent *)
{
    return;
    if(isPainting)
    {
        return;
    }
    isPainting = true;
    isPainting = false;
}

void Dialog::resizeEvent(QResizeEvent *)
{
    QSize size = QApplication::desktop()->availableGeometry().size();

    if(size.width() > size.height())
    {
        if(!isHengPin)
        {
            isHengPin = true;
            hengPinSize = QSize(0, 0);
            hengPinBigSize = QSize(0, 0);
#ifdef Q_OS_ANDROID
            isHideControls = true;
            ui->widgetCamera->setMaximumSize(size);
            ui->label->setMaximumSize(size);
            switchControlVisible();
#endif
        }
    }
    else
    {
        isHengPin = false;
#ifdef Q_OS_ANDROID
            isHideControls = false;
            ui->widgetCamera->setMaximumSize(size);
            ui->label->setMaximumSize(size);
            switchControlVisible();

#endif
    }
}

void Dialog::timerEvent(QTimerEvent *event)
{
    if(event->timerId() == checkUdpTimerId)
    {
        if(isConnected && canRequestTcp &&
                tcpThread != Q_NULLPTR && tcpThread->getIsRunning())
        {
            int elsp = QDateTime::currentDateTime().toMSecsSinceEpoch() -
                    lastReceiveUdpData.toMSecsSinceEpoch();
            if(elsp < 0 || elsp > 5000)
            {
                tcpArrayCache.clear();
                isRequestText = false;//debug image
                tcpThread->sendText(isRequestText? "ISTCPTURNTEXT" : "ISTCPTURNIMAGE");
                canRequestTcp = false;
                isRequestText = !isRequestText;
            }
        }
    }
}

void Dialog::stopUdpClient()
{
    if(udpClient != Q_NULLPTR)
    {
        udpClient->abort();
        udpClient->deleteLater();
        udpClient = Q_NULLPTR;
    }
}

void Dialog::newUdpClient(QString ip, quint16 port)
{
    udpClient = new QUdpSocket();
    connect(udpClient, SIGNAL(readyRead()), this, SLOT(onReadyRead()));
    if(udpClient->bind(QHostAddress(ip), port))
    {
        ui->labelStatus->setText(tr("bind on ") + ip + " " + QString::number(port));
    }
    else
    {
        qDebug() << udpClient->errorString();
        ui->labelStatus->setText(udpClient->errorString());
        tcpThread->setStop();
    }
}

void Dialog::newUdpClient(qint32 ip, quint16 port)
{
    udpClient = new QUdpSocket();
    connect(udpClient, SIGNAL(readyRead()), this, SLOT(onReadyRead()));
    if(udpClient->bind(QHostAddress(ip), port))
    {
        ui->labelStatus->setText(tr("bind on ") + QString::number(ip, 10) + " "
                                 + QString::number(port));
    }
    else
    {
        qDebug() << udpClient->errorString();
        ui->labelStatus->setText(udpClient->errorString());
        tcpThread->setStop();
    }
}

void Dialog::on_pushButtonConnect_clicked()
{
    canRequestTcp = false;
    if(tcpThread == Q_NULLPTR)
    {
        return;
    }
    if(tcpThread->getIsRunning())
    {
        tcpThread->setStop();
    }
    else
    {
        QString ip = ui->lineEditIP->text().trimmed();
        quint16 port = ui->lineEditPort->text().trimmed().toInt();

        ui->labelStatus->setText(tr("Ready"));

        //return;

        tcpThread->setIp(ip);
        tcpThread->setPort(port);
        tcpThread->startRun();
    }

    setButtonConnectText();

    lastReceiveUdpData = QDateTime::currentDateTime().addDays(-1);
    if(checkUdpTimerId <= 0)
    {
        checkUdpTimerId = startTimer(100);
    }
}

void Dialog::onConnected()
{
    isConnected = true;
    ui->labelStatus->setText(tr("connected"));
    setButtonConnectText();
}

void Dialog::onDisconnected()
{
    isConnected = false;
    ui->labelStatus->setText(tr("disconnected"));
    setButtonConnectText();
}

void Dialog::onStateChanged(QAbstractSocket::SocketState)
{

}

void Dialog::onError(QAbstractSocket::SocketError)
{

}

void Dialog::onTcpReadyRead(QString text)
{
    if(text.isNull() || text.isEmpty())
    {}
    else
    {
        if(text.startsWith("IP@") && text.contains("@IP"))
        {
            QString ip = text.mid(3);
            if(ip.contains("@IP"))
            {
                ip = ip.mid(0, ip.indexOf("@IP"));
                stopUdpClient();
                qint32 ipv4 = ip.toInt();
                newUdpClient(ipv4, 12345);
            }
            canRequestTcp = true;
        }
        else
        {
            //ui->labelStatus->setText(text);
        }
    }
}

void Dialog::onReadyRead(QByteArray array)
{
    if(isCoverMessage)
    {
        return;
    }
    isCoverMessage = true;
    if(array.isNull() || array.isEmpty())
    {

    }
    else
    {
        if(array.startsWith(textHeader) && array.endsWith(textTag))
        {
            QByteArray bytes = array.remove(0, textHeader.length());
            bytes = bytes.remove(bytes.length() - textTag.length(), textTag.length());
            if(bytes.isNull() || bytes.isEmpty())
            {
                return;
            }
            QString text = "";
            if(utf8Code != 0)
            {
                text = utf8Code->toUnicode(bytes);
            }
            else
            {
                text = QString(bytes);
            }
            ui->labelMessage->setText(text);
        }
        else if(array.startsWith(imageHeader) && array.endsWith(imageTag))
        {
            if(ui->label->isVisible())
            {
                QByteArray bytes = array.remove(0, imageHeader.length());
                bytes = bytes.remove(bytes.length() - imageTag.length(), imageTag.length());
                QBuffer buffer(&bytes);
                buffer.open(QIODevice::ReadOnly);
                QImageReader reader(&buffer, "JPG");
                imageCache = reader.read();
                if(!imageCache.isNull())
                {
                    QSize size = QSize(ui->label->width() - 10, ui->label->height() - 10);
                    if(isHengPin)
                    {
                        if(isHideControls)
                        {
                            if(hengPinBigSize.width() < 1 && hengPinBigSize.height() < 1)
                            {
                                hengPinBigSize = size;
                            }
                            size = hengPinBigSize;
                        }
                        else
                        {
                            if(hengPinSize.width() < 1 && hengPinSize.height() < 1)
                            {
                                hengPinSize = size;
                            }
                            size = hengPinSize;
                        }
                    }
                    imageCache = imageCache.scaled(size, Qt::KeepAspectRatio);
                }
                QPixmap pix = QPixmap::fromImage(imageCache);
                if(!pix.isNull())
                {
                    if(ui->label->isVisible())
                    {
                        ui->label->setPixmap(pix);
                        ui->label->update();
                    }
                }
            }
        }
    }
    isCoverMessage = false;
}

void Dialog::onReadyRead()
{
    while(udpClient->hasPendingDatagrams())
    {
        QByteArray datagram;
        //QHostAddress host;
        //quint16 port;
        datagram.resize(udpClient->pendingDatagramSize());
        udpClient->readDatagram(datagram.data(), datagram.size());//, &host, &port);

        if(datagram.contains(imageHeader) || datagram.contains(imageTag))
        {
            qDebug() << "Image";
        }

        if(datagram.startsWith(textHeader) || datagram.startsWith(imageHeader))
        {
            udpArrayCache.clear();
        }
        udpArrayCache.append(datagram);
        if(udpArrayCache.contains(textTag) || udpArrayCache.contains(imageTag))
        {
            onReadyRead(udpArrayCache);
        }
        //qDebug() << host << port;
        lastReceiveUdpData = QDateTime::currentDateTime();
    }
}

void Dialog::switchControlVisible()
{
    isCoverMessage = true;
    ui->widgetConnect->setVisible(!isHideControls);
    ui->labelMessage->setVisible(!isHideControls);
    ui->labelStatus->setVisible(!isHideControls);
    isClickedImage = true;
    this->update();
    isCoverMessage = false;
}

void Dialog::setButtonConnectText()
{
    if(tcpThread == Q_NULLPTR)
    {
        return;
    }
    if(tcpThread->getIsRunning())
    {
        ui->pushButtonConnect->setText(tr("Stop"));
    }
    else
    {
        ui->pushButtonConnect->setText(tr("Start"));
    }
}

void Dialog::onImageLabelClicked()
{
#ifdef Q_OS_ANDROID
    return;
#endif
    isHideControls = !isHideControls;
    switchControlVisible();
}

void Dialog::onReadyReadArrray(QByteArray array)
{
    if(array.isNull() || array.isEmpty())
    {}
    else
    {
        /*
        if(array.startsWith(textHeader))
        {
            if(array.contains(textTag))
            {
                QByteArray tmp = array.mid(0, array.indexOf(textTag) + textTag.length());
                onReadyRead(tmp);
            }
        }
        if(array.startsWith(imageHeader))
        {
            if(array.contains(imageTag))
            {
                QByteArray tmp = array;
                tmp = array.mid(0, array.indexOf(imageTag) + imageTag.length());
                onReadyRead(tmp);
            }
        }
        canRequestTcp = true;
        */
        if(tcpArrayCache.isEmpty())
        {
            if(array.startsWith(textHeader) || array.startsWith(imageHeader))
            {
                tcpArrayCache.append(array);
            }
        }
        else
        {
            tcpArrayCache.append(array);
        }
        if(tcpArrayCache.endsWith(textTag) || tcpArrayCache.endsWith(imageTag))
        {
            onReadyRead(tcpArrayCache);
            canRequestTcp = true;
        }
    }
}
