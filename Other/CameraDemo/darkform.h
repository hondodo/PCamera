#ifndef DARKFORM_H
#define DARKFORM_H

#include <QWidget>
#include <QTimerEvent>
#include <QDateTime>
#include <QLCDNumber>
#include <QPainter>
#include <QHideEvent>

namespace Ui {
class DarkForm;
}

class DarkForm : public QWidget
{
    Q_OBJECT

public:
    explicit DarkForm(QWidget *parent = 0);
    ~DarkForm();

    void hideForm();
    bool getRequestHide() const;

protected:
    void timerEvent(QTimerEvent *event);
    void showEvent(QShowEvent *event);
    void paintEvent(QPaintEvent *event);
    void hideEvent(QHideEvent *event);

private:
    Ui::DarkForm *ui;
    int timeId;
    QDateTime hideTime;
    bool showHello;
    bool showLight;
    bool fromselfhide;
    bool requestHide;
    QImage lightImage, helloImage;
};

#endif // DARKFORM_H
