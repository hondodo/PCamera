#ifndef TCPCLIENTTHREAD_H
#define TCPCLIENTTHREAD_H

#include <QTcpSocket>
#include <QObject>
#include <QTimerEvent>
#include <QTextCodec>

class TcpClientThread : public QObject
{
    Q_OBJECT
public:
    explicit TcpClientThread(QObject *parent = nullptr);
    ~TcpClientThread();

    void startRun();
    void setStop();

    QString getIp() const;
    void setIp(const QString &value);

    int getPort() const;
    void setPort(int value);

    void runTcp();
    void sendText(QString text);

    bool getIsRunning() const;
    void setIsRunning(bool isRunning);

protected:
    void timerEvent(QTimerEvent *event);

signals:
    void onConnected();
    void onDisconnected();
    void onStateChanged(QAbstractSocket::SocketState);
    void onError(QAbstractSocket::SocketError);
    void onReadyRead(QString array);

public slots:
    void connected();
    void disconnected();
    void stateChanged(QAbstractSocket::SocketState state);
    void error(QAbstractSocket::SocketError errorMsg);
    void readyRead();

private:
    QTcpSocket *client;
    QString ip;
    int port;
    int reConnectTimerId;
    int beatIndex;
    bool _isRunning;
    bool _isConnected;
    QTextCodec *utf8Code;
};

#endif // TCPCLIENTTHREAD_H
