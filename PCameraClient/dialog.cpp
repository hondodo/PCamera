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
}

Dialog::~Dialog()
{
    if(tcpThread != Q_NULLPTR)
    {
        tcpThread->setStop();
    }
    delete ui;
}

void Dialog::paintEvent(QPaintEvent *event)
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
        QImage image = imageCache.scaled(640, 480, Qt::KeepAspectRatio);
        int x = ui->widgetCamera->x();
        int y = ui->widgetCamera->y();
        painter.drawImage(x, y, image);
    }
    isPainting = false;
}

void Dialog::on_pushButtonConnect_clicked()
{
    QString ip = ui->lineEditIP->text().trimmed();
    int port = ui->lineEditPort->text().trimmed().toInt();
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
            this->update();
            QPixmap pix = QPixmap::fromImage(imageCache);
            ui->label->setPixmap(pix);
            ui->label->update();
        }
    }
    isCoverMessage = false;
}

