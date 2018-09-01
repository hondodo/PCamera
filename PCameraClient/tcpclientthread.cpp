#include "tcpclientthread.h"

TcpClientThread::TcpClientThread(QObject *parent) : QObject(parent)
{
    ip = "127.0.0.1";//"192.168.31.111";
    port = 10086;
    _isConnected = false;
    _isRunning = false;
    client = Q_NULLPTR;
    reConnectTimerId = 0;
    beatIndex = 0;
    utf8Code = QTextCodec::codecForName("UTF-8");
}

TcpClientThread::~TcpClientThread()
{
    if(reConnectTimerId > 0)
    {
        killTimer(reConnectTimerId);
        reConnectTimerId = 0;
    }
    setStop();
}

void TcpClientThread::startRun()
{
    runTcp();
}

void TcpClientThread::setStop()
{
    _isRunning = false;
    if(client != Q_NULLPTR)
    {
        if(client->isOpen())
        {
            client->close();
            client->abort();
        }
        client->deleteLater();
        client = Q_NULLPTR;
    }
    emit onDisconnected();
}

void TcpClientThread::connected()
{
    emit onConnected();
    _isConnected = true;
    qDebug() << "connected";
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
    if(array.isNull() || array.isEmpty())
    {}
    else
    {
        QString text = "";
        if(utf8Code != 0)
        {
            text = utf8Code->toUnicode(array);
        }
        else
        {
            text = QString(array);
        }
        emit onReadyRead(text);
    }
}

bool TcpClientThread::getIsRunning() const
{
    return _isRunning;
}

void TcpClientThread::setIsRunning(bool isRunning)
{
    _isRunning = isRunning;
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

void TcpClientThread::sendText(QString text)
{
    if(client != Q_NULLPTR && client->isValid() && client->isOpen())
    {
        QByteArray array;
        array.clear();
        if(utf8Code != 0)
        {
            array = utf8Code->fromUnicode(text);
        }
        else
        {
            array = text.toUtf8();
        }
        if(array.isNull() || array.isEmpty())
        {}
        else
        {
            client->write(array);
            client->flush();
        }
    }
}

void TcpClientThread::timerEvent(QTimerEvent *event)
{
    if(event->timerId() == reConnectTimerId)
    {
        if(_isRunning && !_isConnected)
        {
            if(client != Q_NULLPTR)
            {
                emit onReadyRead(tr("try to connect"));
                client->connectToHost(ip, port);
                client->waitForConnected();
            }
        }
        if(_isRunning && _isConnected)
        {
            if(client != Q_NULLPTR && client->isOpen())
            {
                if(beatIndex % 100 == 0)
                {
                    sendText("Beat");
                }
            }
        }
        beatIndex++;
        if(beatIndex > 10000)
        {
            beatIndex = 0;
        }
    }
}
