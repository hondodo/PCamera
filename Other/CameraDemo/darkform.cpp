#include "darkform.h"
#include "ui_darkform.h"

DarkForm::DarkForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DarkForm)
{
    ui->setupUi(this);
    this->setWindowState(Qt::WindowFullScreen);
}

DarkForm::~DarkForm()
{
    delete ui;
}
