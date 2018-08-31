#include "tcpclientthread.h"

TcpClientThread::TcpClientThread(QObject *parent) : QObject(parent)
{
    ip = "127.0.0.1";//"192.168.31.111";
    port = 10086;
    _isConnected = false;
    client = Q_NULLPTR;
    reConnectTimerId = 0;
}

TcpClientThread::~TcpClientThread()
{
    if(reConnectTimerId > 0)
    {
        killTimer(reConnectTimerId);
        reConnectTimerId = 0;
    }
    if(client != Q_NULLPTR)
    {
        if(client->isOpen())
        {
            client->close();
            client->abort();
        }
        client->deleteLater();
    }
}

void TcpClientThread::startRun()
{
    runTcp();
}

void TcpClientThread::setStop()
{
    _isRunning = false;
}

void TcpClientThread::connected()
{
    emit onConnected();
    _isConnected = true;
    qDebug() << "connected";
    client->write("Hey");
    client->flush();
}

void TcpClientThread::disconnected()
{
    emit onDisconnected();
    _isConnected = false;
    qDebug() << "DisConnected";
}

void TcpClientThread::stateChanged(QAbstractSocket::SocketState state)
{
    emit onStateChanged(state);
}

void TcpClientThread::error(QAbstractSocket::SocketError errorMsg)
{
    emit onError(errorMsg);
}

void TcpClientThread::readyRead()
{
    QByteArray array = client->readAll();
    emit onReadyRead(array);
}

int TcpClientThread::getPort() const
{
    return port;
}

void TcpClientThread::setPort(int value)
{
    port = value;
}

QString TcpClientThread::getIp() const
{
    return ip;
}

void TcpClientThread::setIp(const QString &value)
{
    ip = value;
}

void TcpClientThread::runTcp()
{
    client = new QTcpSocket();
    connect(client, SIGNAL(connected()), this, SLOT(connected()));
    connect(client, SIGNAL(disconnected()), this, SLOT(disconnected()));
    connect(client, SIGNAL(stateChanged(QAbstractSocket::SocketState)),
            this, SLOT(stateChanged(QAbstractSocket::SocketState)));
    connect(client, SIGNAL(error(QAbstractSocket::SocketError)),
            this, SLOT(error(QAbstractSocket::SocketError)));
    connect(client, SIGNAL(readyRead()), this, SLOT(readyRead()));
    _isRunning = true;
    reConnectTimerId = startTimer(100);
}

void TcpClientThread::timerEvent(QTimerEvent *event)
{
    if(event->timerId() == reConnectTimerId)
    {
        if(_isRunning && !_isConnected)
        {
            client->connectToHost(ip, port);
            client->waitForConnected();
        }
    }
}
