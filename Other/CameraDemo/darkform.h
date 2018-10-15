#ifndef DARKFORM_H
#define DARKFORM_H

#include <QWidget>

namespace Ui {
class DarkForm;
}

class DarkForm : public QWidget
{
    Q_OBJECT

public:
    explicit DarkForm(QWidget *parent = 0);
    ~DarkForm();

private:
    Ui::DarkForm *ui;
};

#endif // DARKFORM_H
