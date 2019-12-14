#-------------------------------------------------
#
# Project created by QtCreator 2018-04-20T11:50:52
#
#-------------------------------------------------

QT       += core gui sql printsupport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = FullAutoQT1000
TEMPLATE = app

#LIBS += -lDbgHelp

#QMAKE_CXXFLAGS += -std=c++0x

#QMAKE_LFLAGS_RELEASE = /INCREMENTAL:NO /DEBUG

SOURCES    += main.cpp\
    logview.cpp \
    mainwindow.cpp \
    msgboxdlg.cpp \
    testinterface.cpp \
    queryinterface.cpp \
    qextserial/qextserialport.cpp \
    circlelayout.cpp \
    qcinterface.cpp \
    setinterface.cpp \
    cqtprodb.cpp \
    cutilsettings.cpp \
    resualcalc/resultcalc.cpp \
    qcustomplot.cpp \
    logoutput.cpp \
    cutiltool.cpp \
    renfvalue.cpp \
    debugintefacedlg.cpp \
    logindlg.cpp \
    usersetdlg.cpp \
    cpackcustomplot.cpp \
    csimplehl7.cpp \
    qmysplashscreen.cpp \
    udiskshow.cpp \
    upgrading.cpp
win32{
    SOURCES += qextserial/qextserialport_win.cpp
}
unix{
    SOURCES += qextserial/qextserialport_unix.cpp
}

HEADERS    += mainwindow.h \
    logview.h \
    msgboxdlg.h \
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
    CommonDef.h \
    qcustomplot.h \
    logoutput.h \
    paramdef.h \
    cutiltool.h \
    renfvalue.h \
    debugintefacedlg.h \
    logindlg.h \
    usersetdlg.h \
    cpackcustomplot.h \
    csimplehl7.h \
    qmysplashscreen.h \
    udiskshow.h \
    upgrading.h

FORMS    += mainwindow.ui \
    msgboxdlg.ui \
    testinterface.ui \
    queryinterface.ui \
    qcinterface.ui \
    setinterface.ui \
    renfvalue.ui \
    debugintefacedlg.ui \
    logindlg.ui \
    usersetdlg.ui \
    udiskshow.ui

include($$PWD/input2018/input2018.pri)

RESOURCES += \
    res.qrc

DISTFILES += \
    res/image/Login/LoginBackground.png
