#ifndef PATHHELPER_H
#define PATHHELPER_H

#include <QObject>
#include <QDir>
#include <QFileInfo>
#include <QFileInfoList>
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

    static QFileInfoList getAllFiles(QString path, QStringList filters, bool childDir = true);
    static int getEarliestFile(const QFileInfoList *files);

    static QString getRingPath();

signals:

public slots:

private:
    QString cameraName;
    static QString fileExtn;
    static QString rootPath;
    static QString recPath;
    static QString ringPath;
    QString currentFileName;
    QString tempFileName;

    void setRootPath(const QString &value);
};

#endif // PATHHELPER_H
