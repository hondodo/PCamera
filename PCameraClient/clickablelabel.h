#ifndef CLICKABLELABEL_H
#define CLICKABLELABEL_H

#include <QLabel>

class ClickableLabel : public QLabel
{
    Q_OBJECT
public:
    explicit ClickableLabel(QWidget *parent = nullptr);

protected:
    void mousePressEvent(QMouseEvent *ev);

signals:
    void clicked();

public slots:

private:
};

#endif // CLICKABLELABEL_H
