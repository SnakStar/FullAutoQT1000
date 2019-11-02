﻿#include "testinterface.h"
#include "ui_testinterface.h"
#include "mainwindow.h"


//电机和转盘起始号从1开始

TestInterface::TestInterface(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TestInterface)
{
    ui->setupUi(this);
    //ui->checkBox_Test_Emergency->hide();
    ui->le_Test_SampleNumber->setEnabled(false);
    ui->le_Test_SampleNumber->installEventFilter(this);
    //MainWindow* MainWnd = (MainWindow*)parent;
    //
    m_pMainWnd = parent;
    //急诊状态
    m_bEmerStatus = false;
    //开始测试状态
    //m_bTestStatus = false;
    //加样状态
    m_bAddSampleStatus = false;
    //检测状态
    m_bCheckStatus = false;
    //调试参数状态
    m_bDebugMode = false;
    m_bConnectPC = false;
    //编号索引
    m_nNumIndex = 1;
    //初始化已完成测试总数
    m_nCompleteTestCount = 0;
    //急诊插入
    ui->pb_Test_InsertEmer->setEnabled(false);
    //新建测试不可用
    ui->pb_Test_NewTest->setEnabled(false);
    ui->pb_Test_DetailsInfo->setEnabled(false);
    //获取配置文件对象
    m_settins = (static_cast<MainWindow*>(m_pMainWnd))->GetSettingsObj();
    //获取数据库对象
    m_Devdb = (static_cast<MainWindow*>(m_pMainWnd))->GetDatabaseObj();
    //获取HL7串口对象
    m_ConnPCSerial = (static_cast<MainWindow*>(m_pMainWnd))->GetHL7SerialPort();
    //设置输入法属性
    ui->le_Test_SampleCount->setProperty("flag","number");
    ui->le_Test_SampleNumber->setProperty("noinput",true);
    ui->le_Test_StartNumber->setProperty("flag","number");
    ui->rb_Test_StartNumberClose->setChecked(true);
    //m_myInputPanel = (static_cast<MainWindow*>(m_pMainWnd) )->GetInputPanelContext();
    //ui->le_Test_SampleCount->installEventFilter(this);
    //初始化圆界面
    InitCircle();
    //初始化清洗液当前参数
    m_nFluidMaxValue =  m_settins->GetParam(CLEANOUTMAXVALUE).toInt();
    m_fCleanoutCurValue = m_settins->GetParam(CLEANOUTCURVALUE).toInt();
    m_fEffluentCurValue = m_settins->GetParam(EFFLUENTCURVALUE).toInt();
    //进度条控件,显示试剂卡剩余数量
    InitProgressBar();
    //列表框
    InitTableInfo();
    //初始化试剂队列位置
    InitReagentPos();
    //初始化测试信息
    //InitVectorTestInfo();
    //隐藏测试项目
    ShowTestItem(false);
    //初始化当前可测试项目和试剂数量
    InitValidTestItem();
    //初始化检测板串口
    bool bRel = false;
    bRel = (static_cast<MainWindow*>(m_pMainWnd))->InitCheckPanelsSerial();
    //bRel = InitCheckPanelsSerial();
    m_CheckPanelsSerial = NULL;
    m_ControlPanelsSerial = NULL;
    if(bRel){
        m_CheckPanelsSerial = (static_cast<MainWindow*>(m_pMainWnd))->GetCheckPanelsSerial() ;
        connect(m_CheckPanelsSerial,SIGNAL(readyRead()), this, SLOT(RecvCheckPanelsData()) );
        //初始化控制板串口
        bRel = (static_cast<MainWindow*>(m_pMainWnd))->InitControlPanelsSerial();
        if(bRel){
            m_ControlPanelsSerial = (static_cast<MainWindow*>(m_pMainWnd))->GetControlPanelsSerial();
            connect(m_ControlPanelsSerial,SIGNAL(readyRead()), this, SLOT(RecvControlPanelsData()) );
            //查询测试板初始化情况
            connect(&m_TimerTestCheck, SIGNAL(timeout()), this, SLOT(TestCheckFunc()) );
            m_TimerTestCheck.start(1000);
        }
    }

    //初始化扫码模式
    InitScanMode();
    //连接设置界面扫码槽
    connect(m_settins,SIGNAL(SettingChange()),this,SLOT(ScanModeChange()) );
    //清洗液最大值改变
    connect(m_settins,SIGNAL(SettingCleanoutValueChange()),this,SLOT(CleanoutValueChange()) );
    //连接设置界面测试试剂卡信息
    connect(m_settins,SIGNAL(SettingDebugCardChange()),this,SLOT( TestCardInfoChange() ) );
    m_TimerWaitResult = new QTimer;
    //
    connect(m_TimerWaitResult,SIGNAL(timeout()), this, SLOT(ProcWaitResultOpt()));
    m_TimerWaitResult->start(1000);
    connect(&m_TimerLogQuery,SIGNAL(timeout()), this, SLOT(QueryLogFileSize()) );
    m_TimerLogQuery.start(1000);

    connect(&m_TimerZeroClear,SIGNAL(timeout()), this, SLOT(ZeroClearNumber()) );
    m_TimerZeroClear.start(1000);
    //设置日志字体大小
    ui->tb_Test_RunInfoShow->setFontPointSize(12);
    //已完成测试总数初始化
    m_nCompleteTestCount = 0;
    //QString strContent = QString("本次测试开始,共%1例样本").arg(ui->lb_Test_SampleCount->text().toInt());
    //ShowLogText(ui->textBrowser,strContent,MAGENTA);

    //模拟ID卡闸
    if(m_settins->GetParam(TESTCARDMODE).toInt() == 1){
        SimulationIDCardCast();
    }
    ui->tb_Test_RunInfoShow->verticalScrollBar()->setStyleSheet("QScrollBar:vertical{"
                                                                "background-color:rgb(240,240,240);"
                                                                "border: 2px solid;"
                                                                "width:40px;}");
    ui->tb_Test_ShowLog->verticalScrollBar()->setStyleSheet("QScrollBar:vertical{"
                                                            "background-color:rgb(240,240,240);"
                                                            "border: 2px solid;"
                                                            "width:40px;}");

    //更新测试按钮
    InitValidTestItem();
    //qDebug()<<QObject::tr("测试中文");
    //隐藏重置按钮
    ui->pb_Test_Reset->hide();
    //载入样式
    ui->proBar_Test_Reagent1->setOrientation(Qt::Vertical);
    QFile fi(":/res/qss/TestStyle.qss");
    if(fi.open(QIODevice::ReadOnly)){
        QTextStream ts(&fi);
        QString strStyle = ts.readAll();
        this->setStyleSheet(strStyle);
        fi.close();
    }

    /*char c[]  = {0x33,0x30,0x35,0x11,0x11,0x11,0x11,0x02,0x01,0x02,0x00,0x00,0x01,0x02,0x00,0x00, 0x01,0x02,0x00,0x00};
    QByteArray s = c;
    //qDebug()<<(unsigned char)s[0];
    ScanBarCodeInfo bar;
    memcpy_s(&bar,sizeof(bar),s,sizeof(bar));
    QString ss;
    for(int n=0;n<16;n++){
        ss += bar.scaninfo[0].CODE[n];
    }
    qDebug()<<ss.size();
    qDebug()<<bar.scaninfo[0].Scan_State;*/

    //m_LogObj.SetLogSavePath(LOGPATH);
    //m_LogObj.StartLog();
}

TestInterface::~TestInterface()
{
    delete ui;
}

bool TestInterface::eventFilter(QObject *obj, QEvent *e){
    /*if(obj == ui->le_Test_SampleCount && (e->type() == QEvent::MouseButtonPress )){
        qDebug()<<m_myInputPanel;
        m_myInputPanel->showInputPanel();
        return true;
    }*/
    if(e->type() == QEvent::KeyPress){
        QKeyEvent* kevent = static_cast<QKeyEvent*>(e);
        if(kevent->key() == Qt::Key_Return){
            if(obj == ui->le_Test_SampleNumber){
                QString strNum = ui->le_Test_SampleNumber->text();
                int nRow = ui->tw_Test_TestInfo->currentRow();
                if(nRow+1>16){
                    nRow = 0;
                }
                ui->tw_Test_TestInfo->item(nRow,1)->setText(strNum);
                ui->le_Test_SampleNumber->setText("");
                ui->tw_Test_TestInfo->selectRow(nRow+1);
                if(nRow+1 == 16){
                    ui->lb_Test_CurrentSamPos->setText(QString::number(nRow+1));
                }else{
                    ui->lb_Test_CurrentSamPos->setText(QString::number(nRow+2));
                }

            }
        }
    }
    return QObject::eventFilter(obj,e);
}

void TestInterface::SimulationIDCardCast(){
    m_mapIDCardInfo.clear();
    //测试用数据卡
    QString strIDCard="3130313031313130303030303031202020202020003C0600303138313130303414061E641405010001190A1400C8687343525020202020202020202020202020202075672F4C202020202020016400003200C80000113F062F42052B00000309322D000000043A2B00000000002B00000000002B00000000000D121A052B000004313D2D0000091B0D2B00000000002B00000000002B0000000000145313052B00000635412D00002311002B00000000002B00000000002B00000000001B3B51052B00000B443C2D00008E335A2B00000000002B00000000002B0000000000000000002B00000000002B00000000002B00000000002B00000000002B0000000000FFFFFF";
    IDCardInfo ii;
    ii.m_byarrIDCardData = StrToBytes(strIDCard);
    ii.m_strTestItem = ii.m_byarrIDCardData.mid(46,20).trimmed();
    ii.m_nRemaining = 20;
    m_mapIDCardInfo["0"] = ii;

    m_mapTestItemPos[ii.m_strTestItem] = "0";

    UpdateIDCardInfo(0,20,ii.m_strTestItem);

    IDCardInfo ii2 = ii;
    ii2.m_strTestItem = "BMP";
    m_mapIDCardInfo["1"] = ii2;

    m_mapTestItemPos[ii2.m_strTestItem] = "1";

    UpdateIDCardInfo(1,20,ii2.m_strTestItem);

    //
    IDCardInfo ii3 = ii;
    ii3.m_strTestItem = "PCT";
    m_mapIDCardInfo["2"] = ii3;

    m_mapTestItemPos[ii2.m_strTestItem] = "2";

    UpdateIDCardInfo(2,20,ii3.m_strTestItem);
    //
    IDCardInfo ii4 = ii;
    ii3.m_strTestItem = "hsCRP2";
    m_mapIDCardInfo["3"] = ii4;
    m_mapTestItemPos[ii.m_strTestItem] = "3";

    UpdateIDCardInfo(3,20,ii4.m_strTestItem);
}


/********************************************************
 *@Name:        GetTestItems
 *@Author:      HuaT
 *@Description: 获取测试项目名称
 *@Param1:      无
 *@Return:      返回当前ID卡的测试项目映射
 *@Version:     1.0
 *@Date:        2018-5-21
********************************************************/
QMap<QString, QString> TestInterface::GetTestItems()
{
    return m_mapTestItemPos;
}

QByteArray TestInterface::StrToBytes(QString str)
{
    QByteArray byarrData;
    for(int n=0; n<str.size(); n+=2){
        byarrData.append( str.mid(n,2).toInt(0,16) );
    }
    return byarrData;
}


/********************************************************
 *@Name:        ParseSerialData
 *@Author:      HuaT
 *@Description: 解析串口发来的一帧数据，检测是否为完整的一帧数据
 *@Param1:      串口数据
 *@Return:      如果为完整一帧数据则返回真，否则返回false
 *@Version:     1.0
 *@Date:        2018-5-21
********************************************************/
bool TestInterface::ParseSerialData(const QByteArray &Data)
{
    MsgData CurrData;
    //qDebug()<<"CurrData.m_frameHeader:"<<CurrData.m_frameHeader;
    //qDebug()<<"Data.mid(0,4).toHex().toUInt(0,16):"<<Data.mid(0,4).toHex().toUInt(0,16);
    //qDebug()<<"CurrData.m_frameEnd:"<<CurrData.m_frameEnd;
    //qDebug()<<"Data.right(4).toHex().toUInt(0,16):"<<Data.right(4).toHex().toUInt(0,16);
    if(CurrData.m_frameHeader == BytesToInt( Data.mid(0,4) )
            && CurrData.m_frameEnd == BytesToInt( Data.right(4) ) ){
        return true;
    }else{
        return false;
    }
}

/********************************************************
 *@Name:        FillValidSerialData
 *@Author:      HuaT
 *@Description: 装填有效的数据到MsgData结构体
 *@Param1:      完整的一帧串口数据
 *@Param2:      需要填充的消息结构体
 *@Return:      返回是否填充成功
 *@Version:     1.0
 *@Date:        2018-5-21
********************************************************/
bool TestInterface::FillValidSerialData(QByteArray SerailData, MsgData& CurrData)
{
    //qDebug()<<SerailData.toHex().toUpper()<<SerailData.size();
    int nDataCount = SerailData.size();
    quint32 nContentLen = BytesToInt(SerailData.mid(12,4));
    CurrData.m_CRC = BytesToInt(SerailData.mid(nDataCount-8,4));
    //qDebug()<<"nContentLen"<<nContentLen;
    quint32 nCRC = GetCheckSum(SerailData.mid(12,4),SerailData.mid(16,nContentLen*4));
    //qDebug()<<"my check code:"<<nCRC<<" currdata.m_crc:"<<CurrData.m_CRC;
    if(nCRC == CurrData.m_CRC ){
        CurrData.m_ProductID = BytesToInt( SerailData.mid(4,4) );
        CurrData.m_DeviceAdd = BytesToInt( SerailData.mid(8,2) );
        CurrData.m_FuncIndex = BytesToInt( SerailData.mid(10,2) );
        CurrData.m_DataLength = BytesToInt( SerailData.mid(12,4) );
        CurrData.m_Data = SerailData.mid(16,CurrData.m_DataLength*4) ;
        return true;
    }else{
        return false;
    }
}

/********************************************************
 *@Name:        ParseTestResultData
 *@Author:      HuaT
 *@Description: 解析测试结果数据
 *@Param1:      接收到的结果数据
 *@Return:      解析是否成功
 *@Version:     1.0
 *@Date:        2018-6-16
********************************************************/
bool TestInterface::ParseTestResultData(const QByteArray &AckData)
{
    if(AckData.size() != 648){
        MsgBoxDlg msgbox;
        msgbox.ShowMsg(QObject::tr("结果数据长度不符,长度为：%1").arg(AckData.size()),0);
        msgbox.exec();
        return false;
    }
    //样本位
    quint32 nSamplePos = BytesToInt(AckData.mid(0,4));
    //测试位
    quint32 nReagentPos = BytesToInt(AckData.mid(4,4));
    QByteArray byarrResultData = AckData.mid(8,AckData.size()-8);
    //
    for(int n=0; n<m_listSampleInfo.size(); n++){
        for(int i=0; i<m_listSampleInfo.at(n).m_listTestItem.size(); i++){
            if(m_listSampleInfo.at(n).m_listTestItem.at(i).m_bResultWaitState == true){
                m_listSampleInfo[n].m_listTestItem[i].m_bResultWaitState = false;
            }
        }
    }
    //qDebug()<<byarrResultData.size(); //640
    //解析测试结果命令
    //qDebug()<<"parse result:";
    QString strLog;
    strLog = QString(CHECKINGCOMPLETE).arg(nSamplePos+1).arg(nReagentPos+1);
    ShowLogText(ui->tb_Test_RunInfoShow,strLog,LOGNORMAL);
    QByteArray byarrIDCardData;
    for(int i=0; i<m_listSampleInfo.size(); i++){
        for(int n=0; n<m_listSampleInfo.at(i).m_listTestItem.size(); n++){
            if(m_listSampleInfo[i].m_listTestItem[n].m_nItemTestPos == nReagentPos){
                //获取ID卡数据
                byarrIDCardData = m_listSampleInfo[i].m_listTestItem[n].m_ReagentObj.m_byarrIDCardData;
                //QByteArray byarrIDCardData = m_mapIDCardInfo.value(strIDCardIndex).m_byarrIDCardData;
                //qDebug()<<byarrIDCardData.size()<<byarrIDCardData.toHex().toUpper();
                //结果计算
                m_listSampleInfo[i].m_listTestItem[n].m_byteDrawData = byarrResultData;
                //ResultDataInfo DataObj;
                QString strResult = CalcResult(m_listSampleInfo[i].m_listTestItem[n].m_rawTestInfo,byarrResultData,byarrIDCardData);

                //modify result
                //strResult = RandomResult( m_vecSampleInfo[nSamplePos].m_strSampleID );
                //

                m_listSampleInfo[i].m_listTestItem[n].m_strResult = strResult;
                qDebug()<<"result is:"<<strResult;
                //保存数据到数据库
                SaveResultToDB(m_listSampleInfo.at(i),nReagentPos );
                //
                if(m_settins->GetParam(LISMODE).toInt() == 1){
                    QString strTempResult = strResult.replace(">","");
                    strTempResult = strResult.replace("<","");
                    float fResult = strTempResult.toFloat();
                    if(fResult > 0){
                        SendHL7ResultInfo(m_ConnPCSerial,m_listSampleInfo.at(i).m_strSampleID,
                                          "","","",m_listSampleInfo.at(i).m_listTestItem.at(n).m_strItemName,
                                          strResult,m_listSampleInfo.at(i).m_listTestItem.at(n).m_ReagentObj.m_strReagentUnit);
                    }

                }
                //
                if(m_bConnectPC == true){
                    RawTestInfo ti = m_listSampleInfo.at(i).m_listTestItem.at(n).m_rawTestInfo;
                    QByteArray byteTestData = m_listSampleInfo.at(i).m_listTestItem.at(n).m_byteDrawData;
                    QByteArray byteFill;
                    byteFill.resize(640);
                    byteFill.fill(0);
                    byteTestData.append(byteFill);
                    QByteArray pcData = PackageTestDataToSerial(ti,byteTestData);
                    m_ConnPCSerial->write(pcData);
                }
                //UI显示完成
                m_SampleCircle->SetItemColor(nReagentPos+1,COMPLETESTATUS);
                m_SampleCircle->SetItemText(nReagentPos+1,QObject::tr("完成"));
                //流程已修改为测试完成立马退卡，所以不需要统一退卡了
                //删除本次完成的项目
                m_listSampleInfo[i].m_listTestItem.removeAt(n);
                //如果项目完成，删除此样本信息
                if(m_listSampleInfo[i].m_listTestItem.size() == 0){
                    m_listSampleInfo.removeAt(i);
                }
                //更新已完成测试
                m_nCompleteTestCount++;
                //测试基本信息修改
                //UpdateTestInfoBaseParam(m_nCompleteTestCount);
                return true;
            }
        }
    }
}

void TestInterface::SendHL7ResultInfo(QextSerialPort *SerialHL7, QString strNumberID,
                                      QString strName,QString strAge, QString strSex,
                                      QString strItem,QString strResult, QString strUnit)
{
    QDateTime curTime = QDateTime::currentDateTime();
    QString strCurTime = curTime.toString("yyyyMMddhhmmss");

    m_SimpleHL7.WriteHL7Msg(SerialHL7,strNumberID,strName,strAge,strSex,
                            strItem,strResult,
                            strUnit,strCurTime,"ORU^R01");
}


