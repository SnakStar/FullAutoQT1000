﻿#include "debugintefacedlg.h"
#include "ui_debugintefacedlg.h"

debugIntefaceDlg::debugIntefaceDlg(CQtProDB* db, CUtilSettings *settings,QextSerialPort* ControlSerial,TestInterface* ti, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::debugIntefaceDlg)
{
    ui->setupUi(this);
    m_devDb = db;
    m_pSettinsObj = settings;
    m_pControlPanelsSerial = ControlSerial;
    m_pTestInterface = ti;
    m_bHardwareDebug = false;
    //初始化对话框样式
    InitDialogStyle();
    //初始化导航栏
    InitNavigate();
    //初始化图形对象
    m_packPlot = new CPackCustomPlot(ui->wg_Curve_CustomPlot);
    m_packPlot->InitPlotParam(700);
    //初始化曲线界面控件
    InitDebugControl();
    //初始化参数设置界面内容值
    InitParamSetShow();
    //初始化硬件界面输入法模式
    InitHardwareInputMode();
    //屏蔽时间控件输入法弹出
    ui->de_Curve_StartTime->setProperty("noinput",true);
    ui->de_Curve_EndTime->setProperty("noinput",true);
    //测试界面接收到的硬件调试参数
    connect(m_pTestInterface,SIGNAL(sendDebugHardwareArmData(QByteArray)),this,SLOT(RecvDebugHardwareArmData(QByteArray)) );
    connect(m_pTestInterface,SIGNAL(sendDebugHardwarePushMotorData(QByteArray)),this,SLOT(RecvDebugHardwarePushMotorData(QByteArray)) );
    connect(m_pTestInterface,SIGNAL(sendDebugHardwareDialData(QByteArray)),this,SLOT(RecvDebugHardwareDialData(QByteArray)) );
}

debugIntefaceDlg::~debugIntefaceDlg()
{
    delete ui;
}


/********************************************************
 *@Name:        InitNavigate
 *@Author:      HuaT
 *@Description: 初始化调试界面导航栏
 *@Param1:      无
 *@Return:      无
 *@Version:     1.0
 *@Date:        2019-3-4
********************************************************/
void debugIntefaceDlg::InitNavigate()
{
    QStringList listNavigate;
    listNavigate<<"参数设置"<<"常规调试"<<"硬件参数"<<"硬件参数2"<<"数据库操作"<<"曲线显示"<<"退出";
    for(int n=0; n<listNavigate.size(); n++){
        QListWidgetItem* item = new QListWidgetItem(listNavigate.at(n));
        item->setSizeHint(QSize(187,80));
        ui->lw_debug_Navigate->addItem(item);
    }
}

/********************************************************
 *@Name:        InitDialogStyle
 *@Author:      HuaT
 *@Description: 初始化对话框样式
 *@Param1:      无
 *@Return:      无
 *@Version:     1.0
 *@Date:        2019-3-4
********************************************************/
void debugIntefaceDlg::InitDialogStyle()
{
    this->setWindowFlags(Qt::CustomizeWindowHint|Qt::FramelessWindowHint);
    QDesktopWidget* dw = qApp->desktop();
    this->move( (dw->width() - this->width())/2 , (dw->height() - this->height())/2);
}

/********************************************************
 *@Name:        InitHardwareInputMode
 *@Author:      HuaT
 *@Description: 初始化硬件调试界面输入法模式
 *@Param1:      无
 *@Return:      无
 *@Version:     1.0
 *@Date:        2019-3-4
********************************************************/
void debugIntefaceDlg::InitHardwareInputMode()
{
    ui->le_HardParam_BufferDepth->setProperty("flag","number");
    ui->le_HardParam_BufferOffset->setProperty("flag","number");
    ui->le_HardParam_BufferX->setProperty("flag","number");
    ui->le_HardParam_BufferY->setProperty("flag","number");
    //
    ui->le_HardParam_ClearoutX->setProperty("flag","number");
    ui->le_HardParam_ClearoutY->setProperty("flag","number");
    ui->le_HardParam_WaterClearCount->setProperty("flag","number");
    ui->le_HardParam_FluidClearCount->setProperty("flag","number");
    ui->le_HardParam_IntoWaterTime->setProperty("flag","number");
    //
    ui->le_HardParam_DialPos->setProperty("flag","number");
    ui->le_HardParam_DialOffset->setProperty("flag","number");
    ui->le_HardParam_DialInitStep->setProperty("flag","number");
    //
    ui->le_HardParam_MixingDepth->setProperty("flag","number");
    ui->le_HardParam_MixingX->setProperty("flag","number");
    ui->le_HardParam_MixingY->setProperty("flag","number");
    ui->le_HardParam_MixingFluidClearCount->setProperty("flag","number");
    ui->le_HardParam_MixingIntoWater->setProperty("flag","number");
    ui->le_HardParam_MixingWaitTime->setProperty("flag","number");
    ui->le_HardParam_MixingWaterClearCount->setProperty("flag","number");
    ui->le_HardParam_MixingBufferDepth->setProperty("flag","number");
    //
    ui->le_HardParam_PlungerPumpSize->setProperty("flag","number");
    ui->le_HardParam_PlungerPumpNumber->setProperty("flag","number");
    ui->le_HardParam_PumpNumber->setProperty("flag","number");
    ui->le_HardParam_PushMaxLen->setProperty("flag","number");
    ui->le_HardParam_PushNumber->setProperty("flag","number");
    ui->le_HardParam_SampleAddX->setProperty("flag","number");
    ui->le_HardParam_SampleAddY->setProperty("flag","number");
    ui->le_HardParam_SuctionDepth->setProperty("flag","number");
    ui->le_HardParam_SuctionOffset->setProperty("flag","number");
    ui->le_HardParam_SuctionX->setProperty("flag","number");
    ui->le_HardParam_SuctionY->setProperty("flag","number");
    ui->le_HardParam_CancelCardMaxLen->setProperty("flag","number");
    ui->le_HardParam_SolenoidNumber->setProperty("flag","number");
    //
    ui->le_HardParam_CurrentPosX->setProperty("flag","number");
    ui->le_HardParam_CurrentPosY->setProperty("flag","number");
}

/********************************************************
 *@Name:        InitDebugControl
 *@Author:      HuaT
 *@Description: 初始化调试界面控件状态
 *@Param1:      无
 *@Return:      无
 *@Version:     1.0
 *@Date:        2019-3-4
********************************************************/
void debugIntefaceDlg::InitDebugControl()
{
    //初始化单选控件
    ui->rb_Curve_Overlay->setChecked(true);
    ui->rb_Curve_RawData->setChecked(true);
    //初始化表格控件
    InitResultTableWidget();
    //初始化日期控件
    QDate dt = QDate::currentDate();
    ui->de_Curve_StartTime->setDate(dt);
    ui->de_Curve_EndTime->setDate(dt);
    //编辑框数字键
    ui->le_Param_Amp->setProperty("flag","number");
    ui->le_Param_CalcMethod->setProperty("flag","number");
    ui->le_Param_Diluent->setProperty("flag","number");
    ui->le_Param_SampleSize->setProperty("flag","number");
    ui->le_Param_ScanStart->setProperty("flag","number");
    ui->le_Param_TestTime->setProperty("flag","number");
    //参数设置数字键
    ui->le_Normal_DiluentAddConst->setProperty("flag","number");
    ui->le_Normal_DiluentAddCoeffOffset->setProperty("flag","number");
    ui->le_Normal_SampleAddConst->setProperty("flag","number");
    ui->le_Normal_SampleAddCoeffOffset->setProperty("flag","number");


}

/*
 *
 * 初始化结果列表控件
 *
 */
void debugIntefaceDlg::InitResultTableWidget()
{
    QStringList listHeader;
    listHeader<<QObject::tr("编号")<<QObject::tr("面积1")<<QObject::tr("面积2")<<QObject::tr("比值")
              <<QObject::tr("结果")<<QObject::tr("计算方案")<<QObject::tr("扫描起始点")
              <<QObject::tr("质控峰长度")<<QObject::tr("放大倍数")<<QObject::tr("检测峰长度")
              <<QObject::tr("波峰位置1")<<QObject::tr("波峰位置1左")<<QObject::tr("波峰位置1右")
              <<QObject::tr("波峰位置2")<<QObject::tr("波峰位置2左")<<QObject::tr("波峰位置2右")
              <<QObject::tr("测试时间")<<QObject::tr("原始数据");
    ui->tw_Curve_ResultInfo->setColumnCount(listHeader.size());
    ui->tw_Curve_ResultInfo->setHorizontalHeaderLabels(listHeader);
    ui->tw_Curve_ResultInfo->setEditTriggers(QTableWidget::NoEditTriggers);
    ui->tw_Curve_ResultInfo->setSelectionBehavior(QTableWidget::SelectRows);
    //ui->tw_Curve_ResultInfo->setRowCount(7);
}

/*
 *
 * 读取参数界面内容到界面显示
 *
 */
