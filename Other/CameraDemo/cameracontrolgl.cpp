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
    glWidget.setGeometry(0, 0, videoWidth(), videoHeight());
}

void CameraControlGL::onFrame(unsigned char *yuvData)
{
    glWidget.onFrame(yuvData);
}

void CameraControlGL::onYUVFrame(const unsigned char *y_data, const unsigned char *u_data, const unsigned char *v_data)
{
    glWidget.onYUVFrame(y_data, u_data, v_data);
}