QByteArray TestInterface::PackageTestDataToSerial(RawTestInfo TestInfo,QByteArray byteTestData)
{
    QByteArray byteDebugDataToSerial;
    QByteArray byteContent;
    byteDebugDataToSerial[0] = (0x5A);
    byteDebugDataToSerial[1] = (0x1A);
    byteDebugDataToSerial[2] = (0xA1);
    byteDebugDataToSerial[3] = (0xA5);
    //卡号
    byteDebugDataToSerial[4] = (0x00);
    byteDebugDataToSerial[5] = (0x00);
    byteDebugDataToSerial[6] = (0x00);
    byteDebugDataToSerial[7] = (0x00);
    //命令-6在文档中定义为测试数据,以图形显示此数据
    byteDebugDataToSerial[8] = (0x00);
    byteDebugDataToSerial[9] = (0x00);
    byteDebugDataToSerial[10] = (0x00);
    byteDebugDataToSerial[11] = (0x06);
    //填充内容
    byteContent.append(byteTestData);

    //测试信息顺序为:
    //面积1，面积2，比值1，面积1，面积2，比值2，结果，计算方案，
    //扫描起始点，质控峰积分长度，放大参数，检测峰积分长度，质控峰高度,
    //测试通道号,波峰位置1,波谷左1,波谷右1,波峰位置2,波谷左2,波谷右2

    //面积1,长度3
    //qDebug()<<(quint8)DataObj.m_RawTestInfo.m_nTest1Area1/65536;
    byteContent.append((quint8)(TestInfo.m_nTest1Area1/65536) );
    byteContent.append((quint8)(TestInfo.m_nTest1Area1/256) );
    byteContent.append((quint8)(TestInfo.m_nTest1Area1%256) );
    //qDebug()<<(quint8)(DataObj.m_RawTestInfo.m_nTest1Area1/65536);
    //qDebug()<<(quint8)(DataObj.m_RawTestInfo.m_nTest1Area1/256);
    //qDebug()<<(quint8)(DataObj.m_RawTestInfo.m_nTest1Area1%256);
    //面积2
    byteContent.append((quint8)(TestInfo.m_nTest1Area2/65536) );
    byteContent.append((quint8)(TestInfo.m_nTest1Area2/256) );
    byteContent.append((quint8)(TestInfo.m_nTest1Area2%256) );
    //比值1
    byteContent.append((quint8)(TestInfo.m_nTest1Ratio/65536) );
    byteContent.append((quint8)(TestInfo.m_nTest1Ratio/256) );
    byteContent.append((quint8)(TestInfo.m_nTest1Ratio%256) );
    //byteContent.append(ValueToHex(QString::number(TestInfo.m_nTest1Ratio),4,2));
    //面积1
    byteContent.append( static_cast<char>(0) );
    byteContent.append( static_cast<char>(0) );
    byteContent.append( static_cast<char>(0) );
    //面积2
    byteContent.append( static_cast<char>(0) );
    byteContent.append( static_cast<char>(0) );
    byteContent.append( static_cast<char>(0) );
    //比值2
    byteContent.append( static_cast<char>(0) );
    byteContent.append( static_cast<char>(0) );
    byteContent.append( static_cast<char>(0) );
    //结果,长度4
    byteContent.append(ValueToHex(QString::number(TestInfo.m_fTestResult),4,2));
    //计算方案,长度1
    byteContent.append((quint8)(TestInfo.m_nComputeMothed) );
    //扫描起始点
    byteContent.append((quint8)(TestInfo.m_nScanStart) );
    //质控峰积分长度
    byteContent.append((quint8)(TestInfo.m_nQCIntegralBreadth) );
    //放大参数
    byteContent.append((quint8)(TestInfo.m_nAmpParam) );
    //检测峰积分长度
    byteContent.append((quint8)(TestInfo.m_nCheckIntegralBreadt) );
    //质控峰高度
    byteContent.append((quint8)(TestInfo.m_nQCMinHeightValue) );
    //测试通道号
    byteContent.append(static_cast<char>(0));
    //波峰位置1
    byteContent.append((quint8)(TestInfo.m_nCrestPos1/256) );
    byteContent.append((quint8)(TestInfo.m_nCrestPos1%256) );
    //波谷左1
    byteContent.append((quint8)(TestInfo.m_nTroughPosLeft1/256) );
    byteContent.append((quint8)(TestInfo.m_nTroughPosLeft1%256) );
    //波谷右1
    byteContent.append((quint8)(TestInfo.m_nTroughPosRight1/256) );
    byteContent.append((quint8)(TestInfo.m_nTroughPosRight1%256) );
    //波峰位置2
    byteContent.append((quint8)(TestInfo.m_nCrestPos2/256) );
    byteContent.append((quint8)(TestInfo.m_nCrestPos2%256) );
    //波谷左2
    byteContent.append((quint8)(TestInfo.m_nTroughPosLeft2/256) );
    byteContent.append((quint8)(TestInfo.m_nTroughPosLeft2%256) );
    //波谷右2
    byteContent.append((quint8)(TestInfo.m_nTroughPosRight2/256) );
    byteContent.append((quint8)(TestInfo.m_nTroughPosRight2%256) );
    //长度
    byteDebugDataToSerial[12] = (quint8)(byteContent.size()/65536*256);
    byteDebugDataToSerial[13] = (quint8)(byteContent.size()/65536);
    byteDebugDataToSerial[14] = (quint8)(byteContent.size()/256);
    byteDebugDataToSerial[15] = (quint8)(byteContent.size()%256);

    byteDebugDataToSerial.append(byteContent);
    //校验数据
    qint32 nCheckSum,nTempTotal,nTemp;
    nTempTotal = 0;
    nTemp = 0;
    qint32 nCurrentLen = byteDebugDataToSerial.size();
    for(int n=12; n<nCurrentLen; n++){
        nTemp = (quint8)byteDebugDataToSerial.at(n);
        nTempTotal += nTemp;
    }
    nCheckSum = ~nTempTotal + 1;
    quint8 nCheck1 = (nCheckSum>>24);
    quint8 nCheck2 = (nCheckSum>>16);
    quint8 nCheck3 = (nCheckSum>>8);
    quint8 nCheck4 = (nCheckSum%(256));
    byteDebugDataToSerial.append(nCheck1);
    byteDebugDataToSerial.append(nCheck2);
    byteDebugDataToSerial.append(nCheck3);
    byteDebugDataToSerial.append(nCheck4);

    byteDebugDataToSerial.append(0xA5);
    byteDebugDataToSerial.append(0xA1);
    byteDebugDataToSerial.append(0x1A);
    byteDebugDataToSerial.append(0x5A);

    return byteDebugDataToSerial;
}

/********************************************************
 *@Name:        ValueToHex
 *@Author:      HuaT
 *@Description: 将指定值转换为十六进制
 *@Param1:      需要转换的字符串数值
 *@Param2:      十六进制位数
 *@Param3:      小数点位数
 *@Return:      无
 *@Version:     1.0
 *@Date:        2017-3-27
********************************************************/
QByteArray TestInterface::ValueToHex(QString strValue, quint8 nHexTotal, quint8 nDecimal){
    QByteArray byteResult;
    qint32 nInteger,nDecimalPoint;
    qint16  nFindDecimalPoint;
    quint8  nBit1,nBit2,nBit3,nBit4,nBit5;
    qint16  nSymbol;
    double  fTemp1,fTemp2;
    char cZero = 0x00;
    switch(nHexTotal){
    case 1:{
        nInteger = strValue.toInt();
        byteResult.append((quint8)nInteger);
        break;
    }
    case 2:{
        nFindDecimalPoint = strValue.indexOf(".");
        switch(nDecimal){
        //没有小数点位
        case 0:
            nBit1 = strValue.toInt()/256;
            nBit2 = strValue.toInt()%256;
            byteResult.append(nBit1);
            byteResult.append(nBit2);
            break;
        //一个字节的小数点位
        case 1:
            if(-1 == nFindDecimalPoint){
                nBit1 = strValue.toInt()/256;
                nBit2 = strValue.toInt()%256;
                byteResult.append(nBit1);
                byteResult.append(nBit2);
            }else{
                nInteger = strValue.left(nFindDecimalPoint).toInt();
                fTemp1 = (strValue.toDouble()*100);
                nDecimalPoint = ((quint32)fTemp1)%100;
                byteResult.append(nInteger);
                byteResult.append(nDecimalPoint);
            }
            break;
        }

        break;
    }
    case 3:{//总个三个字节
        nFindDecimalPoint = strValue.indexOf(".");
        switch(nDecimal){
        case 0://没有小数字节
            nBit1 = strValue.toInt()/65536;
            nBit2 = strValue.toInt()/256;
            nBit3 = strValue.toInt()%256;
            byteResult.append(nBit1);
            byteResult.append(nBit2);
            byteResult.append(nBit3);
            break;
        case 1://一个小数字节
            if(-1 == nFindDecimalPoint){
                nBit1 = strValue.toInt()/256;
                nBit2 = strValue.toInt()%256;
                byteResult.append(nBit1);
                byteResult.append(nBit2);
                byteResult.append(cZero);
            }else{
                nInteger = strValue.left(nFindDecimalPoint).toInt();
                fTemp1 = (strValue.toDouble()*100);
                nDecimalPoint = ((quint32)fTemp1)%100;
                nBit1 = nInteger/256;
                nBit2 = nInteger%256;
                byteResult.append(nBit1);
                byteResult.append(nBit2);
                byteResult.append(nDecimalPoint);
            }
            break;
        case 2://二个小数字节
            if(-1 == nFindDecimalPoint){
                nBit1 = strValue.toInt();
                byteResult.append(nBit1);
                byteResult.append(cZero);
                byteResult.append(cZero);
            }else{
                nInteger = strValue.left(nFindDecimalPoint).toInt();
                //nDecimalPoint = strValue.mid(nFindDecimalPoint+1).toInt();
                nBit1 = nInteger;
                fTemp1 = (strValue.toDouble()*100);
                fTemp2 = (strValue.toDouble()*10000);
                nBit2 = ((quint32)fTemp1)%100;
                nBit3 = ((quint32)fTemp2)%100;
                byteResult.append(nBit1);
                byteResult.append(nBit2);
                byteResult.append(nBit3);
            }
            break;
        }
        break;
    }
    case 4:{//总共4个字节
        nFindDecimalPoint = strValue.indexOf(".");
        switch(nDecimal){
        case 0://没有小数点位
            nBit2 = strValue.toInt()/65536*256;
            nBit2 = strValue.toInt()/65536;
            nBit3 = strValue.toInt()/256;
            nBit4 = strValue.toInt()%256;
            byteResult.append(nBit1);
            byteResult.append(nBit2);
            byteResult.append(nBit3);
            byteResult.append(nBit4);
            break;
        case 1://一个小数点位
            if(-1 == nFindDecimalPoint){
                nBit1 = strValue.toInt()/65536;
                nBit2 = strValue.toInt()/256;
                nBit3 = strValue.toInt()%256;
                byteResult.append(nBit1);
                byteResult.append(nBit2);
                byteResult.append(nBit3);
                byteResult.append(cZero);
            }else{
                nInteger = strValue.left(nFindDecimalPoint).toInt();
                fTemp1 = (strValue.toDouble()*100);
                nDecimalPoint = ((quint32)fTemp1)%100;
                nBit1 = nInteger/65536;
                nBit2 = nInteger/256;
                nBit3 = nInteger%256;
                byteResult.append(nBit1);
                byteResult.append(nBit2);
                byteResult.append(nBit3);
                byteResult.append(nDecimalPoint);
            }
            break;
        case 2://2个小数点位
            if(-1 == nFindDecimalPoint){
                nBit1 = strValue.toInt()/256;
                nBit2 = strValue.toInt()%256;
                byteResult.append(nBit1);
                byteResult.append(nBit2);
                byteResult.append(cZero);
                byteResult.append(cZero);
            }else{
                nInteger = strValue.left(nFindDecimalPoint).toInt();
                //nDecimalPoint = strValue.mid(nFindDecimalPoint+1).toInt();
                nBit1 = nInteger/256;
                nBit2 = nInteger%256;
                fTemp1 = (strValue.toDouble()*100);
                fTemp2 = (strValue.toDouble()*10000);
                nBit3 = ((quint32)fTemp1)%100;
                nBit4 = ((quint32)fTemp2)%100;
                byteResult.append(nBit1);
                byteResult.append(nBit2);
                byteResult.append(nBit3);
                byteResult.append(nBit4);
            }
            break;
        }
        break;
    }
    case 5:{//总共5个字节
        nFindDecimalPoint = strValue.indexOf(".");
        nSymbol = strValue.indexOf("-");
        if(-1 == nSymbol){
            byteResult.append("+");
        }else{
            byteResult.append("-");
        }
        switch(nDecimal){
        case 0://没有小数点位
            nBit1 = qAbs( strValue.toInt() )/65536*256*256;
            nBit2 = qAbs( strValue.toInt() )/65536*256;
            nBit3 = qAbs( strValue.toInt() )/256*256;
            nBit4 = qAbs( strValue.toInt() )/256;
            nBit5 = qAbs( strValue.toInt() )%256;
            byteResult.append(nBit1);
            byteResult.append(nBit2);
            byteResult.append(nBit3);
            byteResult.append(nBit4);
            byteResult.append(nBit5);
            break;
        case 1://1个小数点位
            if(-1 == nFindDecimalPoint){
                nBit1 = qAbs( strValue.toInt() )/65536*256;
                nBit2 = qAbs( strValue.toInt() )/256*256;
                nBit3 = qAbs( strValue.toInt() )/256;
                nBit4 = qAbs( strValue.toInt() )%256;
                byteResult.append(nBit1);
                byteResult.append(nBit2);
                byteResult.append(nBit3);
                byteResult.append(nBit4);
                byteResult.append(cZero);
            }else{
                nInteger = qAbs( strValue.left(nFindDecimalPoint).toInt() );
                fTemp1 = (qAbs(strValue.toDouble())*100);
                nDecimalPoint = ((quint32)fTemp1)%100;
                nBit1 = nInteger/65536*256;
                nBit2 = nInteger/65536;
                nBit3 = nInteger/256;
                nBit4 = nInteger%256;
                byteResult.append(nBit1);
                byteResult.append(nBit2);
                byteResult.append(nBit3);
                byteResult.append(nBit4);
                byteResult.append(nDecimalPoint);
            }
            break;
        case 2://2个小数点位
            if(-1 == nFindDecimalPoint){
                nBit1 = qAbs( strValue.toInt() )/65536;
                nBit2 = qAbs( strValue.toInt() )/256;
                nBit3 = qAbs( strValue.toInt() )%256;
                byteResult.append(nBit1);
                byteResult.append(nBit2);
                byteResult.append(nBit3);
                byteResult.append(cZero);
                byteResult.append(cZero);
            }else{
                nInteger = qAbs(strValue.left(nFindDecimalPoint).toInt());
                //nDecimalPoint = qAbs(strValue.mid(nFindDecimalPoint+1).toInt());
                nBit1 = nInteger/65536;
                nBit2 = nInteger/256;
                nBit3 = nInteger%256;
                fTemp1 = (qAbs(strValue.toDouble())*100);
                fTemp2 = (qAbs(strValue.toDouble())*10000);
                nBit4 = ((quint32)fTemp1)%100;
                nBit5 = ((quint32)fTemp2)%100;
                byteResult.append(nBit1);
                byteResult.append(nBit2);
                byteResult.append(nBit3);
                byteResult.append(nBit4);
                byteResult.append(nBit5);
            }
            break;
        }
        break;
    }

    }
    return byteResult;
}

QString TestInterface::CalcResult(RawTestInfo &TestInfoObj, QByteArray byarrResultData, QByteArray byarrIDCardData)
{
    //bNewCard = false;
    QVector<int> vectorRawPoint;
    int nIndex;
    for(int n=0; n<byarrResultData.size(); n+=2){
        nIndex = (quint8)byarrResultData.at(n) + (quint8)byarrResultData.at(n+1)*256;
        vectorRawPoint.append(nIndex);
        //qDebug() << vectorRawPoint;
    }
    if(vectorRawPoint.size() != 320){
        qDebug()<<"Calc Result: Failed";
        //return ;
    }
    unsigned int ScanData1[320];
    //unsigned int ScanData2[320];
    for(int i=0; i<320; i++){
        ScanData1[i] = vectorRawPoint.at(i);
    }
    //数据滤波
    m_ResultCalc.low_passfilter(ScanData1);
    //m_ResultCalc.low_passfilter(ScanData2);
    //用ID卡数据计算结果
    unsigned char* pRecord = new unsigned char[50];
    //unsigned char *cIDMessage = m_ResultCalc.ConvetIDCardToChar(byarrIDCardData);
    unsigned char *cIDMessage = (unsigned char*)byarrIDCardData.data();
    memset(pRecord,0,50);
    //判断计算方案
    int nCalcNum = cIDMessage[23];
    if(m_bDebugMode == true){
        QString strCalcMethod = m_settins->GetParam(DEBUGCALCMETHOD);
        if(!strCalcMethod.isEmpty()){
            nCalcNum = m_settins->GetParam(DEBUGCALCMETHOD).toInt();
        }
    }
    TestInfoObj.m_nComputeMothed = nCalcNum;
    qDebug()<<"calc mothed"<<nCalcNum;
    QString strResult;
    float fResultRatioCoeff,fResultRatioConst;
    fResultRatioCoeff = m_settins->GetParam(RESULTRATIOCOEFF).toFloat();
    fResultRatioConst = m_settins->GetParam(RESULTRATIOCONSTA).toFloat();
    if(fResultRatioCoeff == 0 && fResultRatioConst == 0){
        strResult = m_ResultCalc.calculateResult2(TestInfoObj,ScanData1,pRecord,cIDMessage);
    }else{
        strResult = m_ResultCalc.calculateResult3(TestInfoObj,ScanData1,pRecord,cIDMessage,fResultRatioCoeff,fResultRatioConst);
    }

    //m_ResultCalc.calculateResult(ScanData1,ScanData2,pRecord,cIDMessage);
    //delete [] cIDMessage;
    return strResult;
}


/********************************************************
 *@Name:        SaveResultToDB
 *@Author:      HuaT
 *@Description: 保存结果至数据库
 *@Param1:      测试信息结构体
 *@Param2:      此项目在试剂盘上的测试位置
 *@Return:      保存数据是否成功
 *@Version:     1.0
 *@Date:        2018-6-16
********************************************************/
bool TestInterface::SaveResultToDB(SampleObj so,quint32 nTestPos)
{
    QString strCurDate = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    for(int n=0; n<so.m_listTestItem.count(); n++){
        if(so.m_listTestItem.at(n).m_nItemTestPos == nTestPos){
            bool bPatient,bTestInfo;
            //病人信息
            QString strDrawData = so.m_listTestItem.at(n).m_byteDrawData.toHex();
            QString strSQL = QString("insert into patient values(null,%1,'',0,0,'%2','%3','%4','%5','','')")
                    .arg(so.m_strSampleID.toLongLong())
                    .arg(so.m_listTestItem.at(n).m_strItemName)
                    .arg(so.m_listTestItem.at(n).m_strResult)
                    .arg(so.m_listTestItem.at(n).m_ReagentObj.m_strReagentUnit)
                    .arg(strCurDate);
            //qDebug()<<strSQL;
            bPatient = m_Devdb->Exec(strSQL);
            //检测信息
            strSQL = QString("insert into testinfo values(null,%1,'%2','%3','%4','%5',%6,%7,%8,%9,"
                             "%10,%11,%12,%13,%14,%15,%16,'%17','%18')")
                    .arg( so.m_strSampleID.toULongLong() )
                    .arg( QString::number(so.m_listTestItem.at(n).m_rawTestInfo.m_nTest1Area1) )
                    .arg( QString::number(so.m_listTestItem.at(n).m_rawTestInfo.m_nTest1Area2) )
                    .arg( QString::number(so.m_listTestItem.at(n).m_rawTestInfo.m_nTest1Ratio) )
                    .arg( so.m_listTestItem.at(n).m_strResult )
                    .arg( so.m_listTestItem.at(n).m_rawTestInfo.m_nComputeMothed )
                    .arg( so.m_listTestItem.at(n).m_rawTestInfo.m_nScanStart )
                    .arg( so.m_listTestItem.at(n).m_rawTestInfo.m_nQCIntegralBreadth )
                    .arg( so.m_listTestItem.at(n).m_rawTestInfo.m_nAmpParam)
                    .arg( so.m_listTestItem.at(n).m_rawTestInfo.m_nCheckIntegralBreadt)
                    .arg( so.m_listTestItem.at(n).m_rawTestInfo.m_nCrestPos1)
                    .arg( so.m_listTestItem.at(n).m_rawTestInfo.m_nTroughPosLeft1)
                    .arg( so.m_listTestItem.at(n).m_rawTestInfo.m_nTroughPosRight1)
                    .arg( so.m_listTestItem.at(n).m_rawTestInfo.m_nCrestPos2)
                    .arg( so.m_listTestItem.at(n).m_rawTestInfo.m_nTroughPosLeft2)
                    .arg( so.m_listTestItem.at(n).m_rawTestInfo.m_nTroughPosRight2)
                    .arg( strCurDate)
                    .arg( strDrawData);
            bTestInfo = m_Devdb->Exec(strSQL);
            return bPatient && bTestInfo;
        }
    }
    return false;
}

void TestInterface::UpdateTestInfoBaseParam(quint8 nCompeletCount)
{
    //测试基本信息修改
    /*int nTestCount = ui->lb_Test_SampleCount->text().toInt();
    if(nTestCount == 0){
        ui->lb_Test_WaitTestNumValue->setText("0");
        //ui->lb_Test_CompTestNumValue->setText("0");
    }else{
        int nWaitTestCount = GetWaitTestCount();
        ui->lb_Test_WaitTestNumValue->setText(QString::number(nWaitTestCount));
        ui->lb_Test_CompTestNumValue->setText(QString::number(nCompeletCount));
    }*/
}

/********************************************************
 *@Name:        GetWaitTestCount
 *@Author:      HuaT
 *@Description: 获取当前等待测试数量
 *@Param1:      无
 *@Return:      无
 *@Version:     1.0
 *@Date:        2019-3-25
********************************************************/
int TestInterface::GetWaitTestCount()
{
    int nWaitTestCount = 0;
    for(int n=0; n<m_listSampleInfo.size(); n++){
        nWaitTestCount = nWaitTestCount + m_listSampleInfo.at(n).m_listTestItem.size();
    }
    return nWaitTestCount;
}

