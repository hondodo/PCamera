#ifndef ADDCAMERAFORM_H
#define ADDCAMERAFORM_H

#include <QWidget>
#include <QCamera>
#include <QFile>
#include <QAudioDeviceInfo>
#include "cameratype.h"

namespace Ui {
class AddCameraForm;
}

class AddCameraForm : public QWidget
{
    Q_OBJECT

public:
    explicit AddCameraForm(QWidget *parent = 0);
    ~AddCameraForm();

    QString getCameraUrl() const;
    CAMERATYPE getCameraType() const;
    int getState() const;
    void setExistsCameraUrls(const QList<QString> &value);

    QString getCameraName() const;

    CAMERASIZE getCameraSize() const;

    bool getIsTv() const;

protected:
    void showEvent(QShowEvent *event);
    void closeEvent(QCloseEvent *event);

signals:
    void onClose(int code);

private slots:
    void on_comboBoxType_currentIndexChanged(int index);
    void on_pushButtonOK_clicked();
    void on_lineEditUrl_textChanged(const QString &arg1);
    void on_comboBoxUrl_currentIndexChanged(int index);

private:
    Ui::AddCameraForm *ui;
    bool isFirstShow;
    void initCameraComboBox();
    void initCameraName();
    QString cameraUrl;
    CAMERATYPE cameraType;
    CAMERASIZE cameraSize;
    int state;
    QList<QString> existsCameraUrls;
    QString cameraName;
};

#endif // ADDCAMERAFORM_H
