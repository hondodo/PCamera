#ifndef DATETIMECONTROL_H
#define DATETIMECONTROL_H

#include <QWidget>
#include <QDateTime>
#include <QTimerEvent>
#include <QScrollBar>
#include "3rd/chineseyear.h"
#include "ringthread.h"

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
    void resizeEvent(QResizeEvent *event);

private:
    Ui::DateTimeControl *ui;
    int timeId;
    int nextTipTime;
    int dateFontSize, timeFontSize, weekFontSize, yearFontSize;
    RingThread *ringThread;
    void deleteRingThread();
    void startNewRingThread(QString filename);

    QString buildHtmlText();
    QString buildHtmlParagraph(QString text, int fontsize);
    void trimFonSize(bool up);
    bool isCanDown();
    bool isCanUp();
    void resizeFontSize();
};

#endif // DATETIMECONTROL_H