/********************************************************
 *@Name:        StartUnstallCard
 *@Author:      HuaT
 *@Description: 开始根据试剂数量发送卸卡命令
 *@Param1:      测试结果数据，里面有样本位置和测试位置,前4个为样本位置,接着为测试位置
 *@Return:      无
 *@Version:     1.0
 *@Date:        2018-5-21
********************************************************/
void TestInterface::StartUnstallCard(const QByteArray &AckData)
{
    //测试位
    quint32 nReagentPos = BytesToInt(AckData.mid(4,4));
    //打包测试位
    QByteArray byarrReagentPos = IntToBytes(nReagentPos);
    //发送准备卸卡命令
    SendSerialData(m_ControlPanelsSerial,PL_FUN_AM_PREPARE_UNSTALL,byarrReagentPos);
}

/********************************************************
 *@Name:        AllTestComplete
 *@Author:      HuaT
 *@Description: 所有测试完成后，清空结构体，恢复界面新建
 *@Param1:      无
 *@Return:      无
 *@Version:     1.0
 *@Date:        2018-5-21
********************************************************/
void TestInterface::AllTestComplete()
{
    //恢复界面
    //急诊插入
    ui->pb_Test_InsertEmer->setEnabled(false);
    //新建测试和开始测试可用
    ui->pb_Test_NewTest->setEnabled(true);
    ui->pb_Test_StartTest->setEnabled(true);
    //详细信息界面按钮可用
    SetRuntimeButtonEnable(true);
    //初始化列表信息
    InitTableInfo();
    //初始化圆的颜色
    DefaultCircleUI();
    //样本总数置0
    ui->lb_Test_SampleCount->setText("0");
    //基本信息初始化
    //ui->lb_Test_WaitTestNumValue->setText("0");
    //ui->lb_Test_CompTestNumValue->setText("0");
    //测试项目名称清空
    ui->lb_Test_TestProject->setText("");
    //加样状态关闭
    m_bAddSampleStatus = false;
    //检测状态关闭
    m_bCheckStatus = false;
    //测试完成
    ShowLogText(ui->tb_Test_RunInfoShow,ENDTEST,LOGNOTE2);

}

/********************************************************
 *@Name:        AllAddSampleComplete
 *@Author:      HuaT
 *@Description: 所有加样测试完成后，可以打开新建测试按钮,允许新一轮测试
 *@Param1:      无
 *@Return:      无
 *@Version:     1.0
 *@Date:        2018-5-21
********************************************************/
void TestInterface::AllAddSampleComplete()
{
    //恢复界面
    //急诊插入
    ui->pb_Test_InsertEmer->setEnabled(false);
    //新建测试和开始测试可用
    ui->pb_Test_NewTest->setEnabled(true);
    ui->pb_Test_StartTest->setEnabled(true);
    //详细信息界面按钮可用
    SetRuntimeButtonEnable(true);
    //初始化列表信息
    InitTableInfo();
    //测试项目名称清空
    ui->lb_Test_TestProject->setText("");
}

/********************************************************
 *@Name:        ParseIDCardData
 *@Author:      HuaT
 *@Description: 解析发来的ID卡数据0-3
 *@Param1:      接收到的ID卡数据
 *@Return:      无
 *@Version:     1.0
 *@Date:        2018-6-16
********************************************************/
void TestInterface::ParseIDCardData(const QByteArray &AckData)
{
    if(AckData.size() != 264){
        qDebug()<<QObject::tr("ID卡数据长度不符：")<<AckData.size();
        return;
    }
    quint32 nReagentPos = BytesToInt(AckData.mid(0,4));
    qDebug()<<"nReagentPos:"<<nReagentPos;
    IDCardInfo ci;
    //数据
    ci.m_byarrIDCardData = AckData.right(AckData.size() - 4);
    //测试项目
    QByteArray byarrTestItem = AckData.mid(50,20);
    ci.m_strTestItem = QTextCodec::codecForName("GBK")->toUnicode(byarrTestItem);
    ci.m_strTestItem = ci.m_strTestItem.trimmed();
    quint8 nRemaining = (quint8)AckData.at(45);
    ci.m_nRemaining = nRemaining;
    ci.m_strTestUnit = ci.m_byarrIDCardData.mid(66,10).trimmed();
    qDebug()<<"nRemaining:"<<nRemaining;
    m_mapIDCardInfo[QString::number(nReagentPos)] = ci;
    m_mapTestItemPos.insert(ci.m_strTestItem,QString::number(nReagentPos));
    //检测此项目的血液类型是否存在
    QString strSql = QString("select itemname from SampleType where itemname='%1'").arg(ci.m_strTestItem);
    QStringList listCount = m_Devdb->ExecQuery(strSql);
    //类型不存在
    if(listCount.size() == 0){
        //插入数据库，默认血清-0
        strSql = QString("insert into sampletype values(null,'%1','0')").arg(ci.m_strTestItem);
        bool bRet = m_Devdb->Exec(strSql);
        if(bRet == false){
            qDebug()<<"insert into sampletype is failed";
        }
        //更新样本数据内容
        m_mapSampleType[ci.m_strTestItem] = "0";
        //更新设置界面加样样本列表框数据
        emit sendSampleTypeAddSignals();
    }
    //更新ID卡界面参数信息
    UpdateIDCardInfo(nReagentPos,nRemaining,ci.m_strTestItem);
    //更新测试按钮
    InitValidTestItem();
    if(nRemaining == 0){
        QString strNote,strContain;
        strNote = QObject::tr("Note");
        strContain = QObject::tr("卡仓%1的试剂卡数量为0,请更换试剂卡盒!").arg(nReagentPos+1);
        MsgBoxDlg* msgbox = new MsgBoxDlg;
        msgbox->setAttribute(Qt::WA_ShowModal,true);
        msgbox->ShowMsg(strContain,0);
        msgbox->show();
        return;
    }

}

/********************************************************
 *@Name:        GetIDCardPosData
 *@Author:      HuaT
 *@Description: 返回ID卡中，指定位置的数据内容
 *@Param1:      试剂卡数据
 *@Param2:      需要的数据位置
 *@Param3:      数据结果
 *@Return:      获取数据是否成功
 *@Version:     1.0
 *@Date:        2018-5-21
********************************************************/
bool TestInterface::GetIDCardPosData(QByteArray byarrIDCardData, quint16 DataPos, quint8& nResult)
{
    nResult = (quint8)byarrIDCardData.at(DataPos);
    return true;
}

bool TestInterface::GetIDCardPosData(QString strIDCardIndex, QByteArray &byarrData)
{
    byarrData = m_mapIDCardInfo.value( strIDCardIndex ).m_byarrIDCardData;
    if(byarrData.isEmpty()){
        return false;
    }else{
        return true;
    }
}


/********************************************************
 *@Name:        RecvCheckPanelsData
 *@Author:      HuaT
 *@Description: 接收仪器检测板串口数据
 *@Param1:      无
 *@Return:      无
 *@Version:     1.0
 *@Date:        2018-5-21
********************************************************/
void TestInterface::RecvCheckPanelsData()
{
    //qDebug()<<m_DeviceSerial.readAll().toHex();
    QByteArray byarrData = m_CheckPanelsSerial->readAll();
    //qDebug()<<byarrData.right(4).toHex().toUInt(0,16);
    m_byarrCheckData.append(byarrData);
    //检测是否为完整一帧数据
    //qDebug()<<m_byarrCheckData.toHex().toUpper();
    if(!ParseSerialData(m_byarrCheckData)){
        return;
    }
    //qDebug()<<"RecvCheckPanelsData:"<<m_byarrCheckData.toHex().toUpper()<<m_byarrCheckData.size();
    //填充数据
    MsgData CurrData;
    //一帧里面的总数据条数
    int nCountFrame = m_byarrCheckData.count(IntToBytes( CurrData.m_frameHeader ));
    //一帧数据
    QByteArray byarrSingleData;
    //查找帧起始点
    quint16 nStartIndex = 0;
    //查找帧结束点
    quint16 nEndIndex = 0;
    for(int n=0 ;n<nCountFrame; n++){
        //qDebug()<<IntToBytes( CurrData.m_frameEnd).toHex().toUpper();
        nStartIndex = m_byarrCheckData.indexOf( IntToBytes( CurrData.m_frameHeader) ,   nStartIndex);
        nEndIndex   = m_byarrCheckData.indexOf( IntToBytes( CurrData.m_frameEnd) ,nEndIndex);
        byarrSingleData = m_byarrCheckData.mid(nStartIndex, nEndIndex+4 - nStartIndex);
        //qDebug()<<"byarrSingleData:"<<byarrSingleData.toHex().toUpper()<<byarrSingleData.size();
        if(FillValidSerialData(byarrSingleData,CurrData)){
            //qDebug()<<"RecvCheckPanelsData:"<<CurrData.m_FuncIndex;
            DebugOutput(CurrData.m_FuncIndex,CurrData.m_Data,"recvCheck");
            //MsgData msgd;
            switch(CurrData.m_FuncIndex){
            case PL_FUN_ACK:
                //返回ACK
                ParseCheckPanelsAck(CurrData.m_Data);
                break;
            case PL_FUN_ALARM:
                //仪器报警
                break;
                /*case PL_FUN_AM_HARDWEAR_CHECK:
                        qDebug()<<"PL_FUN_AM_HARDWEAR_CHECK";
                        msgd.Clear();
                        msgd.m_FuncIndex = PL_FUN_AM_HARDWEAR_CHECK;
                        msgd.m_DataLength = 4;
                        break;*/
            case PL_FUN_AM_TEST_RESULT :
                //收到测试结果
                qDebug()<<"PL_FUN_AM_TEST_RESULT ";
                ParseTestResultData(CurrData.m_Data);
                //收到结果后，准备卸载试剂卡
                StartUnstallCard(CurrData.m_Data);
                break;
            case PL_FUN_AM_START_UNSTALL_COMPLETE:{
                //卸卡完成
                quint32 nCardNum = BytesToInt(CurrData.m_Data);
                if(m_bIsCheckStatus){//如果是自检状态
                    //解析卸卡的数据
                    ParseUnstallCardData();
                    m_SampleCircle->SetItemColor(nCardNum+1,NORMALSTATUS);
                    m_SampleCircle->SetItemText(nCardNum+1,QString::number(nCardNum+1));
                }else{//如果是测试状态
                    //当前卸卡完成
                    QString strLog;
                    strLog = QString(EXITCARDCOMPLETE).arg(nCardNum+1);
                    for(int n=0; n<m_listReagentPos.size(); n++){
                        if(m_listReagentPos.at(n)>nCardNum){
                            m_listReagentPos.insert(n,nCardNum);
                            break;
                        }
                    }
                    ShowLogText(ui->tb_Test_RunInfoShow,strLog,LOGNORMAL);
                    //卸卡完成,检测是否还有测试等待完成，没有则本轮测试完成
                    bool bComplete = CheckTestComplete();
                    if(bComplete){
                        ShowLogText(ui->tb_Test_RunInfoShow,ENDTEST,LOGNOTE2);
                        AllTestComplete();
                    }
                    //检测状态关闭
                    m_bCheckStatus = false;
                    //如果等待检测队列为空
                    if(m_listWaitTestQueue.isEmpty()){
                        if(m_listWaitAddSampleQueue.isEmpty()){
                            break;
                        }else{
                            QString strLog = QString(RUNADDSAMPLE).arg(m_listWaitAddSampleQueue.front().m_nSamplePos+1);
                            ShowLogText(ui->tb_Test_RunInfoShow,strLog,LOGNORMAL);
                            QByteArray byarrTestNumbleData;
                            byarrTestNumbleData = GetWaitAddSampleInfoToQueue();
                            if(!byarrTestNumbleData.isEmpty()){
                                //发送测试信息-开始加样
                                SendSerialData(m_ControlPanelsSerial,PL_FUN_AM_START_SAMPLE,byarrTestNumbleData);
                                //开始加样状态开启
                                m_bAddSampleStatus = true;
                            }
                        }
                    }else{//否则
                        QString strLog = QString(RUNCHECKING).arg(m_listWaitTestQueue.front().m_nSamplePos);
                        ShowLogText(ui->tb_Test_RunInfoShow,strLog,LOGNORMAL);
                        QByteArray byarrData = GetWaitCheckInfoToQueue();
                        if(!byarrData.isEmpty()){
                            int nSamplePos = BytesToInt(byarrData.mid(0,4));
                            int nReagentPos = BytesToInt(byarrData.mid(4,4));
                            for(int n=0; n<m_listSampleInfo.size(); n++){
                                if(m_listSampleInfo.at(n).m_nSamplePos == nSamplePos){
                                    for(int i=0; i<m_listSampleInfo.at(n).m_listTestItem.size(); i++){
                                        if(m_listSampleInfo.at(n).m_listTestItem.at(i).m_nItemTestPos == nReagentPos){
                                            m_listSampleInfo[n].m_listTestItem[i].m_bResultWaitState = true;
                                            break;
                                        }
                                    }
                                }
                            }
                            qDebug()<<"卸卡完成接连测试,时间到,发送准备测试命令:nSamplePos->"<<nSamplePos
                                   <<",nReagentPos->"<<nReagentPos;
                            SendSerialData(m_ControlPanelsSerial,PL_FUN_AM_PREPARE_TEST,byarrData);
                            //检测状态开启
                            m_bCheckStatus = true;
                        }
                    }
                }
                break;
            }
            case PL_FUN_AM_ID_DEVICE_SCANOFFSET_COMPLETE:
                emit sendDebugHardware2ScanOffsetData(CurrData.m_Data);
                break;
            default:
                qDebug()<<"command is invalid!";
            }
        }
        CurrData.Clear();
        nStartIndex = nStartIndex + 1;
        nEndIndex = (nEndIndex+4 - nStartIndex) + 1;
    }
    m_byarrCheckData.clear();

}

void TestInterface::RecvControlPanelsData()
{
    //qDebug()<<m_DeviceSerial.readAll().toHex();
    QByteArray byarrData = m_ControlPanelsSerial->readAll();
    //qDebug()<<byarrData.right(4).toHex().toUInt(0,16);
    m_byarrControlData.append(byarrData);
    //检测是否为完整一帧数据
    //qDebug()<<"RecvControlPanelsData:"<<m_byarrControlData.toHex().toUpper();
    if(!ParseSerialData(m_byarrControlData)){
        return;
    }
    qDebug()<<QObject::tr("RecvControlPanelsData:")<<m_byarrControlData.toHex().toUpper();
    //填充数据
    MsgData CurrData;
    //一帧里面的总数据条数
    int nCountFrame = m_byarrControlData.count(IntToBytes( CurrData.m_frameEnd ));
    //一帧数据
    QByteArray byarrSingleData;
    //查找帧起始点
    quint16 nStartIndex = 0;
    //查找帧结束点
    quint16 nEndIndex = 0;
    for(int n=0 ;n<nCountFrame; n++){
        //qDebug()<<"IntToBytes(CurrData.m_frameHeader):"<<IntToBytes(CurrData.m_frameHeader).toHex().toUpper();
        //CurrData.m_frameHeader:A5A11A5A
        nStartIndex = m_byarrControlData.indexOf( IntToBytes(CurrData.m_frameHeader) ,nStartIndex);
        nEndIndex = m_byarrControlData.indexOf( IntToBytes( CurrData.m_frameEnd) ,nEndIndex);
        byarrSingleData = m_byarrControlData.mid(nStartIndex, nEndIndex+4 - nStartIndex);
        if(FillValidSerialData(byarrSingleData,CurrData)){
            //qDebug()<<"RecvControlPanelsData:"<<m_byarrControlData.toHex().toUpper();
            //debug output
            DebugOutput(CurrData.m_FuncIndex,CurrData.m_Data,"recvControl");
            //MsgData msgd;
            switch(CurrData.m_FuncIndex){
            case PL_FUN_ACK:
                //返回ACK
                ParseControlPanelsAck(CurrData.m_Data);
                break;
            case PL_FUN_ALARM:
                //仪器报警
                ParseControlPanelsAlarmData(CurrData.m_Data);
                break;
                /*case PL_FUN_AM_HARDWEAR_CHECK:
                    qDebug()<<"PL_FUN_AM_HARDWEAR_CHECK";
                    msgd.Clear();
                    msgd.m_FuncIndex = PL_FUN_AM_HARDWEAR_CHECK;
                    msgd.m_DataLength = 4;
                    break;*/
            case PL_FUN_AM_SCAN_RESULT:
                //收到扫码结果
                ParseScanBarCodeInfo(CurrData.m_Data);
                //ShowBarCodeInfo(m_listSampleInfo);
                break;
            case PL_FUN_AM_SAMPLE_COMPLETE:
                //收到加样完成，进行倒计时
                ProceStartLoadSample(CurrData.m_Data);
                break;
            case PL_FUN_AM_PREPARE_TEST_COMPLETE:
            {
                //收到准备测试完成，发送开始测试
                PorceLoadSampleComplete(CurrData.m_Data);
                break;
            }
            case PL_FUN_AM_PREPARE_UNSTALL_COMPLETE:
            {
                //卸卡准备完成，开始卸卡
                QByteArray byarrUnstallData = IntToBytes(BytesToInt(CurrData.m_Data.mid(0,4)));
                SendSerialData(m_CheckPanelsSerial,PL_FUN_AM_START_UNSTALL,byarrUnstallData);
                break;
            }
            case PL_FUN_AM_ID_CARD_DATA:
                //收到ID卡数据
                ParseIDCardData(CurrData.m_Data);
                break;
            case PL_FUN_AM_ID_EMER_SCANRESULT:
                //qDebug()<<CurrData.m_Data.size();
                ParseScanBarCodeInfo(CurrData.m_Data,true);
                ShowBarCodeInfo(m_listSampleInfo);
                break;
            case PL_FUN_AM_ID_DEBUG_LOADINGARMRCEV:
                emit sendDebugHardwareArmData(CurrData.m_Data);
                break;
            case PL_FUN_AM_ID_DEBUG_PUSHMOTORRECV:
                emit sendDebugHardwarePushMotorData(CurrData.m_Data);
                break;
            case PL_FUN_AM_ID_DEBUG_DIALPARAMRECV:
                emit sendDebugHardwareDialData(CurrData.m_Data);
                break;
            case PL_FUN_AM_ID_DEVICE_HARDVERSION:
                emit sendHardwareVersionData(CurrData.m_Data);
                break;
            case PL_FUN_AM_ID_DEVICE_HARDSHUTDOWN:
            {
                QString strContent;
                strContent = QObject::tr("您确定关闭仪器吗");
                MsgBoxDlg msgbox;
                msgbox.ShowMsg(strContent,1);
                if(1 == msgbox.exec()){
                    RecvShutdownOprt();
                }
                break;
            }
            case PL_FUN_AM_ID_REQUEST_REAGENTPOS:
            {
                ParseRequestDialPostion(CurrData.m_Data);
                break;
            }
            default:
                qDebug()<<"command is invalid!";
            }
        }
        CurrData.Clear();
        nStartIndex = nStartIndex + 1;
        nEndIndex = (nEndIndex+4 - nStartIndex) + 1;
    }
    m_byarrControlData.clear();

}

