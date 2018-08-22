#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include <QUrl>
#include "cameracontrol.h"

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

private:
    Ui::Dialog *ui;
    int getCameraCount() const;
    int getShowingCameraCount() const;
    int cameraIdIndex;
    RingHelper ring;
};

#endif // DIALOG_H