void debugIntefaceDlg::InitParamSetShow()
{
    if(m_pSettinsObj->GetParam(DEBUGMODE) == "0"){
        ui->rb_Param_Close->setChecked(true);
    }else{
        ui->rb_Param_Open->setChecked(true);
    }
    if(m_pSettinsObj->GetParam(CONNPCMODE) == "0"){
        ui->rb_Param_PCConnClose->setChecked(true);
    }else{
        ui->rb_Param_PCConnOpen->setChecked(true);
    }
    if(m_pSettinsObj->GetParam(TESTCARDMODE) == "0" ){
        ui->rb_Param_CardClose->setChecked(true);
    }else{
        ui->rb_Param_CardOpen->setChecked(true);
    }
    //调试参数
    QString strAmp = m_pSettinsObj->GetParam(DEBUGAMPPARAM);
    ui->le_Param_Amp->setText(strAmp);
    ui->le_Param_CalcMethod->setText(m_pSettinsObj->GetParam(DEBUGCALCMETHOD));
    ui->le_Param_Diluent->setText(m_pSettinsObj->GetParam(DILUENT));
    ui->le_Param_SampleSize->setText(m_pSettinsObj->GetParam(SAMPLESIZE));
    ui->le_Param_ScanStart->setText(m_pSettinsObj->GetParam(DEBUGSCANSTART));
    ui->le_Param_TestTime->setText(m_pSettinsObj->GetParam(DEBUGTESTTIME));
    //仪器参数
    ui->le_Normal_SampleAddCoeffOffset->setText(m_pSettinsObj->GetParam(SAMPLEOFFSETCOEFF));
    ui->le_Normal_SampleAddConst->setText(m_pSettinsObj->GetParam(SAMPLEOFFSETCONST));
    ui->le_Normal_DiluentAddCoeffOffset->setText(m_pSettinsObj->GetParam(DILUENTOFFSETCOEFF));
    ui->le_Normal_DiluentAddConst->setText(m_pSettinsObj->GetParam(DILUENTOFFSETCONST));
    ui->le_Normal_ResultRatioCoeffOffset->setText(m_pSettinsObj->GetParam(RESULTRATIOCOEFF));
    ui->le_Normal_ResultRatioConst->setText(m_pSettinsObj->GetParam(RESULTRATIOCONSTA));

}


/////////////////////////////////////信号接收区//////////////////////////////////////////
/*
 *
 * 处理显示加样臂数据
 *
 */
void debugIntefaceDlg::RecvDebugHardwareArmData(QByteArray byData)
{
    if(byData.size() == 0){
        return;
    }
    quint32 nOptionType = m_pTestInterface->BytesToInt(byData.mid(0,4));
    quint32 nPosType = m_pTestInterface->BytesToInt(byData.mid(4,4));;
    quint32 nMoveSize = m_pTestInterface->BytesToInt(byData.mid(8,4));;
    switch (nOptionType) {
    //加样位
    case 0:
        if(nPosType == 0){
            ui->le_HardParam_SampleAddX->setText(QString::number(nMoveSize));
        }else if(nPosType == 1){
            ui->le_HardParam_SampleAddY->setText(QString::number(nMoveSize));
        }
        break;
    //清洗位
    case 1:
        if(nPosType == 0){
            ui->le_HardParam_ClearoutX->setText(QString::number(nMoveSize));
        }else if(nPosType == 1){
            ui->le_HardParam_ClearoutY->setText(QString::number(nMoveSize));
        }else if(nPosType == 6){
            ui->le_HardParam_FluidClearCount->setText(QString::number(nMoveSize));
        }else if(nPosType == 7){
            ui->le_HardParam_WaterClearCount->setText(QString::number(nMoveSize));
        }else if(nPosType == 5){
            ui->le_HardParam_IntoWaterTime->setText(QString::number(nMoveSize));
        }
        break;
    //混匀位
    case 2:
        if(nPosType == 0){
            ui->le_HardParam_MixingX->setText(QString::number(nMoveSize));
        }else if(nPosType == 1){
            ui->le_HardParam_MixingY->setText(QString::number(nMoveSize));
        }else if(nPosType == 2){
            ui->le_HardParam_MixingDepth->setText(QString::number(nMoveSize));
        }else if(nPosType == 4){
            ui->le_HardParam_MixingWaitTime->setText(QString::number(nMoveSize));
        }else if(nPosType == 5){
            ui->le_HardParam_MixingIntoWater->setText(QString::number(nMoveSize));
        }else if(nPosType == 6){
            ui->le_HardParam_MixingFluidClearCount->setText(QString::number(nMoveSize));
        }else if(nPosType == 7){
            ui->le_HardParam_MixingWaterClearCount->setText(QString::number(nMoveSize));
        }else if(nPosType == 8){
            ui->le_HardParam_MixingBufferDepth->setText(QString::number(nMoveSize));
        }
        break;
    //缓冲液位
    case 3:
        if(nPosType == 0){
            ui->le_HardParam_BufferX->setText(QString::number(nMoveSize));
        }else if(nPosType == 1){
            ui->le_HardParam_BufferY->setText(QString::number(nMoveSize));
        }else if(nPosType == 2){
            ui->le_HardParam_BufferDepth->setText(QString::number(nMoveSize));
        }else if(nPosType == 3){
            ui->le_HardParam_BufferOffset->setText(QString::number(nMoveSize));
        }
        break;
    //吸样位
    case 4:
        if(nPosType == 0){
            ui->le_HardParam_SuctionX->setText(QString::number(nMoveSize));
        }else if(nPosType == 1){
            ui->le_HardParam_SuctionY->setText(QString::number(nMoveSize));
        }else if(nPosType == 2){
            ui->le_HardParam_SuctionDepth->setText(QString::number(nMoveSize));
        }else if(nPosType == 3){
            ui->le_HardParam_SuctionOffset->setText(QString::number(nMoveSize));
        }
        break;
    //当前位置
    case 5:
        if(nPosType == 0){
            ui->le_HardParam_CurrentPosX->setText(QString::number(nMoveSize));
        }else if(nPosType == 1){
            ui->le_HardParam_CurrentPosY->setText(QString::number(nMoveSize));
        }
        break;
    default:
        qDebug()<<"debug hardware Data - nOptionType error";
        break;
    }
}

/*
 *
 * 处理显示推卡电机数据
 *
 */
void debugIntefaceDlg::RecvDebugHardwarePushMotorData(QByteArray byData)
{
    if(byData.size() == 0){
        return;
    }
    //quint32 nMotorID = m_pTestInterface->BytesToInt(byData.mid(0,4));
    quint32 nSetupSize = m_pTestInterface->BytesToInt(byData.mid(4,4));;
    ui->le_HardParam_PushMaxLen->setText(QString::number(nSetupSize));
}

/*
 *
 * 处理显示转盘数据
 *
 */
void debugIntefaceDlg::RecvDebugHardwareDialData(QByteArray byData)
{
    if(byData.size() == 0){
        return;
    }
    //操作类型，0：零点位置，1：转盘位置，2：偏移量
    quint32 nParamType = m_pTestInterface->BytesToInt(byData.mid(0,4));
    quint32 nParamSize = m_pTestInterface->BytesToInt(byData.mid(4,4));
    switch (nParamType) {
    case 0:
        ui->le_HardParam_DialInitStep->setText(QString::number(nParamSize));
        break;
    case 2:
        ui->le_HardParam_DialOffset->setText(QString::number(nParamSize));
        break;
    default:
        break;
    }
}


/*
 *
 * 导航项目更改事件
 *
 */
void debugIntefaceDlg::on_lw_debug_Navigate_currentRowChanged(int currentRow)
{
    if(currentRow == 6){
        //调试硬件参数结束
        QByteArray byarrData;
        m_pTestInterface->SendSerialData(m_pControlPanelsSerial,PL_FUN_AM_ID_DEBUG_HARDWARE_END,byarrData);
        this->close();
    }else{
        ui->stackedWidget->setCurrentIndex(currentRow);
        if(currentRow == 2){
            //调试硬件参数开始
            if(m_bHardwareDebug != true){
                m_bHardwareDebug = true;
                QByteArray byarrData;
                m_pTestInterface->SendSerialData(m_pControlPanelsSerial,PL_FUN_AM_ID_DEBUG_HARDWARE_START,byarrData);
            }
        }
    }
}

/*
 *
 *调试界面查询数据事件
 *
 */
void debugIntefaceDlg::on_pb_Curve_QueryData_clicked()
{
    ui->tw_Curve_ResultInfo->setRowCount(0);
    QString strStartDate = QString("%1 00:00:00").arg(ui->de_Curve_StartTime->date().toString("yyyy-MM-dd"));
    QString strEndDate = QString("%2 23:59:59").arg(ui->de_Curve_EndTime->date().toString("yyyy-MM-dd"));
    QString strSql;
    strSql = QString("select * from testinfo where testdate between '%1' and '%2' order by testdate desc")
             .arg(strStartDate).arg(strEndDate);
    QStringList strlistRe;
    strlistRe = m_devDb->ExecQuery(strSql);
    //qDebug()<<strlistRe;
    for(int n=0; n<strlistRe.size(); n+=19){
        InsertTestInfoToTab(strlistRe.at(n+1),strlistRe.at(n+2),strlistRe.at(n+3),strlistRe.at(n+4),
                            strlistRe.at(n+5),strlistRe.at(n+6),strlistRe.at(n+7),strlistRe.at(n+8),
                            strlistRe.at(n+9),strlistRe.at(n+10),strlistRe.at(n+11),strlistRe.at(n+12),
                            strlistRe.at(n+13),strlistRe.at(n+14),strlistRe.at(n+15),strlistRe.at(n+16),
                            strlistRe.at(n+17),strlistRe.at(n+18) );
    }
}

