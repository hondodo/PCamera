#ifndef CAMERACONTROL_H
#define CAMERACONTROL_H

#include <QWidget>
#include <QPixmapCache>
#include <QMenu>
#include "3rd/videoplayer.h"
#include "camerathreadmux.h"

namespace Ui {
class CameraControl;
}

class CameraControl : public QWidget
{
    Q_OBJECT

public:
    explicit CameraControl(QWidget *parent = 0);
    ~CameraControl();

    QString getCameraUrl() const;
    void setCameraUrl(const QString &value);

    CAMERATYPE getCameraType() const;
    void setCameraType(const CAMERATYPE &value);

    void start();
    void stop();

    QString getCameraName() const;
    void setCameraName(const QString &value);

    int getImageWidth() const;
    void setImageWidth(int value);

    int getImageHeight() const;
    void setImageHeight(int value);

    bool getCheckBrighness() const;
    void setCheckBrighness(bool value);

    bool getFixBrighnessByTime() const;
    void setFixBrighnessByTime(bool value);

    bool getCheckMog() const;
    void setCheckMog(bool value);

    bool getSaveOnlyMog() const;
    void setSaveOnlyMog(bool value);

protected:
    bool eventFilter(QObject *watched, QEvent *event);

signals:
    void onRemoveRequest();

private slots:
    void onImage(const QImage &image);
    void on_label_customContextMenuRequested(const QPoint &pos);
    void onMenuClickRemove();
    void onMenuClickCheckBrighness();
    void onMenuClickFixBrighnessbyTime();
    void onMenuClickCheckMog();
    void onMenuClickSaveOnlyMog();

private:
    Ui::CameraControl *ui;
    QString cameraUrl;
    CAMERATYPE cameraType;//0-local 1-web
    QString cameraName;
    CameraThreadMUX *player;
    int imageWidth, imageHeight;
    bool checkBrighness;
    bool fixBrighnessByTime;
    bool checkMog;
    bool saveOnlyMog;
    void initMenu();
    void disConnectMenu();
    QMenu *menu;
    QAction *removeAction, *checkBrighnessAction, *fixBrighnessByTimeAction,
    *checkMogAction, *saveOnlyMogAction;
};

#endif // CAMERACONTROL_H
