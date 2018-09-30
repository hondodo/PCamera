#ifndef PATHHELPER_H
#define PATHHELPER_H

#include <QObject>
#include <QDir>
#include <QDateTime>

class PathHelper : public QObject
{
    Q_OBJECT
public:
    explicit PathHelper(QObject *parent = nullptr);

    void init();

    QString getCameraName() const;
    QString getCameraNameForFileName() const;
    QString getTempFileName() const;
    QString getRootPath() const;
    QString getRecPath() const;
    QString getCurrentFileName() const;
    void creatNewFileName();

    void setCameraName(const QString &value);
    void setRootPath(const QString &value);

signals:

public slots:

private:
    QString cameraName;
    QString rootPath;
    QString recPath;
    QString currentFileName;
    QString tempFileName;
};

#endif // PATHHELPER_H