/********************************************************
 *@Name:        ParseControlPanelsAlarmData
 *@Author:      HuaT
 *@Description: 解析
 *@Param1:      无
 *@Return:      无
 *@Version:     1.0
 *@Date:        2018-5-21
********************************************************/
void TestInterface::ParseControlPanelsAlarmData(const QByteArray &AlarmData)
{
    if(AlarmData.size() != 8){
        qDebug()<<"Alarm Data size does not match:"<<AlarmData.size();
        return;
    }
    QString strContent;
    quint32 nAlarmType = BytesToInt( AlarmData.mid(0,4));
    quint32 nAlarmData = BytesToInt( AlarmData.mid(4,4));
    QByteArray byarrAlarmType = QString("%1").arg(nAlarmType,32,2,QLatin1Char('0')).toAscii();
    if(byarrAlarmType.at(31) == 49){
        //ID卡被移除
        QString strReagentPos = QString::number(nAlarmData);
        m_mapIDCardInfo.remove(strReagentPos);
        m_mapTestItemPos.remove( m_mapTestItemPos.key(strReagentPos));
        //改变界面ID卡状态
        UpdateIDCardInfo(nAlarmData,0,"无");
        InitValidTestItem();
        m_strCurErrMsg = QString("ID卡被移除");
        qDebug()<<QString("ID卡被移除");
    }else if(byarrAlarmType.at(30) == 49){
        //ID卡错误
        m_strCurErrMsg = QString("ID卡错误");
        qDebug()<<QString("ID卡错误");
        //提醒用户更换ID卡槽
    }else if(byarrAlarmType.at(29) == 49){
        //仪器运行时错误
        m_strCurErrMsg = QString("仪器运行时错误");
        qDebug()<<QString("仪器运行时错误");
    }else if(byarrAlarmType.at(28) == 49){
        //复位错误
        m_strCurErrMsg = QString("仪器复位错误");
        qDebug()<<QString("仪器复位错误");
    }else if(byarrAlarmType.at(27) == 49){
        //缓冲液为空
        m_strCurErrMsg = QString(" 未吸到缓冲液，测试失败，请稍后重新测试!");
        qDebug()<<QString("样本%1未吸到缓冲液，测试失败，请稍后重新测试!").arg(nAlarmData+1);
    }else if(byarrAlarmType.at(26) == 49){
        //样本为空
        m_strCurErrMsg = QString(" 未吸到样本，测试失败，请稍后重新测试!");
        strContent = QString("样本%1未吸到样本，测试失败，请稍后重新测试!").arg(nAlarmData+1);
        qDebug()<<strContent;
    }else if(byarrAlarmType.at(25) == 49){
        //装载电机错误
        m_strCurErrMsg = QString("装载电机错误");
        qDebug()<<QString("装载电机错误");
    }else if(byarrAlarmType.at(24) == 49){
        //泵电机错误
        m_strCurErrMsg = QString("泵电机错误");
        qDebug()<<QString("泵电机错误");
    }else if(byarrAlarmType.at(23) == 49){
        //转盘电机错误
        m_strCurErrMsg = QString("转盘电机错误");
        qDebug()<<QString("转盘电机错误");
    }else if(byarrAlarmType.at(22) == 49){
        //混均槽为空错误
        m_strCurErrMsg = QString("混均槽为空错误");
        qDebug()<<QString("混均槽为空错误");
    }else if(byarrAlarmType.at(21) == 49){
        //X轴未到达预定位置
        m_strCurErrMsg = QString("X轴未到达预定位置");
        qDebug()<<QString("X轴未到达预定位置");
    }else if(byarrAlarmType.at(20) == 49){
        //Y轴未到达预定位置
        m_strCurErrMsg = QString("Y轴未到达预定位置");
        qDebug()<<QString("Y轴未到达预定位置");
    }else if(byarrAlarmType.at(19) == 49){
        //推卡为空错误
        m_strCurErrMsg = QString("卡槽 %1 推卡失败，请检查试剂卡是否推空或者未到位,并手动推入！").arg(nAlarmData+1);
        strContent = m_strCurErrMsg;
        MsgBoxDlg* msg = new MsgBoxDlg;
        msg->setAttribute(Qt::WA_ShowModal,true);
        msg->ShowMsg(strContent,1);
        msg->show();
        qDebug()<<m_strCurErrMsg;
    }else{
        m_strCurErrMsg = QString("未知控制板命令索引！");
        qDebug()<<"未知控制板命令索引！";
    }

}

void TestInterface::ParseRequestDialPostion(const QByteArray &AckData)
{
    if(AckData.size() != 8){
        qDebug()<<"DialPostion Data size does not match:"<<AckData.size();
        return;
    }
    int nSamplePos = BytesToInt (AckData.mid(0,4));
    int nReagentPos = BytesToInt (AckData.mid(4,4));
    if(m_listReagentPos.size() == 0){
        qDebug()<<"List Ragent Pos is Full"<<"--"<<__FUNCTION__;
        return;
    }
    int nCurDialPos;
    nCurDialPos = m_listReagentPos.at(0);
    m_listReagentPos.removeAt(0);
    //
    QByteArray byarrdata;
    byarrdata.append(IntToBytes(nCurDialPos));
    SendSerialData(m_ControlPanelsSerial,PL_FUN_AM_ID_REQUEST_REAGENTPOS_COMPLETE,byarrdata);
    //
    QString str = QString(REQUESTPOSITION).arg(nSamplePos+1).arg(nCurDialPos+1);
    ShowLogText(ui->tb_Test_RunInfoShow,str,LOGNORMAL);
#if 0
    qDebug()<<"test";
#endif
//    for(int n=0; n<m_listReagentPos.size(); n++){
//        if(m_listReagentPos.at(n) >= nReagentPos){
//            //nCurDialPos = m_listReagentPos.at(n);
//            nCurDialPos = m_listReagentPos.first();
//            m_listReagentPos.removeAt(n);

//            //
//            QByteArray byarrdata;
//            byarrdata.append(IntToBytes(nCurDialPos));
//            SendSerialData(m_ControlPanelsSerial,PL_FUN_AM_ID_REQUEST_REAGENTPOS_COMPLETE,byarrdata);
//            //
//            QString str = QString(REQUESTPOSITION).arg(nSamplePos+1).arg(nCurDialPos+1);
//            ShowLogText(ui->tb_Test_RunInfoShow,str,LOGNORMAL);
//            break;
//        }
//    }
}


/********************************************************
 *@Name:        CountDown
 *@Author:      HuaT
 *@Description: 倒计时函数
 *@Param1:      无
 *@Return:      无
 *@Version:     1.0
 *@Date:        2018-5-21
********************************************************/
void TestInterface::CountDown()
{
    //quint32 nTime = sender()->property("time").toInt();
    quint32 nReagentPos = sender()->property("ReagentPos").toInt();
    quint32 nSamplePos = sender()->property("SamplePos").toInt();
    //qDebug()<<nTime;
    //处理倒计时事件
    ProceCountDownEvent(nReagentPos,nSamplePos);
}

/********************************************************
 *@Name:        TestCheckFunc
 *@Author:      HuaT
 *@Description: 计时器测试板自检函数
 *@Param1:      无
 *@Return:      无
 *@Version:     1.0
 *@Date:        2018-5-21
********************************************************/
void TestInterface::TestCheckFunc()
{
    SendSerialData(m_CheckPanelsSerial,PL_FUN_AM_HARDWEAR_CHECK);
}

/********************************************************
 *@Name:        ControlCheckFunc
 *@Author:      HuaT
 *@Description: 计时器控制板自检函数
 *@Param1:      无
 *@Return:      无
 *@Version:     1.0
 *@Date:        2018-5-21
********************************************************/
void TestInterface::ControlCheckFunc()
{
    SendSerialData(m_ControlPanelsSerial,PL_FUN_AM_HARDWEAR_CHECK);
}

/********************************************************
 *@Name:        ScanModeChange
 *@Author:      HuaT
 *@Description: 扫码模式发生改变
 *@Param1:      无
 *@Return:      无
 *@Version:     1.0
 *@Date:        2018-5-21
********************************************************/
void TestInterface::ScanModeChange()
{
    //
    InitScanMode();
    //更新调试模式状态
    m_bDebugMode = m_settins->GetParam(DEBUGMODE).toInt();
    //PC连接模式
    m_bConnectPC = m_settins->GetParam(CONNPCMODE).toInt();
}


/********************************************************
 *@Name:        TestCardInfoChange
 *@Author:      HuaT
 *@Description: 测试试剂卡是否显示操作
 *@Param1:      无
 *@Return:      无
 *@Version:     1.0
 *@Date:        2019-6-17
********************************************************/
void TestInterface::TestCardInfoChange()
{
    int nRet = m_settins->GetParam(TESTCARDMODE).toInt();
    if(0 == nRet){
        m_mapIDCardInfo.clear();
        UpdateIDCardInfo(0,0,"无");
        UpdateIDCardInfo(1,0,"无");
        UpdateIDCardInfo(2,0,"无");
        UpdateIDCardInfo(3,0,"无");
        InitValidTestItem();
    }else{
        if(m_mapIDCardInfo.size() == 0){
            SimulationIDCardCast();
            InitValidTestItem();
        }
    }
}

void TestInterface::RecvShutdownOprt()
{
    //
    if(m_ControlPanelsSerial != NULL){
        QByteArray byarrdata;
        byarrdata.append(IntToBytes(0));
        SendSerialData(m_ControlPanelsSerial,PL_FUN_AM_ID_DEVICE_SOFTSHUTDOWN,byarrdata);
        //关闭系统
        ShutdownSystem();
    }else{
        //
        ShowLogText(ui->tb_Test_RunInfoShow,"通讯异常,无法正常关机,请联系客服",LOGERROR);
        //
        qDebug()<<"通讯异常,无法正常关机,请联系客服";
    }
}

void TestInterface::ProcWaitResultOpt()
{
    for(int n=0; n<m_listSampleInfo.size(); n++){
        for(int i=0; i<m_listSampleInfo.at(n).m_listTestItem.size(); i++){
            if(m_listSampleInfo.at(n).m_listTestItem.at(i).m_bResultWaitState == true){
                if(m_listSampleInfo.at(n).m_listTestItem.at(i).m_nResultWaitTime == 0){
                    qDebug()<<"m_nResultWaitTime:"<<m_listSampleInfo[n].m_listTestItem[i].m_nResultWaitTime;
                    //等待时间到
                    //等待状态归0
                    m_listSampleInfo.at(n).m_listTestItem.at(i).m_bResultWaitState == false;
                    //结果标记为异常
                    m_listSampleInfo[n].m_listTestItem[i].m_strResult = "-1";
                    qDebug()<<"result is:"<<-1;
                    //保存数据到数据库
                    int nReagentPos = m_listSampleInfo.at(n).m_listTestItem.at(i).m_ReagentObj.m_nReagentPos;
                    SaveResultToDB(m_listSampleInfo.at(n),nReagentPos );
                    //UI显示完成
                    m_SampleCircle->SetItemColor(nReagentPos+1,COMPLETESTATUS);
                    m_SampleCircle->SetItemText(nReagentPos+1,QObject::tr("完成"));
                    //流程已修改为测试完成立马退卡，所以不需要统一退卡了
                    //删除本次完成的项目
                    m_listSampleInfo[n].m_listTestItem.removeAt(i);
                    //更新已完成测试
                    m_nCompleteTestCount++;
                    //测试基本信息修改
                    //UpdateTestInfoBaseParam(m_nCompleteTestCount);
                    //发送退卡命令
                    //打包试剂位
                    QByteArray byarrReagentPos = IntToBytes(nReagentPos);
                    //如果项目完成，删除此样本信息
                    if(m_listSampleInfo[n].m_listTestItem.size() == 0){
                        m_listSampleInfo.removeAt(n);
                    }
                    //发送准备卸卡命令
                    SendSerialData(m_ControlPanelsSerial,PL_FUN_AM_PREPARE_UNSTALL,byarrReagentPos);
                    break;
                }else{
                    m_listSampleInfo[n].m_listTestItem[i].m_nResultWaitTime--;
                    qDebug()<<"m_listSampleInfo[n].m_listTestItem[i].m_nResultWaitTime:"<<m_listSampleInfo[n].m_listTestItem[i].m_nResultWaitTime;
                }
            }
        }
    }
}

void TestInterface::QueryLogFileSize()
{
    QFileInfo fi("/home/root/FullAutoQT/FullAutoLog.txt");
    if(fi.exists()){
        QString strLogSize = QString("%1 Kb").arg(fi.size()/1000);
        ui->lb_Test_LogSize->setText(strLogSize);
        if(fi.size()/1000 >= 200){
            QFile::remove("/home/root/FullAutoQT/FullAutoLog.txt");
        }
    }
}

void TestInterface::ZeroClearNumber()
{
    QString strCur = QTime::currentTime().toString("hh:mm:ss");
    if(strCur.compare("00:00:01")==0){
        m_nNumIndex = 1;
        ui->lb_Test_CurrentEmerPos->setText(QString::number(m_nNumIndex));
    }
}

/********************************************************
 *@Name:        ShutdownSystem
 *@Author:      HuaT
 *@Description: 关闭系统
 *@Param1:      无
 *@Return:      无
 *@Version:     1.0
 *@Date:        2019-5-21
********************************************************/
void TestInterface::ShutdownSystem()
{
#ifdef Q_OS_WIN32
        qApp->closeAllWindows();
#elif __arm__
        qApp->closeAllWindows();
        system("shutdown -h now");
#endif
}

/*
 *新建测试响应事件
 */
void TestInterface::on_pb_Test_NewTest_clicked()
{
    ui->stackedWidget->setCurrentIndex(1);
    m_TestCircle->HideAllCircle(true);
    m_SampleCircle->HideAllCircle(true);
    //判断是否为扫码设置
//    int isScan = m_settins->GetParam(SCANMODE).toInt();
//    if(isScan == 1){
//        //发送扫码信息
//        QByteArray byarrData;
//        byarrData.append(IntToBytes(0));
//        SendSerialData(m_ControlPanelsSerial,PL_FUN_AM_NEW_TEST,byarrData);
//    }else{
//        //自动编码
//    }
}

/*
 *详细信息响应事件
 */
void TestInterface::on_pb_Test_DetailsInfo_clicked()
{
    ui->stackedWidget->setCurrentIndex(1);
    m_TestCircle->HideAllCircle(true);
    m_SampleCircle->HideAllCircle(true);

    //QVector<QString> vecTest;
    //vecTest.push_back("11");
    //vecTest.pop_front();
    //qDebug()<<vecTest;
}

/*
 *详细页返回按钮响应事件
 */
void TestInterface::on_pb_Test_Back_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
    m_TestCircle->HideAllCircle(false);
    m_SampleCircle->HideAllCircle(false);
}

/*
 *详细页开始按钮响应事件
 */
void TestInterface::on_pb_Test_StartTest_clicked()
{
    //初始化样本圆的颜色
    DefaultTestCircleUI();
    //
    int nCount = 0;
    QString strItemName,strSampleNo;
    for(int n=0; n<TABLE_ROWS_COUNT; n++){
        strSampleNo = ui->tw_Test_TestInfo->item(n,TABLE_COLUMN_COUNT-2)->text();
        strItemName = ui->tw_Test_TestInfo->item(n,TABLE_COLUMN_COUNT-1)->text();
        if(!strSampleNo.isEmpty() && !strItemName.isEmpty() ){
            QStringList listItemName = strItemName.split("/");
            for(int i=0; i<listItemName.count(); i++){
                nCount++;
            }
        }
    }
    if( m_fCleanoutCurValue - nCount < 0 ){
        MsgBoxDlg msgbox;
        msgbox.ShowMsg(QObject::tr("清洗液过少，无法满足此次检测,请处理后再继续测试!"),1);
        msgbox.exec();
        return;
    }
    if( m_fEffluentCurValue + nCount > m_nFluidMaxValue ){
        MsgBoxDlg msgbox;
        msgbox.ShowMsg(QObject::tr("废液过多，请处理后再继续测试!"),1);
        msgbox.exec();
        return;
    }
    //检测用户输入
    bool isValid = CheckUserInputValid();
    if(!isValid){
        MsgBoxDlg msgbox;
        msgbox.ShowMsg(QObject::tr("试剂卡剩余数量不足 或 测试信息至少填写一项"),0);
        msgbox.exec();
        return;
    }
    //跳回状态页
    ui->stackedWidget->setCurrentIndex(0);
    //显示圆
    m_TestCircle->HideAllCircle(false);
    m_SampleCircle->HideAllCircle(false);
    //新建测试和开始测试不可用
    ui->pb_Test_NewTest->setEnabled(false);
    ui->pb_Test_StartTest->setEnabled(false);
    //详细信息界面按钮不可用
    SetRuntimeButtonEnable(false);
    //打包测试信息数据
    QByteArray byarrTestNumbleData;
    PacketTestInfoToQueue(m_listSampleInfo);
    //开始日志
    QString strContent = "---" + QString(STARTTEST).arg(ui->lb_Test_SampleCount->text().toInt()) + "---";
    ShowLogText(ui->tb_Test_RunInfoShow,strContent,LOGNOTE2);
    if(m_bAddSampleStatus != true && m_bCheckStatus != true){
        //生成日志内容
        if(!m_listSampleInfo.isEmpty()){
            QString strLog = QString(RUNADDSAMPLE).arg(m_listSampleInfo.front().m_nSamplePos+1);
            ShowLogText(ui->tb_Test_RunInfoShow,strLog,LOGNORMAL);
        }
        byarrTestNumbleData = GetWaitAddSampleInfoToQueue();
        if(byarrTestNumbleData.isEmpty()){
            return;
        }
        //发送测试信息-开始加样
        SendSerialData(m_ControlPanelsSerial,PL_FUN_AM_START_SAMPLE,byarrTestNumbleData);
        //开始加样状态开启
        m_bAddSampleStatus = true;
        //测试基本信息修改
        //UpdateTestInfoBaseParam(m_nCompleteTestCount);
        //开启急诊插入
        ui->pb_Test_InsertEmer->setEnabled(true);
    }
}

/********************************************************
 *@Name:        GetWaitAddSampleInfoToQueue
 *@Author:      HuaT
 *@Description: 从等待加样测试命令中获取一条测试信息返回
 *@Param1:      无
 *@Return:      封装好的测试信息，如果没有则为空
 *@Version:     1.0
 *@Date:        2019-01-17
********************************************************/
QByteArray TestInterface::GetWaitAddSampleInfoToQueue(){
    QByteArray byarrTestData;
    if(!m_listWaitAddSampleQueue.isEmpty()){
        byarrTestData = m_listWaitAddSampleQueue.front().m_byteContentData;
        m_listWaitAddSampleQueue.removeFirst();
    }
    return byarrTestData;
}

QByteArray TestInterface::GetWaitCheckInfoToQueue(){
    QByteArray byarrTestData;
    if(!m_listWaitTestQueue.isEmpty()){
        byarrTestData = m_listWaitTestQueue.front().m_byteContentData;
        m_listWaitTestQueue.removeFirst();
    }
    return byarrTestData;
}

/********************************************************
 *@Name:        GetSampleTypeObj
 *@Author:      HuaT
 *@Description: 从等待加样测试命令中获取一条测试信息返回
 *@Param1:      无
 *@Return:      返回记录的设置界面加样样本类型数据
 *@Version:     1.0
 *@Date:        2019-01-17
********************************************************/
QMap<QString, QString> *TestInterface::GetSampleTypeObj()
{
    return &m_mapSampleType;
}


/********************************************************
 *@Name:        PacketTestInfoToQueue
 *@Author:      HuaT
 *@Description: 把所有测试信息封装成测试队列，等待发送
 *@Param1:      所有样本信息结构体
 *@Return:      无
 *@Version:     1.1
 *@Date:        2018-05-21
 *@Last Modify: 2019-01-17
********************************************************/
void TestInterface::PacketTestInfoToQueue(QList<SampleObj> &listSampleInfo){
    QByteArray byarrTestInfo;
    QString strTestItem;
    QList<SampleObj>::iterator iter;
    QString strLogSamplePos,strLogSampleName,strLogSampleType,strLogSampleSize,strLogBufferSize;
    for(iter = listSampleInfo.begin(); iter != listSampleInfo.end(); iter++){
        for(int n=0; n<iter->m_listTestItem.size(); n++){
            if( iter->m_listTestItem[n].m_nCurRunStatus == 0 ){
                //是否测试标记（现在发过去的项目都测试，所以这个标记已弃用,统一填充为0）
                byarrTestInfo.append(IntToBytes(0));
                //样本位置
                byarrTestInfo.append( IntToBytes( iter->m_nSamplePos) );
                //改变颜色,由于样本盘编号由0到15,圆的设定由1开始,所以加1
                m_TestCircle->SetItemColor( iter->m_nSamplePos+1,WAITSTATUS);
                //项目名称
                strTestItem = iter->m_listTestItem[n].m_strItemName;
                //ID卡位置编号
                quint8 nIDCardIndex = 0;
                nIDCardIndex = iter->m_listTestItem[n].m_ReagentObj.m_nReagentPos;
                //推卡电机位置
                byarrTestInfo.append(IntToBytes(nIDCardIndex));
                //缓冲液位置编号(0-3)
                byarrTestInfo.append(IntToBytes(nIDCardIndex));
                //样本量大小
                quint16 nSampleSize = 0;
                //样本量血清大小
                quint16 nSampleSerumSize = 0;
                nSampleSerumSize = static_cast<quint16>( iter->m_listTestItem[n].m_ReagentObj.m_byarrIDCardData.at(42) );
                //样本量全血大小
                quint16 nSampleBloodSize = 0;
                nSampleBloodSize = static_cast<quint16>( iter->m_listTestItem[n].m_ReagentObj.m_byarrIDCardData.at(43) );
                //查找配置文件使用的是哪个样本类型
                //int nType = m_settins->GetParam(BLOODTYPE).toInt();
                QString strType = m_mapSampleType.value(strTestItem,"0");
                if(strType.compare("0") == 0){
                    nSampleSize = nSampleSerumSize;
                    strLogSampleType = "血清";
                }else{
                    nSampleSize = nSampleBloodSize;
                    strLogSampleType = "全血";
                }
                //缓冲液大小
                quint16 nBufferSize = 0;
                quint8 nBufHight = 0;
                quint8 nBufLow = 0;
                nBufHight = static_cast<quint8>( iter->m_listTestItem[n].m_ReagentObj.m_byarrIDCardData.at(44) );
                nBufLow = static_cast<quint8>( iter->m_listTestItem[n].m_ReagentObj.m_byarrIDCardData.at(45) );
                nBufferSize = nBufHight * 256 + nBufLow;
                if(m_bDebugMode == true){
                    QString strSampleSize = m_settins->GetParam(SAMPLESIZE);
                    if(!strSampleSize.isEmpty() && strSampleSize.toInt() != 0){
                        nSampleSize = static_cast<quint8>(strSampleSize.toInt());
                    }
                    QString strBufferSize = m_settins->GetParam(DILUENT);
                    if(!strBufferSize.isEmpty() && strBufferSize.toInt() != 0){
                        nBufferSize = static_cast<quint16>(strBufferSize.toInt());
                    }
                }
                float fDiluentOffsetCoeff = m_settins->GetParam(DILUENTOFFSETCOEFF).toFloat();
                float fDiluentOffsetConst = m_settins->GetParam(DILUENTOFFSETCONST).toFloat();
                if(fDiluentOffsetCoeff>0){
                    int nTempBufferSize = static_cast<int>(fDiluentOffsetCoeff * nBufferSize + fDiluentOffsetConst);
                    if(nTempBufferSize >= 6){
                        nBufferSize = nTempBufferSize;
                    }
                }
                float fSampleOffsetCoeff = m_settins->GetParam(SAMPLEOFFSETCOEFF).toFloat();
                float fSampleOffsetConst = m_settins->GetParam(SAMPLEOFFSETCONST).toFloat();
                if(fSampleOffsetCoeff > 0){
                    int nTempSampleSize = static_cast<int>(fSampleOffsetCoeff * nSampleSize + fSampleOffsetConst);
                    if(nTempSampleSize >= 6){
                        nSampleSize = nTempSampleSize;
                    }
                }
                byarrTestInfo.append(IntToBytes(nSampleSize));
                byarrTestInfo.append(IntToBytes(nBufferSize));
                //qDebug()<<"nSampleSize:"<<nSampleSize<<"nBufferSize:"<<nBufferSize;
                strLogSamplePos = QString::number(iter->m_nSamplePos+1);
                strLogSampleSize = QString::number(nSampleSize);
                strLogBufferSize = QString::number(nBufferSize);
                strLogSampleName = strTestItem;
                QString strLog;
                strLog = QString("样本位置:"+strLogSamplePos
                                 +",卡槽位置:"+QString::number(nIDCardIndex+1)
                                 +",测试名称:"+strLogSampleName
                                 +",样本量大小:"+strLogSampleSize+",缓冲液大小:"
                                 +strLogBufferSize+".");
                ShowLogText(ui->tb_Test_RunInfoShow,strLog,LOGNORMAL);
                //本项目已经打包
                iter->m_bIsPacket = true;
                iter->m_listTestItem[n].m_nCurRunStatus = 1;
                SendQueue sq;
                sq.m_nSamplePos = iter->m_nSamplePos;
                sq.m_strItemName = strTestItem;
                sq.m_byteContentData = byarrTestInfo;
                m_listWaitAddSampleQueue.append(sq);
                byarrTestInfo.clear();
            }
        }
    }
}

