#ifndef CAMERACONTROL_H
#define CAMERACONTROL_H

#include <QWidget>
#include "3rd/videoplayer.h"
#include "cameracontrolgl.h"

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

    QString getCameraName() const;
    void setCameraName(const QString &value);

    int getImageWidth() const;
    void setImageWidth(int value);

    int getImageHeight() const;
    void setImageHeight(int value);

    bool getCheckBrighness() const;
    void setCheckBrighness(bool value);

    bool getFixBrighnessByTime() const;
    void setFixBrighnessByTime(bool value);

private slots:
    void onImage(QImage image);
    void onFreamSize(int width, int height);
    void onFream(unsigned char *yuvData);

private:
    Ui::CameraControl *ui;
    QString cameraUrl;
    CAMERATYPE cameraType;//0-local 1-web
    QString cameraName;
    VideoPlayer *player;
    int imageWidth, imageHeight;
    bool checkBrighness;
    bool fixBrighnessByTime;
    CameraControlGL *glcontrol;
};

#endif // CAMERACONTROL_H
