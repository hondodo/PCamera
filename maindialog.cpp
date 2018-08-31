#include "maindialog.h"
#include "ui_maindialog.h"

MainDialog::MainDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MainDialog)
{
    ui->setupUi(this);
    ui->widgetCameraA->setCameraId(0);
    ui->widgetCameraB->setCameraId(1);
    ui->widgetCameraC->setCameraId(2);
    ui->widgetCameraD->setCameraId(3);

    ui->widgetCameraA->setDefaultText();
    ui->widgetCameraA->startNewCameraThread(0);
    ui->widgetCameraA->setIsFacing(true);
    ui->widgetCameraA->setIsRinging(true);

    ui->widgetCameraB->setDefaultText();

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
}

MainDialog::~MainDialog()
{
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
    int totalhight = ui->widgetMain->height();
    int bigheight = totalhight / 3 * 2;
    ui->widgetCameraA->setMinimumHeight(bigheight);
    ui->widgetCameraA->setMaximumHeight(bigheight);
}

void MainDialog::onImage(const QImage &image)
{
    if(image.isNull() || isConverImage)
    {
        return;
    }
    isConverImage = true;
    int els = QDateTime::currentDateTime().toMSecsSinceEpoch() - lastSendImage.toMSecsSinceEpoch();
    if(els < 0 || els > 200)
    {
        QBuffer buffer;
        buffer.open(QIODevice::ReadWrite);
        image.save(&buffer, "jpg", 10);
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
    client->write("Hello");
    client->flush();
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
        qDebug() << client->readAll();
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
            if(client->isValid() && client->isOpen())
            {
                client->write(array);
                client->flush();
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
            allClient.removeAll(client);
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