/*
 *
 *插入一条数据至测试信息表格
 *
 */
void debugIntefaceDlg::InsertTestInfoToTab(QString strNumber, QString strArea1, QString strArea2, QString strRatio,
                                           QString strResult, QString strCalcMethod,
                                           QString strScanStart,QString strQCBreadth,
                                           QString strCheckBreadth, QString strAmpParam,
                                           QString strCrestPos1, QString strCrestPosLeft1,
                                           QString strCrestPosRight1,QString strCrestPos2,
                                           QString strCrestPosLeft2, QString strCrestPosRight2,
                                           QString strTestDate,QString strRawData)
{
    int nRowCount = ui->tw_Curve_ResultInfo->rowCount();
    ui->tw_Curve_ResultInfo->insertRow(nRowCount);
    QTableWidgetItem* itemNumber = new QTableWidgetItem;
    itemNumber->setText(strNumber);
    ui->tw_Curve_ResultInfo->setItem(nRowCount,0,itemNumber);

    QTableWidgetItem* itemArea1 = new QTableWidgetItem;
    itemArea1->setText(strArea1);
    ui->tw_Curve_ResultInfo->setItem(nRowCount,1,itemArea1);

    QTableWidgetItem* itemArea2 = new QTableWidgetItem;
    itemArea2->setText(strArea2);
    ui->tw_Curve_ResultInfo->setItem(nRowCount,2,itemArea2);

    QTableWidgetItem* itemRatio = new QTableWidgetItem;
    itemRatio->setText(strRatio);
    ui->tw_Curve_ResultInfo->setItem(nRowCount,3,itemRatio);

    QTableWidgetItem* itemResult = new QTableWidgetItem;
    itemResult->setText(strResult);
    ui->tw_Curve_ResultInfo->setItem(nRowCount,4,itemResult);

    QTableWidgetItem* itemCalcMethod = new QTableWidgetItem;
    itemCalcMethod->setText(strCalcMethod);
    ui->tw_Curve_ResultInfo->setItem(nRowCount,5,itemCalcMethod);

    QTableWidgetItem* itemScanStart = new QTableWidgetItem;
    itemScanStart->setText(strScanStart);
    ui->tw_Curve_ResultInfo->setItem(nRowCount,6,itemScanStart);

    QTableWidgetItem* itemQCBreadth = new QTableWidgetItem;
    itemQCBreadth->setText(strQCBreadth);
    ui->tw_Curve_ResultInfo->setItem(nRowCount,7,itemQCBreadth);

    QTableWidgetItem* itemCheckBreadth = new QTableWidgetItem;
    itemCheckBreadth->setText(strCheckBreadth);
    ui->tw_Curve_ResultInfo->setItem(nRowCount,8,itemCheckBreadth);

    QTableWidgetItem* itemAmpParam = new QTableWidgetItem;
    itemAmpParam->setText(strAmpParam);
    ui->tw_Curve_ResultInfo->setItem(nRowCount,9,itemAmpParam);

    QTableWidgetItem* itemCrestPos1 = new QTableWidgetItem;
    itemCrestPos1->setText(strCrestPos1);
    ui->tw_Curve_ResultInfo->setItem(nRowCount,10,itemCrestPos1);

    QTableWidgetItem* itemCrestPosLeft1 = new QTableWidgetItem;
    itemCrestPosLeft1->setText(strCrestPosLeft1);
    ui->tw_Curve_ResultInfo->setItem(nRowCount,11,itemCrestPosLeft1);

    QTableWidgetItem* itemCrestPosRight1 = new QTableWidgetItem;
    itemCrestPosRight1->setText(strCrestPosRight1);
    ui->tw_Curve_ResultInfo->setItem(nRowCount,12,itemCrestPosRight1);

    QTableWidgetItem* itemCrestPos2 = new QTableWidgetItem;
    itemCrestPos2->setText(strCrestPos2);
    ui->tw_Curve_ResultInfo->setItem(nRowCount,13,itemCrestPos2);

    QTableWidgetItem* itemCrestPosLeft2 = new QTableWidgetItem;
    itemCrestPosLeft2->setText(strCrestPosLeft2);
    ui->tw_Curve_ResultInfo->setItem(nRowCount,14,itemCrestPosLeft2);

    QTableWidgetItem* itemCrestPosRight2 = new QTableWidgetItem;
    itemCrestPosRight2->setText(strCrestPosRight2);
    ui->tw_Curve_ResultInfo->setItem(nRowCount,15,itemCrestPosRight2);

    QTableWidgetItem* itemTestDate = new QTableWidgetItem;
    itemTestDate->setText(strTestDate);
    ui->tw_Curve_ResultInfo->setItem(nRowCount,16,itemTestDate);

    QTableWidgetItem* itemRawData = new QTableWidgetItem;
    itemRawData->setText(strRawData);
    ui->tw_Curve_ResultInfo->setItem(nRowCount,17,itemRawData);
}

/*
 *
 *结果信息表选择行响应事件
 *
 */
void debugIntefaceDlg::on_tw_Curve_ResultInfo_cellPressed(int row, int column)
{
    if(row < 0){
        return;
    }
    QString strRawData = ui->tw_Curve_ResultInfo->item(row,17)->text();
    QByteArray byteRawData;
    for(int n=0; n<strRawData.size(); n+=2){
        bool re;
        byteRawData.append( strRawData.mid(n,2).toUShort(&re,16) );
    }
    //qDebug()<<byteRawData.size();
    bool bov = ui->rb_Curve_NoOverlay->isChecked();
    if(bov == true){
        m_packPlot->SetOverlay(false);
    }else{
        m_packPlot->SetOverlay(true);
    }
    QVector<double> vecData = ConvertGraphData(byteRawData);
    if(ui->rb_Curve_RawData->isChecked() == true){
        m_packPlot->DrawGraph(vecData);
    }else{
        low_passfilter(vecData);
        m_packPlot->DrawGraph(vecData);
    }

}

/*
 *
 *320个原始数据过滤
 *
 */
void debugIntefaceDlg::low_passfilter(QVector<double>& oldData)
{
    int i,k;
    double temp[320];
    double tempdata;
    //unsigned int nShow;
    for(k=0;k<4;k++)
    {
        for(i=0;i<320;i++){
            temp[i] = oldData.at(i);
        }
        for(i=6;i<(320-6);i++)
        {
            tempdata =temp[i-6]+temp[i-5]+temp[i-4]+temp[i-3]+temp[i-2]+temp[i-1]+temp[i]+temp[i+1]+temp[i+2]+temp[i+3]+temp[i+4]+temp[i+5]+temp[i+6];
            oldData[i] =  ceil(tempdata/13) ;
            //nShow = scanData[i];
            //qDebug()<<i<<":"<<nShow<<"  "<<"tempdata:"<<tempdata;
        }
    }
}

/*
 *
 *清除图形控件内容响应事件
 *
 */
void debugIntefaceDlg::on_pb_Curve_ClearCurve_clicked()
{
    m_packPlot->ClearGraph();
}

/*
 *
 *转换640个高低数据为320个点数据
 *
 */
QVector<double> debugIntefaceDlg::ConvertGraphData(QByteArray byteGraphData)
{
    QVector<double> vecData;
    for(int n=0; n<byteGraphData.size(); n+=2){
        vecData.append(byteGraphData.at(n) + byteGraphData.at(n+1)*256);
    }
    return vecData;
}

/*
 *
 *保存设定参数响应事件
 *
 */
void debugIntefaceDlg::on_pb_Param_SaveParam_clicked()
{
    if(ui->rb_Param_Close->isChecked() == true){
        m_pSettinsObj->SetParam(DEBUGMODE,"0");
    }else{
        m_pSettinsObj->SetParam(DEBUGMODE,"1");
    }
    if(ui->rb_Param_PCConnClose->isChecked() == true){
        m_pSettinsObj->SetParam(CONNPCMODE,"0");
    }else{
        m_pSettinsObj->SetParam(CONNPCMODE,"1");
    }
    if(ui->rb_Param_CardClose->isChecked() == true){
        m_pSettinsObj->SetParam(TESTCARDMODE,"0");
    }else{
        m_pSettinsObj->SetParam(TESTCARDMODE,"1");
    }

    m_pSettinsObj->SetParam(DILUENT,ui->le_Param_Diluent->text());
    m_pSettinsObj->SetParam(SAMPLESIZE,ui->le_Param_SampleSize->text());
    m_pSettinsObj->SetParam(DEBUGSCANSTART,ui->le_Param_ScanStart->text());
    m_pSettinsObj->SetParam(DEBUGCALCMETHOD,ui->le_Param_CalcMethod->text());
    m_pSettinsObj->SetParam(DEBUGAMPPARAM,ui->le_Param_Amp->text());
    m_pSettinsObj->SetParam(DEBUGTESTTIME,ui->le_Param_TestTime->text());

    m_pSettinsObj->SaveAllSettingsInfoToFile();
    QMessageBox::information(this,"提示","保存成功",QMessageBox::Ok);

    emit m_pSettinsObj->emitSettingChange();

}

