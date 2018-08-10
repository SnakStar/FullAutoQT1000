#ifndef LOGOUTPUT_H
#define LOGOUTPUT_H

#include <QObject>
#include <QDateTime>
#include <QTextStream>
#include <QFile>


class LogOutput : public QObject
{
    Q_OBJECT
public:
    explicit LogOutput(QObject *parent = NULL);
public:
    void StartLog();
    void StopLog();

    void SetLogSavePath(QString strPath);

    QString ReadLog();

private:
    static void myMessageOutput(QtMsgType type, const char* msg);

private:
    static QString m_strLogPath;

signals:

public slots:
};



#endif // LOGOUTPUT_H
