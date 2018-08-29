#ifndef CAMERACONTROL_H
#define CAMERACONTROL_H

#include <QWidget>
#include <QLabel>
#include <QDateTime>
#include <QPaintEvent>
#include <QPainter>
#include <QTimerEvent>
#include "Thread/camarathread.h"
#include "Thread/ringthread.h"

namespace Ui {
class CameraControl;
}

class CameraControl : public QWidget
{
    Q_OBJECT

public:
    explicit CameraControl(QWidget *parent = 0);
    CameraControl(int id, QWidget *parent);
    ~CameraControl();
    static int Width;
    static int Height;

    int getCameraId() const;
    void setCameraId(int value);

    void reConnect();
    void startNewCameraThread(int id);
    void setDefaultText();

    bool getIsFacing() const;
    void setIsFacing(bool value);

    bool getIsRinging() const;
    void setIsRinging(bool value);

protected:
    void resizeEvent(QResizeEvent *);
    void paintEvent(QPaintEvent *event);

public slots:
    void updateTime(QDateTime time);
    void setImage(QImage const &image);
    void setTip(QString const &tip);
    void onConnectChanged(bool connected);
    void onFaceDetected(int faceCount);

private slots:
    void on_pushButtonConnect_clicked();
    void on_checkBoxFace_toggled(bool checked);
    void on_checkBoxRing_toggled(bool checked);

signals:
    void onImage(QImage const &image);
    void onTip(QString message);

private:
    Ui::CameraControl *ui;

    int CameraId;
    QLabel *timeLabel;
    CamaraThread *camThread;
    void deleteCameraThread();

    QDateTime lastDetectFaceTime;
    //RingHelper ringHelper;
    bool _isRing;
    RingThread *ringThread;
    void deleteRingThread();
    void startNewRingThread();
    QImage imageCache;
};

#endif // CAMERACONTROL_H
