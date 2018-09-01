#ifndef MAINDIALOG_H
#define MAINDIALOG_H

#include <QDialog>
#include <QDateTime>
#include <QTcpServer>
#include <QTcpSocket>
#include <QTextCodec>
#include <QUdpSocket>
#include <QNetworkInterface>
#include "Thread/oledthread.h"
#include "Thread/cameracollectorthread.h"

namespace Ui {
class MainDialog;
}

class MainDialog : public QDialog
{
    Q_OBJECT

public:
    explicit MainDialog(QWidget *parent = 0);
    ~MainDialog();

protected:
    void resizeEvent(QResizeEvent *);

private slots:
    void onImage(QImage const &image);
    void onTip(QString message);
    void newConnection();
    void acceptError(QAbstractSocket::SocketError socketError);
    void readyRead();
    void disconnected();
    void onReadyRead();

private:
    Ui::MainDialog *ui;
    OLedThread *oledThread;
    QTcpServer *server;
    QList<QTcpSocket *> allClient;
    QMap<QTcpSocket *, QDateTime> clientBeatTime;
    QByteArray textHeader, imageHeader, textTag, imageTag;
    QTextCodec *utf8Code;
    void sendMessage(QByteArray array);
    QDateTime lastSendImage;
    bool isConverImage;
    bool isSending;
    QUdpSocket *udpServer;
    quint16 udpPort;
};

#endif // MAINDIALOG_H
