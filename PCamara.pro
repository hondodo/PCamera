#-------------------------------------------------
#
# Project created by QtCreator 2018-08-08T17:20:51
#
#-------------------------------------------------

QT       += core gui multimedia network network xml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = PCamara
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
        dialog.cpp \
    cameracontrol.cpp \
    Core/ImageFormat.cpp \
    Thread/camarathread.cpp \
    #Core/cascadedetectoradapter.cpp
    Core/facedetecthelper.cpp \
    Core/ringhelper.cpp \
    Thread/cameracollectorthread.cpp \
    Core/oled12864.cpp \
    maindialog.cpp \
    Thread/ringthread.cpp \
    Thread/oledthread.cpp \
    Core/videoprop.cpp \
    Core/mogdetectobject.cpp \
    Core/diskhelper.cpp \
    datetimecontrol.cpp \
    3rd/chineseyear.cpp \
    weathercontrol.cpp \
    weatherreportcontrol.cpp

HEADERS += \
        dialog.h \
    cameracontrol.h \
    Core/Blob.h \
    Core/ImageFormat.h \
    Thread/camarathread.h \
    #Core/cascadedetectoradapter.h
    Core/facedetecthelper.h \
    Core/ringhelper.h \
    Thread/cameracollectorthread.h \
    Core/oled12864.h \
    maindialog.h \
    Thread/ringthread.h \
    Thread/oledthread.h \
    Core/videoprop.h \
    Core/mogdetectobject.h \
    Core/diskhelper.h \
    datetimecontrol.h \
    3rd/chineseyear.h \
    weathercontrol.h \
    weatherreportcontrol.h

FORMS += \
        dialog.ui \
    cameracontrol.ui \
    maindialog.ui \
    datetimecontrol.ui \
    weathercontrol.ui \
    weatherreportcontrol.ui


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

RESOURCES += \
    prc.qrc
