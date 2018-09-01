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
    connect(tcpThread, SIGNAL(onReadyRead(QByteArray)), this, SLOT(onReadyRead(QByteArray)));
    connect(tcpThread, SIGNAL(onStateChanged(QAbstractSocket::SocketState)),
            this, SLOT(onStateChanged(QAbstractSocket::SocketState)));
    utf8Code = QTextCodec::codecForName("UTF-8");

    this->setWindowFlags(Qt::Window);
    this->showMaximized();
    isPainting = false;
    isCoverMessage = false;

    udpClient = Q_NULLPTR;
}

Dialog::~Dialog()
{
    if(tcpThread != Q_NULLPTR)
    {
        tcpThread->setStop();
    }
    delete ui;
}

void Dialog::paintEvent(QPaintEvent *)
{return;
    if(isPainting)
    {
        return;
    }
    isPainting = true;
    if(imageCache.isNull())
    {}
    else
    {
        int w = imageCache.width();
        int h = imageCache.height();
        if(w > 1000 || h > 1000 || h < 1 || w < 1)
        {
            return;
        }
        QPainter painter(this);
        int ww = ui->widgetCamera->width();
        int wh = ui->widgetCamera->height();
        if(ww > 2000 && wh > 2000)
        {
            ui->widgetCamera->resize(800, 600);
            ww = 800;
            wh = 600;
        }
        QImage image = imageCache.scaled(ww, wh, Qt::KeepAspectRatio);
        int x = ui->widgetCamera->x() + (ww - image.width()) / 2;
        int y = ui->widgetCamera->y() + (wh - image.height()) / 2;
        painter.drawImage(x, y, image);
    }
    isPainting = false;
}

void Dialog::on_pushButtonConnect_clicked()
{
    QString ip = ui->lineEditIP->text().trimmed();
    quint16 port = ui->lineEditPort->text().trimmed().toInt();

    ui->labelStatus->setText(tr("Ready"));

    if(udpClient != Q_NULLPTR)
    {
        udpClient->abort();
        udpClient->deleteLater();
        udpClient = Q_NULLPTR;
    }

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
    }

    return;

    tcpThread->setIp(ip);
    tcpThread->setPort(port);
    tcpThread->startRun();
}

void Dialog::onConnected()
{
    ui->labelStatus->setText(tr("connected"));
}

void Dialog::onDisconnected()
{
    ui->labelStatus->setText(tr("disconnected"));
}

void Dialog::onStateChanged(QAbstractSocket::SocketState)
{

}

void Dialog::onError(QAbstractSocket::SocketError)
{

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
        else if(array.startsWith(imageHeader))
        {
            QByteArray bytes = array.remove(0, imageHeader.length());
            bytes = bytes.remove(bytes.length() - imageTag.length(), imageTag.length());
            QBuffer buffer(&bytes);
            buffer.open(QIODevice::ReadOnly);
            QImageReader reader(&buffer, "JPG");
            imageCache = reader.read();
            //this->update();
            if(!imageCache.isNull())
            {
                imageCache = imageCache.scaled(ui->label->width() - 10,
                                               ui->label->height() - 10,
                                               Qt::KeepAspectRatio);
            }
            QPixmap pix = QPixmap::fromImage(imageCache);
            if(!pix.isNull())
            {
                ui->label->setPixmap(pix);
                ui->label->update();
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
        onReadyRead(datagram);
        //qDebug() << host << port;
    }
}


void Dialog::on_pushButtonSend_clicked()
{
    QString ip = ui->lineEditSendIp->text().trimmed();
    quint16 port = ui->lineEditSendPort->text().trimmed().toInt();
    udpClient->writeDatagram("Hello camera", QHostAddress(ip), port);
}
