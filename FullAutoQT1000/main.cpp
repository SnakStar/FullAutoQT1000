﻿#include <QApplication>
#include <QTextCodec>
#include "mainwindow.h"

#include <QDir>
//#include <windows.h>
//#include <dbghelp.h>

/*long ApplicationCrashHandler(EXCEPTION_POINTERS *pException){
    {
        // 在程序exe的上级目录中创建dmp文件夹
        QDir *dmp = new QDir;
        bool exist = dmp->exists("../dmp/");
        if(exist == false)
            dmp->mkdir("../dmp/");
    }
    QDateTime current_date_time = QDateTime::currentDateTime();
    QString current_date = current_date_time.toString("yyyy_MM_dd_hh_mm_ss");
    QString time =  current_date + ".dmp";
    EXCEPTION_RECORD *record = pException->ExceptionRecord;
    QString errCode(QString::number(record->ExceptionCode, 16));
    QString errAddr(QString::number((uint)record->ExceptionAddress, 16));
    QString errFlag(QString::number(record->ExceptionFlags, 16));
    QString errPara(QString::number(record->NumberParameters, 16));
    qDebug()<<"errCode: "<<errCode;
    qDebug()<<"errAddr: "<<errAddr;
    qDebug()<<"errFlag: "<<errFlag;
    qDebug()<<"errPara: "<<errPara;
    HANDLE hDumpFile = CreateFile((LPCWSTR)QString("../dmp/" + time).utf16(),
                                  GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if(hDumpFile != INVALID_HANDLE_VALUE) {
        MINIDUMP_EXCEPTION_INFORMATION dumpInfo;
        dumpInfo.ExceptionPointers = pException;
        dumpInfo.ThreadId = GetCurrentThreadId();
        dumpInfo.ClientPointers = TRUE;
        MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(),hDumpFile, MiniDumpNormal, &dumpInfo, NULL, NULL);
        CloseHandle(hDumpFile);
    }
    else{
        qDebug()<<"hDumpFile == null";
    }
    return EXCEPTION_EXECUTE_HANDLER;
}

LONG ApplicationCrashHandler(EXCEPTION_POINTERS *pException){//程式异常捕获

    //保存数据代码***

    //这里弹出一个错误对话框并退出程序
    EXCEPTION_RECORD* record = pException->ExceptionRecord;
    QString errCode(QString::number(record->ExceptionCode,16)),errAdr(QString::number((uint)record->ExceptionAddress,16)),errMod;
    QMessageBox::critical(NULL,"程式崩溃","<FONT size=4><div><b>对于发生的错误，表示诚挚的歉意</b><br/></div>"+
        QString("<div>错误代码：%1</div><div>错误地址：%2</div></FONT>").arg(errCode).arg(errAdr),
        QMessageBox::Ok);
    return EXCEPTION_EXECUTE_HANDLER;
}*/

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));

    //SetUnhandledExceptionFilter((LPTOP_LEVEL_EXCEPTION_FILTER)ApplicationCrashHandler);

    MainWindow w;
    //w.setWindowFlags(Qt::CustomizeWindowHint|Qt::FramelessWindowHint);
    w.show();
    
    return a.exec();
}
