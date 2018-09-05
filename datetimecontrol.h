#ifndef DATETIMECONTROL_H
#define DATETIMECONTROL_H

#include <QWidget>
#include <QDateTime>
#include <QTimerEvent>
#include "3rd/chineseyear.h"

namespace Ui {
class DateTimeControl;
}

class DateTimeControl : public QWidget
{
    Q_OBJECT

public:
    explicit DateTimeControl(QWidget *parent = 0);
    ~DateTimeControl();

    void start();

protected:
    void timerEvent(QTimerEvent *event);

private:
    Ui::DateTimeControl *ui;
    int timeId;
};

#endif // DATETIMECONTROL_H
