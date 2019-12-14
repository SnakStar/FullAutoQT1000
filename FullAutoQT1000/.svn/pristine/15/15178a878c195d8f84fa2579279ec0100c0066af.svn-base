#ifndef LOGVIEW_H
#define LOGVIEW_H

#include <QObject>
#include <QFile>
#include <qmath.h>
#include <QTextStream>

#define PAGESIZE 50000  //50K

class LogView : public QObject
{
    Q_OBJECT
public:
    explicit LogView(QObject *parent = NULL);

    bool OpenFile(QString strPath);
    void CloseFile();
    QString GetFilePath();
    qint16 GetCurPage();
    qint16 GetCountPage();
    //QString ReadSize(qint64 nSize);
    QString ReadAll();
    QString PrePage();
    QString NextPage();
    QString HeadPage();
    QString TailPage();

signals:

public slots:
private:
    QFile* m_pFile;
    QTextStream* m_pTs;
    qint64 m_nFileSeekIndex;
    qint64 m_nFileSize;
    QString m_strFilePath;
    qint32 m_nCountPage;
    qint16 m_nCurPage;
};

#endif // LOGVIEW_H
