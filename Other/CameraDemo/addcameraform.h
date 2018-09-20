#ifndef ADDCAMERAFORM_H
#define ADDCAMERAFORM_H

#include <QWidget>
#include <QCamera>
#include <QFile>
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

protected:
    void showEvent(QShowEvent *event);

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
    QString cameraUrl;
    CAMERATYPE cameraType;
    int state;
    QList<QString> existsCameraUrls;
};

#endif // ADDCAMERAFORM_H
