#include "cutiltool.h"

CUtilTool::CUtilTool()
{

}

/********************************************************
 *@Name:        ProcValue
 *@Author:      HuaT
 *@Description: 处理参考值上下限的格式
 *@Param1:      参考值下限
 *@Param2:      参考值上限
 *@Return:      格式化的参考值，不符合要求返回空值
 *@Version:     1.0
 *@Date:        2016-5-13
********************************************************/
QString CUtilTool::ProcValue(QString lower, QString upper)
{
    QString FormatValue = "";
    if(lower.isEmpty() && upper.isEmpty()){
        return FormatValue;
    }
    if(lower.isEmpty()){
        FormatValue = QString("<%1").arg(upper);
    }else if(upper.isEmpty()){
        FormatValue = QString(">%1").arg(lower);
    }else{
        FormatValue = QString("%1-%2").arg(lower).arg(upper);
    }
    return FormatValue;
}

/********************************************************
 *@Name:        ParseValue
 *@Author:      HuaT
 *@Description: 解析参考值
 *@Param:       带符号的参考值
 *@Return:      返回下限、上限值，返回格式固定为：
 *              0：下限，没有则插入空
 *              1：上限，没有则插入空
 *@Version:     1.0
 *@Date:        2016-5-13
********************************************************/
QStringList CUtilTool::ParseValue(QString value)
{
    QStringList valueList;
    int index = -1;
    if(value.indexOf("<") != -1){
        index = value.indexOf("<");
        valueList.append("");
        valueList.append(value.mid(index+1));
    }else if(value.indexOf(">") != -1){
        index = value.indexOf(">");
        valueList.append(value.mid(index+1));
        valueList.append("");
    }else if(value.indexOf("-") != -1){
        index = value.indexOf("-");
        valueList.append(value.mid(0,index));
        valueList.append(value.mid(index+1));
    }else{
        valueList.append("");
        valueList.append("");
    }
    return valueList;
}

/********************************************************
 *@Name:        ParseTestUnit
 *@Author:      HuaT
 *@Description: 把结果和单位分离并返回
 *@Param:       带结果单位的字符串
 *@Return:      返回结果和单位的字符链表
 *@Version:     1.0
 *@Date:        2017-2-22
********************************************************/
QStringList CUtilTool::ParseTestUnit(QString strResult)
{
    QStringList listResult;
    quint8 nAsc;
    for(int n=0; n<strResult.size(); n++){
        nAsc = (quint8)strResult.at(n).toAscii();
        //如果不是数字了,则就是单位开始了
        if(nAsc > 62){
            listResult.append(strResult.left(n));
            listResult.append(strResult.right(strResult.size()-n));
        }
    }
    return listResult;
}


/********************************************************
 *@Name:        GetRenfValue
 *@Author:      HuaT
 *@Description: 通过病人信息，获取参考值
 *@Param1:      通
 *@Param2:      对应测试状态
 *@Return:      对应病人结果的参考值
 *@Version:     1.0
 *@Date:        2016-12-29
********************************************************/
QString CUtilTool::GetRenfValue(QString strAge, QStringList listRenfValue)
{
    QString strRenf;
    quint8 nAge = strAge.toInt();
    if(listRenfValue.size() == 0){
        return strRenf;
    }
    quint32 nAgeLow,nAgeHight;
    for(int n=0; n<listRenfValue.size(); n+=5){

        if(listRenfValue.at(n+2).isEmpty() && !listRenfValue.at(n+2).isEmpty()){
            nAgeLow = 0;
            nAgeHight = listRenfValue.at(n+3).toInt();
            if(nAge <= nAgeHight){
                strRenf = listRenfValue.at(n+1);
            }
        }else if(!listRenfValue.at(n+2).isEmpty() && listRenfValue.at(n+3).isEmpty()){
            nAgeLow = listRenfValue.at(n+2).toInt();
            nAgeHight = 0;
            if(nAge >= nAgeLow){
                strRenf = listRenfValue.at(n+1);
            }
        }else if(listRenfValue.at(n+2).isEmpty() && listRenfValue.at(n+3).isEmpty()){
            strRenf = listRenfValue.at(n+1);
        }else{
            nAgeLow = listRenfValue.at(n+2).toInt();
            nAgeHight = listRenfValue.at(n+3).toInt();
            if(nAge>=nAgeLow && nAge<=nAgeHight){
                strRenf = listRenfValue.at(n+1);
            }
        }
    }
    return strRenf;

}


/********************************************************
 *@Name:        GetResultFlag
 *@Author:      HuaT
 *@Description: 通过病人信息，获取参考值
 *@Param1:      病人对应项目参考值
 *@Param2:      病人检测结果
 *@Return:      返回病人打印标志是↑↓
 *@Version:     1.0
 *@Date:        2016-12-29
********************************************************/
QString CUtilTool::GetResultFlag(QString strRenf, QString strResult)
{
    QString strFlag;
    if(strRenf.isEmpty()){
        return strFlag;
    }
    QString re;
    quint8 nAscii = 0;
    quint16 nResultCount = strResult.size();
    for(int n=0; n<nResultCount; n++){
        nAscii = (quint8)(strResult.at(n).toAscii());
        if(nAscii == 62 || nAscii == 60){
            continue;
        }
        if(nAscii >= 48 && nAscii <=57 || nAscii == 46){
            re.append(strResult.at(n));
        }else{
            break;
        }
    }
    float fResult = re.toFloat();
    float fRenfLow,fRenfHight;
    QStringList listRenf = ParseValue(strRenf);
    if(!strResult.isEmpty()){
        nAscii = (quint8)(strResult.at(0).toAscii());
    }
    //只有上限，没有下限，小于号系列
    if(listRenf.at(0).isEmpty()){
        fRenfLow = 0;
        fRenfHight = listRenf.at(1).toFloat();
        if(fResult >= fRenfHight){
            strFlag="↑";
        }
        if( (fResult == fRenfHight) && (nAscii == 60) ){
            strFlag="";
        }
    //只有下限，没有上限，大于号系列
    }else if(listRenf.at(1).isEmpty()){
        fRenfLow = listRenf.at(0).toFloat();
        fRenfHight = 0;
        if(fResult <=fRenfLow){
            strFlag = "↓";
        }
        if( (fResult == fRenfLow) && (nAscii == 62) ){
            strFlag="";
        }
    //区间值系列
    }else{
        fRenfLow = listRenf.at(0).toFloat();
        fRenfHight = listRenf.at(1).toFloat();
        if(fResult >= fRenfHight){
            strFlag = "↑";
        }else if(fResult <= fRenfLow){
            strFlag = "↓";
        }
    }
    return strFlag;

}
