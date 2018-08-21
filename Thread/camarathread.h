#ifndef CAMARATHREAD_H
#define CAMARATHREAD_H

#include <QThread>
#include <QDateTime>
#include <QDir>
#include "opencv/cv.h"
#include "Core/ImageFormat.h"
#include "opencv2/video/background_segm.hpp"

class CamaraThread : public QThread
{
    Q_OBJECT
public:
    explicit CamaraThread(QObject *parent = nullptr);
    void setStop();

    int getCamaraId() const;
    void setCamaraId(int value);

    QString getRecDir() const;
    void setRecDir(const QString &value);

    int getRecMinSecond() const;
    void setRecMinSecond(int value);

protected:
    void run();

signals:
    void onImage(QImage const &image);
    void onTip(QString const &tip);
    void onNotify();
    void onConnectChanged(bool connected);

public slots:

private:
    bool _isRunning;
    int camaraId;
    int recMinSecond;
    int recMaxSencond;
    QTime time;
    bool _isConnect;
    QString recDir;
};

#endif // CAMARATHREAD_H
