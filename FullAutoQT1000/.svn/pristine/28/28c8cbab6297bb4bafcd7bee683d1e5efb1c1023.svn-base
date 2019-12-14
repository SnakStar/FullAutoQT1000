#include "logview.h"

LogView::LogView(QObject *parent) : QObject(parent)
{
    m_nFileSeekIndex = 0;
    m_nFileSize = 0;
    m_pFile = NULL;
    m_strFilePath.clear();
    m_nCountPage = 0;
    m_nCurPage = 0;
}

void LogView::CloseFile()
{
    if(m_pFile == NULL){
        return;
    }
    if(m_pFile->isOpen()){
        m_pFile->close();
        delete m_pFile;
        delete m_pTs;
        m_pFile = NULL;
        m_pTs = NULL;
        m_nCurPage = 0;
        m_nCountPage = 0;
    }
    m_nFileSeekIndex = 0;
}

QString LogView::GetFilePath()
{
    return m_strFilePath;
}

qint16 LogView::GetCurPage()
{
    return m_nCurPage;
}

qint16 LogView::GetCountPage()
{
    return m_nCountPage;
}

bool LogView::OpenFile(QString strPath)
{
    CloseFile();
    m_strFilePath = strPath;
    m_pFile = new QFile(strPath);
    m_pTs = new QTextStream(m_pFile);
    m_nFileSize = m_pFile->size();
    double f = ((double)m_nFileSize / PAGESIZE);
    m_nCountPage = qCeil(f);
    return m_pFile->open(QIODevice::ReadOnly);

}

//QString LogView::ReadSize(qint64 nSize)
//{
//    //m_pTs->seek(pos)
//    return m_pTs->read(nSize);
//}

QString LogView::ReadAll()
{
    return m_pTs->readAll();
}

QString LogView::PrePage()
{
    QString strData;
    m_nCurPage -= 1;
    if(m_nCurPage<1){
        m_nCurPage = 1;
    }
    if(m_nFileSeekIndex - PAGESIZE <= 0){
        m_nFileSeekIndex = PAGESIZE;
        m_pTs->seek(0);
        strData = m_pTs->read(PAGESIZE);
        return strData;
    }else{
        m_nFileSeekIndex = m_nFileSeekIndex-PAGESIZE*2;
        if(m_nFileSeekIndex<0){
            m_nFileSeekIndex = 0;
        }
        m_pTs->seek(m_nFileSeekIndex);
        strData = m_pTs->read(PAGESIZE);
        m_nFileSeekIndex += PAGESIZE;
        return strData;
    }
}

QString LogView::NextPage()
{
    QString strData;
    m_nCurPage += 1;
    if(m_nCurPage > m_nCountPage){
        m_nCurPage = m_nCountPage;
    }
    if(m_nFileSeekIndex + PAGESIZE >= m_nFileSize){
        m_nFileSeekIndex = m_nFileSize;
        m_nFileSeekIndex -= PAGESIZE;
        m_pTs->seek(m_nFileSeekIndex);
        strData = m_pTs->read(PAGESIZE);
        if(strData.isEmpty()){
            return "已到结尾";
        }else{
            return strData;
        }
    }else{
        m_nFileSeekIndex += PAGESIZE;
        //m_pTs->seek(m_nFileSeekIndex);
        strData = m_pTs->read(PAGESIZE);
        return strData;
    }


}

QString LogView::HeadPage()
{
    QString strData;
    m_nFileSeekIndex = 0;
    m_pTs->seek(0);
    strData = m_pTs->read(PAGESIZE);
    m_nCurPage = 1;
    return strData;
}

QString LogView::TailPage()
{
    QString strData;
    m_nCurPage = m_nCountPage;
    if(m_nFileSize < PAGESIZE){
        HeadPage();
    }else{
        m_nFileSeekIndex = m_nFileSize-PAGESIZE;
        m_pTs->seek(m_nFileSeekIndex);
        strData = m_pTs->read(PAGESIZE);
        return strData;
    }
}
