#include "darkform.h"
#include "ui_darkform.h"

DarkForm::DarkForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DarkForm)
{
    ui->setupUi(this);
    this->setWindowState(Qt::WindowFullScreen);
    timeId = startTimer(500);
    showLight = false;
    showHello = false;
    fromselfhide = false;
    requestHide = false;
    lightImage = QImage(":/Rcs/light.png");
    helloImage = QImage(":/Rcs/hello.png");
    setWindowTitle(tr("Time Form"));
//    this->setWindowFlag(Qt::WindowStaysOnTopHint, true);
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

void DarkForm::hideForm()
{
    requestHide = true;
    hideTime = QDateTime::currentDateTime();
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

            if(requestHide)
            {
                qint64 elsp = QDateTime::currentDateTime().toMSecsSinceEpoch() - hideTime.toMSecsSinceEpoch();
                if(elsp >= 0 && elsp < 3000)
                {
                    ui->widget->setVisible(false);
                    if(elsp < 2000)
                    {
                        showHello = true;
                        showLight = false;
                        this->update();
                    }
                    else
                    {
                        showHello = false;
                        showLight = true;
                        this->update();
                    }
                }
                else
                {
                    fromselfhide = true;
                    this->hide();
                }
            }
            else
            {
                ui->widget->setVisible(true);
            }
        }
    }
}

void DarkForm::showEvent(QShowEvent *event)
{
    Q_UNUSED(event);
    requestHide = false;
}

void DarkForm::paintEvent(QPaintEvent *event)
{
    if(!lightImage.isNull() && showLight)
    {
        QPainter painter(this);
        painter.fillRect(this->rect(), Qt::black);
        painter.drawImage(this->rect(), lightImage, lightImage.rect());
    }
    else if(!helloImage.isNull() && showHello)
    {
        QPainter painter(this);
        painter.fillRect(this->rect(), Qt::black);
        painter.drawImage(this->rect(), helloImage, helloImage.rect());
    }
    else
    {
        QWidget::paintEvent(event);
    }
}

void DarkForm::hideEvent(QHideEvent *event)
{
    Q_UNUSED(event);
    showHello = false;
    showLight = false;
}

bool DarkForm::getRequestHide() const
{
    return requestHide;
}
