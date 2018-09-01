#include "clickablelabel.h"

ClickableLabel::ClickableLabel(QWidget *parent) : QLabel(parent)
{

}

void ClickableLabel::mousePressEvent(QMouseEvent *ev)
{
    Q_UNUSED(ev);
    emit clicked();
}
