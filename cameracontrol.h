#ifndef CAMERACONTROL_H
#define CAMERACONTROL_H

#include <QWidget>
#include <QLabel>
#include <QDateTime>
#include "Thread/camarathread.h"

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

public slots:
    void updateTime(QDateTime time);
    void setImage(QImage const &image);
    void setTip(QString const &tip);
    void onConnectChanged(bool connected);


private slots:
    void on_pushButtonConnect_clicked();

private:
    Ui::CameraControl *ui;
    void setDefaultText();
    int CameraId;
    QLabel *timeLabel;
    CamaraThread *camThread;
    void deleteCameraThread();
    void startNewCameraThread(int id);
};

#endif // CAMERACONTROL_H
