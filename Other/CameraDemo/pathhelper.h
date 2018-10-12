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

    QString getFileExtn() const;

signals:

public slots:

private:
    QString cameraName;
    static QString fileExtn;
    static QString rootPath;
    static QString recPath;
    QString currentFileName;
    QString tempFileName;

    void setRootPath(const QString &value);
};

#endif // PATHHELPER_H
