#ifndef DISKHELPER_H
#define DISKHELPER_H

#include <QObject>
#include <QStorageInfo>

class DiskHelper : public QObject
{
    Q_OBJECT
public:
    explicit DiskHelper(QObject *parent = nullptr);
    ~DiskHelper();

    QString getPath() const;
    void setPath(const QString &value);

    qint64 bytesTotal();
    qint64 bytesFree();
    qint64 bytesAvailable();
    QString toSizeInfo(quint64 size);

    QStorageInfo *getStorageInfo() const;

signals:

public slots:

private:
    QStorageInfo *storageInfo;
    QString path;
    qint64 kbSize, mbSize, gbSize, tbSize;
};

#endif // DISKHELPER_H