void debugIntefaceDlg::on_pb_Normal_SaveParam_clicked()
{
    m_pSettinsObj->SetParam(DILUENTOFFSETCOEFF,ui->le_Normal_DiluentAddCoeffOffset->text());
    m_pSettinsObj->SetParam(DILUENTOFFSETCONST,ui->le_Normal_DiluentAddConst->text());
    m_pSettinsObj->SetParam(SAMPLEOFFSETCOEFF, ui->le_Normal_SampleAddCoeffOffset->text());
    m_pSettinsObj->SetParam(SAMPLEOFFSETCONST,ui->le_Normal_SampleAddConst->text());
    m_pSettinsObj->SetParam(RESULTRATIOCOEFF, ui->le_Normal_ResultRatioCoeffOffset->text());
    m_pSettinsObj->SetParam(RESULTRATIOCONSTA, ui->le_Normal_ResultRatioConst->text());

    m_pSettinsObj->SaveAllSettingsInfoToFile();
    QMessageBox::information(this,"提示","保存成功",QMessageBox::Ok);
}


/********************************************************
 *@Name:        HardwareDebugLoadingArmCommand
 *@Author:      HuaT
 *@Description: 硬件调试加样臂命令
 *@Param1:      操作类型,0:加样位，1：清洗位，2：混匀位，3：缓冲液位，4：吸样位
 *@Param2:      位置类型,0:横向，1，纵向，2：液位探测，3偏移量，4停留时间，5注水时间，6清洗液清洗次数，7清水清洗次数，8加缓冲液深度
 *@Param3:      移动距离
 *@Param4:      数据类型，0：写入，1：请求
 *@Return:      无
 *@Version:     1.0
 *@Date:        2019-3-4
********************************************************/
void debugIntefaceDlg::HardwareDebugLoadingArmCommand(qint32 OptionType, qint32 PosType, qint32 MoveSize, qint32 DataType)
{
    QByteArray byarrData;
    byarrData.append(m_pTestInterface->IntToBytes(OptionType));
    byarrData.append(m_pTestInterface->IntToBytes(PosType));
    byarrData.append(m_pTestInterface->IntToBytes(MoveSize));
    byarrData.append(m_pTestInterface->IntToBytes(DataType));
    m_pTestInterface->SendSerialData(m_pControlPanelsSerial,PL_FUN_AM_ID_DEBUG_LOADINGARMSET,byarrData);
}

/*
 *
 *硬件调试界面-加样臂-加样位-横向写入
 *
 */
void debugIntefaceDlg::on_pb_HardParam_SampleAddXWrite_clicked()
{
    //操作类型-加样位
    int nOptionType = 0;
    //位置类型
    int nPosType = 0;
    //移动距离
    int nMoveSize = ui->le_HardParam_SampleAddX->text().toInt();
    if(nMoveSize < 0 ){
        return;
    }
    //数据类型
    int nDataType = 0;
    HardwareDebugLoadingArmCommand(nOptionType,nPosType,nMoveSize,nDataType);
    m_pSettinsObj->SetParam(DEBUGHWARMSAMPLEAddX,QString::number(nMoveSize));
}

/*
 *
 *硬件调试界面-加样臂-加样位-纵向写入
 *
 */
void debugIntefaceDlg::on_pb_HardParam_SampleAddYWrite_clicked()
{
    //操作类型-加样位
    int nOptionType = 0;
    //位置类型
    int nPosType = 1;
    //移动距离
    int nMoveSize = ui->le_HardParam_SampleAddY->text().toInt();
    if(nMoveSize < 0 ){
        return;
    }
    //数据类型
    int nDataType = 0;
    HardwareDebugLoadingArmCommand(nOptionType,nPosType,nMoveSize,nDataType);
    m_pSettinsObj->SetParam(DEBUGHWARMSAMPLEAddY,QString::number(nMoveSize));
}

/*
 *
 *硬件调试界面-加样臂-加样位-横向读取
 *
 */
void debugIntefaceDlg::on_pb_HardParam_SampleAddXRead_clicked()
{
    //操作类型-加样位
    int nOptionType = 0;
    //位置类型
    int nPosType = 0;
    //移动距离
    int nMoveSize = 0;
    //数据类型
    int nDataType = 1;
    HardwareDebugLoadingArmCommand(nOptionType,nPosType,nMoveSize,nDataType);
}

/*
 *
 *硬件调试界面-加样臂-加样位-纵向读取
 *
 */
void debugIntefaceDlg::on_pb_HardParam_SampleAddYRead_clicked()
{
    //操作类型-加样位
    int nOptionType = 0;
    //位置类型
    int nPosType = 1;
    //移动距离
    int nMoveSize = 0;
    //数据类型
    int nDataType = 1;
    HardwareDebugLoadingArmCommand(nOptionType,nPosType,nMoveSize,nDataType);
}

/*
 *
 *硬件调试界面-加样臂-混匀位-横向写入
 *
 */
void debugIntefaceDlg::on_pb_HardParam_MixingXWrite_clicked()
{
    //操作类型-混匀位
    int nOptionType = 2;
    //位置类型
    int nPosType = 0;
    //移动距离
    int nMoveSize = ui->le_HardParam_MixingX->text().toInt();
    if(nMoveSize < 0 ){
        return;
    }
    //数据类型
    int nDataType = 0;
    HardwareDebugLoadingArmCommand(nOptionType,nPosType,nMoveSize,nDataType);
    m_pSettinsObj->SetParam(DEBUGHWARMMIXINGX,QString::number(nMoveSize));
}

/*
 *
 *硬件调试界面-加样臂-混匀位-横向读取
 *
 */
void debugIntefaceDlg::on_pb_HardParam_MixingXRead_clicked()
{
    //操作类型-混匀位
    int nOptionType = 2;
    //位置类型
    int nPosType = 0;
    //移动距离
    int nMoveSize = 0;
    //数据类型
    int nDataType = 1;
    HardwareDebugLoadingArmCommand(nOptionType,nPosType,nMoveSize,nDataType);
}

/*
 *
 *硬件调试界面-加样臂-混匀位-纵向写入
 *
 */
void debugIntefaceDlg::on_pb_HardParam_MixingYWrite_clicked()
{
    //操作类型-混匀位
    int nOptionType = 2;
    //位置类型
    int nPosType = 1;
    //移动距离
    int nMoveSize = ui->le_HardParam_MixingY->text().toInt();
    if(nMoveSize < 0 ){
        return;
    }
    //数据类型
    int nDataType = 0;
    HardwareDebugLoadingArmCommand(nOptionType,nPosType,nMoveSize,nDataType);
    m_pSettinsObj->SetParam(DEBUGHWARMMIXINGY,QString::number(nMoveSize));
}

/*
 *
 *硬件调试界面-加样臂-混匀位-纵向读取
 *
 */
void debugIntefaceDlg::on_pb_HardParam_MixingYRead_clicked()
{
    //操作类型-混匀位
    int nOptionType = 2;
    //位置类型
    int nPosType = 1;
    //移动距离
    int nMoveSize = 0;
    //数据类型
    int nDataType = 1;
    HardwareDebugLoadingArmCommand(nOptionType,nPosType,nMoveSize,nDataType);
}

/*
 *
 *硬件调试界面-加样臂-混匀位-触液深度写入
 *
 */
void debugIntefaceDlg::on_pb_HardParam_MixingDepthWrite_clicked()
{
    //操作类型-混匀位
    int nOptionType = 2;
    //位置类型
    int nPosType = 2;
    //移动距离
    int nMoveSize = ui->le_HardParam_MixingDepth->text().toInt();
    if(nMoveSize < 0 ){
        return;
    }
    //数据类型
    int nDataType = 0;
    HardwareDebugLoadingArmCommand(nOptionType,nPosType,nMoveSize,nDataType);
    m_pSettinsObj->SetParam(DEBUGHWARMMIXINGDEPTH,QString::number(nMoveSize));
}

/*
 *
 *硬件调试界面-加样臂-混匀位-触液深度读取
 *
 */
void debugIntefaceDlg::on_pb_HardParam_MixingDepthRead_clicked()
{
    //操作类型-混匀位
    int nOptionType = 2;
    //位置类型
    int nPosType = 2;
    //移动距离
    int nMoveSize = 0;
    //数据类型
    int nDataType = 1;
    HardwareDebugLoadingArmCommand(nOptionType,nPosType,nMoveSize,nDataType);
}

/*
 *
 *硬件调试界面-加样臂-混匀位-加缓冲液深度写入
 *
 */
void debugIntefaceDlg::on_pb_HardParam_MixingBufferDepthWrite_clicked()
{
    //操作类型-混匀位
    int nOptionType = 2;
    //位置类型
    int nPosType = 8;
    //移动距离
    int nMoveSize = ui->le_HardParam_MixingBufferDepth->text().toInt();
    if(nMoveSize < 0 ){
        return;
    }
    //数据类型
    int nDataType = 0;
    HardwareDebugLoadingArmCommand(nOptionType,nPosType,nMoveSize,nDataType);
    //m_pSettinsObj->SetParam(DEBUGHWARMMIXINGDEPTH,QString::number(nMoveSize));
}

