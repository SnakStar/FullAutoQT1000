﻿#include <QApplication>
#include <QTextCodec>
#include "mainwindow.h"
#include <QDir>
#include "input2018/frminput2018.h"
#include "qmysplashscreen.h"
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

    frmInput2018::Instance()->hide();
    frmInput2018::Instance()->setFrmSize(750, 280);
    frmInput2018::Instance()->setStyle("silvery");
    //
    /*QFont fonta;
    fonta.setPixelSize(16);
    fonta.setFamily("WenQuanYi Micro Hei");
    a.setFont(fonta);*/
    //
    QPixmap pix(":/res/image/background.png");
    QMySplashScreen splash(pix);
    QFont font("WenQuanYi Micro Hei",12);
    splash.setFont(font);
    splash.show();
    //
    QString strMsg = "仪器正在启动,请稍后";
    QString strTemp = "";
    int nIndex = 0;
    int nDelayTime = 27;//27

    QDateTime now;
    QElapsedTimer elapsed;
    elapsed.start();
    while(elapsed.elapsed() < nDelayTime*1000){
        if(nIndex == 5){
            nIndex = 0;
            strTemp = "";
        }else{
            nIndex++;
            strTemp += ".";
        }
        splash.showMessage(strMsg+strTemp,Qt::AlignCenter|Qt::AlignBottom,QColor(43,155,217));
        a.processEvents();
        //
        QDateTime dt = QDateTime::currentDateTime();
        while (dt.secsTo(now)<1) {
            now = QDateTime::currentDateTime();
            a.processEvents();
        }
    }

    //
    MainWindow w;
    w.setWindowFlags(Qt::CustomizeWindowHint|Qt::FramelessWindowHint);
    //
    /*QFile qssFile(":/res/qss/style.qss");
    qssFile.open(QIODevice::ReadOnly);
    if(qssFile.isOpen()){
        w.setStyleSheet(qssFile.readAll());
    }*/
    w.show();
    splash.finish(&w);

    return a.exec();
}
