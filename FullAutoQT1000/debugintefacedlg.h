﻿#ifndef DEBUGINTEFACEDLG_H
#define DEBUGINTEFACEDLG_H

#include <QDialog>
#include <QDesktopWidget>
#include <QVector>
#include <QMessageBox>
#include <QScrollBar>
#include <QDir>
#include "cpackcustomplot.h"
#include "cqtprodb.h"
#include "cutilsettings.h"
#include "paramdef.h"
#include "testinterface.h"
#include "upgrading.h"


static void myMessageOutput(QtMsgType type, const char* msg)
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
    QString strDirPath = "/home/root/FullAutoQT/Log/";
    QDir dir(strDirPath);
    if(!dir.exists(strDirPath)){
        dir.mkpath(strDirPath);
    }
    QString strDate = QDate::currentDate().toString("yyyy-MM-dd")+QString(".txt");
    QFile file(strDirPath+strDate);
    file.open(QIODevice::WriteOnly | QIODevice::Append);
    QTextStream ts(&file);
    ts<<text<<endl;
}



namespace Ui {
class debugIntefaceDlg;
}

class debugIntefaceDlg : public QDialog
{
    Q_OBJECT

public:
    explicit debugIntefaceDlg(CQtProDB* db,CUtilSettings* settings,QextSerialPort* ControlSerial,QextSerialPort* CheckSerial,TestInterface* ti, QWidget *parent = 0);
    ~debugIntefaceDlg();

private slots:
    void RecvDebugHardwareArmData(QByteArray byData);
    void RecvDebugHardwarePushMotorData(QByteArray byData);
    void RecvDebugHardwareDialData(QByteArray byData);
    void RecvDebugHardwareExitMotorData(QByteArray byData);
    void RecvDebugHardware2ScanOffsetData(QByteArray byData);
    void RecvDebugHardware2BufferSetData(QByteArray byData);

private slots:
    void on_lw_debug_Navigate_currentRowChanged(int currentRow);

    void on_pb_Param_SaveParam_clicked();

    void on_pb_Normal_SaveParam_clicked();

    void on_pb_HardParam_SampleAddXWrite_clicked();

    void on_pb_HardParam_SampleAddYWrite_clicked();

    void on_pb_HardParam_SampleAddXRead_clicked();

    void on_pb_HardParam_SampleAddYRead_clicked();

    void on_pb_HardParam_MixingXWrite_clicked();

    void on_pb_HardParam_MixingYWrite_clicked();

    void on_pb_HardParam_MixingDepthWrite_clicked();

    void on_pb_HardParam_MixingXRead_clicked();

    void on_pb_HardParam_MixingYRead_clicked();

    void on_pb_HardParam_MixingDepthRead_clicked();

    void on_pb_HardParam_ClearoutXWrite_clicked();

    void on_pb_HardParam_ClearoutYWrite_clicked();

    void on_pb_HardParam_ClearoutXRead_clicked();

    void on_pb_HardParam_ClearoutYRead_clicked();

    void on_pb_HardParam_BufferXWrite_clicked();

    void on_pb_HardParam_BufferYWrite_clicked();

    void on_pb_HardParam_BufferDepthWrite_clicked();

    void on_pb_HardParam_BufferOffsetWrite_clicked();

    void on_pb_HardParam_BufferXRead_clicked();

    void on_pb_HardParam_BufferYRead_clicked();

    void on_pb_HardParam_BufferDepthRead_clicked();

    void on_pb_HardParam_BufferOffsetRead_clicked();

    void on_pb_HardParam_SuctionXWrite_clicked();

    void on_pb_HardParam_SuctionYWrite_clicked();

    void on_pb_HardParam_SuctionDepthWrite_clicked();

    void on_pb_HardParam_SuctionOffsetWrite_clicked();

    void on_pb_HardParam_SuctionXRead_clicked();

    void on_pb_HardParam_SuctionYRead_clicked();

    void on_pb_HardParam_SuctionDepthRead_clicked();

    void on_pb_HardParam_SuctionOffsetRead_clicked();

    void on_pb_HardParam_CurrentPosXRead_clicked();

    void on_pb_HardParam_CurrentPosYRead_clicked();

    void on_pb_HardParam_ParamAllSave_clicked();


    void on_pb_HardParam_CurrentPosXReset_clicked();

    void on_pb_HardParam_CurrentPosYReset_clicked();

    void on_pb_HardParam_DialPosReset_clicked();

    void on_pb_HardParam_DialInitStep_clicked();

    void on_pb_HardParam_DialPos_clicked();

    void on_pb_HardParam_DialPosOffset_clicked();

    void on_pb_HardParam_PushMotorOk_clicked();

    void on_pb_HardParam_PushMotorReset_clicked();

    void on_pb_HardParam_PushMotorRead_clicked();

    void on_pb_HardParam_DialInitStepRead_clicked();

    void on_pb_HardParam_DialPosOffsetRead_clicked();

    void on_pb_HardParam_FluidClearCountWrite_clicked();

