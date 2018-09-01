#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include <QTextCodec>
#include <QBuffer>
#include <QImageReader>
#include <QPainter>
#include <QUdpSocket>
#include "tcpclientthread.h"

namespace Ui {
class Dialog;
}

class Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog(QWidget *parent = 0);
    ~Dialog();

    void stopUdpClient();

    void newUdpClient(QString ip, quint16 port);
    void newUdpClient(qint32 ip, quint16 port);

protected:
    void paintEvent(QPaintEvent *);

private slots:
    void on_pushButtonConnect_clicked();
    void onConnected();
    void onDisconnected();
    void onStateChanged(QAbstractSocket::SocketState);
    void onError(QAbstractSocket::SocketError);
    void onTcpReadyRead(QString text);
    void onReadyRead(QByteArray array);
    void onReadyRead();
    void onImageLabelClicked();

private:
    Ui::Dialog *ui;
    QTextCodec *utf8Code;
    QByteArray textHeader, imageHeader, textTag, imageTag;
    QImage imageCache;
    bool isPainting;
    bool isCoverMessage;
    bool isHideControls;
    TcpClientThread *tcpThread;
    QUdpSocket *udpClient;
    void switchControlVisible();
    void setButtonConnectText();
};

#endif // DIALOG_H