void TestInterface::PacketTestInfoToQueue(QList<SampleObj> &vecSampleInfo, bool isEmer){
    QByteArray byarrTestInfo;
    QString strTestItem;
    SampleObj tiEmer = vecSampleInfo.last();
    for(int n=0; n<tiEmer.m_listTestItem.size(); n++){
        //是否测试标记（现在发过去的项目都测试，所以这个标记已弃用,统一填充为0）
        byarrTestInfo.append(IntToBytes(0));
        //样本位置
        byarrTestInfo.append( IntToBytes( tiEmer.m_nSamplePos) );
        //改变颜色,由于样本盘编号由0到15,圆的设定由1开始,所以加1
        m_TestCircle->SetItemColor( tiEmer.m_nSamplePos+1,WAITSTATUS);
        //项目名称
        strTestItem = tiEmer.m_listTestItem[n].m_strItemName;
        //ID卡位置编号
        quint8 nIDCardIndex = 0;
        nIDCardIndex = tiEmer.m_listTestItem[n].m_ReagentObj.m_nReagentPos;
        byarrTestInfo.append(IntToBytes(nIDCardIndex));
        //缓冲液位置编号(0-3)
        byarrTestInfo.append(IntToBytes(nIDCardIndex));
        //vecTestInfo[15].m_nIDCardIndex = nIDCardIndex;
        //样本量大小
        quint8 nSampleSize = 0;
        nSampleSize = static_cast<quint8>( (tiEmer.m_listTestItem[n].m_ReagentObj.m_byarrIDCardData.at(42) ) );
        //GetIDCardPosData(vecSampleInfo[15].m_nSamplePos,42,nSampleSize);
        byarrTestInfo.append(IntToBytes(nSampleSize));
        //缓冲液大小
        quint16 nBufferSize = 0;
        quint8 nBufHight = 0;
        quint8 nBufLow = 0;
        nBufHight = static_cast<quint8>( (tiEmer.m_listTestItem[n].m_ReagentObj.m_byarrIDCardData.at(43) ) );;
        nBufLow = static_cast<quint8>( (tiEmer.m_listTestItem[n].m_ReagentObj.m_byarrIDCardData.at(44) ) );;
        //GetIDCardPosData(vecSampleInfo[15].m_nSamplePos,43,nBufHight);
        //GetIDCardPosData(vecSampleInfo[15].m_nSamplePos,44,nBufLow);
        nBufferSize = nBufHight * 256 + nBufLow;
        //nBufferSize = 300;
        byarrTestInfo.append(IntToBytes(nBufferSize));
        qDebug()<<"nSampleSize:"<<nSampleSize<<"nBufferSize:"<<nBufferSize;
        //本项目已经发送测试
        //vecSampleInfo[15].m_listTestItem[0].m_bIsSend = true;
        SendQueue sq;
        sq.m_nSamplePos = tiEmer.m_nSamplePos;
        sq.m_strItemName = strTestItem;
        sq.m_byteContentData = byarrTestInfo;
        m_listWaitAddSampleQueue.insert(0,sq);
        byarrTestInfo.clear();
    }
}


/********************************************************
 *@Name:        CheckUserInputValid
 *@Author:      HuaT
 *@Description: 检测用户输入项目类型是否有效
 *@Param1:      无
 *@Return:      是否为有效输入
 *@Version:     1.0
 *@Date:        2018-5-22
********************************************************/
bool TestInterface::CheckUserInputValid()
{
    int nCount = 0;
    QString strSampleNo;
    QString strItemName;
    bool bRet = false;
    for(qint8 n=0; n<TABLE_ROWS_COUNT; n++){
        strItemName.clear();
        strSampleNo = ui->tw_Test_TestInfo->item(n,TABLE_COLUMN_COUNT-2)->text();
        //qDebug()<<strSampleNo;
        strItemName = ui->tw_Test_TestInfo->item(n,TABLE_COLUMN_COUNT-1)->text();
        //qDebug()<<strItemName;
        if(!strSampleNo.isEmpty() && !strItemName.isEmpty() ){
            SampleObj so;
            so.m_strSampleID = strSampleNo;
            so.m_bIsEmpty = false;
            so.m_nSamplePos = static_cast<qint8>(n);
            m_listSampleInfo.append(so);
            QStringList listItemName = strItemName.split("/");
            for(int i=0; i<listItemName.count(); i++){
                bRet = AddSampleTestItemInfo(n,strSampleNo,listItemName.at(i),m_listSampleInfo);
                if(bRet == true){
                    nCount++;
                }
            }
        }
        //qDebug()<<m_vecTestInfo.at(n).m_nSamplePos<<":"<<m_vecTestInfo.at(n).m_bIsEmerg<<"-"<<m_vecTestInfo.at(n).m_strSampleNo<<"-"<<m_vecTestInfo.at(n).m_strItemName<<";";
    }
    //测试项目总数
    ui->lb_Test_SampleCount->setText(QString("%1").arg(nCount));
    if(nCount == 0){
        return false;
    }else{
        return true;
    }
}

bool TestInterface::AddSampleTestItemInfo(qint8 nSamplePos,QString strSampleID, QString strItemName, QList<SampleObj> &listSampleInfo){
    TestItem ti;
    ti.m_timer = new QTimer();
    connect(ti.m_timer,SIGNAL(timeout()),this,SLOT(CountDown()));
    ti.m_strItemName = strItemName;
    bool bOk = false;
    QMap<QString,IDCardInfo>::Iterator it;
    for(it = m_mapIDCardInfo.begin(); it != m_mapIDCardInfo.end(); it++){
        if( (it.value().m_strTestItem == ti.m_strItemName) ){
            if(it.value().m_nRemaining == 0){
                continue;
            }else{
                //试剂卡槽位置
                ti.m_ReagentObj.m_nReagentPos = it.key().toInt();
                //id卡数据
                ti.m_ReagentObj.m_byarrIDCardData = it.value().m_byarrIDCardData;
                //试剂卡单位
                ti.m_ReagentObj.m_strReagentUnit = it.value().m_strTestUnit;
                //剩余数量减1
                it.value().m_nRemaining--;
                //添加测试项目
                for(int n=0; n<listSampleInfo.size(); n++){
                    if(listSampleInfo.at(n).m_nSamplePos == nSamplePos && listSampleInfo.at(n).m_bIsPacket == false){
                        listSampleInfo[n].m_listTestItem.append(ti);
                    }
                }
                bOk = true;
                //
                break;
            }
        }
    }
    if(bOk == false){
        //所有的试剂卡都不够了
        qDebug()<<"m_nRemaining == 0";
        QString strLogMsg = QString("样本%1 测试项目%2 试剂卡数量不够，新建测试失败!请插卡后重新新建!")
                            .arg(nSamplePos + 1).arg(strItemName);
        ShowLogText(ui->tb_Test_RunInfoShow,strLogMsg,LOGWARNING);
        for(int n=0; n<listSampleInfo.size(); n++){
            if(listSampleInfo.at(n).m_nSamplePos == nSamplePos){
                if(listSampleInfo.at(n).m_listTestItem.size() == 0){
                    listSampleInfo.removeAt(n);
                }
            }
        }
        return false;
    }else{
        return true;
    }

}

/*
 *初始化测试圆和样品圆界面
 */
void TestInterface::InitCircle()
{
    //测试盘
    m_TestCircle = new CircleLayout(this);
    QPoint posTest(210,220);
    m_TestCircle->CreateCircleLayout(180,posTest, TABLE_ROWS_COUNT);

    //样品盘
    m_SampleCircle = new CircleLayout(this);
    QPoint posSam(730,220);
    m_SampleCircle->CreateCircleLayout(180,posSam,20);

}

/********************************************************
 *@Name:        InitScanMode
 *@Author:      HuaT
 *@Description: 初始化扫码模式控件
 *@Param1:      无
 *@Return:      无
 *@Version:     1.0
 *@Date:        2018-5-21
********************************************************/
void TestInterface::InitScanMode()
{
    QString strMode = m_settins->GetParam(SCANMODE);
    if(strMode.toInt() == 0){
        //自动编码
        ui->lb_Test_SampleCount->hide();
        ui->le_Test_SampleCount->show();
        ui->pb_Test_ScanModel->setText(QObject::tr("自动编码"));
        ui->lb_Test_StartNumber->setText("起始编号");
        ui->le_Test_StartNumber->setText("");
        ui->rb_Test_StartNumberClose->setText("关闭");
        ui->rb_Test_StartNumberOpen->setText("开启");
        ui->le_Test_StartNumber->setText("");

        ui->lb_Test_CurrentEmerPosTitle->show();
        ui->lb_Test_CurrentEmerPos->show();
        ui->lb_Test_CurrentEmerPosTitle->setText("下一编号:");
        ui->lb_Test_CurrentEmerPos->setText(QString::number(m_nNumIndex));
    }else{
        //自动扫码
        ui->le_Test_SampleCount->hide();
        ui->lb_Test_SampleCount->show();
        ui->pb_Test_ScanModel->setText(QObject::tr("自动扫码"));
        ui->lb_Test_StartNumber->setText("扫码总数");
        ui->le_Test_StartNumber->setText("");
        ui->rb_Test_StartNumberClose->setText("总数");
        ui->rb_Test_StartNumberOpen->setText("单数");

        ui->lb_Test_CurrentEmerPosTitle->hide();
        ui->lb_Test_CurrentEmerPos->hide();
    }
}

/********************************************************
 *@Name:        InitReagentPos
 *@Author:      HuaT
 *@Description: 初始化试剂位置队列
 *@Param1:      无
 *@Return:      无
 *@Version:     1.0
 *@Date:        2019-8-12
********************************************************/
void TestInterface::InitReagentPos()
{
    for(int n=0; n<20; n++){
        m_listReagentPos.append(n);
    }

}


/********************************************************
 *@Name:        DefaultCircleUI
 *@Author:      HuaT
 *@Description: 默认测试界面圆颜色
 *@Param1:      无
 *@Return:      无
 *@Version:     1.0
 *@Date:        2018-5-21
********************************************************/
void TestInterface::DefaultCircleUI()
{
    DefaultSampleCircleUI();
    DefaultTestCircleUI();
}

void TestInterface::DefaultSampleCircleUI()
{
    for(int i=1; i<21;i++){
        m_SampleCircle->SetItemText(i,QString::number(i));
        m_SampleCircle->SetItemColor(i,NORMALSTATUS);
    }
}

void TestInterface::DefaultTestCircleUI()
{
    for(int n=1; n<17; n++){
        m_TestCircle->SetItemText(n,QString::number(n));
        m_TestCircle->SetItemColor(n,NORMALSTATUS);
    }
}

/********************************************************
 *@Name:        InitProgressBar
 *@Author:      HuaT
 *@Description: 初始化进度条参数
 *@Param1:      无
 *@Return:      无
 *@Version:     1.0
 *@Date:        2018-5-21
********************************************************/
void TestInterface::InitProgressBar()
{
    //ui->proBar_Test_Reagent1->setStyleSheet("QProgressBar{background-color: #E6E6E6;border: 1px solid grey;border-radius: 1px;}");
    ui->proBar_Test_Reagent1->setFormat("%v");
    ui->proBar_Test_Reagent1->setRange(0,20);
    UpdateIDCardInfo(0,0,"无");
    //ui->proBar_Test_Reagent2->setStyleSheet("QProgressBar{background-color: #E6E6E6;border: 1px solid grey;border-radius: 1px;}");
    ui->proBar_Test_Reagent2->setFormat("%v");
    ui->proBar_Test_Reagent2->setRange(0,20);
    UpdateIDCardInfo(1,0,"无");
    //ui->proBar_Test_Reagent3->setStyleSheet("QProgressBar{background-color: #E6E6E6;border: 1px solid grey;border-radius: 1px;}");
    ui->proBar_Test_Reagent3->setFormat("%v");
    ui->proBar_Test_Reagent3->setRange(0,20);
    UpdateIDCardInfo(2,0,"无");
    //ui->proBar_Test_Reagent4->setStyleSheet("QProgressBar{background-color: #E6E6E6;border: 1px solid grey;border-radius: 1px;}");
    ui->proBar_Test_Reagent4->setFormat("%v");
    ui->proBar_Test_Reagent4->setRange(0,20);
    UpdateIDCardInfo(3,0,"无");
    //清洗液废液
    ui->proBar_Test_Cleanout->setFormat("%v");
    ui->proBar_Test_Cleanout->setRange(0,m_nFluidMaxValue);
    UpdateCleanoutValue(2);
    //
    ui->proBar_Test_Effluent->setFormat("%v");
    ui->proBar_Test_Effluent->setRange(0,m_nFluidMaxValue);
    UpdateFluidValue(2);
}

void TestInterface::UpdateCleanoutValue(float fValue)
{
    m_fCleanoutCurValue = m_fCleanoutCurValue - fValue;
    if(m_fCleanoutCurValue<0){
        m_fCleanoutCurValue = 0;
    }
    ui->proBar_Test_Cleanout->setValue(static_cast<qint32>(m_fCleanoutCurValue));
    m_settins->SaveSettingsInfo(CLEANOUTCURVALUE,QString::number(m_fCleanoutCurValue));
    m_settins->SetParam(CLEANOUTCURVALUE,QString::number(m_fCleanoutCurValue));
    qDebug()<<"UpdateCleanoutValue:"<<m_fCleanoutCurValue;
}

void TestInterface::UpdateFluidValue(float fValue)
{
    m_fEffluentCurValue = m_fEffluentCurValue + fValue;
    if(m_fEffluentCurValue>m_nFluidMaxValue){
        m_fEffluentCurValue = m_nFluidMaxValue;
    }
    ui->proBar_Test_Effluent->setValue(static_cast<qint32>(m_fEffluentCurValue));
    m_settins->SaveSettingsInfo(EFFLUENTCURVALUE,QString::number(m_fEffluentCurValue));
    m_settins->SetParam(EFFLUENTCURVALUE,QString::number(m_fEffluentCurValue));
    qDebug()<<"UpdateFluidValue:"<<m_fEffluentCurValue;
}

/********************************************************
 *@Name:        CleanoutValueChange
 *@Author:      HuaT
 *@Description: 液体最大值参数发生改变
 *@Param1:      无
 *@Return:      无
 *@Version:     1.0
 *@Date:        2019-10-11
********************************************************/
void TestInterface::CleanoutValueChange()
{
    qint32 nCurMaxValue= m_settins->GetParam(CLEANOUTMAXVALUE).toInt();
    if(m_nFluidMaxValue == nCurMaxValue){
        return;
    }
    m_nFluidMaxValue = nCurMaxValue;
    ui->proBar_Test_Cleanout->setRange(0,m_nFluidMaxValue);
    ui->proBar_Test_Effluent->setRange(0,m_nFluidMaxValue);
}

/********************************************************
 *@Name:        InitTableInfo
 *@Author:      HuaT
 *@Description: 初始化列表信息控件
 *@Param1:      无
 *@Return:      无
 *@Version:     1.0
 *@Date:        2018-5-21
********************************************************/
void TestInterface::InitTableInfo()
{
    //
    QStringList header;
    //header<<QObject::tr("急诊")<<QObject::tr("样本位")<<QObject::tr("样本编号")<<QObject::tr("测试项目");
    header<<QObject::tr("样本位")<<QObject::tr("样本编号")<<QObject::tr("测试项目");
    //
    ui->tw_Test_TestInfo->setColumnCount(TABLE_COLUMN_COUNT);
    ui->tw_Test_TestInfo->setRowCount(TABLE_ROWS_COUNT);
    //
    ui->tw_Test_TestInfo->setHorizontalHeaderLabels(header);
    ui->tw_Test_TestInfo->verticalHeader()->setHidden(true);

    //ui->tw_Test_TestInfo->verticalHeader()->setHidden(true);
    ui->tw_Test_TestInfo->setEditTriggers(QTableWidget::NoEditTriggers);
    ui->tw_Test_TestInfo->setSelectionBehavior(QTableWidget::SelectRows);
    //
    //ui->tw_Test_TestInfo->setColumnWidth(0,40);
    ui->tw_Test_TestInfo->setColumnWidth(0,80);
    ui->tw_Test_TestInfo->setColumnWidth(1,230);
    ui->tw_Test_TestInfo->setColumnWidth(2,220);
    ui->tw_Test_TestInfo->horizontalHeader()->setStretchLastSection(true);
    //
    ui->tw_Test_TestInfo->verticalHeader()->setDefaultSectionSize(37);
    //
    for(int i=0; i< TABLE_ROWS_COUNT; i++){
        //ui->tw_Test_TestInfo->setRowHeight(i,37);
        QRadioButton* radio = new QRadioButton();
        radio->setStyleSheet("margin-left:12px");

        QTableWidgetItem* item1 = new QTableWidgetItem();
        item1->setText(QString("%1").arg(i+1));
        item1->setTextAlignment(Qt::AlignCenter);

        QTableWidgetItem* item2 = new QTableWidgetItem();
        //item2->setText(QString("Number:%11111111").arg(i));
        item2->setText("");
        item2->setTextAlignment(Qt::AlignCenter);

        QTableWidgetItem* item3 = new QTableWidgetItem();
        //item3->setText(QString("POS:%1").arg(i));
        item3->setText("");
        item3->setTextAlignment(Qt::AlignCenter);

        //ui->tw_Test_TestInfo->setCellWidget(i,0,radio);
        ui->tw_Test_TestInfo->setItem(i,0,item1);
        ui->tw_Test_TestInfo->setItem(i,1,item2);
        ui->tw_Test_TestInfo->setItem(i,2,item3);
    }
    //ui->tw_Test_TestInfo->itemAt()
    //ui->tw_Test_TestInfo->setAlternatingRowColors(true);

}

/********************************************************
 *@Name:        ShowTestItem
 *@Author:      HuaT
 *@Description: 显示或者隐藏测试项目按钮
 *@Param1:      是否显示
 *@Return:      无
 *@Version:     1.0
 *@Date:        2018-5-21
********************************************************/
void TestInterface::ShowTestItem(bool isShow)
{
    if(isShow){
        ui->pb_Test_TestName1->show();
        ui->pb_Test_TestName2->show();
        ui->pb_Test_TestName3->show();
        ui->pb_Test_TestName4->show();
    }else{
        ui->pb_Test_TestName1->hide();
        ui->pb_Test_TestName2->hide();
        ui->pb_Test_TestName3->hide();
        ui->pb_Test_TestName4->hide();
    }
}