/*
 *
 *硬件调试界面-加样臂-混匀位-加缓冲液深度读取
 *
 */
void debugIntefaceDlg::on_pb_HardParam_MixingBufferDepthRead_clicked()
{
    //操作类型-混匀位
    int nOptionType = 2;
    //位置类型
    int nPosType = 8;
    //移动距离
    int nMoveSize = 0;
    //数据类型
    int nDataType = 1;
    HardwareDebugLoadingArmCommand(nOptionType,nPosType,nMoveSize,nDataType);
}

/*
 *
 *硬件调试界面-加样臂-混匀位-清洗液清洗次数写入
 *
 */
void debugIntefaceDlg::on_pb_HardParam_MixingClearCountWrite_clicked()
{
    //操作类型-混匀位
    int nOptionType = 2;
    //位置类型
    int nPosType = 6;
    //移动距离
    int nMoveSize = ui->le_HardParam_MixingFluidClearCount->text().toInt();
    if(nMoveSize < 0 ){
        return;
    }
    //数据类型
    int nDataType = 0;
    HardwareDebugLoadingArmCommand(nOptionType,nPosType,nMoveSize,nDataType);
    m_pSettinsObj->SetParam(DEBUGHWARMMIXINGDEPTH,QString::number(nMoveSize));
}

/*
 *
 *硬件调试界面-加样臂-混匀位-清洗液清洗次数读取
 *
 */
void debugIntefaceDlg::on_pb_HardParam_MixingClearCountRead_clicked()
{
    //操作类型-混匀位
    int nOptionType = 2;
    //位置类型
    int nPosType = 6;
    //移动距离
    int nMoveSize = 0;
    //数据类型
    int nDataType = 1;
    HardwareDebugLoadingArmCommand(nOptionType,nPosType,nMoveSize,nDataType);
}

/*
 *
 *硬件调试界面-加样臂-混匀位-清水清洗次数写入
 *
 */
void debugIntefaceDlg::on_pb_HardParam_MixingWaterClearCountWrite_clicked()
{
    //操作类型-混匀位
    int nOptionType = 2;
    //位置类型
    int nPosType = 7;
    //移动距离
    int nMoveSize = ui->le_HardParam_MixingWaterClearCount->text().toInt();
    if(nMoveSize < 0 ){
        return;
    }
    //数据类型
    int nDataType = 0;
    HardwareDebugLoadingArmCommand(nOptionType,nPosType,nMoveSize,nDataType);
    m_pSettinsObj->SetParam(DEBUGHWARMMIXINGDEPTH,QString::number(nMoveSize));
}

/*
 *
 *硬件调试界面-加样臂-混匀位-清水清洗次数读取
 *
 */
void debugIntefaceDlg::on_pb_HardParam_MixingWaterClearCountRead_clicked()
{
    //操作类型-混匀位
    int nOptionType = 2;
    //位置类型
    int nPosType = 7;
    //移动距离
    int nMoveSize = 0;
    //数据类型
    int nDataType = 1;
    HardwareDebugLoadingArmCommand(nOptionType,nPosType,nMoveSize,nDataType);
}

/*
 *
 *硬件调试界面-加样臂-混匀位-停留时间写入
 *
 */
void debugIntefaceDlg::on_pb_HardParam_MixingWaitTimeWrite_clicked()
{
    //操作类型-混匀位
    int nOptionType = 2;
    //位置类型
    int nPosType = 4;
    //移动距离
    int nMoveSize = ui->le_HardParam_MixingWaitTime->text().toInt();
    if(nMoveSize < 0 ){
        return;
    }
    //数据类型
    int nDataType = 0;
    HardwareDebugLoadingArmCommand(nOptionType,nPosType,nMoveSize,nDataType);
    m_pSettinsObj->SetParam(DEBUGHWARMMIXINGDEPTH,QString::number(nMoveSize));
}

/*
 *
 *硬件调试界面-加样臂-混匀位-停留时间读取
 *
 */
void debugIntefaceDlg::on_pb_HardParam_MixingWaitTimeRead_clicked()
{
    //操作类型-混匀位
    int nOptionType = 2;
    //位置类型
    int nPosType = 4;
    //移动距离
    int nMoveSize = 0;
    //数据类型
    int nDataType = 1;
    HardwareDebugLoadingArmCommand(nOptionType,nPosType,nMoveSize,nDataType);
}

/*
 *
 *硬件调试界面-加样臂-混匀位-注水时间写入
 *
 */
void debugIntefaceDlg::on_pb_HardParam_MixingIntoWaterWrite_clicked()
{
    //操作类型-混匀位
    int nOptionType = 2;
    //位置类型
    int nPosType = 5;
    //移动距离
    int nMoveSize = ui->le_HardParam_MixingIntoWater->text().toInt();
    if(nMoveSize < 0 ){
        return;
    }
    //数据类型
    int nDataType = 0;
    HardwareDebugLoadingArmCommand(nOptionType,nPosType,nMoveSize,nDataType);
    m_pSettinsObj->SetParam(DEBUGHWARMMIXINGDEPTH,QString::number(nMoveSize));
}

/*
 *
 *硬件调试界面-加样臂-混匀位-注水时间读取
 *
 */
void debugIntefaceDlg::on_pb_HardParam_MixingIntoWaterRead_clicked()
{
    //操作类型-混匀位
    int nOptionType = 2;
    //位置类型
    int nPosType = 5;
    //移动距离
    int nMoveSize = 0;
    //数据类型
    int nDataType = 1;
    HardwareDebugLoadingArmCommand(nOptionType,nPosType,nMoveSize,nDataType);
}

/*
 *
 *硬件调试界面-加样臂-清洗位-横向写入
 *
 */
void debugIntefaceDlg::on_pb_HardParam_ClearoutXWrite_clicked()
{
    //操作类型-混匀位
    int nOptionType = 1;
    //位置类型
    int nPosType = 0;
    //移动距离
    int nMoveSize = ui->le_HardParam_ClearoutX->text().toInt();
    if(nMoveSize < 0 ){
        return;
    }
    //数据类型
    int nDataType = 0;
    HardwareDebugLoadingArmCommand(nOptionType,nPosType,nMoveSize,nDataType);
    m_pSettinsObj->SetParam(DEBUGHWARMMIXINGX,QString::number(nMoveSize));
}

/*
 *
 *硬件调试界面-加样臂-清洗位-纵向写入
 *
 */
void debugIntefaceDlg::on_pb_HardParam_ClearoutYWrite_clicked()
{
    //操作类型-混匀位
    int nOptionType = 1;
    //位置类型
    int nPosType = 1;
    //移动距离
    int nMoveSize = ui->le_HardParam_ClearoutY->text().toInt();
    if(nMoveSize < 0 ){
        return;
    }
    //数据类型
    int nDataType = 0;
    HardwareDebugLoadingArmCommand(nOptionType,nPosType,nMoveSize,nDataType);
    m_pSettinsObj->SetParam(DEBUGHWARMMIXINGX,QString::number(nMoveSize));
}

/*
 *
 *硬件调试界面-加样臂-清洗位-横向读取
 *
 */
void debugIntefaceDlg::on_pb_HardParam_ClearoutXRead_clicked()
{
    //操作类型-清洗位
    int nOptionType = 1;
    //位置类型
    int nPosType = 0;
    //移动距离
    int nMoveSize = 0;
    //数据类型
    int nDataType = 1;
    HardwareDebugLoadingArmCommand(nOptionType,nPosType,nMoveSize,nDataType);
}

/*
 *
 *硬件调试界面-加样臂-清洗位-纵向读取
 *
 */
void debugIntefaceDlg::on_pb_HardParam_ClearoutYRead_clicked()
{
    //操作类型-清洗位
    int nOptionType = 1;
    //位置类型
    int nPosType = 1;
    //移动距离
    int nMoveSize = 0;
    //数据类型
    int nDataType = 1;
    HardwareDebugLoadingArmCommand(nOptionType,nPosType,nMoveSize,nDataType);
}

/*
 *
 *硬件调试界面-加样臂-清洗位-清洗液清洗次数写入
 *
 */
void debugIntefaceDlg::on_pb_HardParam_FluidClearCountWrite_clicked()
{
    //操作类型-混匀位
    int nOptionType = 1;
    //位置类型
    int nPosType = 6;
    //移动距离
    int nMoveSize = ui->le_HardParam_FluidClearCount->text().toInt();
    if(nMoveSize < 0 ){
        return;
    }
    //数据类型
    int nDataType = 0;
    HardwareDebugLoadingArmCommand(nOptionType,nPosType,nMoveSize,nDataType);
    m_pSettinsObj->SetParam(DEBUGHWARMMIXINGX,QString::number(nMoveSize));
}

/*
 *
 *硬件调试界面-加样臂-清洗位-清洗液清洗次数读取
 *
 */
