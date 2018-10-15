#include "darkform.h"
#include "ui_darkform.h"

DarkForm::DarkForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DarkForm)
{
    ui->setupUi(this);
    this->setWindowState(Qt::WindowFullScreen);
    timeId = startTimer(500);
}

DarkForm::~DarkForm()
{
    if(timeId > 0)
    {
        killTimer(timeId);
        timeId = 0;
    }
    delete ui;
}

void DarkForm::timerEvent(QTimerEvent *event)
{
    if(event->timerId() == timeId)
    {
        if(!this->isHidden())
        {
            QDateTime now = QDateTime::currentDateTime();
            QTime time = now.time();
            int hh = time.hour() / 10;
            int hl = time.hour() % 10;
            int mh = time.minute() / 10;
            int ml = time.minute() % 10;
            int sh = time.second() / 10;
            int sl = time.second() % 10;
            ui->lcdNumberHH->display(hh);
            ui->lcdNumberHL->display(hl);
            ui->lcdNumberMH->display(mh);
            ui->lcdNumberML->display(ml);
            ui->lcdNumberSH->display(sh);
            ui->lcdNumberSL->display(sl);
        }
    }
}
