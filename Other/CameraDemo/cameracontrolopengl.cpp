#include "cameracontrolopengl.h"
#include "ui_cameracontrolopengl.h"

CameraControlOpenGL::CameraControlOpenGL(int width, int height, QWidget *parent) :
    QWidget(parent),
    glWidget(width, height, this),
    ui(new Ui::CameraControlOpenGL)
{
    ui->setupUi(this);
}

CameraControlOpenGL::~CameraControlOpenGL()
{
    delete ui;
}


void CameraControlOpenGL::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event);
    glWidget.setGeometry(0, 0, width(), height());
}

void CameraControlOpenGL::onFrame(unsigned char *yuvData)
{
    glWidget.onFrame(yuvData);
}
