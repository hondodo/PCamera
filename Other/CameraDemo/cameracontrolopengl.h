#ifndef CAMERACONTROLOPENGL_H
#define CAMERACONTROLOPENGL_H

#include <QWidget>
#include "3rd/gl_widget.h"

namespace Ui {
class CameraControlOpenGL;
}

class CameraControlOpenGL : public QWidget
{
    Q_OBJECT

public:
    explicit CameraControlOpenGL(int videoWidth, int videoHeight, QWidget *parent = 0);
    ~CameraControlOpenGL();

protected:
    void resizeEvent(QResizeEvent *event);

public slots:
    void onFrame(unsigned char *yuvData);

private:
    Ui::CameraControlOpenGL *ui;
    Gl_widget glWidget;
};

#endif // CAMERACONTROLOPENGL_H