void debugIntefaceDlg::on_pb_HardParam_FluidClearCountRead_clicked()
{
    //操作类型-清洗位
    int nOptionType = 1;
    //位置类型
    int nPosType = 6;
    //移动距离
    int nMoveSize = 0;
    //数据类型
    int nDataType = 1;
    HardwareDebugLoadingArmCommand(nOptionType,nPosType,nMoveSize,nDataType);
}

/*
 *
 *硬件调试界面-加样臂-清洗位-纯水清洗次数写入
 *
 */
void debugIntefaceDlg::on_pb_HardParam_WaterClearCountWrite_clicked()
{
    //操作类型-混匀位
    int nOptionType = 1;
    //位置类型
    int nPosType = 7;
    //移动距离
    int nMoveSize = ui->le_HardParam_WaterClearCount->text().toInt();
    if(nMoveSize < 0 ){
        return;
    }
    //数据类型
    int nDataType = 0;
    HardwareDebugLoadingArmCommand(nOptionType,nPosType,nMoveSize,nDataType);
    m_pSettinsObj->SetParam(DEBUGHWARMMIXINGX,QString::number(nMoveSize));
}

/*
 *
 *硬件调试界面-加样臂-清洗位-纯水清洗次数读取
 *
 */
void debugIntefaceDlg::on_pb_HardParam_WaterClearCountRead_clicked()
{
    //操作类型-清洗位
    int nOptionType = 1;
    //位置类型
    int nPosType = 7;
    //移动距离
    int nMoveSize = 0;
    //数据类型
    int nDataType = 1;
    HardwareDebugLoadingArmCommand(nOptionType,nPosType,nMoveSize,nDataType);
}

/*
 *
 *硬件调试界面-加样臂-清洗位-注水时间写入
 *
 */
void debugIntefaceDlg::on_pb_HardParam_ClearTimeWrite_clicked()
{
    //操作类型-混匀位
    int nOptionType = 1;
    //位置类型
    int nPosType = 5;
    //移动距离
    int nMoveSize = ui->le_HardParam_IntoWaterTime->text().toInt();
    if(nMoveSize < 0 ){
        return;
    }
    //数据类型
    int nDataType = 0;
    HardwareDebugLoadingArmCommand(nOptionType,nPosType,nMoveSize,nDataType);
    m_pSettinsObj->SetParam(DEBUGHWARMMIXINGX,QString::number(nMoveSize));
}

/*
 *
 *硬件调试界面-加样臂-清洗位-注水时间读取
 *
 */
void debugIntefaceDlg::on_pb_HardParam_ClearTimeRead_clicked()
{
    //操作类型-清洗位
    int nOptionType = 1;
    //位置类型
    int nPosType = 5;
    //移动距离
    int nMoveSize = 0;
    //数据类型
    int nDataType = 1;
    HardwareDebugLoadingArmCommand(nOptionType,nPosType,nMoveSize,nDataType);
}

/*
 *
 *硬件调试界面-加样臂-缓冲液位-横向写入
 *
 */
void debugIntefaceDlg::on_pb_HardParam_BufferXWrite_clicked()
{
    //操作类型-缓冲液位
    int nOptionType = 3;
    //位置类型
    int nPosType = 0;
    //移动距离
    int nMoveSize = ui->le_HardParam_BufferX->text().toInt();
    if(nMoveSize < 0 ){
        return;
    }
    //数据类型
    int nDataType = 0;
    HardwareDebugLoadingArmCommand(nOptionType,nPosType,nMoveSize,nDataType);
    m_pSettinsObj->SetParam(DEBUGHWARMMIXINGX,QString::number(nMoveSize));
}

/*
 *
 *硬件调试界面-加样臂-缓冲液位-纵向写入
 *
 */
void debugIntefaceDlg::on_pb_HardParam_BufferYWrite_clicked()
{
    //操作类型-缓冲液位
    int nOptionType = 3;
    //位置类型
    int nPosType = 1;
    //移动距离
    int nMoveSize = ui->le_HardParam_BufferY->text().toInt();
    if(nMoveSize < 0 ){
        return;
    }
    //数据类型
    int nDataType = 0;
    HardwareDebugLoadingArmCommand(nOptionType,nPosType,nMoveSize,nDataType);
    m_pSettinsObj->SetParam(DEBUGHWARMMIXINGX,QString::number(nMoveSize));
}

/*
 *
 *硬件调试界面-加样臂-缓冲液位-触液深度向写入
 *
 */
void debugIntefaceDlg::on_pb_HardParam_BufferDepthWrite_clicked()
{
    //操作类型-缓冲液位
    int nOptionType = 3;
    //位置类型
    int nPosType = 2;
    //移动距离
    int nMoveSize = ui->le_HardParam_BufferDepth->text().toInt();
    if(nMoveSize < 0 ){
        return;
    }
    //数据类型
    int nDataType = 0;
    HardwareDebugLoadingArmCommand(nOptionType,nPosType,nMoveSize,nDataType);
    m_pSettinsObj->SetParam(DEBUGHWARMMIXINGX,QString::number(nMoveSize));
}

/*
 *
 *硬件调试界面-加样臂-缓冲液位-偏移量写入
 *
 */
void debugIntefaceDlg::on_pb_HardParam_BufferOffsetWrite_clicked()
{
    //操作类型-缓冲液位
    int nOptionType = 3;
    //位置类型
    int nPosType = 3;
    //移动距离
    int nMoveSize = ui->le_HardParam_BufferOffset->text().toInt();
    if(nMoveSize < 0 ){
        return;
    }
    //数据类型
    int nDataType = 0;
    HardwareDebugLoadingArmCommand(nOptionType,nPosType,nMoveSize,nDataType);
    m_pSettinsObj->SetParam(DEBUGHWARMMIXINGX,QString::number(nMoveSize));
}

/*
 *
 *硬件调试界面-加样臂-缓冲液位-横向读取
 *
 */
void debugIntefaceDlg::on_pb_HardParam_BufferXRead_clicked()
{
    //操作类型-缓冲液位
    int nOptionType = 3;
    //位置类型
    int nPosType = 0;
    //移动距离
    int nMoveSize = 0;
    //数据类型
    int nDataType = 1;
    HardwareDebugLoadingArmCommand(nOptionType,nPosType,nMoveSize,nDataType);
}

/*
 *
 *硬件调试界面-加样臂-缓冲液位-纵向读取
 *
 */
void debugIntefaceDlg::on_pb_HardParam_BufferYRead_clicked()
{
    //操作类型-缓冲液位
    int nOptionType = 3;
    //位置类型
    int nPosType = 1;
    //移动距离
    int nMoveSize = 0;
    //数据类型
    int nDataType = 1;
    HardwareDebugLoadingArmCommand(nOptionType,nPosType,nMoveSize,nDataType);
}

/*
 *
 *硬件调试界面-加样臂-缓冲液位-触液深度读取
 *
 */
void debugIntefaceDlg::on_pb_HardParam_BufferDepthRead_clicked()
{
    //操作类型-缓冲液位
    int nOptionType = 3;
    //位置类型
    int nPosType = 2;
    //移动距离
    int nMoveSize = 0;
    //数据类型
    int nDataType = 1;
    HardwareDebugLoadingArmCommand(nOptionType,nPosType,nMoveSize,nDataType);
}

/*
 *
 *硬件调试界面-加样臂-缓冲液位-偏移量读取
 *
 */
void debugIntefaceDlg::on_pb_HardParam_BufferOffsetRead_clicked()
{
    //操作类型-缓冲液位
    int nOptionType = 3;
    //位置类型
    int nPosType = 3;
    //移动距离
    int nMoveSize = 0;
    //数据类型
    int nDataType = 1;
    HardwareDebugLoadingArmCommand(nOptionType,nPosType,nMoveSize,nDataType);
}

/*
 *
 *硬件调试界面-加样臂-吸样位-横向写入
 *
 */
void debugIntefaceDlg::on_pb_HardParam_SuctionXWrite_clicked()
{
    //操作类型-吸样位
    int nOptionType = 4;
    //位置类型
    int nPosType = 0;
    //移动距离
    int nMoveSize = ui->le_HardParam_SuctionX->text().toInt();
    if(nMoveSize < 0 ){
        return;
    }
    //数据类型
    int nDataType = 0;
    HardwareDebugLoadingArmCommand(nOptionType,nPosType,nMoveSize,nDataType);
    m_pSettinsObj->SetParam(DEBUGHWARMMIXINGX,QString::number(nMoveSize));
}

/*
 *
 *硬件调试界面-加样臂-吸样位-纵向写入
 *
 */
void debugIntefaceDlg::on_pb_HardParam_SuctionYWrite_clicked()
{
    //操作类型-吸样位
    int nOptionType = 4;
    //位置类型
    int nPosType = 1;
    //移动距离
    int nMoveSize = ui->le_HardParam_SuctionY->text().toInt();
    if(nMoveSize < 0 ){
        return;
    }
    //数据类型
    int nDataType = 0;
    HardwareDebugLoadingArmCommand(nOptionType,nPosType,nMoveSize,nDataType);
    m_pSettinsObj->SetParam(DEBUGHWARMMIXINGX,QString::number(nMoveSize));
}

