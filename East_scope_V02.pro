#-------------------------------------------------
#
# Project created by QtCreator 2017-12-19T14:30:36
#
#-------------------------------------------------

QT       += core gui
RC_ICONS += scope_.ico
QT += charts
QT += widgets serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = East_scope_V02
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
    datasource.cpp \
    mainview.cpp \
    magnifyrect.cpp \
    wareformgenerator.cpp \
    magnifyview.cpp \
    controllbar.cpp \
    mainwindow.cpp \
    chanelpickdialog.cpp \
    channelimfordelegate.cpp \
    checkboxdelegate.cpp \
    communicationpara.cpp \
    messurecursor.cpp \
    dataprocessor.cpp \
    callout.cpp \
    connectdialog.cpp \
    scithread.cpp \
    scopeimfor.cpp \
    recorddata.cpp \
    customtablemodel.cpp \
    eventtable.cpp \
    eventselectdialog.cpp \
    canthread.cpp \
    canframe.cpp

HEADERS += \
    datasource.h \
    mainview.h \
    magnifyrect.h \
    wareformgenerator.h \
    magnifyview.h \
    controllbar.h \
    mainwindow.h \
    chanelpickdialog.h \
    channelimfordelegate.h \
    checkboxdelegate.h \
    communicationpara.h \
    messurecursor.h \
    dataprocessor.h \
    callout.h \
    connectdialog.h \
    scithread.h \
    scopeimfor.h \
    recorddata.h \
    customtablemodel.h \
    eventtable.h \
    eventselectdialog.h \
    ControlCAN.h \
    canthread.h \
    canframe.h

FORMS += \
    chanelpickdialog.ui \
    connectdialog.ui

win32: LIBS += -L$$PWD/./ -lControlCAN
INCLUDEPATH += $$PWD/.
DEPENDPATH += $$PWD/.

RESOURCES += \
    image.qrc \
