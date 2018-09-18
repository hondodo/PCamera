#ifndef CAMERACONTROL_H
#define CAMERACONTROL_H

#include <QWidget>
#include "3rd/videoplayer.h"

namespace Ui {
class CameraControl;
}

class CameraControl : public QWidget
{
    Q_OBJECT

public:
    explicit CameraControl(QWidget *parent = 0);
    ~CameraControl();

    QString getCameraUrl() const;
    void setCameraUrl(const QString &value);

    CAMERATYPE getCameraType() const;
    void setCameraType(const CAMERATYPE &value);

    void start();
    void stop();

private slots:
    void onImage(QImage image);

private:
    Ui::CameraControl *ui;
    QString cameraUrl;
    CAMERATYPE cameraType;//0-local 1-web
};

#endif // CAMERACONTROL_H
