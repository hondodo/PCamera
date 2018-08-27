#ifndef MAINDIALOG_H
#define MAINDIALOG_H

#include <QDialog>
#include <QDateTime>
#include "Core/oled12864.h"

namespace Ui {
class MainDialog;
}

class MainDialog : public QDialog
{
    Q_OBJECT

public:
    explicit MainDialog(QWidget *parent = 0);
    ~MainDialog();

protected:
    void resizeEvent(QResizeEvent *);

private slots:
    void onImage(QImage const &image);

private:
    Ui::MainDialog *ui;
    OLed12864 oled;
    QDateTime lastShowLed;
};

#endif // MAINDIALOG_H
