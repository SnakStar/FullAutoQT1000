#-------------------------------------------------
#
# Project created by QtCreator 2018-04-20T11:50:52
#
#-------------------------------------------------

QT       += core gui sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = FullAutoQT1000
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    testinterface.cpp \
    queryinterface.cpp \
    qextserial/qextserialport.cpp \
    circlelayout.cpp \
    qcinterface.cpp \
    setinterface.cpp \
    cqtprodb.cpp \
    cutilsettings.cpp \
    resualcalc/resultcalc.cpp
win32{
    SOURCES += qextserial/qextserialport_win.cpp
}
unix{
    SOURCES += canbus.cpp
    SOURCES += qextserial/qextserialport_unix.cpp
    HEADERS += canbus.h
}

HEADERS  += mainwindow.h \
    testinterface.h \
    queryinterface.h \
    qextserial/qextserialport_p.h \
    qextserial/qextserialport_global.h \
    qextserial/qextserialport.h \
    circlelayout.h \
    qcinterface.h \
    setinterface.h \
    cqtprodb.h \
    cutilsettings.h \
    resualcalc/resultcalc.h \
    resualcalc/CalcDef.h \
    CommandStructDef.h \
    ResultStructDef.h \
    CommonDef.h

FORMS    += mainwindow.ui \
    testinterface.ui \
    queryinterface.ui \
    qcinterface.ui \
    setinterface.ui
