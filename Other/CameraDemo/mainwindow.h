#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QThread>
#include <QTimer>
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

public slots:
    void slot_GetOneFrame(QImage img);

private slots:
    void on_pushButton_clicked();
    void on_pushButtonAddCamera_clicked();
    void onAddCameraFormClose(int code);

private:
    Ui::MainWindow *ui;
    VideoPlayer *worker;
    QThread *t;
    QList<QString> existsCameraUrls;
};

#endif // MAINWINDOW_H
