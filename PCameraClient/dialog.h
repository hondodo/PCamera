#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include <QTextCodec>
#include <QBuffer>
#include <QImageReader>
#include <QPainter>
#include <QUdpSocket>
#include <QApplication>
#include <QDateTime>
#include <QDesktopWidget>
#include <QTimerEvent>
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
    void resizeEvent(QResizeEvent *);
    void timerEvent(QTimerEvent *event);

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
    void onReadyReadArrray(QByteArray array);

private:
    Ui::Dialog *ui;
    QTextCodec *utf8Code;
    QByteArray textHeader, imageHeader, textTag, imageTag;
    QString blackBgStyleSheet, whiteBgStyleSheet;
    QImage imageCache;
    bool isPainting;
    bool isCoverMessage;
    bool isHideControls;
    bool isClickedImage;
    bool isHengPin;
    bool isConnected;
    QSize hengPinSize, hengPinBigSize;
    TcpClientThread *tcpThread;
    QUdpSocket *udpClient;
    void switchControlVisible();
    void setButtonConnectText();
    int checkUdpTimerId;
    QDateTime lastReceiveUdpData;
    bool canRequestTcp;
    bool isRequestText;
    QByteArray tcpArrayCache;
    QByteArray udpArrayCache;
};

#endif // DIALOG_H
