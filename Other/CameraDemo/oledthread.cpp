#include "oledthread.h"

using namespace cv;

OLedThread *OLedThread::Init = new OLedThread();
OLedThread::OLedThread(QObject *parent) : QThread(parent)
{
    _isRunning = false;
    top = 0;
    btm = 0;
    mid = 0;
    pixmap = cv::Mat(CvSize(128, 64), IPL_DEPTH_8U);
}

OLedThread::~OLedThread()
{
    setStop();
}

void OLedThread::setStop()
{
    _isRunning = false;
    oled.DisplayOff();
}

void OLedThread::testFontMargin(int fontPixe)
{
    QString text = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
    //text = text + "\n" + text;
    int width = fontPixe * text.length();
    //int height = fontPixe * 2;
    QImage img = QImage(width, fontPixe, QImage::Format_ARGB32);
    QPainter painter(&img);
    painter.fillRect(img.rect(), Qt::black);
    painter.setPen(Qt::white);
    QFont font = painter.font();
    font.setPixelSize(fontPixe);
    painter.setFont(font);
    painter.drawText(img.rect(), text);

    img.save("D:/ch.png", "PNG");

    bool findtop = false, findbtm = false;;
    for(int h = 0; h < fontPixe; h++)
    {
        if(findtop) break;
        for(int w = 0; w < width; w++)
        {
            QColor color = img.pixelColor(w, h);
            if(color.red() != 0 || color.green() != 0 || color.blue() != 0)
            {
                top = h;
                findtop = true;
                break;
            }
        }
    }

    for(int h = fontPixe - 1; h >= 0; h--)
    {
        if(findbtm) break;
        for(int w = 0; w < width; w++)
        {
            QColor color = img.pixelColor(w, h);
            if(color.red() != 0 || color.green() != 0 || color.blue() != 0)
            {
                btm = fontPixe - 1 - h;
                findbtm = true;
                break;
            }
        }
    }
}

void OLedThread::writeText(cv::Mat image, std::string text)
{
    image.setTo(cv::Scalar(0, 0, 0));

    int font_face = cv::FONT_HERSHEY_PLAIN;
    double font_scale = 0.82;
    int thickness = 1;
    int baseline;
    int textBtm = 2;
    int eachline = 16;
    int textheight = 8;

    cv::Size text_size = cv::getTextSize(text, font_face, font_scale, thickness, &baseline);
    textheight = text_size.height;

    QList<std::string> lines;
    int textlen = text.length();
    int textindex = 0;
    for(int i = 0; i < 8; i++)
    {
        std::string tmpline = "";
        if(textindex >= textlen - 1)
        {
            break;
        }
        for(int j = 0; j < eachline; j++)
        {
            char ch = text[textindex];
            textindex++;
            if(ch == '\n')
            {
                break;
            }
            tmpline += ch;
        }
        std::string line = "";
        int tmplinelen = tmpline.length();
        for(int k = 0; k < tmplinelen; k++)
        {
            char ch = tmpline[k];
            if(ch == '\0')
            {
                break;
            }
            line += ch;
        }
        lines.append(line);
    }

    cv::Point point;
    point.x = 0;
    point.y = textheight + 1;
    for(int i = 0; i < lines.count(); i++)
    {
        if(i != 0)
        {
            point.y = point.y + textheight + textBtm;
        }
        cv::putText(image, lines.at(i), point, font_face, font_scale,
                    cv::Scalar(255, 255, 255), thickness, 8, 0);
    }

    //cv::imwrite("D:/cv.png", image);
}

void OLedThread::run()
{
#ifdef Q_OS_UNIX
    _isRunning = true;
    oled.Init(0x3c);
    oled.DisplayOn();
    oled.CleanScreen();

    cv::Mat image = cv::Mat::zeros(cv::Size(128, 64), CV_8UC3);
    QString nonewritetext = "potatokid 10086 10010 10000";

    /*
    int fontPix = 16;
    QImage img = QImage(128, 128, QImage::Format_ARGB32);
    QPainter painter(&img);
    painter.setPen(Qt::white);
    QFont font = painter.font();
    font.setPixelSize(fontPix);
    painter.setFont(font);
    QImage showimg = QImage(128, 64, QImage::Format_ARGB32);
    QPainter painterTmp(&showimg);
    testFontMargin(fontPix);
    QFontMetrics fontMet(font);
    QRect targetRect;
    */
    while(_isRunning)
    {
        /*
        painter.fillRect(img.rect(), Qt::black);
        painter.drawText(img.rect(), message);

        targetRect = fontMet.boundingRect(img.rect(), Qt::TextWordWrap, message);

        if(targetRect.height() > 64 && (top + btm) < 1)
        {
            showimg = img.copy(0, 0, 128, 64);
        }
        else
        {
            painterTmp.fillRect(showimg.rect(), Qt::black);
            int textheight = fontPix - top - btm;
            int nowheight = 0;
            int line = 0;
            int eachline = targetRect.height() / (64.0 / fontPix);
            while (nowheight < 64)
            {
                int targetY = line * eachline + top;
                painterTmp.drawImage(0, nowheight, img, 0, targetY, 128, textheight);
                line++;
                nowheight += textheight + 2;
                qDebug() << targetY;
            }

        }
        img.save("D:/src.png", "PNG");
        showimg.save("D:/test.png", "PNG");
        */
        if(message != nonewritetext)
        {
            writeText(image, message.toStdString());
            nonewritetext = message;
            QImage showimage = ImageFormat::Mat2QImage(image);
            oled.WriteImage(&showimage);
        }
        for(int i = 0; i < 50; i++)
        {
            if(!_isRunning)
            {
                break;
            }
            this->msleep(20);
        }
    }
    setStop();
#endif
}

void OLedThread::setImage(QImage image)
{
    imageCache = image.copy();
}

void OLedThread::setMessage(QString message)
{
    this->message = message;
}