/********************************************************
 *@Name:        InitValidTestItem
 *@Author:      HuaT
 *@Description: 初始化当前可测试的项目类型
 *@Param1:      无
 *@Return:      无
 *@Version:     1.0
 *@Date:        2018-5-21
********************************************************/
void TestInterface::InitValidTestItem()
{
    QString strReagent1 = m_mapIDCardInfo.value("0").m_strTestItem;
    QString strReagent2 = m_mapIDCardInfo.value("1").m_strTestItem;
    QString strReagent3 = m_mapIDCardInfo.value("2").m_strTestItem;
    QString strReagent4 = m_mapIDCardInfo.value("3").m_strTestItem;
    if(!strReagent1.isEmpty()){
        ui->pb_Test_TestName1->setText(strReagent1);
        ui->pb_Test_TestName1->show();
    }else{
        ui->pb_Test_TestName1->setText("");
        ui->pb_Test_TestName1->hide();
    }
    if(!strReagent2.isEmpty()){
        ui->pb_Test_TestName2->setText(strReagent2);
        ui->pb_Test_TestName2->show();
    }else{
        ui->pb_Test_TestName2->setText("");
        ui->pb_Test_TestName2->hide();
    }
    if(!strReagent3.isEmpty()){
        ui->pb_Test_TestName3->setText(strReagent3);
        ui->pb_Test_TestName3->show();
    }else{
        ui->pb_Test_TestName3->setText("");
        ui->pb_Test_TestName3->hide();
    }
    if(!strReagent4.isEmpty()){
        ui->pb_Test_TestName4->setText(strReagent4);
        ui->pb_Test_TestName4->show();
    }else{
        ui->pb_Test_TestName4->setText("");
        ui->pb_Test_TestName4->hide();
    }
    if(strReagent1.isEmpty()&&strReagent2.isEmpty()&&strReagent3.isEmpty()&&strReagent4.isEmpty()){
        ui->lb_Test_TestItemStatus->show();
    }else{
        ui->lb_Test_TestItemStatus->hide();
    }
}

/********************************************************
 *@Name:        ClearTableInfo
 *@Author:      HuaT
 *@Description: 清除列表控件中的内容
 *@Param1:      无
 *@Return:      无
 *@Version:     1.0
 *@Date:        2018-5-21
********************************************************/
void TestInterface::ClearTableInfo()
{
    ui->tw_Test_TestInfo->setRowCount(0);
    ui->tw_Test_TestInfo->setColumnCount(0);
    InitTableInfo();
}

/*
 *详细页样本全选勾选框响应事件
 */
void TestInterface::on_checkBox_Test_AllSelect_clicked()
{

}

/********************************************************
 *@Name:        ClearCellContent
 *@Author:      HuaT
 *@Description: 清除指定单元格内容
 *@Param1:      单元格所在列
 *@Param2:      单元格所在行
 *@Return:      无
 *@Version:     1.0
 *@Date:        2018-5-21
********************************************************/
void TestInterface::ClearCellContent(int Column, int Row)
{
    QTableWidgetItem* item = ui->tw_Test_TestInfo->item(Row,Column);
    item->setText("");
}

/********************************************************
 *@Name:        SetCellContent
 *@Author:      HuaT
 *@Description: 清除指定单元格内容
 *@Param1:      单元格所在列
 *@Param2:      单元格所在行
 *@Param3:      单元格内容
 *@Return:      无
 *@Version:     1.0
 *@Date:        2018-5-21
********************************************************/
void TestInterface::SetCellContent(int Column, int Row, QString strContent)
{
    QTableWidgetItem* item = ui->tw_Test_TestInfo->item(Row,Column);
    item->setText(strContent);
}


/*
 *详细页清除按钮响应事件
 */
void TestInterface::on_pb_Test_ClearTestName_clicked()
{
    if(ui->checkBox_Test_AllSelect->isChecked() == true){
        MsgBoxDlg msgbox;
        msgbox.ShowMsg("确定删除所有项目名称？",1);
        if(1 == msgbox.exec()){
            for(int i=0; i< TABLE_ROWS_COUNT; i++){
                ClearCellContent(TABLE_COLUMN_COUNT-1, i);
            }
        }
    }else{
        int nCurRow = ui->tw_Test_TestInfo->currentRow();
        if(nCurRow == -1){
            return ;
        }
        ClearCellContent(TABLE_COLUMN_COUNT-1,nCurRow);
    }
    ui->lb_Test_TestProject->setText("");
}


/*
 *详细页自动扫码按钮响应事件
 */
void TestInterface::on_pb_Test_ScanModel_clicked()
{
    QString strScanModel = ui->pb_Test_ScanModel->text();
    qint32 nSampleSize = ui->le_Test_StartNumber->text().toInt();
    if(nSampleSize == 0 || nSampleSize>16){
        nSampleSize = 16;
    }
    qint32 nSampleType = 0;
    if(ui->rb_Test_StartNumberClose->isChecked()){
        nSampleType = 0;
    }else{
        nSampleType = 1;
    }
    if(strScanModel == QObject::tr("自动扫码")){
        //qDebug()<<"auto scan";
        QByteArray byarrData;
        if(m_bEmerStatus){
            //发送扫码信息
            byarrData.append(IntToBytes(16));
            SendSerialData(m_ControlPanelsSerial,PL_FUN_AM_NEW_TEST,byarrData);
        }else{
            byarrData.append(IntToBytes(nSampleSize));
            byarrData.append(IntToBytes(nSampleType));
            SendSerialData(m_ControlPanelsSerial,PL_FUN_AM_NEW_TEST,byarrData);
        }
    }else{
        //qDebug()<<"manual scan";
        int nCount = ui->le_Test_SampleCount->text().toInt();
        if(nCount > 16){
            nCount = 16;
        }
        QString strStartNumber = ui->le_Test_StartNumber->text();
        if(strStartNumber.isEmpty() == false && ui->rb_Test_StartNumberOpen->isChecked()){
            m_nNumIndex = strStartNumber.toInt();
        }
        if(nCount > 0){
            QString strCurDate;
            QString strIndex;
            for(int n = 0; n<nCount; n++){
                //strCurDate = QDate::currentDate().toString("yyyyMMdd");
                //strIndex = QString("%1").arg(m_nNumIndex,4,10,QLatin1Char('0'));
                strIndex = QString("%1").arg(m_nNumIndex);
                m_nNumIndex++;
                ui->tw_Test_TestInfo->item(n,1)->setText(strCurDate+strIndex);
            }
        }else{
            QString strCurDate;
            QString strIndex;
            int nCurRow = ui->tw_Test_TestInfo->currentRow();
            //strCurDate = QDate::currentDate().toString("yyyyMMdd");
            //strIndex = QString("%1").arg(m_nNumIndex,4,10,QLatin1Char('0'));
            strIndex = QString("%1").arg(m_nNumIndex);
            m_nNumIndex++;
            ui->tw_Test_TestInfo->item(nCurRow,1)->setText(strCurDate+strIndex);
        }
        ui->lb_Test_CurrentEmerPos->setText(QString::number(m_nNumIndex));
        /*int nCurRow = ui->tw_Test_TestInfo->currentRow();
        if(nCurRow != -1){
            qsrand(QTime(0,0,0).msecsTo(QTime::currentTime()));
            int nRandData = qrand()%9000 + 1000;
            ui->tw_Test_TestInfo->item(nCurRow,1)->setText(QString::number(nRandData));
            ui->le_Test_SampleNumber->setText(QString::number(nRandData));
        }*/
    }
    //int nte = 12345;
    //qDebug()<<QString("%1").arg(nte,32,2,QLatin1Char('0')).toAscii();

}

/********************************************************
 *@Name:        TransformProjectName
 *@Author:      HuaT
 *@Description: 项目名称转化,hsCPR在hsCRP/PCT经过转化后为PCT,如果没有hsCRP则添加hsCRP
 *@Param1:      目标名称
 *@Param2:      现有的名称集,hsCRP/PCT/cTnI格式
 *@Return:      返回处理过后的字符串
 *@Version:     1.0
 *@Date:        2018-5-22
********************************************************/
QString TestInterface::TransformProjectName(QString strCurName, QString strNameRecord)
{
    QString strResult = "";

    if(strNameRecord.isEmpty()){
        strResult = strCurName;
    }else{
        QStringList listName = strNameRecord.split("/");
        int nIndex = listName.indexOf(strCurName);
        if(nIndex == -1){
            listName.append(strCurName);
        }else{
            listName.removeAt(nIndex);
        }
        int nCount = listName.size();
        for(int n=0; n<nCount; n++){
            strResult += listName.at(n);
            if(n == nCount-1){
                break;
            }
            strResult += "/";
        }
    }
    return strResult;
}


/********************************************************
 *@Name:        ResponseProjectButton
 *@Author:      HuaT
 *@Description: 响应指定按钮项目名称
 *@Param1:      按钮对象
 *@Param2:      试剂卡位置
 *@Return:      无
 *@Version:     1.0
 *@Date:        2018-5-22
********************************************************/
void TestInterface::ResponseProjectButton(QPushButton *butObj, int nReagentIndex)
{
    QString strResult = "";
    QString strCurName = butObj->text();
    QString strNameRecord = ui->lb_Test_TestProject->text();
    //转换测试名称
    strResult = TransformProjectName(strCurName,strNameRecord);

    if(ui->checkBox_Test_AllSelect->isChecked() == true){
        for(int n=0; n< TABLE_ROWS_COUNT; n++){
            SetCellContent(TABLE_COLUMN_COUNT-1,n,strResult);
            //m_vecSampleInfo[n].m_TestItem.m_ReagentObj.m_nReagentPos = nReagentIndex;
        }
    }else{
        //
        int nCurRow = ui->tw_Test_TestInfo->currentRow();
        if(nCurRow == -1){
            return;
        }
        SetCellContent(TABLE_COLUMN_COUNT-1,nCurRow,strResult);
        //m_vecSampleInfo[nCurRow].m_listTestItem.m_ReagentObj.m_nReagentPos = nReagentIndex;
    }
    ui->lb_Test_TestProject->setText(strResult);
}

/********************************************************
 *@Name:        SetRuntimeButtonEnable
 *@Author:      HuaT
 *@Description: 设置测试时详细界面的按钮状态
 *@Param1:      按钮状态是否可用
 *@Return:      无
 *@Version:     1.0
 *@Date:        2018-5-22
********************************************************/
void TestInterface::SetRuntimeButtonEnable(bool isTrue)
{
    ui->pb_Test_ScanModel->setEnabled(isTrue);
    ui->pb_Test_ClearTestName->setEnabled(isTrue);

    SetSampleEditButtonEnable(isTrue);
}

void TestInterface::SetSampleEditButtonEnable(bool isTrue)
{
    ui->pb_Test_TestName1->setEnabled(isTrue);
    ui->pb_Test_TestName2->setEnabled(isTrue);
    ui->pb_Test_TestName3->setEnabled(isTrue);
    ui->pb_Test_TestName4->setEnabled(isTrue);

    ui->le_Test_SampleNumber->setEnabled(isTrue);

    ui->pb_Test_One->setEnabled(isTrue);
    ui->pb_Test_Two->setEnabled(isTrue);
    ui->pb_Test_Three->setEnabled(isTrue);
    ui->pb_Test_Four->setEnabled(isTrue);
    ui->pb_Test_Five->setEnabled(isTrue);
    ui->pb_Test_Six->setEnabled(isTrue);
    ui->pb_Test_Seven->setEnabled(isTrue);
    ui->pb_Test_Eight->setEnabled(isTrue);
    ui->pb_Test_Nine->setEnabled(isTrue);
    ui->pb_Test_Zero->setEnabled(isTrue);
    ui->pb_Test_Clear->setEnabled(isTrue);
    ui->pb_Test_OK->setEnabled(isTrue);
}

/********************************************************
 *@Name:        SetRowColor
 *@Author:      HuaT
 *@Description: 设置行颜色
 *@Param1:      行颜色对象
 *@Param2:      行号
 *@Return:      无
 *@Version:     1.0
 *@Date:        2018-5-22
********************************************************/
void TestInterface::SetRowColor(QColor color,quint8 nRow)
{
    ui->tw_Test_TestInfo->item(nRow,0)->setBackgroundColor(color);
    ui->tw_Test_TestInfo->item(nRow,1)->setBackgroundColor(color);
    ui->tw_Test_TestInfo->item(nRow,2)->setBackgroundColor(color);
}

/********************************************************
 *@Name:        GetCheckSum
 *@Author:      HuaT
 *@Description: 获取数据的校验和
 *@Param1:      数据长度
 *@Param2:      数据内容
 *@Return:      无
 *@Version:     1.0
 *@Date:        2018-5-22
********************************************************/
quint32 TestInterface::GetCheckSum(QByteArray byarrDataLength, QByteArray byarrData)
{
    //qDebug()<<byarrDataLength.toHex().toUpper();
    //qDebug()<<byarrData.toHex().toUpper();
    quint32 nCount = 0;
    for(int n=0; n<byarrDataLength.size(); n++){
        nCount += (quint8)byarrDataLength.at(n);
    }
    for(int n=0; n<byarrData.size(); n++){
        nCount += (quint8)byarrData.at(n);
    }
    //qDebug()<<nCount << ~nCount << ~nCount + 1;
    return ~nCount + 1;
}

/********************************************************
 *@Name:        IntToBytes
 *@Author:      HuaT
 *@Description: 整形转字节型
 *@Param1:      数据
 *@Return:      数据的字节形式
 *@Version:     1.0
 *@Date:        2018-5-22
********************************************************/
QByteArray TestInterface::IntToBytes(qint32 nValue, quint8 nCount)
{
    QByteArray byarrCheckSum;
    //高位在前,低位在后
    /*for(int n=nCount-1; n != -1; n--){
        byarrCheckSum.append( (quint8)( (nValue>> (n*8) ) & 0xFF) );
    }*/
    //低位在前,高位在后
    for(int n=0; n < nCount; n++){
        byarrCheckSum.append( (quint8)( (nValue>> (n*8) ) & 0xFF) );
    }
    return byarrCheckSum;
}

/********************************************************
 *@Name:        BytesToInt
 *@Author:      HuaT
 *@Description: 字节型转整型
 *@Param1:      数组数据，字节由低到高
 *@Return:      整型
 *@Version:     1.0
 *@Date:        2018-5-22
********************************************************/
quint32 TestInterface::BytesToInt(QByteArray byarr)
{
    quint32 result = 0;
    if(byarr.size() == 4){
        int a = (byarr[3] & 0xff) << 24;
        int b = (byarr[2] & 0xff) << 16;
        int c = (byarr[1] & 0xff) << 8;
        int d = (byarr[0] & 0xff);
        result = a | b | c | d;
    }else if(byarr.size() == 2){
        int a = (byarr[1] & 0xff) << 8;
        int b = (byarr[0] & 0xff);
        result = a | b;
    }
    return result;
}


/********************************************************
 *@Name:        InitVectorTestInfo
 *@Author:      HuaT
 *@Description: 初始化测试数据
 *@Param1:      无
 *@Return:      无
 *@Version:     1.0
 *@Date:        2018-5-22
********************************************************/
void TestInterface::InitVectorTestInfo()
{
    for(int n=0; n<TABLE_ROWS_COUNT; n++){
        //TestInfo ti;
        SampleObj so;
        so.m_nSamplePos = n;
        //ti.m_si.m_SamPos.m_nSamplePos = n;
        //ti.m_timer = new QTimer;
        //connect(ti.m_timer,SIGNAL(timeout()),this,SLOT(CountDown()));
        m_listSampleInfo.append(so);
        //ti.m_timer->setProperty()
    }
}

/********************************************************
 *@Name:        SendSerialData
 *@Author:      HuaT
 *@Description: 发送串口数据
 *@Param1:      串口对象
 *@Param2:      功能号
 *@Param3:      数据内容
 *@Return:      数据结构体
 *@Version:     1.0
 *@Date:        2018-6-16
********************************************************/
void TestInterface::SendSerialData(QextSerialPort *ser, quint16 nFuncIndex,QByteArray arrData)
{
    QByteArray byarrMsg;
    MsgData msgd;
    msgd.m_FuncIndex = nFuncIndex;
    msgd.m_DataLength = arrData.size()/4;
    //qDebug()<<"arrData.size()"<<arrData.size();
    msgd.m_Data = arrData;
    byarrMsg.append(IntToBytes(msgd.m_frameHeader));
    byarrMsg.append(IntToBytes(msgd.m_ProductID));
    //qDebug()<<msgd.m_ProductID;
    byarrMsg.append(IntToBytes(msgd.m_DeviceAdd,2));
    byarrMsg.append(IntToBytes(msgd.m_FuncIndex,2));
    byarrMsg.append(IntToBytes(msgd.m_DataLength));
    //debug output
    DebugOutput(msgd.m_FuncIndex,msgd.m_Data,"send");
    //数据插入
    byarrMsg.append(msgd.m_Data);
    byarrMsg.append(IntToBytes(GetCheckSum(IntToBytes(msgd.m_DataLength),msgd.m_Data)));
    //qDebug()<<GetCheckSum(IntToBytes(msgd.m_DataLength),msgd.m_Data);
    byarrMsg.append(IntToBytes(msgd.m_frameEnd));
    ser->write(byarrMsg);
    //qDebug()<<"Send-Data-Full:"<<byarrMsg.toHex().toUpper();
}

void TestInterface::DebugOutput(quint16 nFuncIndex, QByteArray byarrContentData, QString type){
    QString strResult;
    if(type == "send"){
        strResult = "Send-Data:";
    }else if(type == "recvControl"){
        strResult = "Recv-Control-Data:";
    }else if(type == "recvCheck"){
        strResult = "Recv-Check-Data:";
    }else{
        strResult = type;
    }
    strResult = strResult + QString(" m_FuncIndex: %1 /").arg(nFuncIndex);
    strResult = strResult + QString(" m_DataLength: %1 /").arg(byarrContentData.size()/4);
    strResult = strResult + QString(" m_Data: %1 ").arg( QString(byarrContentData.toHex().toUpper()) );
    qDebug()<<strResult;
}


/*
 *详细页测试项目1按钮响应事件
 */
void TestInterface::on_pb_Test_TestName1_clicked()
{
    ResponseProjectButton(ui->pb_Test_TestName1,0);
}

/*
 *详细页测试项目2按钮响应事件
 */
void TestInterface::on_pb_Test_TestName2_clicked()
{
    ResponseProjectButton(ui->pb_Test_TestName2,1);
}

/*
 *详细页测试项目3按钮响应事件
 */
void TestInterface::on_pb_Test_TestName3_clicked()
{
    ResponseProjectButton(ui->pb_Test_TestName3,2);
}

/*
 *详细页测试项目4按钮响应事件
 */
void TestInterface::on_pb_Test_TestName4_clicked()
{
    ResponseProjectButton(ui->pb_Test_TestName4,3);
}

/*
 *详细页数字1响应事件
 */
void TestInterface::on_pb_Test_One_clicked()
{
    QString strCur = ui->le_Test_SampleNumber->text();
    ui->le_Test_SampleNumber->setText(strCur+"1");
}

/*
 *详细页数字2响应事件
 */
void TestInterface::on_pb_Test_Two_clicked()
{
    QString strCur = ui->le_Test_SampleNumber->text();
    ui->le_Test_SampleNumber->setText(strCur+"2");
}

/*
 *详细页数字3响应事件
 */
void TestInterface::on_pb_Test_Three_clicked()
{
    QString strCur = ui->le_Test_SampleNumber->text();
    ui->le_Test_SampleNumber->setText(strCur+"3");
}

/*
 *详细页数字4响应事件
 */
void TestInterface::on_pb_Test_Four_clicked()
{
    QString strCur = ui->le_Test_SampleNumber->text();
    ui->le_Test_SampleNumber->setText(strCur+"4");
}

/*
 *详细页数字5响应事件
 */
void TestInterface::on_pb_Test_Five_clicked()
{
    QString strCur = ui->le_Test_SampleNumber->text();
    ui->le_Test_SampleNumber->setText(strCur+"5");
}

/*
 *详细页数字6响应事件
 */
void TestInterface::on_pb_Test_Six_clicked()
{
    QString strCur = ui->le_Test_SampleNumber->text();
    ui->le_Test_SampleNumber->setText(strCur+"6");
}

/*
 *详细页数字7响应事件
 */
void TestInterface::on_pb_Test_Seven_clicked()
{
    QString strCur = ui->le_Test_SampleNumber->text();
    ui->le_Test_SampleNumber->setText(strCur+"7");
}

/*
 *详细页数字8响应事件
 */
void TestInterface::on_pb_Test_Eight_clicked()
{
    QString strCur = ui->le_Test_SampleNumber->text();
    ui->le_Test_SampleNumber->setText(strCur+"8");
}

/*
 *详细页数字9响应事件
 */
void TestInterface::on_pb_Test_Nine_clicked()
{
    QString strCur = ui->le_Test_SampleNumber->text();
    ui->le_Test_SampleNumber->setText(strCur+"9");
}

/*
 *详细页清除按钮响应事件
 */
