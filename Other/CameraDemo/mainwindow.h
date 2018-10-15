#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QThread>
#include <QTimer>
#include <QApplication>
#include <QDesktopWidget>
#include <QTimerEvent>
#include <QLabel>
#include "3rd/videoplayer.h"
#include "cameracontrol.h"
#include "rendercontrol.h"
#include "addcameraform.h"
#include "cameracontrolgl.h"
#include "datetimecontrol.h"
#include "weathercontrol.h"
#include "keyboardthread.h"
#include "ringthread.h"
#include "Core/diskhelper.h"
#include "checkdiskthread.h"
#include "3rd/gleswidget.h"
#include "darkform.h"

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
    void on_checkBoxTimeControl_stateChanged(int arg1);
    void on_checkBoxWeatherControl_stateChanged(int arg1);
    void onCameraControlRequestRemove();
    void onKey(int key);
    void onRingThreadFinish();

    void on_pushButtonTestYUV_clicked();

private:
    Ui::MainWindow *ui;
    bool isFirstShow;
    QList<QString> existsCameraUrls;
    QList<CameraControl *> allCameraControls;
    DateTimeControl *timeControl;
    RingThread *ringThread;
    QString ringFileName;
    DiskHelper diskHelper;
    PathHelper pathHelper;
    GLESWidget *glesWidget;
    QLabel labelDiskInfo, labelCamAInfo, labelCamBInfo, labelCamCInfo, labelCamDInfo, labelRingInfo,
    labelPeople, labelDark;

    bool showDateTimeControlToCamerasComtrol;
    bool showWeathControl;

    void showCamera();
    void resizeCameraControl();
    int maxwidth;
    int maxheight;

    int timerId;
    int timerFrames;
    int camBigIndex;
    CameraControl *camBigShowingWidget;
    WeatherControl *weatherControl;
    void deleteRingThread();
    void startNewRingThread();

    bool isDark, isPeople;
    DarkForm *darkForm;
};

#endif // MAINWINDOW_H
