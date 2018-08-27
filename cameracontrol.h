#ifndef CAMERACONTROL_H
#define CAMERACONTROL_H

#include <QWidget>
#include <QLabel>
#include <QDateTime>
#include "Thread/camarathread.h"
#include "Core/ringhelper.h"

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

private:
    Ui::CameraControl *ui;

    int CameraId;
    QLabel *timeLabel;
    CamaraThread *camThread;
    void deleteCameraThread();

    QDateTime lastDetectFaceTime;
    RingHelper ringHelper;
    bool _isRing;

};

#endif // CAMERACONTROL_H
