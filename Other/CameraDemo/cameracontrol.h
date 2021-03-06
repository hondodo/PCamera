#ifndef CAMERACONTROL_H
#define CAMERACONTROL_H

#include <QWidget>
#include <QPixmapCache>
#include <QMenu>
#include <QAction>
#include <QTimerEvent>
#include "3rd/videoplayer.h"
#include "camerathreadmux.h"
#include "camerathreadh264.h"
#include "3rd/gl_widget.h"
#include "keyboardthread.h"

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

    void start(bool delayOpenCamera = false);
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

    bool getRestartCameraPre30Min() const;
    void setRestartCameraPre30Min(bool value);

    CAMERASIZE getCameraSize() const;
    void setCameraSize(const CAMERASIZE &value);

    QString getMessage() const;
    void setMessage(const QString &value);

    bool getIsTv() const;
    void setIsTv(bool value);

protected:
    bool eventFilter(QObject *watched, QEvent *event);
    void timerEvent(QTimerEvent *event);
    void resizeEvent(QResizeEvent *event);

signals:
    void onRemoveRequest();

private slots:
    void onImage(const QImage &image);
    void onFrame();
    void on_label_customContextMenuRequested(const QPoint &pos);
    void onMenuClickRemove();
    void onMenuClickCheckBrighness();
    void onMenuClickFixBrighnessbyTime();
    void onMenuClickCheckMog();
    void onMenuClickSaveOnlyMog();
    void onMenuClickRestartPre30Min();
    void onMenuClickFillScreen();
    void onMenuClickSkipFrame();
    void onStartRecoing(int width, int height, int pixOut);
    void onStopRecoding();

private slots:
#ifdef USE_OPENGL
    void onYUVFrame(const unsigned char* y_data, const unsigned char* u_data, const unsigned char* v_data);
#endif
    void onMessage(const QString text);

private:
    Ui::CameraControl *ui;
#ifdef USE_OPENGL
    Gl_widget *glWidget;
#endif
    QString cameraUrl;
    CAMERATYPE cameraType;//0-local 1-web
    CAMERASIZE cameraSize;
    QString cameraName;
    QString message;
#ifdef USE_H264
    CameraThreadH264 *player;
#else
    CameraThreadMUX *player;
#endif
    bool isTv;
    int imageWidth, imageHeight;
    bool checkBrighness;
    bool fixBrighnessByTime;
    bool checkMog;
    bool saveOnlyMog;
    bool restartCameraPre30Min;
    bool fillScreen;
    bool skipFrame;
    bool hasNewFrame;
    bool hasStopThread;
    bool hasInitDecodingRecs;
    QDateTime lastRestart;
    QDateTime lastReceiveImageTime;
    int restartTimerId;
    int frameTimerId;
    int restartTimeElsp;
    int restartByNoImageElsp;
    void initMenu();
    void disConnectMenu();
    QMenu *menu;
    QAction *removeAction, *checkBrighnessAction, *fixBrighnessByTimeAction,
    *checkMogAction, *saveOnlyMogAction, *restartPre30MinAction, *fillScreenAction, *skipFrameAction;

    //decode
    int widthOut;
    int heightOut;
    AVPixelFormat pixOut;
    AVFrame *pFrameRGB;
    struct SwsContext *imgConvertCtcRGB;
    AVPixelFormat rgbFmt;
    int rgbBytes;
    uint8_t *rgbOutBuffer;
    cv::Mat mRGB, temp;

    void resetDecodingRecs();
    void initDecodingRecs();
    void decodeFrameAndShow(AVFrame **filtedFrame);

    bool isDecodingTurn;
};

#endif // CAMERACONTROL_H
