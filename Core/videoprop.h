#ifndef VIDEOPROP_H
#define VIDEOPROP_H

#include <QtCore>
#include <QDateTime>

class VideoProp
{
public:
    VideoProp();

    int getWidth() const;
    void setWidth(int value);

    int getHeight() const;
    void setHeight(int value);

    int getFps() const;
    void setFps(int value);

    QString getFileName() const;
    void setFileNameBuildNew();

    QString getDir() const;
    void setDirBuildNew();

    QString getFileNameTag() const;
    void setFileNameTag(const QString &value);

private:
    int width;
    int height;
    int fps;
    QString fileName;
    QString fileNameTag;
    QString dir;
};

#endif // VIDEOPROP_H
