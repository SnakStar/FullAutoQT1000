/*******************************************************************
 *  Copyright(c) 2000-2020 Company Name
 *  All rights reserved.
 *
 *  文件名称: CUtilSettings.cpp
 *  简要描述: 生成一个由多个控件围成一个圆,显示在父窗口上
 *
 *  创建日期: 2016-6-5
 *  作者: HuaT
 *  说明:
 *
 *  修改日期:
 *  作者:
 *  说明:
 ******************************************************************/

#include "circlelayout.h"


CircleLayout::CircleLayout(QWidget *parent) :
    QWidget(parent)
{

}

/********************************************************
 *@Name:        CreateCircleLayout
 *@Author:      HuaT
 *@Description: 创建一个圆形摆放的控件对象
 *@Param1:      圆的半径
 *@Param2:      圆的中心位置
 *@Param3:      此圆形对象有多少个小圆
 *@Return:      无
 *@Version:     1.0
 *@Date:        2018-4-13
********************************************************/
void CircleLayout::CreateCircleLayout(quint16 radius, QPoint CircleCenter, quint8 CirCount)
{
    m_nRadius = radius;
    m_posCircleCenter = CircleCenter;
    m_nCount = CirCount;

    LoopAddCircle(m_nRadius,m_posCircleCenter,m_nCount);

}

/********************************************************
 *@Name:        CreateCircleLayout
 *@Author:      HuaT
 *@Description: 创建一个圆形摆放的控件对象
 *@Param1:      圆的中心位置
 *@Param2:      此圆形对象有多少个小圆
 *@Return:      无
 *@Version:     1.0
 *@Date:        2018-4-13
********************************************************/
void CircleLayout::CreateCircleLayout(QPoint CircleCenter,quint8 CirCount)
{
    m_nRadius = 120;
    m_posCircleCenter = CircleCenter;
    m_nCount = CirCount;

    LoopAddCircle(m_nRadius,m_posCircleCenter,m_nCount);
}

/********************************************************
 *@Name:        SetItemColor
 *@Author:      HuaT
 *@Description: 设置控件背景色
 *@Param1:      需要设置的对象
 *@Return:      无
 *@Version:     1.0
 *@Date:        2018-4-13
********************************************************/
void CircleLayout::SetItemColor(QLabel *lb)
{
    QString strStyleSheet;
    strStyleSheet = QString("QLabel{border:1px ;border-radius:25px;padding:2px 4px; background:gray}QToolTip{background:white}");
    lb->setStyleSheet(strStyleSheet);
}

/********************************************************
 *@Name:        SetItemColor
 *@Author:      HuaT
 *@Description: 设置控件背景色
 *@Param1:      需要设置的对象
 *@Param2:      背景色,格式为：rgb(255,255,255)或者 red
 *@Return:      无
 *@Version:     1.0
 *@Date:        2018-4-13
********************************************************/
void CircleLayout::SetItemColor(QLabel* lb,QString strBackGroundColor)
{
    QString strStyleSheet;
    strStyleSheet = QString("border:1px solid gray;border-radius:25px;padding:2px 4px; background:%1;").arg(strBackGroundColor);
    lb->setStyleSheet(strStyleSheet);
}

void CircleLayout::SetItemColor(quint8 nIndex, QString strBackGroundColor)
{
    nIndex = nIndex - 1;
    if(m_vecPbItem.isEmpty()){
        return;
    }
    if(nIndex<0 || nIndex>m_nCount){
        return;
    }
    if(strBackGroundColor.isEmpty()){
        return;
    }
    //QString strStyleSheet;
    //strStyleSheet = QString("border:1px solid gray;border-radius:25px;padding:2px 4px; background:%1;").arg(strBackGroundColor);
    //m_vecPbItem.at(nIndex)->setStyleSheet(strStyleSheet);
    SetItemColor(m_vecPbItem.at(nIndex),strBackGroundColor);
}

