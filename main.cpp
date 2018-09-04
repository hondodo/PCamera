#include "dialog.h"
#include <QApplication>
#include "maindialog.h"

int main(int argc, char *argv[])
{
#if (QT_VERSION >= QT_VERSION_CHECK(5, 6, 0))
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif
    QApplication a(argc, argv);

    /*
    cv::Mat mat = cv::imread("d:/face.jpg");
    cv::Mat tranlateMat;
    cv::VideoCapture cap("D:/face3.avi");
    int frame = 0;

    cv::Rect cutrect;
    cutrect.x = 407;
    cutrect.y = 197;
    cutrect.width = 230;
    cutrect.height = 250;

    QTime time;
    time.start();

    cv::Mat kernel = (cv::Mat_<float>(3, 3) << 0, -1, 0, 0, 5, 0, 0, -1, 0);

    while (!mat.empty())
    {
        time.restart();

        tranlateMat = mat(cutrect);
//        cv::cvtColor(tranlateMat, tranlateMat, cv::COLOR_BGR2GRAY);
//        cv::filter2D(tranlateMat ,tranlateMat, CV_8UC3, kernel);
        std::vector<cv::Rect_<int> > faces = CameraCollectorThread::Init->findFace(tranlateMat);

        if(((int)faces.size()) > 0)
        {
            for(int i = 0; i < (int)faces.size(); i++)
            {
                cv::rectangle(tranlateMat, faces.at(i), cv::Scalar(0, 0, 255), 2);
                QString filename = "D:/faces/" + QString::number(frame) + ".png";
                cv::imwrite(filename.toStdString(), tranlateMat);
            }
        }
        QString filename = "D:/faces/source/" + QString::number(frame) + ".png";
        cv::imwrite(filename.toStdString(), tranlateMat);

        qDebug() << frame << faces.size() << time.elapsed();

        frame++;
        cap.read(mat);
    }
    cap.release();
    return 0;
    */

    MainDialog w;
    w.show();
    return a.exec();
}
