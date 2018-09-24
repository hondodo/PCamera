#include "cameracontrolgl.h"

CameraControlGL::CameraControlGL(int width, int height, QWidget *parent) :
    QWidget(parent),
    glWidget(width, height, this)
{

}

CameraControlGL::~CameraControlGL()
{
}

void CameraControlGL::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event);
    glWidget.setGeometry(0, 0, width(), height());
}

void CameraControlGL::onFrame(unsigned char *yuvData)
{
    glWidget.onFrame(yuvData);
}
