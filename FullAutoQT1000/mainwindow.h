/*******************************************************************
 *  Copyright(c) 2000-2020 Company Name
 *  All rights reserved.
 *
 *  文件名称: mainwindow.h
 *  简要描述: 主框架头文件，进行对象声明
 *
 *  创建日期: 2018-4-20
 *  作者: HuaT
 *  说明:
 *
 *  修改日期:
 *  作者:
 *  说明:
 ******************************************************************/
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMap>
#include <QTimer>
#include <QDateTime>
#include <QProcess>
#include <QFile>
#include <QTextStream>
#include"testinterface.h"
#include"queryinterface.h"
#include"qcinterface.h"
#include"setinterface.h"
#include"cqtprodb.h"
#include"cutilsettings.h"
#include"paramdef.h"
#include "logindlg.h"


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    //初始化检测板串口
    bool InitCheckPanelsSerial();
    //初始化控制板串口
    bool InitControlPanelsSerial();
    //获取数据库对象
    CQtProDB* GetDatabaseObj();
    //获取配置文件对象
    CUtilSettings* GetSettingsObj();
    //设置自检信息显示或者隐藏
    void SetCheckStatusHide(bool isHide);
    //获取打印串口对象
    QextSerialPort* GetPrintSerialPort();
    //获取hl7对象
    QextSerialPort *GetHL7SerialPort();
    //获取检测板串口对象
    QextSerialPort *GetCheckPanelsSerial();
    //获取控制板串口对象
    QextSerialPort *GetControlPanelsSerial();
    //获取输入法对象
    //MyInputPanelContext* GetInputPanelContext();
signals:
    //发送关闭设备信息
    void SendShutdownDev();
private slots:
    void UpdateSystemTime();
    //接收打印机串口数据
    void RecvPrintSerialData();

private slots:
    void on_pb_Main_Test_clicked();

    void on_pb_Main_Query_clicked();

    void on_pb_Main_QC_clicked();

    void on_pb_Main_Setting_clicked();

    void on_pb_Main_PowerOff_clicked();

private:
    //初始化主界面对象
    void InitMainInterface();
    //初始化配置文件参数
    void InitSettinsParam();
    //初始化打印串口
    bool InitPrintSerial();
    //初始化hl7串口
    bool InitHl7Serial();

public:
    //获取界面对象
    TestInterface* GetTestInterface();
    //获取查询对象
    QueryInterface* GetQueryInterface();

private:
    TestInterface*  m_TestInterface;
    QueryInterface* m_QueryInterface;
    QCInterface*    m_QCInterface;
    SetInterface*   m_SetInterface;

    //数据库对象
    CQtProDB m_devdb;
    //配置文件对象
    CUtilSettings* m_settings;
    //配置文件参数表键值
    QMap<QString,QString> m_mapSetParam;
    //系统时间
    QTimer m_timerSysTime;
    //打印串口对象
    QextSerialPort m_SerialPrint;
    //hl7串口对象
    QextSerialPort m_SerialHL7;
    //检测板串口对象
    QextSerialPort m_CheckPanelsSerial;
    //控制板串口对象
    QextSerialPort m_ControlPanelsSerial;
    //输入法对象
    //MyInputPanelContext* m_myInputPanel;
    //
    LoginDlg* m_LoginDlg;
    
private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