void TestInterface::on_pb_Test_Clear_clicked()
{
    ui->le_Test_SampleNumber->setText("");
    if(ui->checkBox_Test_AllNumber->isChecked()){
        for(int n=0; n<16; n++){
            ui->tw_Test_TestInfo->item(n,1 )->setText("");
        }
    }else{
        ui->tw_Test_TestInfo->item(ui->lb_Test_CurrentSamPos->text().toInt()-1,1 )->setText("");
    }

}

/*
 *详细页数字0响应事件
 */
void TestInterface::on_pb_Test_Zero_clicked()
{
    QString strCur = ui->le_Test_SampleNumber->text();
    ui->le_Test_SampleNumber->setText(strCur+"0");
}

/*
 *详细页确定按钮响应事件
 */
void TestInterface::on_pb_Test_OK_clicked()
{
    QString strCur = ui->le_Test_SampleNumber->text();
    if(strCur.isEmpty() || strCur.trimmed().isEmpty()){
        return;
    }
    int nCurRow = ui->tw_Test_TestInfo->currentRow();
    if(nCurRow == -1){
        MsgBoxDlg msgbox;
        msgbox.ShowMsg(QObject::tr("请先选中行!"),0);
        msgbox.exec();
    }else{
        SetCellContent(TABLE_COLUMN_COUNT-2,nCurRow,strCur);
        ui->tw_Test_TestInfo->selectRow(nCurRow+1);
        ui->le_Test_SampleNumber->setText("");
        if(nCurRow+2>16){
            ui->lb_Test_CurrentSamPos->setText(QString::number(nCurRow+1));
        }else{
            ui->lb_Test_CurrentSamPos->setText(QString::number(nCurRow+2));
        }
    }
}

/*
 *详细页列表点击响应事件
 */
void TestInterface::on_tw_Test_TestInfo_cellClicked(int row, int column)
{
    QTableWidgetItem* itemPos = ui->tw_Test_TestInfo->item(row,TABLE_COLUMN_COUNT-3);
    QTableWidgetItem* itemNum = ui->tw_Test_TestInfo->item(row,TABLE_COLUMN_COUNT-2);
    QTableWidgetItem* itemName = ui->tw_Test_TestInfo->item(row,TABLE_COLUMN_COUNT-1);
    ui->lb_Test_TestProject->setText(itemName->text());
    ui->le_Test_SampleNumber->setText(itemNum->text());
    ui->lb_Test_CurrentSamPos->setText(itemPos->text());

    //急诊位置
//    int nEmerPos = ui->lb_Test_CurrentEmerPos->text().toInt();
//    if(nEmerPos == row+1){
//        ui->checkBox_Test_Emergency->setChecked(true);
//    }else{
//        ui->checkBox_Test_Emergency->setChecked(false);
//    }

    if(m_listWaitAddSampleQueue.isEmpty() && m_bAddSampleStatus == false){
        SetSampleEditButtonEnable(true);
    }else{
        SetSampleEditButtonEnable(false);
    }
    ui->le_Test_SampleNumber->setFocus();
}

/*
 *急诊选项点击响应事件
 */
//void TestInterface::on_checkBox_Test_Emergency_clicked(bool checked)
//{
//    int nRow = ui->tw_Test_TestInfo->currentRow();
//    if(nRow < 0){
//        QMessageBox::information(this,QObject::tr("提示"),QObject::tr("请先选择急诊对象"),QMessageBox::Ok);
//        return;
//    }
//    //急诊
//    if(ui->checkBox_Test_Emergency->isChecked()){
//        for(int n=0; n<TABLE_ROWS_COUNT; n++){
//            SetRowColor(QColor(255,255,255),n);
//        }
//        SetRowColor(QColor(0,255,0),nRow);
//        //显示当前的急诊行
//        ui->lb_Test_CurrentEmerPos->setText(QString("%1").arg(nRow+1));
//    }//取消急诊
//    else{
//        SetRowColor(QColor(255,255,255),nRow);
//        //当前无急诊
//        ui->lb_Test_CurrentEmerPos->setText(QObject::tr("无"));
//    }

//}

/*
 *插入急诊点击事件
 */
void TestInterface::on_pb_Test_InsertEmer_clicked()
{
    QString strBtName = ui->pb_Test_InsertEmer->text();
    QString strTitle,strContent;
    strTitle = QObject::tr("Note");
    if(strBtName.compare( QObject::tr("插入急诊")) == 0){
        //检测16号位是否可用。
        for(int i=0; i<m_listSampleInfo.size(); i++){
            if(m_listSampleInfo.at(i).m_nSamplePos == 15){
                strContent = QObject::tr("%1号急诊位已被使用,请等待测试完成!").arg(EMER_No);
                MsgBoxDlg msgbox;
                msgbox.ShowMsg(strContent,0);
                msgbox.exec();
                return;
            }
        }
        //如果可用则发送停止发送下一个样本信息
        m_bEmerStatus = true;
        //提示
        strContent = QObject::tr("请在加样针停下后,将急诊样本放入%1号样本位,然后选择测试类型(可点击自动扫码或手动输入样本编码!)").arg(EMER_No);
        MsgBoxDlg msgbox;
        msgbox.ShowMsg(strContent,0);
        msgbox.exec();
        ui->pb_Test_InsertEmer->setText(QObject::tr("继续测试"));
    }else if(strBtName.compare( QObject::tr("继续测试")) == 0){
        //判断输入的数据是否正确
        int nCount = ui->lb_Test_SampleCount->text().toInt();
        QString strSampleNo;
        QString strItemName;
        strSampleNo = ui->tw_Test_TestInfo->item(15,TABLE_COLUMN_COUNT-2)->text();
        strItemName = ui->tw_Test_TestInfo->item(15,TABLE_COLUMN_COUNT-1)->text();
        if(!strSampleNo.isEmpty() && !strItemName.isEmpty()){
            SampleObj so;
            so.m_strSampleID = strSampleNo;
            so.m_nSamplePos = 15;
            //是否有样本
            so.m_bIsEmpty = false;
            m_listSampleInfo.append(so);
            QStringList listItemName = strItemName.split("/");
            for(int i=0; i<listItemName.count(); i++){
                //逐个添加项目
                AddSampleTestItemInfo(15,strSampleNo,listItemName.at(i),m_listSampleInfo);
                ui->lb_Test_SampleCount->setText(QString("%1").arg(nCount++));
            }
            //m_vecSampleInfo[n].m_TestItem.m_strItemName = strItemName;
        }else{
            //提示
            strContent = QObject::tr("条码和测试类型不能为空");
            MsgBoxDlg msgbox;
            msgbox.ShowMsg(strContent,0);
            msgbox.exec();
            return;
        }

        //如果正确，填入16号样本数组中，并发送16号测试数据
        PacketTestInfoToQueue(m_listSampleInfo,true);

        //急诊插入完成
        m_bEmerStatus = false;
        //改变文本
        ui->pb_Test_InsertEmer->setText(QObject::tr("插入急诊"));
        ui->pb_Test_InsertEmer->setEnabled(false);

    }
}


//解析条码数据
/********************************************************
 *@Name:        ParseScanBarCodeInfo
 *@Author:      HuaT
 *@Description: 解析条码数据
 *@Param1:      条码数据数组,数据格式为：16字节char，加4字节状态
 *@Return:      无
 *@Version:     1.0
 *@Date:        2018-6-16
********************************************************/
void TestInterface::ParseScanBarCodeInfo(const QByteArray &ScanData, bool bEmer)
{
    qDebug()<<ScanData.size();
    if(ScanData.size() != 320 && ScanData.size() != 20){
        return;
    }
    ScanBarCodeInfo barcode;
    Scan_Info_t singleBarcode;
    if(bEmer){
        memcpy(&singleBarcode, ScanData, ScanData.size());
        //qDebug()<<sizeof(ScanBarCodeInfo);
        QString strBarCode;
        //qDebug()<<barcode.scaninfo[n].Scan_State;
        if(singleBarcode.Scan_State == 2){
            for(int i=0; i<BARCODELENGTH; i++){
                if(static_cast<quint8>(singleBarcode.CODE[i]) == 0){
                    break;
                }
                strBarCode += singleBarcode.CODE[i];
            }
        }
        //qDebug()<<strBarCode;
        ui->tw_Test_TestInfo->item(15,1)->setText(strBarCode.trimmed());
//        for(int i = 0; i<m_listSampleInfo.size(); i++){
//            if(m_listSampleInfo.at(i).m_nSamplePos == 15){
//                m_listSampleInfo[i].m_strSampleID = strBarCode.trimmed();
//            }
//        }
    }else{
        memcpy(&barcode, ScanData, ScanData.size());
        //qDebug()<<sizeof(ScanBarCodeInfo);
        QString strBarCode;
        for(int n=0; n<SAMPLECOUNT; n++){
            strBarCode.clear();
            //qDebug()<<barcode.scaninfo[n].Scan_State;
            if(barcode.scaninfo[n].Scan_State == 2){
                //strBarCode.clear();
                for(int i=0; i<BARCODELENGTH; i++){
                    if(static_cast<quint8>(barcode.scaninfo[n].CODE[i]) == 0){
                        break;
                    }
                    strBarCode += barcode.scaninfo[n].CODE[i];
                }
                ui->tw_Test_TestInfo->item(n,1)->setText(strBarCode.trimmed());
            }
        }
    }
}



/********************************************************
 *@Name:        ShowBarCodeInfo
 *@Author:      HuaT
 *@Description: 显示条码信息
 *@Param1:      条码信息数组
 *@Return:      无
 *@Version:     1.0
 *@Date:        2018-6-16
********************************************************/
void TestInterface::ShowBarCodeInfo(QList<SampleObj> listSampleInfo)
{
    for(int n=0; n<listSampleInfo.size(); n++){
        /////////////////////////////////////////////感觉有问题///////////////////////////////////////////////
        ui->tw_Test_TestInfo->item(n,0)->setText(QString::number(listSampleInfo[n].m_nSamplePos));
        /////////////////////////////////////////////////////////////////////////////////////////////////////
        ui->tw_Test_TestInfo->item(n,1)->setText(listSampleInfo[n].m_strSampleID);
        //ui->tw_Test_TestInfo->item(n,2)->setText(vecSampleInfo[n].m_TestItem.m_strItemName);
        //qDebug()<<"1:"<<(vecTestInfo[n].m_nSamplePos)<<" 2:"<<vecTestInfo[n].m_strSampleNo<<" 3:"<<vecTestInfo[n].m_strItemName;
    }
}


/********************************************************
 *@Name:        ParseLoadSampleInfo
 *@Author:      HuaT
 *@Description: 解析加样信息
 *@Param1:      加样信息数组
 *@Param2:      待填充结构体
 *@Return:      返回数据解析是否正确
 *@Version:     1.0
 *@Date:        2018-6-16
********************************************************/
bool TestInterface::ParseLoadSampleInfo(const QByteArray &SampleData, quint32 &SamplePos, quint32 &TestPos)
{
    /*if(SampleData.size() != 16){
        qDebug()<<"ParseLoadSampleInfo:Short Current Length is "<<SampleData.size();
        return false;
    }
    SamplePos = BytesToInt(SampleData.mid(0,4));
    qDebug()<<"ParseLoadSampleInfo-SamplePos:"<<SamplePos;
    TestPos = BytesToInt(SampleData.mid(4,4));

    //ID卡槽位置
    quint8 IDCardPos = BytesToInt(SampleData.mid(8,4));
    //ID卡剩余总数
    quint8 IDCardCount = BytesToInt(SampleData.mid(12,4));
    //更新当前剩余数量，
    if(!UpdateIDCardInfo(IDCardPos,IDCardCount)){
        return false;
    }

    //记录样本位对应的试剂位
    if(SamplePos.m_SamPos.m_nReagentPos == 0){
        m_vecTestInfo[SamplePos.m_SamPos.m_nSamplePos].m_si.m_SamPos.m_nReagentPos = -1;
    }else{
        m_vecTestInfo[SamplePos.m_SamPos.m_nSamplePos].m_si.m_SamPos.m_nReagentPos = SamplePos.m_SamPos.m_nReagentPos;
    }*/

    return true;
}

/********************************************************
 *@Name:        UpdateIDCardInfo
 *@Author:      HuaT
 *@Description: 更新当前ID卡数量,并保存至文件
 *@Param1:      对应卡槽位置1-4
 *@Param2:      卡槽位置剩余卡数量
 *@Return:      更新是否成功
 *@Version:     1.0
 *@Date:        2018-6-16
********************************************************/
bool TestInterface::UpdateIDCardInfo(quint32 nIDCardPos, quint8 nIDCardAmount, QString strTestItem)
{
    switch(nIDCardPos){
    case 0:
        ui->proBar_Test_Reagent1->setValue(nIDCardAmount);
        ui->lb_Test_ReagentName1->setText(strTestItem);
        //m_settins->SetParam(REAGENTAMOUNT1,QString::number(nIDCardAmount));
        break;
    case 1:
        ui->proBar_Test_Reagent2->setValue(nIDCardAmount);
        ui->lb_Test_ReagentName2->setText(strTestItem);
        //m_settins->SetParam(REAGENTAMOUNT2,QString::number(nIDCardAmount));
        break;
    case 2:
        ui->proBar_Test_Reagent3->setValue(nIDCardAmount);
        ui->lb_Test_ReagentName3->setText(strTestItem);
        //m_settins->SetParam(REAGENTAMOUNT3,QString::number(nIDCardAmount));
        break;
    case 3:
        ui->proBar_Test_Reagent4->setValue(nIDCardAmount);
        ui->lb_Test_ReagentName4->setText(strTestItem);
        //m_settins->SetParam(REAGENTAMOUNT4,QString::number(nIDCardAmount));
        break;
    default:
        qDebug()<<"UpdateIDCardInfo:IDCardPos is error type-"<<nIDCardPos;
        return false;
    }
    //写入配置文件
    //m_settins->WriteSettingsInfoToMap();
    return true;
}

bool TestInterface::UpdateIDCardInfo(quint32 nIDCardPos, quint8 nIDCardAmount)
{
    switch(nIDCardPos){
    case 0:
        ui->proBar_Test_Reagent1->setValue(nIDCardAmount);
        break;
    case 1:
        ui->proBar_Test_Reagent2->setValue(nIDCardAmount);
        break;
    case 2:
        ui->proBar_Test_Reagent3->setValue(nIDCardAmount);
        break;
    case 3:
        ui->proBar_Test_Reagent4->setValue(nIDCardAmount);
        break;
    default:
        qDebug()<<"UpdateIDCardInfo:IDCardPos is error type-"<<nIDCardPos;
        return false;
    }
    return true;

}

/********************************************************
 *@Name:        ProceStartLoadSample
 *@Author:      HuaT
 *@Description: 处理加样完成信息，开始计时，并发送下一个测试给仪器
 *@Param1:      加样信息数组
 *@Param2:      待填充结构体
 *@Return:      无
 *@Version:     1.0
 *@Date:        2018-6-16
********************************************************/
void TestInterface::ProceStartLoadSample(const QByteArray &SampleData)
{
    ReagentObj ro;
    //解析仪器发来数据
    if( SampleData.size() != 16 ){
        QMessageBox::information(this,QObject::tr("Note"),QObject::tr("解析加样信息错误"),QMessageBox::Ok);
        return;
    }
    quint32 SamplePos = BytesToInt(SampleData.mid(0,4));
    qint32 TestPos = BytesToInt(SampleData.mid(4,4));
    quint32 IdCardPos = BytesToInt(SampleData.mid(8,4));
    qDebug()<<"ParseLoadSampleInfo-SamplePos:"<<SamplePos<<" TestPos:"<<TestPos;
    if(TestPos == -1){
        //清除本次样本数据
        for(int n=0; n<m_listSampleInfo.size(); n++){
            if( (m_listSampleInfo.at(n).m_nSamplePos == SamplePos) ){
                //打印错误信息
                QString strErrMsg = QString("样本%1 %2").arg(SamplePos+1).arg(m_strCurErrMsg);
                ShowLogText(ui->tb_Test_RunInfoShow,strErrMsg,LOGERROR);
                m_strCurErrMsg.clear();
                if(IdCardPos>=0 && IdCardPos<=3){
                    m_mapIDCardInfo[QString::number(IdCardPos)].m_nRemaining++;
                }
                //发生错误，清洗液减0.5
                UpdateCleanoutValue(0.5);
                UpdateFluidValue(0.5);
                bool bTesting = false;
                for(int k=0; k<m_listSampleInfo.at(n).m_listTestItem.size(); k++){
                    if(m_listSampleInfo.at(n).m_listTestItem.at(k).m_timer->isActive() == true){
                        bTesting = true;
                    }
                }
                if(bTesting == false){
                    m_listSampleInfo.removeAt(n);
                    if( 0 == m_listSampleInfo.size()){
                        AllTestComplete();
                    }else{
                        //去除试剂盘状态
                        //m_SampleCircle->SetItemColor(SamplePos+1,NORMALSTATUS);
                        //样本盘
                        m_TestCircle->SetItemColor(SamplePos+1,ADDSAMPLEFAIL);
                        if(m_listWaitAddSampleQueue.isEmpty()){
                            //可以开启新一轮测试
                            AllAddSampleComplete();
                        }
                        //发送下一条吸样命令
                        if(m_bIsCheckStatus == false){
                            //如果等待测试队列为空
                            if(m_listWaitTestQueue.isEmpty()){
                                //发送下一个加样测试给仪器
                                SendWaitAddSampleOrder();
                            }else{
                                //发送待检测测试给仪器
                                SendWaitCheckOrder();
                            }
                        }else{
                            //加样状态关闭
                            m_bAddSampleStatus = false;
                        }
                    }
                }else{
                    qDebug()<<"ParseLoadSampleInfo-TestPos=-1,bTesting=true";
                }
            }
        }
        return;
    }
    //ID卡槽位置
    quint32 IDCardPos = BytesToInt(SampleData.mid(8,4));
    //ID卡剩余总数
    quint32 IDCardCount = BytesToInt(SampleData.mid(12,4));
    //更新当前剩余数量，
    UpdateIDCardInfo(IDCardPos,IDCardCount);
    m_mapIDCardInfo[QString::number(IDCardPos)].m_nRemaining = IDCardCount;
    //更新清洗液数量
    UpdateCleanoutValue(1);
    UpdateFluidValue(1);

    //记录样本位对应的项目测试位
    int nTestIndex = -1;
    int nSampleIndex = -1;
    for(int i = 0; i<m_listSampleInfo.size(); i++){
        if(m_listSampleInfo.at(i).m_nSamplePos == SamplePos ){
            //查找样本
            for(int n=0; n<m_listSampleInfo[i].m_listTestItem.size(); n++){
                //查找测试项目
                if(m_listSampleInfo.at(i).m_listTestItem.at(n).m_ReagentObj.m_nReagentPos == IDCardPos &&
                        m_listSampleInfo.at(i).m_listTestItem.at(n).m_nCurRunStatus == 1 ){
                    m_listSampleInfo[i].m_listTestItem[n].m_nItemTestPos = TestPos;
                    nTestIndex = n;
                    nSampleIndex = i;
                    break;
                }
            }
        }
    }

    //改变试剂盘颜色
    if(TestPos >= 0){
        //试剂盘颜色改变
        m_SampleCircle->SetItemColor(TestPos+1,INCUBATESTATUS);
        //计时时间
        qDebug()<<"ProceStartLoadSample-nSampleIndex:"<<nSampleIndex<<",nTestIndex:"<<nTestIndex;
        quint8 nRemainTimeHigh = 0;
        quint8 nRemainTimeLow = 0;
        unsigned short nRemainTime = 30;
        QByteArray byarrIDCardData = m_listSampleInfo[nSampleIndex].m_listTestItem[nTestIndex].m_ReagentObj.m_byarrIDCardData;
        GetIDCardPosData(byarrIDCardData,20,nRemainTimeHigh);
        GetIDCardPosData(byarrIDCardData,21,nRemainTimeLow);
        //孵育时间
        nRemainTime = nRemainTimeHigh * 256 + nRemainTimeLow;
        //对应试剂位置开始计时
        if(m_bDebugMode == true){
            int nTempTime = m_settins->GetParam(DEBUGTESTTIME).toInt();
            if(nTempTime > 0){
                nRemainTime = nTempTime;
            }
        }
        m_listSampleInfo[nSampleIndex].m_listTestItem[nTestIndex].m_nItemRemainTime = nRemainTime;
        m_listSampleInfo[nSampleIndex].m_listTestItem[nTestIndex].m_nResultWaitTime = 30;
        m_listSampleInfo.at(nSampleIndex).m_listTestItem[nTestIndex].m_timer->setProperty("ReagentPos",TestPos);
        m_listSampleInfo.at(nSampleIndex).m_listTestItem[nTestIndex].m_timer->setProperty("SamplePos",SamplePos);
        m_listSampleInfo.at(nSampleIndex).m_listTestItem[nTestIndex].m_timer->start(1000);
        m_listSampleInfo[nSampleIndex].m_listTestItem[nTestIndex].m_nCurRunStatus = 2;
        //日志开始
        QString strLog;
        strLog = QString(ADDSAMPLECOMPLETE).arg(SamplePos+1).arg(TestPos+1).arg(nRemainTime);
        ShowLogText(ui->tb_Test_RunInfoShow,strLog,LOGNORMAL);
    }else{//如果没有加到样
        bool bContinue = false;
        bContinue = CheckTestComplete();
        //如果无继续项目,清空数据
        if(!bContinue){
            AllTestComplete();
            return;
        }
    }
    //加样状态关闭
    m_bAddSampleStatus = false;

    //查找是否加样完成
    bool bAddSamComplete = true;
    for(int n=0; n<m_listSampleInfo[nSampleIndex].m_listTestItem.size(); n++){
        if(m_listSampleInfo[nSampleIndex].m_listTestItem.at(n).m_timer->isActive() == false){
            bAddSamComplete = false;
            break;
        }
    }
    //改变样本盘颜色
    if(bAddSamComplete == true){
        m_TestCircle->SetItemColor( SamplePos +1,COMPLETESTATUS);
    }

    //如果在检测状态中，不发送
    if(m_bCheckStatus == true){
        return;
    }
    if(m_listWaitAddSampleQueue.isEmpty()){
        //可以开启新一轮测试
        AllAddSampleComplete();
    }
    //如果等待测试队列为空
    if(m_listWaitTestQueue.isEmpty()){
        //发送下一个加样测试给仪器
        SendWaitAddSampleOrder();
    }else{
        //发送待检测测试给仪器
        SendWaitCheckOrder();
    }
}

