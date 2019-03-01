﻿#include <QApplication>
#include <QTextCodec>
#include "mainwindow.h"
#include "myinputpanelcontext.h"
#include <QDir>
//#include <windows.h>
//#include <dbghelp.h>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));

    //MyInputPanelContext* ic = new MyInputPanelContext;
    //a.setInputContext(ic);

    //SetUnhandledExceptionFilter((LPTOP_LEVEL_EXCEPTION_FILTER)ApplicationCrashHandler);

    MainWindow w;
    //w.setWindowFlags(Qt::CustomizeWindowHint|Qt::FramelessWindowHint);
    w.show();
    
    return a.exec();
}
