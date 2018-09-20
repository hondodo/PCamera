#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QThread>
#include <QTimer>
#include <QApplication>
#include <QDesktopWidget>
#include "3rd/videoplayer.h"
#include "cameracontrol.h"
#include "rendercontrol.h"
#include "addcameraform.h"

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

public slots:

private slots:
    void on_pushButtonAddCamera_clicked();
    void onAddCameraFormClose(int code);
    void on_tabWidget_currentChanged(int index);

private:
    Ui::MainWindow *ui;
    QList<QString> existsCameraUrls;
    QList<CameraControl *> allCameraControls;
    void showCamera();
    void resizeCameraControl();
    int maxwidth;
    int maxheight;
};

#endif // MAINWINDOW_H
