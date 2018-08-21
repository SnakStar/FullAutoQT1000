#ifndef CUTILTOOL_H
#define CUTILTOOL_H


#include<QString>
#include<QStringList>

class CUtilTool
{
public:
    CUtilTool();
public:
    //处理上下限的值，返回需要的格式
    QString ProcValue(QString lower,QString upper);
    //解析传入的结果值，处理成上下限的格式，at(0)为下限，at(1)为上限
    QStringList ParseValue(QString value);
    //分离结果和单位
    QStringList ParseTestUnit(QString strResult);
    //获取参考值
    QString GetRenfValue(QString strAge, QStringList listRenfValue);
    //获取结果标志
    QString GetResultFlag(QString strRenf, QString strResult);
};

#endif // CUTILTOOL_H