void TestInterface::SendWaitAddSampleOrder(){
    //发送下一个加样测试给仪器
    QByteArray byarrTestInfo = GetWaitAddSampleInfoToQueue();
    //如果有等待加样
    if(!byarrTestInfo.isEmpty()){
        int nSamplePos = BytesToInt(byarrTestInfo.mid(4,4));
        QString strLog = QString(RUNADDSAMPLE).arg(nSamplePos+1);
        ShowLogText(ui->tb_Test_RunInfoShow,strLog,LOGNORMAL);
        SendSerialData(m_ControlPanelsSerial,PL_FUN_AM_START_SAMPLE,byarrTestInfo);
        //加样状态开启
        m_bAddSampleStatus = true;
    }else{
        m_bAddSampleStatus = false;
    }
}

void TestInterface::SendWaitCheckOrder(){
    //发送待检测测试给仪器
    QByteArray byarrData = GetWaitCheckInfoToQueue();
    if(!byarrData.isEmpty()){
        //
        int nSamplePos = BytesToInt(byarrData.mid(0,4));
        int nReagentPos = BytesToInt(byarrData.mid(4,4));
        for(int n=0; n<m_listSampleInfo.size(); n++){
            if(m_listSampleInfo.at(n).m_nSamplePos == nSamplePos){
                for(int i=0; i<m_listSampleInfo.at(n).m_listTestItem.size(); i++){
                    if(m_listSampleInfo.at(n).m_listTestItem.at(i).m_nItemTestPos == nReagentPos){
                        m_listSampleInfo[n].m_listTestItem[i].m_bResultWaitState = true;
                        break;
                    }
                }
            }
        }
        //
        SendSerialData(m_ControlPanelsSerial,PL_FUN_AM_PREPARE_TEST,byarrData);
        //检测状态开启
        m_bCheckStatus = true;
        m_bAddSampleStatus = false;
    }else{
        m_bCheckStatus = false;
        m_bAddSampleStatus = false;
    }
}

/********************************************************
 *@Name:        CheckTestComplete
 *@Author:      HuaT
 *@Description: 检测所有测试是否完成
 *@Param1:      无
 *@Return:      无
 *@Version:     1.0
 *@Date:        2019-1-10
********************************************************/
bool TestInterface::CheckTestComplete()
{
    bool isComplete = true;
    if(m_listSampleInfo.size() != 0){
        isComplete = false;
    }
    return isComplete;
}


/********************************************************
 *@Name:        PorceLoadSampleComplete
 *@Author:      HuaT
 *@Description: 处理准备测试完成返回数据
 *@Param1:      ACK数据
 *@Return:      无
 *@Version:     1.0
 *@Date:        2018-6-16
********************************************************/
void TestInterface::PorceLoadSampleComplete(const QByteArray &AckData)
{
    //解析返回数据
    SamplePos sp;
    memcpy(&sp, AckData,AckData.size());
    qDebug()<<"收到准备测试完成:"<<"samplepos->"<<sp.m_nSamplePos<<",reagentpos->"<<sp.m_nReagentPos;
    //发送开始测试信号
    QByteArray byarrData;
    //样本位置
    byarrData.append(IntToBytes(sp.m_nSamplePos));
    //测试试剂位置
    byarrData.append(IntToBytes(sp.m_nReagentPos));
    //扫描起始点,36
    QByteArray byarrIDCardData;
    for(int i=0; i<m_listSampleInfo.size(); i++){
        for(int n = 0; n<m_listSampleInfo.at(i).m_listTestItem.size(); n++){
            if(m_listSampleInfo.at(i).m_nSamplePos == sp.m_nSamplePos && m_listSampleInfo.at(i).m_listTestItem.at(n).m_nItemTestPos == sp.m_nReagentPos){
                byarrIDCardData = m_listSampleInfo.at(i).m_listTestItem.at(n).m_ReagentObj.m_byarrIDCardData;
                quint8 nScanPoint = static_cast<quint8>( byarrIDCardData.at(36) );
                //放大倍数,22
                quint8 nAmpParam = static_cast<quint8>(byarrIDCardData.at(22));
                if(m_bDebugMode == true){
                    int nDebugScanStart = m_settins->GetParam(DEBUGSCANSTART).toInt();
                    if(nDebugScanStart > 0){
                        nScanPoint = static_cast<quint8>(nDebugScanStart);
                    }
                    int nDebugAmpParam = m_settins->GetParam(DEBUGAMPPARAM).toInt();
                    if(nDebugAmpParam > 0){
                        nAmpParam = static_cast<quint8>(nDebugAmpParam);
                    }
                }
                //nScanPoint = 100;
                byarrData.append(IntToBytes(nScanPoint) );
                m_listSampleInfo[i].m_listTestItem[n].m_rawTestInfo.m_nScanStart = nScanPoint;
                //nAmpParam = 0;
                byarrData.append(IntToBytes(nAmpParam) );
                m_listSampleInfo[i].m_listTestItem[n].m_rawTestInfo.m_nAmpParam = nAmpParam;
                SendSerialData(m_CheckPanelsSerial,PL_FUN_AM_START_TEST,byarrData);
            }
        }
    }
}

/********************************************************
 *@Name:        ParseCheckPanelsAck
 *@Author:      HuaT
 *@Description: 解析检测板返回的ACK
 *@Param1:      ACK数据
 *@Return:      无
 *@Version:     1.0
 *@Date:        2018-6-26
********************************************************/
void TestInterface::ParseCheckPanelsAck(const QByteArray &AckData)
{
    if(AckData.size() != 16){
        qDebug()<<"ParseCheckPanelsAck - The length is not enough:"<<AckData.size();
        return ;
    }
    quint32 nFunIndex  = BytesToInt(AckData.mid(0,4));
    quint32 nErrType   = BytesToInt(AckData.mid(4,4));
    quint32 nStatusDef = BytesToInt(AckData.mid(8,4));
    switch(nFunIndex){
    case PL_FUN_AM_HARDWEAR_CHECK:
        if(nErrType == 0){

        }else if(nErrType == 1){
            qDebug()<<"命令："<<nFunIndex<<";数据错误";
        }else if(nErrType == 2){
            qDebug()<<"命令："<<nFunIndex<<";检验和错误";
        }
        //状态定义
        if(nStatusDef == 0){
            //启动定时器，发送命令给控制板，盘可以转动
            m_TimerTestCheck.stop();
            connect(&m_TimerControlCheck,SIGNAL(timeout()), this, SLOT(ControlCheckFunc()) );
            m_TimerControlCheck.start(1000);
        }
        break;
    case PL_FUN_AM_NEW_TEST:
        //新建测试
        break;
    case PL_FUN_AM_START_SAMPLE:
        //开始加样
        break;
    case PL_FUN_AM_PREPARE_TEST:
        //准备测试
        break;
    case PL_FUN_AM_START_TEST :
        //开始测试
        break;
    case PL_FUN_AM_PREPARE_UNSTALL:
        //准备卸卡
        break;
    case PL_FUN_AM_START_UNSTALL:
        //开始卸卡
        break;
    default:
        break;
    }
}

/********************************************************
 *@Name:        ParseControlPanelsAck
 *@Author:      HuaT
 *@Description: 解析控制板返回的ACK
 *@Param1:      ACK数据
 *@Return:      无
 *@Version:     1.0
 *@Date:        2018-6-26
********************************************************/
void TestInterface::ParseControlPanelsAck(const QByteArray& AckData)
{
    if(AckData.size() != 16){
        qDebug()<<"ParseControlPanelsAck - The length is not enough:"<<AckData.size();
        return ;
    }
    quint32 nFunIndex  = BytesToInt(AckData.mid(0,4));
    quint32 nErrType   = BytesToInt(AckData.mid(4,4));
    quint32 nStatusDef = BytesToInt(AckData.mid(8,4));

    //qDebug()<<"m_byarrUnstallCard:"<<m_byarrUnstallCard;
    switch(nFunIndex){
    case PL_FUN_AM_HARDWEAR_CHECK:
        if(nErrType == 0){

        }else if(nErrType == 1){
            qDebug()<<"命令："<<nFunIndex<<";数据错误";
        }else if(nErrType == 2){
            qDebug()<<"命令："<<nFunIndex<<";检验和错误";
        }
        //状态定义
        if(nStatusDef == 0){
            //停止控制查询命令
            m_TimerControlCheck.stop();
            //自检状态
            m_bIsCheckStatus = true;
            //解析卸卡数据
            QString strUnstalValue = QString("%1").arg( BytesToInt(AckData.mid(12,4)) ,32,2,QLatin1Char('0'));
            if(m_byarrUnstallCard.isEmpty()){
                m_byarrUnstallCard = strUnstalValue.toAscii();
            }
            ParseUnstallCardData();
        }else if(nStatusDef == 1){

        }
        break;
    case PL_FUN_AM_NEW_TEST:
        //新建测试
        break;
    case PL_FUN_AM_START_SAMPLE:
        //开始加样
        break;
    case PL_FUN_AM_PREPARE_TEST:
        //准备测试
        break;
    case PL_FUN_AM_START_TEST :
        //开始测试
        break;
    case PL_FUN_AM_PREPARE_UNSTALL:
        //准备卸卡
        break;
    case PL_FUN_AM_START_UNSTALL:
        //开始卸卡
        break;
    default:
        break;
    }
}

/********************************************************
 *@Name:        ParseUnstallCardData
 *@Author:      HuaT
 *@Description: 解析卸卡队列数据
 *@Param1:      无
 *@Return:      无
 *@Version:     1.0
 *@Date:        2018-6-16
********************************************************/
void TestInterface::ParseUnstallCardData()
{
    //qDebug()<<"m_byarrUnstallCard:"<<m_byarrUnstallCard;
    for(int n=31,i=1; n!=11; n--,i++){
        if((quint8)m_byarrUnstallCard[n] == 49){
            m_SampleCircle->SetItemText(i,"退卡中");
            m_SampleCircle->SetItemColor(i,COMPLETESTATUS);
        }
    }
    for(int n=31,i=0; n!=11&&i<20; n--,i++){
        if( (quint8)m_byarrUnstallCard[n] == 49 ){
            //开始卸卡
            QByteArray byarrUnstallNum;
            byarrUnstallNum = IntToBytes(i);
            SendSerialData(m_ControlPanelsSerial,PL_FUN_AM_PREPARE_UNSTALL,byarrUnstallNum);
            m_byarrUnstallCard[n] = 48;
            //qDebug()<<n<<m_byarrUnstallCard;
            return;
        }
    }
    //所有卡卸载完后，自检完成
    m_bIsCheckStatus = false;
    //
    m_byarrUnstallCard.clear();
    //改变样品圆颜色
    DefaultSampleCircleUI();
    //关闭自检提示
    static_cast<MainWindow*>(m_pMainWnd)->SetCheckStatusHide(true);
    //新建测试可用
    ui->pb_Test_NewTest->setEnabled(true);
    ui->pb_Test_DetailsInfo->setEnabled(true);

}


/********************************************************
 *@Name:        ProceCountDownEvent
 *@Author:      HuaT
 *@Description: 试剂计时器处理函数
 *@Param1:      本次倒计时的时间
 *@Param2:      试剂的位置
 *@Param3:      样本的位置
 *@Return:      无
 *@Version:     1.0
 *@Date:        2018-6-16
********************************************************/
void TestInterface::ProceCountDownEvent(quint8 nReagentIndex,quint8 nSamplePos)
{
    //如果没有获得倒计时时间
    int nTestPos = -1;
    int nSampleIndex = -1;
    for(int i = 0; i<m_listSampleInfo.size(); i++){
        if(m_listSampleInfo.at(i).m_nSamplePos == nSamplePos){
            for(int n=0; n<m_listSampleInfo.at(i).m_listTestItem.size(); n++){
                if(m_listSampleInfo.at(i).m_listTestItem.at(n).m_nItemTestPos == nReagentIndex){
                    nTestPos = n;
                    nSampleIndex = i;
                }
            }
        }
    }
    if(nSampleIndex == -1){
        qDebug()<<"ProceCountDownEvent:nSampleIndex = -1";
        m_SampleCircle->SetItemText(nReagentIndex+1,QString("%1").arg(nReagentIndex+1 ) );
        m_SampleCircle->SetItemColor(nReagentIndex+1,NORMALSTATUS);
        return;
    }
    //如果时间到了
    if(m_listSampleInfo[nSampleIndex].m_listTestItem[nTestPos].m_nItemRemainTime == 0){
        //停止计时
        m_listSampleInfo[nSampleIndex].m_listTestItem[nTestPos].m_timer->stop();
        //改变试剂圆显示状态
        m_SampleCircle->SetItemColor(nReagentIndex+1,CHECKSTATUS);
        m_SampleCircle->SetItemText(nReagentIndex+1,"检测中");
        //发送命令开始-准备测试
        QByteArray byarrData = IntToBytes(nSamplePos);
        byarrData.append(IntToBytes(nReagentIndex));
        //准备测试
        if(m_bAddSampleStatus == true || m_bCheckStatus == true){
            //插入等待队列
            SendQueue sq;
            sq.m_nSamplePos = nSamplePos;
            sq.m_byteContentData = byarrData;
            sq.m_strItemName = m_listSampleInfo[nSampleIndex].m_listTestItem[nTestPos].m_strItemName;
            m_listWaitTestQueue.append(sq);
            qDebug()<<"时间到,但仪器在加样或者测试中，本样本加入等待测试队列:nSampleIndex->"<<nSampleIndex
                   <<";nTestPos->"<<nSampleIndex;
            return;
        }else{
            //日志输出
            QString strLog;
            strLog = QString(RUNCHECKING).arg(nSamplePos+1);
            ShowLogText(ui->tb_Test_RunInfoShow,strLog,LOGNORMAL);
            //准备测试
            SendSerialData(m_ControlPanelsSerial,PL_FUN_AM_PREPARE_TEST,byarrData);
            qDebug()<<"时间到,准备测试命令发送:nSampleIndex->"<<nSampleIndex<<";nTestPos->"<<nTestPos;
            //检测状态开启
            m_bCheckStatus = true;
            //等待结果计时开始
            m_listSampleInfo[nSampleIndex].m_listTestItem[nTestPos].m_bResultWaitState = true;
            return;
        }
    }
    //显示剩余时间
    m_listSampleInfo[nSampleIndex].m_listTestItem[nTestPos].m_nItemRemainTime--;
    m_SampleCircle->SetItemText(nReagentIndex+1,QString("%1s").arg( m_listSampleInfo[nSampleIndex].m_listTestItem[nTestPos].m_nItemRemainTime ) );
}

/*
 *
 * 测试界面-查看运行信息按钮
 *
 */
void TestInterface::on_pb_Test_ViewLog_clicked()
{
    ui->stackedWidget->setCurrentIndex(2);
    //m_TestCircle->HideAllCircle(false);
    //m_SampleCircle->HideAllCircle(false);
}

/*
 *
 * 测试界面-进入日志页面按钮
 *
 */
void TestInterface::on_pb_Test_EnterLog_clicked()
{
    ui->stackedWidget->setCurrentIndex(3);
}

/*
 *
 * 测试界面-查看日志按钮
 *
 */
void TestInterface::on_pb_Test_LogView_clicked()
{
    //QFile fi(LogPath);
    QFile fi("/home/root/FullAutoQT/FullAutoLog.txt");
    if(fi.open(QIODevice::ReadOnly)){
        QTextStream ts(&fi);
        ts.setCodec("system");
        QString strData = ts.readAll();
        ui->tb_Test_ShowLog->clear();
        ui->tb_Test_ShowLog->setText( strData );
        ui->tb_Test_ShowLog->moveCursor(QTextCursor::End);
    }
}

/*
 *
 * 测试界面 - 删除日志按钮
 *
 */
void TestInterface::on_pb_Test_LogDelete_clicked()
{
    //QFile fi(LogPath);
    QFile fi("/home/root/FullAutoQT/FullAutoLog.txt");
    if(fi.exists()){
        fi.remove();
    }
}

/*
 *
 * 测试界面- 日志返回按钮
 *
 */
void TestInterface::on_pb_Test_LogBack_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
    m_TestCircle->HideAllCircle(false);
    m_SampleCircle->HideAllCircle(false);
}

/*
 *
 * 测试界面- 运行时日志返回按钮
 *
 */
void TestInterface::on_pb_Test_RunInfoBack_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
    m_TestCircle->HideAllCircle(false);
    m_SampleCircle->HideAllCircle(false);
}

/********************************************************
 *@Name:        ShowLogText
 *@Author:      HuaT
 *@Description: 日志显示控件对象
 *@Param1:      日志内容
 *@Param2:      颜色值
 *@Return:      无
 *@Version:     1.0
 *@Date:        2019-3-18
********************************************************/
void TestInterface::ShowLogText(QTextBrowser *tb, QString strContent, int ColorType){
    QDateTime dt = QDateTime::currentDateTime();
    QString strTime = dt.toString("yy-MM-dd hh:mm:ss zzz    >>>    ");
    int nMaxShowCount = 300;
    if(tb->document()->lineCount() > nMaxShowCount){
        tb->clear();
        tb->setTextColor(QColor(255,107,107));
        tb->append(QString("缓存数量大于%1,已清除缓存").arg(nMaxShowCount));
    }
    switch (ColorType) {
    case LOGNOTE1:
        //GRAY
        tb->setTextColor(QColor(128,128,128));
        break;
    case LOGNORMAL:
        //GREED
        tb->setTextColor(QColor(28,188,126));
        break;
    case LOGERROR:
        //RED
        tb->setTextColor(QColor(255,90,90));
        break;
    case LOGWARNING:
        //YELLOW
        tb->setTextColor(QColor(255,185,15));
        break;
    case LOGNOTE2:
        //MAGENTA,start,end
        tb->setTextColor(QColor(255,0,255));
        break;
    default:
        tb->setTextColor(QColor(192,192,192));
        break;
    }
    tb->append(strTime + strContent);
}

void TestInterface::on_pb_Test_Reset_clicked()
{
    QString strNote,strTitle;
    strTitle = "提示";
    strNote = "重置后,所有数据将清除,是否继续?";
    MsgBoxDlg msgbox;
    msgbox.ShowMsg(strNote,1);
    if(1 == msgbox.exec()){
        AllTestComplete();
        SimulationIDCardCast();
    }
}

void TestInterface::on_pb_Test_Cleanout_clicked()
{
    MsgBoxDlg msgbox;
    msgbox.ShowMsg("您是否已更换完清洗溶液？",1);
    if(1 == msgbox.exec()){
        m_fCleanoutCurValue = m_nFluidMaxValue;
        ui->proBar_Test_Cleanout->setValue(m_fCleanoutCurValue);
        m_settins->SaveSettingsInfo(CLEANOUTCURVALUE,QString::number(m_fCleanoutCurValue));
        m_settins->SetParam(CLEANOUTCURVALUE,QString::number(m_fCleanoutCurValue));
    }
}

void TestInterface::on_pb_Test_Effluent_clicked()
{
    MsgBoxDlg msgbox;
    msgbox.ShowMsg("您是否已清理完废弃溶液？",1);
    if(1 == msgbox.exec()){
        m_fEffluentCurValue = 0;
        ui->proBar_Test_Effluent->setValue(m_fEffluentCurValue);
        m_settins->SaveSettingsInfo(EFFLUENTCURVALUE,QString::number(m_fEffluentCurValue));
        m_settins->SetParam(EFFLUENTCURVALUE,QString::number(m_fEffluentCurValue));
    }
}