/*
 *
 *硬件调试界面-加样臂-吸样位-触液深度写入
 *
 */
void debugIntefaceDlg::on_pb_HardParam_SuctionDepthWrite_clicked()
{
    //操作类型-吸样位
    int nOptionType = 4;
    //位置类型
    int nPosType = 2;
    //移动距离
    int nMoveSize = ui->le_HardParam_SuctionDepth->text().toInt();
    if(nMoveSize < 0 ){
        return;
    }
    //数据类型
    int nDataType = 0;
    HardwareDebugLoadingArmCommand(nOptionType,nPosType,nMoveSize,nDataType);
    m_pSettinsObj->SetParam(DEBUGHWARMMIXINGX,QString::number(nMoveSize));
}

/*
 *
 *硬件调试界面-加样臂-吸样位-偏移量写入
 *
 */
void debugIntefaceDlg::on_pb_HardParam_SuctionOffsetWrite_clicked()
{
    //操作类型-吸样位
    int nOptionType = 4;
    //位置类型
    int nPosType = 3;
    //移动距离
    int nMoveSize = ui->le_HardParam_SuctionOffset->text().toInt();
    if(nMoveSize < 0 ){
        return;
    }
    //数据类型
    int nDataType = 0;
    HardwareDebugLoadingArmCommand(nOptionType,nPosType,nMoveSize,nDataType);
    m_pSettinsObj->SetParam(DEBUGHWARMMIXINGX,QString::number(nMoveSize));
}

/*
 *
 *硬件调试界面-加样臂-吸样位-横向读取
 *
 */
void debugIntefaceDlg::on_pb_HardParam_SuctionXRead_clicked()
{
    //操作类型-吸样位
    int nOptionType = 4;
    //位置类型
    int nPosType = 0;
    //移动距离
    int nMoveSize = 0;
    //数据类型
    int nDataType = 1;
    HardwareDebugLoadingArmCommand(nOptionType,nPosType,nMoveSize,nDataType);
}

/*
 *
 *硬件调试界面-加样臂-吸样位-纵向读取
 *
 */
void debugIntefaceDlg::on_pb_HardParam_SuctionYRead_clicked()
{
    //操作类型-吸样位
    int nOptionType = 4;
    //位置类型
    int nPosType = 1;
    //移动距离
    int nMoveSize = 0;
    //数据类型
    int nDataType = 1;
    HardwareDebugLoadingArmCommand(nOptionType,nPosType,nMoveSize,nDataType);
}

/*
 *
 *硬件调试界面-加样臂-吸样位-触液深度读取
 *
 */
void debugIntefaceDlg::on_pb_HardParam_SuctionDepthRead_clicked()
{
    //操作类型-吸样位
    int nOptionType = 4;
    //位置类型
    int nPosType = 2;
    //移动距离
    int nMoveSize = 0;
    //数据类型
    int nDataType = 1;
    HardwareDebugLoadingArmCommand(nOptionType,nPosType,nMoveSize,nDataType);
}

/*
 *
 *硬件调试界面-加样臂-吸样位-偏移量读取
 *
 */
void debugIntefaceDlg::on_pb_HardParam_SuctionOffsetRead_clicked()
{
    //操作类型-吸样位
    int nOptionType = 4;
    //位置类型
    int nPosType = 3;
    //移动距离
    int nMoveSize = 0;
    //数据类型
    int nDataType = 1;
    HardwareDebugLoadingArmCommand(nOptionType,nPosType,nMoveSize,nDataType);
}

/*
 *
 *硬件调试界面-加样臂-当前位置-横向读取
 *
 */
void debugIntefaceDlg::on_pb_HardParam_CurrentPosXRead_clicked()
{
    //操作类型-当前位置
    int nOptionType = 5;
    //位置类型
    int nPosType = 0;
    //移动距离
    int nMoveSize = 0;
    //数据类型
    int nDataType = 1;
    HardwareDebugLoadingArmCommand(nOptionType,nPosType,nMoveSize,nDataType);
}

/*
 *
 *硬件调试界面-加样臂-当前位置-横向复位
 *
 */
void debugIntefaceDlg::on_pb_HardParam_CurrentPosXReset_clicked()
{
    //操作类型-当前位置
    int nOptionType = 5;
    //位置类型
    int nPosType = 0;
    //移动距离
    int nMoveSize = 0;
    //数据类型
    int nDataType = 2;
    HardwareDebugLoadingArmCommand(nOptionType,nPosType,nMoveSize,nDataType);
}

/*
 *
 *硬件调试界面-加样臂-当前位置-纵向读取
 *
 */
void debugIntefaceDlg::on_pb_HardParam_CurrentPosYRead_clicked()
{
    //操作类型-当前位置
    int nOptionType = 5;
    //位置类型
    int nPosType = 1;
    //移动距离
    int nMoveSize = 0;
    //数据类型
    int nDataType = 1;
    HardwareDebugLoadingArmCommand(nOptionType,nPosType,nMoveSize,nDataType);
}


/*
 *
 *硬件调试界面-加样臂-当前位置-纵向复位
 *
 */
void debugIntefaceDlg::on_pb_HardParam_CurrentPosYReset_clicked()
{
    //操作类型-当前位置
    int nOptionType = 5;
    //位置类型
    int nPosType = 1;
    //移动距离
    int nMoveSize = 0;
    //数据类型
    int nDataType = 2;
    HardwareDebugLoadingArmCommand(nOptionType,nPosType,nMoveSize,nDataType);
}

/*
 *
 *硬件调试界面-所有参数-保存
 *
 */
void debugIntefaceDlg::on_pb_HardParam_ParamAllSave_clicked()
{
    QByteArray byarrData;
    m_pTestInterface->SendSerialData(m_pControlPanelsSerial,PL_FUN_AM_ID_DEBUG_HARDWARE_SAVE,byarrData);
}

/********************************************************
 *@Name:        HardwareDebugDialCommand
 *@Author:      HuaT
 *@Description: 硬件调试转盘命令
 *@Param1:      参数类型,0：写入，1：请求，2：复位
 *@Param2:      操作类型，0：零点位置，1：转盘位置，2：偏移量
 *@Param3:      转盘位置
 *@Return:      无
 *@Version:     1.0
 *@Date:        2019-3-4
********************************************************/
void debugIntefaceDlg::HardwareDebugDialCommand(qint32 OptionType, qint32 ParamType, qint32 DialPos)
{
    QByteArray byarrData;
    byarrData.append(m_pTestInterface->IntToBytes(OptionType));
    byarrData.append(m_pTestInterface->IntToBytes(ParamType));
    byarrData.append(m_pTestInterface->IntToBytes(DialPos));
    m_pTestInterface->SendSerialData(m_pControlPanelsSerial,PL_FUN_AM_ID_DEBUG_DIALPARAM,byarrData);
}

/*
 *
 *硬件调试界面-转盘-复位
 *
 */
void debugIntefaceDlg::on_pb_HardParam_DialPosReset_clicked()
{
    //参数类型-0：写入，1：请求，2：复位
    int nOptionType = 2;
    //操作类型0：零点位置，1：转盘位置，2：偏移量
    int nParamType = 0;
    //参数大小
    int nParamSize = 0;
    //
    HardwareDebugDialCommand(nOptionType,nParamType,nParamSize);
}

/*
 *
 *硬件调试界面-转盘-零点位置设定
 *
 */
void debugIntefaceDlg::on_pb_HardParam_DialInitStep_clicked()
{
    //参数类型-0：写入，1：请求，2：复位
    int nParamType = 0;
    //操作类型0：零点位置，1：转盘位置，2：偏移量
    int nOptionType = 0;
    //参数大小
    int nParamSize = ui->le_HardParam_DialInitStep->text().toInt();
    //
    HardwareDebugDialCommand(nParamType,nOptionType,nParamSize);
}

/*
 *
 *硬件调试界面-转盘-零点位置读取
 *
 */
void debugIntefaceDlg::on_pb_HardParam_DialInitStepRead_clicked()
{
    //参数类型-0：写入，1：请求，2：复位
    int nParamType = 1;
    //操作类型0：零点位置，1：转盘位置，2：偏移量
    int nOptionType = 0;
    //参数大小
    int nParamSize = 0;
    //
    HardwareDebugDialCommand(nParamType,nOptionType,nParamSize);
}

/*
 *
 *硬件调试界面-转盘-转盘位置设定
 *
 */
void debugIntefaceDlg::on_pb_HardParam_DialPos_clicked()
{
    //参数类型-0：写入，1：请求，2：复位
    int nParamType = 0;
    //操作类型0：零点位置，1：转盘位置，2：偏移量
    int nOptionType = 1;
    //参数大小
    int nParamSize = ui->le_HardParam_DialPos->text().toInt();
    //
    HardwareDebugDialCommand(nParamType,nOptionType,nParamSize);
}

/*
 *
 *硬件调试界面-转盘-偏移量设置
 *
 */
void debugIntefaceDlg::on_pb_HardParam_DialPosOffset_clicked()
{
    //参数类型-0：写入，1：请求，2：复位
    int nParamType = 0;
    //操作类型0：零点位置，1：转盘位置，2：偏移量
    int nOptionType = 2;
    //参数大小
    int nParamSize = ui->le_HardParam_DialOffset->text().toInt();
    //
    HardwareDebugDialCommand(nParamType,nOptionType,nParamSize);
}

