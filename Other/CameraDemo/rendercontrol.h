#ifndef RENDERCONTROL_H
#define RENDERCONTROL_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include "cameratype.h"
#include "3rd/videoplayer.h"

class RenderControl : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT
public:
    explicit RenderControl(QWidget *parent = nullptr);
    ~RenderControl();

    QString getCameraUrl() const;
    void setCameraUrl(const QString &value);

    CAMERATYPE getCameraType() const;
    void setCameraType(const CAMERATYPE &value);

    void stop();
    void start();

    QString getCameraName() const;
    void setCameraName(const QString &value);

    int getImageWidth() const;
    void setImageWidth(int value);

    int getImageHeight() const;
    void setImageHeight(int value);

protected:
    void initializeGL() override;
    void paintGL() override;
    void resizeGL(int w, int h) override;
    void paintEvent(QPaintEvent *e);

signals:

public slots:
    void onImage(QImage image);

private:
    QString cameraUrl;
    CAMERATYPE cameraType;
    VideoPlayer *player;
    QImage imageCache;
    bool canUpdateImage;
    QString cameraName;
    int imageWidth, imageHeight;
};

#endif // RENDERCONTROL_H