    void on_pb_HardParam_FluidClearCountRead_clicked();

    void on_pb_HardParam_WaterClearCountWrite_clicked();

    void on_pb_HardParam_WaterClearCountRead_clicked();

    void on_pb_HardParam_ClearTimeWrite_clicked();

    void on_pb_HardParam_ClearTimeRead_clicked();

    void on_pb_HardParam_MixingClearCountWrite_clicked();

    void on_pb_HardParam_MixingClearCountRead_clicked();

    void on_pb_HardParam_MixingWaitTimeWrite_clicked();

    void on_pb_HardParam_MixingWaitTimeRead_clicked();

    void on_pb_HardParam_MixingIntoWaterWrite_clicked();

    void on_pb_HardParam_MixingIntoWaterRead_clicked();

    void on_pb_HardParam_MixingWaterClearCountWrite_clicked();

    void on_pb_HardParam_MixingWaterClearCountRead_clicked();

    void on_pb_HardParam_DiaphPumpOpen_clicked();

    void on_pb_HardParam_DiaphPumpClose_clicked();

    void on_pb_HardParam_SolenoidOpen_clicked();

    void on_pb_HardParam_SolenoidClose_clicked();

    void on_pb_HardParam_PlungerPumpIn_clicked();

    void on_pb_HardParam_PlungerPumpOut_clicked();

    void on_pb_HardParam_PlungerPumpReset_clicked();

    void on_pb_HardParam_MixingBufferDepthWrite_clicked();

    void on_pb_HardParam_MixingBufferDepthRead_clicked();

    void on_pb_Normal_SoftUpdate_clicked();

    void on_rb_Param_CardClose_clicked();

    void on_rb_Param_CardOpen_clicked();

    void on_pb_HardParam2_ScanOffsetWrite_clicked();

    void on_pb_HardParam2_ScanOffsetRead_clicked();

    void on_pb_Normal_DelayStart_clicked();

    void on_pb_Normal_DelayStop_clicked();

    void on_pb_HardParam2_SuckBufferWrite_clicked();

    void on_pb_HardParam2_SuckBufferRead_clicked();

    void on_pb_HardParam2_SpitBufferWrite_clicked();

    void on_pb_HardParam2_SpitBufferRead_clicked();

    void on_pb_HardParam_ParamAllSave2_clicked();

    void on_pb_HardParam_ExitCardWrite_clicked();

    void on_pb_HardParam_ExitCardRead_clicked();

    void on_pb_HardParam_ExitCardReset_clicked();

private:
    Ui::debugIntefaceDlg *ui;
    //数据库对象
    CQtProDB* m_devDb;
    //配置文件对象
    CUtilSettings* m_pSettinsObj;
    //配置文件参数表
    QMap<QString,QString> m_SetParam;
    //控件板串口对象
    QextSerialPort* m_pControlPanelsSerial;
    //检测板串口对象
    QextSerialPort* m_pCheckPanelsSerial;
    //测试界面对象
    TestInterface* m_pTestInterface;
    //硬件调试命令界面开启
    bool m_bHardwareDebug;
private:
    //硬件调试界面参数加样臂命令
    void HardwareDebugLoadingArmCommand(qint32 OptionType,qint32 PosType,qint32 MoveSize,qint32 DataType);
    //硬件调试界面参数转盘命令
    void HardwareDebugDialCommand(qint32 OptionType, qint32 ParamType, qint32 DialPos);
    //硬件调试界面参数推卡电机命令
    void HardwareDebugPushMotorCommand(qint32 OptionType, qint32 ParamType, qint32 DialPos);
    //硬件调试界面参数退卡电机命令
    void HardwareDebugExitMotorCommand(qint32 OptionType, qint32 ParamSize);
    //硬件调试界面参数隔膜泵命令
    void HardwareDebugDiaphPumCommand(qint32 OptionType,qint32 PumpNumber);
    //硬件调试界面参数电磁阀命令
    void HardwareDebugValvesCommand(qint32 OptionType,qint32 ValvesNumber);
    //硬件调试界面参数柱塞泵命令
    void HardwareDebugPumpCommand(qint32 OptionType,qint32 PumpNumber, qint32 PumpSize);
    //硬件调试2界面偏移量大小命令
    void HardwareDebug2ScanOffsetCommand(qint32 ParamType,qint32 OffsetSize);
    //硬件调试2界面缓冲液大小命令
    void HardwareDebug2BufferSetCommand(qint32 OptionType, qint32 ParamType, qint32 ParamSize);

private:
    //初始化导航栏
    void InitNavigate();
    //初始化对话框样式
    void InitDialogStyle();
    //初始化硬件调试窗口输入法模式
    void InitHardwareInputMode();
    //初始化调试界面控件
    void InitDebugControl();
    //初始化调试界面设置参数显示
    void InitParamSetShow();
    //初始化调试界面加样臂参数显示
    void InitDebugArmParamShow();
};

#endif // DEBUGINTEFACEDLG_H
