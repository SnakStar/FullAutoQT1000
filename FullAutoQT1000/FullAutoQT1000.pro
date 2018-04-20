#-------------------------------------------------
#
# Project created by QtCreator 2018-04-20T11:50:52
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = FullAutoQT1000
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    testinterface.cpp \
    queryinterface.cpp \
    qextserial/qextserialport_win.cpp \
    qextserial/qextserialport_unix.cpp \
    qextserial/qextserialport.cpp

HEADERS  += mainwindow.h \
    testinterface.h \
    queryinterface.h \
    qextserial/qextserialport_p.h \
    qextserial/qextserialport_global.h \
    qextserial/qextserialport.h

FORMS    += mainwindow.ui \
    testinterface.ui \
    queryinterface.ui
