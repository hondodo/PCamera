#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QThread>
#include <QTimer>
#include <QApplication>
#include <QDesktopWidget>
#include <QTimerEvent>
#include "3rd/videoplayer.h"
#include "cameracontrol.h"
#include "rendercontrol.h"
#include "addcameraform.h"
#include "cameracontrolgl.h"
#include "datetimecontrol.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:
    void resizeEvent(QResizeEvent *event);
    void timerEvent(QTimerEvent *event);
    void showEvent(QShowEvent *event);

public slots:

private slots:
    void on_pushButtonAddCamera_clicked();
    void onAddCameraFormClose(int code);
    void on_tabWidget_currentChanged(int index);

private:
    Ui::MainWindow *ui;
    bool isFirstShow;
    QList<QString> existsCameraUrls;
    QList<CameraControl *> allCameraControls;
    DateTimeControl *timeControl;

    bool showDateTimeControlToCamerasComtrol;

    void showCamera();
    void resizeCameraControl();
    int maxwidth;
    int maxheight;

    int timerId;
    int timerFrames;
    int camBigIndex;
    CameraControl *camBigShowingWidget;
};

#endif // MAINWINDOW_H
