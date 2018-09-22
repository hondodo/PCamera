#-------------------------------------------------
#
# Project created by QtCreator 2018-09-09T09:26:49
#
#-------------------------------------------------

QT       += core gui multimedia

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = CameraDemo
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
        main.cpp \
        mainwindow.cpp \
    3rd/videoplayer.cpp \
    cameracontrol.cpp \
    rendercontrol.cpp \
    addcameraform.cpp \
    mathelper.cpp

HEADERS += \
        mainwindow.h \
    3rd/videoplayer.h \
    cameracontrol.h \
    cameratype.h \
    rendercontrol.h \
    addcameraform.h \
    mathelper.h

FORMS += \
        mainwindow.ui \
    cameracontrol.ui \
    addcameraform.ui

win32:LIBS += -LD:/ffmpeg/dev/lib/ -lavcodec -lavformat -lswscale -lavutil -lavdevice
unix:LIBS += -lavcodec -lavformat -lswscale -lavutil -lavdevice

INCLUDEPATH += D:/ffmpeg/dev/include

win32: INCLUDEPATH+=D:\Potatokid\OpenCV\build\include
             D:\Potatokid\OpenCV\build\include\opencv
             D:\Potatokid\OpenCV\build\include\opencv2
win32: LIBS += D:\Potatokid\OpenCV\buildOpenCV\lib\libopencv_*.a

win32: INCLUDEPATH+=D:\Potatokid\OpenCV\build\include
             D:\Potatokid\OpenCV\build\include\opencv
             D:\Potatokid\OpenCV\build\include\opencv2
else:unix: INCLUDEPATH += /usr/include/opencv\
               /usr/include/opencv2\
               /usr/local/include\

win32: LIBS += D:\Potatokid\OpenCV\buildOpenCV\lib\libopencv_*.a
else:unix: LIBS += /usr/local/lib/libopencv_*.so\
                   -lmpg123\
                   -lao\
                   -lwiringPi
