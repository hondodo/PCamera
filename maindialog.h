#ifndef MAINDIALOG_H
#define MAINDIALOG_H

#include <QDialog>
#include <QDateTime>
#include "Thread/oledthread.h"
#include "Thread/cameracollectorthread.h"

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
    void onTip(QString message);

private:
    Ui::MainDialog *ui;
    OLedThread *oledThread;
};

#endif // MAINDIALOG_H
