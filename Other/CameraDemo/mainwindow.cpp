#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    worker = Q_NULLPTR;
    t = Q_NULLPTR;
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::slot_GetOneFrame(QImage img)
{
    ui->label->setPixmap(QPixmap::fromImage(img));
}

void MainWindow::onMessage(QString text)
{
    ui->pushButton->setText(text);
}

void MainWindow::on_pushButton_clicked()
{
    //t = new QThread;
    worker = new VideoPlayer;
    //connect(t, &QThread::finished, worker, &QObject::deleteLater);//防止内存泄漏
    connect(worker, SIGNAL(sig_GetOneFrame(QImage)), this, SLOT(slot_GetOneFrame(QImage)));
    connect(worker, SIGNAL(onMessage(QString)), this, SLOT(onMessage(QString)));
    //worker->moveToThread(t);
    //t->start();
    worker->start();
}
