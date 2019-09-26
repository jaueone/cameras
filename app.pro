#-------------------------------------------------
#
# Project created by QtCreator 2019-05-06T17:31:23
#
#-------------------------------------------------

QT       += core gui sql
QT       += serialport
QT       += charts
QT       += network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = app
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

INCLUDEPATH += $$PWD/Includes
INCLUDEPATH += $$PWD/Includes/GenICam
INCLUDEPATH   += $$(HALCONROOT)/include
INCLUDEPATH   += $$(HALCONROOT)/include/halconcpp

LIBS += $$PWD/Libraries/win64/MvCameraControl.lib
LIBS += $$(HALCONROOT)/lib/$$(HALCONARCH)/halconcpp.lib \
        $$(HALCONROOT)/lib/$$(HALCONARCH)/halcon.lib

SOURCES += \
        main.cpp \
        mainwindow.cpp \
        login.cpp \
        choose.cpp \
        main_page/screencheck.cpp \
        main_page/screenresult.cpp \
    main_page/main_page.cpp \
    driveseting.cpp \
    camera.cpp \
    DefectsDetect.cpp \
    my_control.cpp \
    serial.cpp \
    main_page/timeselect.cpp \
    visionworker.cpp \
    worker.cpp \
    pixitem.cpp


HEADERS += \
        mainwindow.h \
    login.h \
    datetime.h \
    choose.h \
    main_page/screencheck.h \
    main_page/screenresult.h \
    my_control.h \
    main_page/main_page.h \
    driveseting.h \
    camera.h \
    DefectsDetect.h \
    serial.h \
    main_page/timeselect.h \
    visionworker.h \
    worker.h \
    pixitem.h


FORMS += \
        mainwindow.ui \
    login.ui \
    choose.ui \
    main_page/screencheck.ui \
    main_page/screenresult.ui \
    main_page/main_page.ui \
    driveseting.ui \
    main_page/timeselect.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    image.qrc

DISTFILES += \
    app.rc

RC_FILE = \
    app.rc
