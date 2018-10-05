#-------------------------------------------------
#
# Project created by QtCreator 2018-09-09T09:26:49
#
#-------------------------------------------------

QT       += core gui multimedia opengl network

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
    mathelper.cpp \
    3rd/ffmpeghelper.cpp \
    videofilehelper.cpp \
    3rd/gl_widget.cpp \
    cameracontrolgl.cpp \
    cameracontrolopengl.cpp \
    3rd/glplayer.cpp \
    #camerathread.cpp \
    camerathreadmux.cpp \
    pathhelper.cpp \
    cameracollectorhepler.cpp \
    #Core/cascadedetectoradapter.cpp \
    Core/diskhelper.cpp \
    Core/facedetecthelper.cpp \
    Core/ImageFormat.cpp \
    Core/mogdetectobject.cpp \
    Core/oled12864.cpp \
    Core/ringhelper.cpp \
    Core/videoprop.cpp \
    datetimecontrol.cpp \
    3rd/chineseyear.cpp \
    oledthread.cpp \
    ringthread.cpp \
    weathercontrol.cpp \
    weatherreportcontrol.cpp \
    camerathreadh264.cpp

HEADERS += \
        mainwindow.h \
    3rd/videoplayer.h \
    cameracontrol.h \
    cameratype.h \
    rendercontrol.h \
    addcameraform.h \
    mathelper.h \
    3rd/ffmpeghelper.h \
    videofilehelper.h \
    3rd/gl_widget.h \
    cameracontrolgl.h \
    cameracontrolopengl.h \
    3rd/glplayer.h \
    #camerathread.h \
    camerathreadmux.h \
    pathhelper.h \
    cameracollectorhelper.h \
    Core/Blob.h \
    #Core/cascadedetectoradapter.h \
    Core/diskhelper.h \
    Core/facedetecthelper.h \
    Core/ImageFormat.h \
    Core/mogdetectobject.h \
    Core/oled12864.h \
    Core/ringhelper.h \
    Core/videoprop.h \
    datetimecontrol.h \
    3rd/chineseyear.h \
    oledthread.h \
    ringthread.h \
    weathercontrol.h \
    weatherreportcontrol.h \
    camerathreadh264.h

FORMS += \
        mainwindow.ui \
    cameracontrol.ui \
    addcameraform.ui \
    cameracontrolopengl.ui \
    datetimecontrol.ui \
    weathercontrol.ui \
    weatherreportcontrol.ui

win32:LIBS += -LD:/ffmpeg/dev/lib/ -lavcodec -lavformat -lswscale -lavutil -lavdevice -lavfilter -lpostproc -lswresample
unix:LIBS += -L/usr/local/lib/ -lavcodec -lavformat -lswscale -lavutil -lavdevice -lavfilter -lpostproc -lswresample

INCLUDEPATH += D:/ffmpeg/dev/include

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

win32:LIBS += -lOpengl32 \
                -lglu32 \
                -lglut
unix: LIBS += -lGL -lGLU -lglut

RESOURCES += \
    rcs.qrc
