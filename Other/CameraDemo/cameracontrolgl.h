#ifndef CAMERACONTROLGL_H
#define CAMERACONTROLGL_H

#include <QWidget>
#include "3rd/gl_widget.h"

namespace Ui {
class CameraControlGL;
}

class CameraControlGL : public QWidget
{
    Q_OBJECT

public:
    explicit CameraControlGL(int width, int height, QWidget *parent = 0);
    ~CameraControlGL();

protected:
    void resizeEvent(QResizeEvent *event);

public slots:
    void onFrame(unsigned char *yuvData);

private:
    Gl_widget glWidget;
};

#endif // CAMERACONTROLGL_H
