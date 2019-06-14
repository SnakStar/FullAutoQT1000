#ifndef DEBUGINTEFACEDLG_H
#define DEBUGINTEFACEDLG_H

#include <QDialog>
#include <QDesktopWidget>
#include <QVector>
#include <QMessageBox>
#include "cpackcustomplot.h"
#include "cqtprodb.h"
#include "cutilsettings.h"
#include "qextserial/qextserialport.h"
#include "paramdef.h"
#include "testinterface.h"

namespace Ui {
class debugIntefaceDlg;
}

class debugIntefaceDlg : public QDialog
{
    Q_OBJECT

public:
    explicit debugIntefaceDlg(CQtProDB* db,CUtilSettings* settings,QextSerialPort* ControlSerial,TestInterface* ti, QWidget *parent = 0);
    ~debugIntefaceDlg();

private slots:
    void RecvDebugHardwareData(QByteArray byData);

private slots:
    void on_lw_debug_Navigate_currentRowChanged(int currentRow);

    void on_pb_Curve_QueryData_clicked();

    void on_tw_Curve_ResultInfo_cellPressed(int row, int column);

    void on_pb_Curve_ClearCurve_clicked();

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

    void on_pb_HardParam_ArmParamAllSave_clicked();


    void on_pb_HardParam_CurrentPosXReset_clicked();

    void on_pb_HardParam_CurrentPosYReset_clicked();

private:
    Ui::debugIntefaceDlg *ui;
    //custom对象
    CPackCustomPlot* m_packPlot;
    //数据库对象
    CQtProDB* m_devDb;
    //配置文件对象
    CUtilSettings* m_pSettinsObj;
    //配置文件参数表
    QMap<QString,QString> m_SetParam;
    //控件板串口对象
    QextSerialPort* m_pControlPanelsSerial;
    //测试界面对象
    TestInterface* m_pTestInterface;
private:
    //
    void HardwareDebugLoadingArmCommand(qint32 OptionType,qint32 PosType,qint32 MoveSize,qint32 DataType);
    //转换640个高低位数据成320个图形数据
    QVector<double> ConvertGraphData(QByteArray byteGraphData);
    //滤波数据
    void low_passfilter(QVector<double> &oldData);
    //插入一条数据至测试信息表格
    void InsertTestInfoToTab(QString strNumber, QString strArea1, QString strArea2, QString strRatio,
                             QString strResult, QString strCalcMethod, QString strScanStart,
                             QString strQCBreadth,QString strCheckBreadth, QString strAmpParam,
                             QString strCrestPos1,QString strCrestPosLeft1, QString strCrestPosRight1,
                             QString strCrestPos2,QString strCrestPosLeft2, QString strCrestPosRight2,
                             QString strTestDate, QString strRawData);
private:
    //初始化导航栏
    void InitNavigate();
    //初始化对话框样式
    void InitDialogStyle();
    //初始化硬件调试窗口输入法模式
    void InitHardwareInputMode();
    //初始化调试界面控件
    void InitDebugControl();
    //初始化结果列表控件
    void InitResultTableWidget();
    //
    void InitParamSetShow();
};

#endif // DEBUGINTEFACEDLG_H
