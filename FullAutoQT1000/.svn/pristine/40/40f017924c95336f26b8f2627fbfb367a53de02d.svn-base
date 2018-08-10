#include "logoutput.h"

QString LogOutput::m_strLogPath = "./log.txt";

LogOutput::LogOutput(QObject *parent) : QObject(parent)
{
    //m_strLogPath = "./log.txt";
}

void LogOutput::StartLog()
{
    qInstallMsgHandler(myMessageOutput);
}

void LogOutput::StopLog()
{
    qInstallMsgHandler(0);
}

void LogOutput::SetLogSavePath(QString strPath)
{
    m_strLogPath = strPath;
}

QString LogOutput::ReadLog()
{
    QFile file(m_strLogPath);
    if(file.open(QIODevice::ReadOnly | QIODevice::Text)){
        QTextStream ts(&file);
        ts.setCodec("system");
        return ts.readAll();
    }
    return "";
}

void LogOutput::myMessageOutput(QtMsgType type, const char* msg)
{
    QString text;
    switch (type)
    {
    case QtDebugMsg:
        text = QString(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss ")+"[Debug]: %1\r\n").arg(msg);
        break;
    case QtWarningMsg:
        text = QString(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss ")+"[Warning]: %1\r\n").arg(msg);
        break;
    case QtCriticalMsg:
        text = QString(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss ")+"[Critical]: %1\r\n").arg(msg);
        break;
    case QtFatalMsg:
        text = QString(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss ")+"[Fatal]: %1\r\n").arg(msg);
        abort();
    }
    QFile file(m_strLogPath);
    file.open(QIODevice::WriteOnly | QIODevice::Append);
    if(file.size()/1000 > 20){
        file.close();
        file.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Truncate);
        file.close();
        file.open(QIODevice::WriteOnly | QIODevice::Append);
    }
    QTextStream ts(&file);
    ts.setCodec("system");
    ts<<text<<endl;
}
