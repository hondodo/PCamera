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
    3rd/capturedemo.cpp

HEADERS += \
        mainwindow.h \
    3rd/videoplayer.h \
    3rd/capturedemo.h

FORMS += \
        mainwindow.ui


LIBS += -lavcodec -lavformat -lswscale -lavutil -lavdevice

win32:LIBS += -LD:/ffmpeg/dev/lib/ -lavcodec -lavformat -lswscale -lavutil -lavdevice
win32:INCLUDEPATH += D:/ffmpeg/dev/include

win32: INCLUDEPATH+=D:\Potatokid\OpenCV\build\include
             D:\Potatokid\OpenCV\build\include\opencv
             D:\Potatokid\OpenCV\build\include\opencv2
win32: LIBS += D:\Potatokid\OpenCV\buildOpenCV\lib\libopencv_*.a

win32: LIBS += -L$$PWD/../../SDL2-devel-2.0.8-VC/SDL2-2.0.8/lib/ -lSDL2

INCLUDEPATH += $$PWD/../../SDL2-devel-2.0.8-VC/SDL2-2.0.8/include
DEPENDPATH += $$PWD/../../SDL2-devel-2.0.8-VC/SDL2-2.0.8/include

win32: LIBS += -L$$PWD/../../SDL2-devel-2.0.8-VC/SDL2-2.0.8/lib/ -lSDL2main

INCLUDEPATH += $$PWD/../../SDL2-devel-2.0.8-VC/SDL2-2.0.8/include
DEPENDPATH += $$PWD/../../SDL2-devel-2.0.8-VC/SDL2-2.0.8/include

win32: LIBS += -L$$PWD/../../SDL2-devel-2.0.8-VC/SDL2-2.0.8/lib/ -lSDL2test

INCLUDEPATH += $$PWD/../../SDL2-devel-2.0.8-VC/SDL2-2.0.8/include
DEPENDPATH += $$PWD/../../SDL2-devel-2.0.8-VC/SDL2-2.0.8/include
