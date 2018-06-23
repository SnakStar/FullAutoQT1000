/*******************************************************************
 *  Copyright(c) 2000-2020 Company Name
 *  All rights reserved.
 *
 *  文件名称: CUtilSettings.h
 *  简要描述: 生成一个由多个控件围成一个圆,显示在父窗口上
 *
 *  创建日期: 2018-4-13
 *  作者: HuaT
 *  说明:
 *
 *  修改日期:
 *  作者:
 *  说明:
 ******************************************************************/
#ifndef CIRCLELAYOUT_H
#define CIRCLELAYOUT_H

#include <QWidget>
#include<qmath.h>
#include<QPushButton>
#include<QLabel>
#include<QDebug>

#define PI 3.1415

class CircleLayout : public QWidget
{
    Q_OBJECT
public:
    explicit CircleLayout(QWidget *parent = 0);
public:
    //根据控件数排列成圆状
    void CreateCircleLayout(quint16 radius, QPoint CircleCenter, quint8 CirCount);
    void CreateCircleLayout(QPoint CircleCenter, quint8 CirCount);
    //设置控件背景色
    void SetItemColor(QLabel *lb);
    void SetItemColor(QLabel *lb, QString strBackGroundColor);
    void SetItemColor(quint8 nIndex, QString strBackGroundColor);
    //设置控件文本
    void SetItemText(QLabel *lb,QString strText);
    void SetItemText(quint8 nIndex, QString strText);
    //设置控件字体样式
    void SetItemFontStyle(quint8 nIndex,QString strFamily,quint8 nFontSize,const QColor color,bool isBold = false);
    void SetItemFontStyle(quint8 nIndex, bool isBold = false);
    //显示圆形对象
    void HideAllCircle(bool bShow);
public:
    //获取控件集合
    QVector<QLabel*> GetAllCircle();
private:
    //循环生成控件对象
    void LoopAddCircle(quint16 radius, QPoint CircleCenter, quint8 CirCount);
    //设置提示内容
    void SetToolTip(quint8 nIndex,quint8 nSamplePos, QString strItem);
    void SetToolTip(QLabel* lb,quint8 nSamplePos, QString strItem);
private:
    //圆的总数
    quint8 m_nCount;
    //圆的中心坐标
    QPoint m_posCircleCenter;
    //圆的半径
    quint16 m_nRadius;
    //按钮集合
    QVector<QLabel*> m_vecPbItem;
    //圆的大小
    QSize m_CircleSize;
    
signals:
    
public slots:
    //void ClickCircle();
    
};

#endif // CIRCLELAYOUT_H
