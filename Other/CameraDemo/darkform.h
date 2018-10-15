#ifndef DARKFORM_H
#define DARKFORM_H

#include <QWidget>
#include <QTimerEvent>
#include <QDateTime>
#include <QLCDNumber>

namespace Ui {
class DarkForm;
}

class DarkForm : public QWidget
{
    Q_OBJECT

public:
    explicit DarkForm(QWidget *parent = 0);
    ~DarkForm();

protected:
    void timerEvent(QTimerEvent *event);

private:
    Ui::DarkForm *ui;
    int timeId;
};

#endif // DARKFORM_H
