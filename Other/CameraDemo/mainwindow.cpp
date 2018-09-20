#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    worker = Q_NULLPTR;
    t = Q_NULLPTR;
    existsCameraUrls.clear();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::slot_GetOneFrame(QImage img)
{
    //ui->label->setPixmap(QPixmap::fromImage(img));
}

void MainWindow::on_pushButton_clicked()
{
//    //t = new QThread;
//    worker = new VideoPlayer;
//    //connect(t, &QThread::finished, worker, &QObject::deleteLater);//防止内存泄漏
//    connect(worker, SIGNAL(sig_GetOneFrame(QImage)), this, SLOT(slot_GetOneFrame(QImage)));
//    //worker->moveToThread(t);
//    //t->start();
//    worker->start();

    /*
    QList<QByteArray> all = QCamera::availableDevices();
    for(int i = 0; i < all.count(); i++)
    {
        QString desc = QCamera::deviceDescription(all.at(i));
        desc = "video=" + desc;
        CameraControl *control = new CameraControl();
        control->setCameraType(CAMERATYPE_LOCAL);
        ui->verticalLayout->addWidget(control);
        control->setCameraUrl(desc);
        control->start();
    }
    CameraControl *control = new CameraControl();
    ui->verticalLayout->addWidget(control);
    control->setCameraUrl("http://admin:12345@192.168.31.87:8081");
    control->setCameraType(CAMERATYPE_WEB);
    control->start();
    */
#ifdef Q_OS_WIN
//    QList<QByteArray> all = QCamera::availableDevices();
//    for(int i = 0; i < all.count(); i++)
//    {
//        QString desc = QCamera::deviceDescription(all.at(i));
//        desc = "video=" + desc;
//        RenderControl *control = new RenderControl();
//        control->setCameraType(CAMERATYPE_LOCAL);
//        ui->verticalLayout->addWidget(control);
//        control->setCameraUrl(desc);
//        control->start();
//    }
#else
//    CameraControl *controllocal = new CameraControl();
//    controllocal->setCameraType(CAMERATYPE_LOCAL);
//    ui->verticalLayout->addWidget(controllocal);
//    controllocal->setCameraUrl("/dev/video0");
//    controllocal->start();
#endif
//    CameraControl *control = new CameraControl();
//    ui->verticalLayout->addWidget(control);
//    control->setCameraUrl("http://admin:12345@192.168.31.87:8081");
//    control->setCameraType(CAMERATYPE_WEB);
//    control->start();
}

void MainWindow::on_pushButtonAddCamera_clicked()
{
    AddCameraForm *form = new AddCameraForm();
    form->setExistsCameraUrls(existsCameraUrls);
    connect(form, SIGNAL(onClose(int)), this, SLOT(onAddCameraFormClose(int)));
    form->show();
}

void MainWindow::onAddCameraFormClose(int code)
{
    AddCameraForm *form = qobject_cast<AddCameraForm *>(QObject::sender());
    if(form != Q_NULLPTR)
    {
        if(code == 0)
        {
            QString url = form->getCameraUrl();
            if(!existsCameraUrls.contains(url.toLower()))
            {
                existsCameraUrls.append(url.toLower());
                CameraControl *control = new CameraControl();
                ui->gridLayoutCameras->addWidget(control);
                control->setCameraUrl(url);
                control->setCameraType(form->getCameraType());
                control->start();
            }
        }
        form->deleteLater();
        form = Q_NULLPTR;
    }
}
