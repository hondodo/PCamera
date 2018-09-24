#include "cameracontrolgl.h"
#include "ui_cameracontrolgl.h"

CameraControlGL::CameraControlGL(int width, int height, QWidget *parent) :
    QWidget(parent),
    glWidget(width, height, this),
    ui(new Ui::CameraControlGL)
{
    ui->setupUi(this);
}

CameraControlGL::~CameraControlGL()
{
    delete ui;
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
