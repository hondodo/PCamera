#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include <QUrl>
#include <QPainter>
#include "cameracontrol.h"
#include "Thread/cameracollectorthread.h"
#include "Core/oled12864.h"

namespace Ui {
class Dialog;
}

class Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog(QWidget *parent = 0);
    ~Dialog();

protected:
    void resizeEvent(QResizeEvent *);

private slots:
    void on_pushButtonStart_clicked();
    void on_restoreCamaras();
    void on_pushButtonStop_clicked();
    void on_pushButtonOLedOn_clicked();
    void on_pushButtonOLedOff_clicked();
    void on_pushButtonOLedAdd_clicked();
    void on_pushButtonOLedMut_clicked();
    void on_pushButtonCleanOLed_clicked();

    void on_pushButtonTes_clicked();

private:
    Ui::Dialog *ui;
    int getCameraCount() const;
    int getShowingCameraCount() const;
    int cameraIdIndex;
    CameraCollectorThread *th;
    void stopCameraCollectThread();

    OLed12864 oled;
    int oledVol;
};

#endif // DIALOG_H