/*
 *
 *硬件调试界面-转盘-偏移量读取
 *
 */
void debugIntefaceDlg::on_pb_HardParam_DialPosOffsetRead_clicked()
{
    //参数类型-0：写入，1：请求，2：复位
    int nParamType = 1;
    //操作类型0：零点位置，1：转盘位置，2：偏移量
    int nOptionType = 2;
    //参数大小
    int nParamSize = 0;
    //
    HardwareDebugDialCommand(nParamType,nOptionType,nParamSize);
}

/********************************************************
 *@Name:        HardwareDebugPushMotorCommand
 *@Author:      HuaT
 *@Description: 硬件调试推卡电机命令
 *@Param1:      参数类型,0：写入，1：请求，2：复位
 *@Param2:      电机ID，0-3
 *@Param3:      步数大小
 *@Return:      无
 *@Version:     1.0
 *@Date:        2019-3-4
********************************************************/
void debugIntefaceDlg::HardwareDebugPushMotorCommand(qint32 OptionType, qint32 MotorID, qint32 SetupSize)
{
    QByteArray byarrData;
    byarrData.append(m_pTestInterface->IntToBytes(OptionType));
    byarrData.append(m_pTestInterface->IntToBytes(MotorID));
    byarrData.append(m_pTestInterface->IntToBytes(SetupSize));
    m_pTestInterface->SendSerialData(m_pControlPanelsSerial,PL_FUN_AM_ID_DEBUG_PUSHMOTOR,byarrData);
}

/*
 *
 *硬件调试界面-推卡电机-步数设定
 *
 */
void debugIntefaceDlg::on_pb_HardParam_PushMotorOk_clicked()
{
    //参数类型-0：写入，1：请求，2：复位
    int nOptionType = 0;
    //电机编号0-3
    int nMotorID = ui->le_HardParam_PushNumber->text().toInt();
    //移动距离
    int nMoveSize = ui->le_HardParam_PushMaxLen->text().toInt();
    //
    HardwareDebugPushMotorCommand(nOptionType,nMotorID,nMoveSize);
}

/*
 *
 *硬件调试界面-推卡电机-步数读取
 *
 */
void debugIntefaceDlg::on_pb_HardParam_PushMotorRead_clicked()
{
    //参数类型-0：写入，1：请求，2：复位
    int nOptionType = 1;
    //电机编号0-3
    int nMotorID = ui->le_HardParam_PushNumber->text().toInt();
    //移动距离
    int nMoveSize = 0;
    //
    HardwareDebugPushMotorCommand(nOptionType,nMotorID,nMoveSize);
}

/*
 *
 *硬件调试界面-推卡电机-复位设定
 *
 */
void debugIntefaceDlg::on_pb_HardParam_PushMotorReset_clicked()
{
    //0：写入，1：请求，2：复位
    int OptionType = 2;
    //电机编号0-3
    int nMotorID = ui->le_HardParam_PushNumber->text().toInt();
    //移动距离
    int nMoveSize = 0;
    //
    HardwareDebugPushMotorCommand(OptionType,nMotorID,nMoveSize);
}

/********************************************************
 *@Name:        HardwareDebugDiaphPumCommand
 *@Author:      HuaT
 *@Description: 硬件调试隔膜泵命令
 *@Param1:      参数类型,0：打开，1：关闭
 *@Param2:      泵编号
 *@Return:      无
 *@Version:     1.0
 *@Date:        2019-3-4
********************************************************/
void debugIntefaceDlg::HardwareDebugDiaphPumCommand(qint32 OptionType, qint32 PumpNumber)
{
    QByteArray byarrData;
    byarrData.append(m_pTestInterface->IntToBytes(OptionType));
    byarrData.append(m_pTestInterface->IntToBytes(PumpNumber));
    m_pTestInterface->SendSerialData(m_pControlPanelsSerial,PL_FUN_AM_ID_DEBUG_DIAPHRAGM,byarrData);
}

/*
 *
 *硬件调试界面-隔膜泵电机-开启设定
 *
 */
void debugIntefaceDlg::on_pb_HardParam_DiaphPumpOpen_clicked()
{
    //0：打开，1：关闭
    int OptionType = 0;
    //泵编号
    int PumpNumber = ui->le_HardParam_PumpNumber->text().toInt();
    //
    HardwareDebugDiaphPumCommand(OptionType,PumpNumber);
}

/*
 *
 *硬件调试界面-隔膜泵电机-关闭设定
 *
 */
void debugIntefaceDlg::on_pb_HardParam_DiaphPumpClose_clicked()
{
    //0：打开，1：关闭
    int OptionType = 1;
    //泵编号
    int PumpNumber = ui->le_HardParam_PumpNumber->text().toInt();
    //
    HardwareDebugDiaphPumCommand(OptionType,PumpNumber);
}

/********************************************************
 *@Name:        HardwareDebugValvesCommand
 *@Author:      HuaT
 *@Description: 硬件调试电磁阀命令
 *@Param1:      参数类型,0：打开，1：关闭
 *@Param2:      阀编号
 *@Return:      无
 *@Version:     1.0
 *@Date:        2019-3-4
********************************************************/
void debugIntefaceDlg::HardwareDebugValvesCommand(qint32 OptionType, qint32 ValvesNumber)
{
    QByteArray byarrData;
    byarrData.append(m_pTestInterface->IntToBytes(OptionType));
    byarrData.append(m_pTestInterface->IntToBytes(ValvesNumber));
    m_pTestInterface->SendSerialData(m_pControlPanelsSerial,PL_FUN_AM_ID_DEBUG_SOLENOID,byarrData);
}

/*
 *
 *硬件调试界面-电磁阀电机-开启设定
 *
 */
void debugIntefaceDlg::on_pb_HardParam_SolenoidOpen_clicked()
{
    //0：打开，1：关闭
    int OptionType = 0;
    //泵编号
    int PumpNumber = ui->le_HardParam_SolenoidNumber->text().toInt();
    //
    HardwareDebugValvesCommand(OptionType,PumpNumber);
}

/*
 *
 *硬件调试界面-电磁阀电机-关闭设定
 *
 */
void debugIntefaceDlg::on_pb_HardParam_SolenoidClose_clicked()
{
    //0：打开，1：关闭
    int OptionType = 1;
    //泵编号
    int PumpNumber = ui->le_HardParam_SolenoidNumber->text().toInt();
    //
    HardwareDebugValvesCommand(OptionType,PumpNumber);
}

/********************************************************
 *@Name:        HardwareDebugPumpCommand
 *@Author:      HuaT
 *@Description: 硬件调试柱塞泵命令
 *@Param1:      参数类型,0：吸样，1：吐样
 *@Param2:      泵编号
 *@Param3:      量大小
 *@Return:      无
 *@Version:     1.0
 *@Date:        2019-3-4
********************************************************/
void debugIntefaceDlg::HardwareDebugPumpCommand(qint32 OptionType, qint32 PumpNumber, qint32 PumpSize)
{
    QByteArray byarrData;
    byarrData.append(m_pTestInterface->IntToBytes(OptionType));
    byarrData.append(m_pTestInterface->IntToBytes(PumpNumber));
    byarrData.append(m_pTestInterface->IntToBytes(PumpSize));
    m_pTestInterface->SendSerialData(m_pControlPanelsSerial,PL_FUN_AM_ID_DEBUG_PLUNGERPUMP,byarrData);
}

/*
 *
 *硬件调试界面-柱塞泵电机-吸样设定
 *
 */
void debugIntefaceDlg::on_pb_HardParam_PlungerPumpIn_clicked()
{
    //0：吸样，1：吐样，2：复位
    int OptionType = 0;
    //泵编号
    int PumpNumber = ui->le_HardParam_PlungerPumpNumber->text().toInt();
    //量大小
    int PumpSize = ui->le_HardParam_PlungerPumpSize->text().toInt();
    HardwareDebugPumpCommand(OptionType,PumpNumber,PumpSize);
}

/*
 *
 *硬件调试界面-柱塞泵电机-吐样设定
 *
 */
void debugIntefaceDlg::on_pb_HardParam_PlungerPumpOut_clicked()
{
    //0：吸样，1：吐样，2：复位
    int OptionType = 1;
    //泵编号
    int PumpNumber = ui->le_HardParam_PlungerPumpNumber->text().toInt();
    //量大小
    int PumpSize = ui->le_HardParam_PlungerPumpSize->text().toInt();
    HardwareDebugPumpCommand(OptionType,PumpNumber,PumpSize);
}

/*
 *
 *硬件调试界面-柱塞泵电机-复位设定
 *
 */
void debugIntefaceDlg::on_pb_HardParam_PlungerPumpReset_clicked()
{
    //0：吸样，1：吐样，2：复位
    int OptionType = 2;
    //泵编号
    int PumpNumber = ui->le_HardParam_PlungerPumpNumber->text().toInt();
    //量大小
    int PumpSize = 0;
    HardwareDebugPumpCommand(OptionType,PumpNumber,PumpSize);
}


