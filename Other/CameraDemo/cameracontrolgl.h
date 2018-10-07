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
    explicit CameraControlGL(int videoWidth, int videoHeight, QWidget *parent = 0);
    ~CameraControlGL();

protected:
    void resizeEvent(QResizeEvent *event);

public slots:
    void onFrame(unsigned char *yuvData);
    void onYUVFrame(const unsigned char* y_data, const unsigned char* u_data, const unsigned char* v_data);

private:
    Gl_widget glWidget;
};

#endif // CAMERACONTROLGL_H
