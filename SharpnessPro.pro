#-------------------------------------------------
#
# Project created by QtCreator 2019-02-27T16:24:41
#
#-------------------------------------------------

QT       += core gui printsupport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = SharpnessPro
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

CONFIG += c++11
QMAKE_CXXFLAGS += "-Wno-old-style-cast"

SOURCES += \
        main.cpp \
        dialogs/mainwindow.cpp \
        services/imgservice.cpp \
        services/mask.cpp \
    services/obj.cpp \
    services/imgprocessor.cpp \
    widgets/qcustomplot.cpp

HEADERS += \
        dialogs/mainwindow.h \
        static.h \
        services/imgservice.h \
        services/mask.h \
    services/obj.h \
    services/imgprocessor.h \
    widgets/qcustomplot.h

FORMS += \
        dialogs/mainwindow.ui

INCLUDEPATH += C:\opencv\build\include

LIBS += C:\opencv-build\bin\libopencv_core410.dll
LIBS += C:\opencv-build\bin\libopencv_highgui410.dll
LIBS += C:\opencv-build\bin\libopencv_imgcodecs410.dll
LIBS += C:\opencv-build\bin\libopencv_imgproc410.dll
LIBS += C:\opencv-build\bin\libopencv_features2d410.dll
LIBS += C:\opencv-build\bin\libopencv_calib3d410.dll

win32: RC_FILE = iconsource.rc

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    sources.qrc