/********************************************************
 *@Name:        LoopAddCircle
 *@Author:      HuaT
 *@Description: 循环象父窗口上添加小圆控件
 *@Param1:      圆的半径
 *@Param2:      圆的中心位置
 *@Param3:      此圆形对象有多少个小圆
 *@Return:      无
 *@Version:     1.0
 *@Date:        2018-4-13
********************************************************/
void CircleLayout::LoopAddCircle(quint16 radius, QPoint CircleCenter, quint8 CirCount)
{
    m_vecPbItem.clear();
    float fAngle = 360.0/CirCount;
    QLabel* pb;
    QPoint PointPos[CirCount];
    for(int n=0; n<CirCount; n++){
        PointPos[n].setX(CircleCenter.x() + radius*sin( (-fAngle*PI*n)/180) );
        PointPos[n].setY(CircleCenter.y() + radius*cos( (-fAngle*PI*n)/180) );
        //qDebug()<<n<<" x:"<<PointPos[n].x()<<"  y:"<<PointPos[n].y();
        pb = new QLabel((QWidget*)this->parent());
        m_vecPbItem.push_back(pb);
        //connect(pb,SIGNAL(clicked()), this, SLOT( ClickCircle() ) );
        pb->setVisible(true);
        pb->setText(QString::number(n+1));
        pb->setGeometry(PointPos[n].x(),PointPos[n].y(),50,50);
        pb->setAlignment(Qt::AlignVCenter|Qt::AlignHCenter);
        //背景色
        SetItemColor(pb);
        //pb->setObjectName();
    }
}

/********************************************************
 *@Name:        GetAllCircle
 *@Author:      HuaT
 *@Description: 获取所有控件的集合
 *@Param1:
 *@Return:      集合对象
 *@Version:     1.0
 *@Date:        2018-4-13
********************************************************/
void CircleLayout::SetToolTip(QLabel *lb,quint8 nSamplePos, QString strItem)
{
    QString strText = QString(QObject::tr("样本位置：%1\n测试项目：%2").arg(nSamplePos).arg(strItem));
    lb->setToolTip(strText);
}

void CircleLayout::SetToolTip(quint8 nIndex, quint8 nSamplePos, QString strItem)
{
    nIndex = nIndex - 1;
    if(nIndex<0 || nIndex>m_nCount){
        return;
    }
    //QString strText = QObject::tr(QString("样本位置：%1\n测试项目：%2").arg(nSamplePos).arg(strItem));
    SetToolTip(m_vecPbItem.at(nIndex),nSamplePos,strItem);
}





/********************************************************
 *@Name:        GetAllCircle
 *@Author:      HuaT
 *@Description: 获取所有控件的集合
 *@Param1:
 *@Return:      集合对象
 *@Version:     1.0
 *@Date:        2018-4-13
********************************************************/
QVector<QLabel *> CircleLayout::GetAllCircle()
{
    return m_vecPbItem;
}

/********************************************************
 *@Name:        SetItemText
 *@Author:      HuaT
 *@Description: 设置索引对象的显示文本
 *@Param1:      集合中的索引位置
 *@Param2:      显示文本
 *@Return:      无
 *@Version:     1.0
 *@Date:        2018-4-13
********************************************************/
void CircleLayout::SetItemText(quint8 nIndex, QString strText)
{
    nIndex = nIndex - 1;
    if(nIndex<0 || nIndex>m_nCount){
        return;
    }
    m_vecPbItem.at(nIndex)->setText(strText);
}

/********************************************************
 *@Name:        SetItemFontStyle
 *@Author:      HuaT
 *@Description: 设置索引对象的字体样式
 *@Param1:      集合中的索引位置
 *@Param2:      字体名称
 *@Param3:      字体大小
 *@Return:      无
 *@Version:     1.0
 *@Date:        2018-4-13
********************************************************/
void CircleLayout::SetItemFontStyle(quint8 nIndex, QString strFamily, quint8 nFontSize,const QColor color,bool isBold)
{
    nIndex = nIndex - 1;
    if(nIndex<0 || nIndex>m_nCount){
        return;
    }
    QFont font;
    font.setFamily(strFamily);
    font.setPixelSize(nFontSize);
    font.setBold(isBold);

    QPalette pa = m_vecPbItem.at(nIndex)->palette();
    pa.setColor(QPalette::WindowText,color);
    m_vecPbItem.at(nIndex)->setPalette(pa);
    m_vecPbItem.at(nIndex)->setFont(font);

}
//重载函数
void CircleLayout::SetItemFontStyle(quint8 nIndex, bool isBold)
{
    SetItemFontStyle(nIndex,"simsun",12,Qt::black,isBold);
}

/********************************************************
 *@Name:        HideAllCircle
 *@Author:      HuaT
 *@Description: 隐藏所有的圆
 *@Param1:      隐藏/显示
 *@Return:      无
 *@Version:     1.0
 *@Date:        2018-5-21
********************************************************/
void CircleLayout::HideAllCircle(bool bShow)
{
    for(int i=0; i<m_nCount; i++){
        m_vecPbItem.at(i)->setHidden(bShow);
    }
}
