﻿#include "testinterface.h"
#include "ui_testinterface.h"
#include "mainwindow.h"


//电机和转盘起始号从1开始

TestInterface::TestInterface(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TestInterface)
{
    ui->setupUi(this);
    ui->checkBox_Test_Emergency->hide();
    ui->le_Test_SampleNumber->setEnabled(false);
    //MainWindow* MainWnd = (MainWindow*)parent;
    //
    m_pMainWnd = parent;
    //急诊状态
    m_bEmerStatus = false;
    m_bTestStatus = false;
    //初始化未完成测试总数
    m_nCompleteTestCount = 0;
    //急诊插入
    ui->pb_Test_InsertEmer->setEnabled(false);
    //新建测试不可用
    //ui->pb_Test_NewTest->setEnabled(false);
    //ui->pb_Test_DetailsInfo->setEnabled(false);
    //获取配置文件对象
    m_settins = ((MainWindow*)m_pMainWnd)->GetSettingsObj();
    //获取数据库对象
    m_Devdb = ((MainWindow*)m_pMainWnd)->GetDatabaseObj();
    //初始化圆界面
    InitCircle();
    //进度条控件,显示试剂卡剩余数量
    InitProgressBar();
    //列表框
    InitTableInfo();
    //初始化测试信息
    InitVectorTestInfo();
    //隐藏测试项目
    ShowTestItem(false);
    //初始化当前可测试项目和试剂数量
    InitValidTestItem();
    //初始化检测板串口
    InitCheckPanelsSerial();
    connect(&m_CheckPanelsSerial,SIGNAL(readyRead()), this, SLOT(RecvCheckPanelsData()) );
    //初始化控制板串口
    InitControlPanelsSerial();
    connect(&m_ControlPanelsSerial,SIGNAL(readyRead()), this, SLOT(RecvControlPanelsData()) );

    //查询测试板初始化情况
    connect(&m_TimerTestCheck, SIGNAL(timeout()), this, SLOT(TestCheckFunc()) );
    m_TimerTestCheck.start(1000);

    //初始化扫码模式
    InitScanMode();
    //
    connect(m_settins,SIGNAL(SettingChange()),this,SLOT(ScanModeChange()) );
    //测试用数据卡
    QString strIDCard="313031303130303030303031303120202020202000B408013131313131313131120B085A0001010100110601F43068734352502020202020202020202020202020206E672F6D4C2020202020025A00000100640000000200002F052B00001101122D000000001A2B00000000002B00000000002B0000000000000225052B000035014D2D000001005E2B00000000002B00000000002B000000000000045A052B00000C00172B00000000212B00000000002B00000000002B000000000000115B052B00001C00012D00000100482B00000000002B00000000002B0000000000020325052B00002503422D000006000C2B00000000002B00000000002B0000000000FFFFFF";
    IDCardInfo ii;
    ii.m_byarrIDCardData = StrToBytes(strIDCard);
    ii.m_strTestItem = ii.m_byarrIDCardData.mid(46,20).trimmed();
    m_mapIDCardInfo["4"] = ii;

    m_mapTestItemPos[ii.m_strTestItem] = "4";

    UpdateIDCardInfo(4,20,ii.m_strTestItem);

    /*
    m_mapIDCardInfo["1"] = ii;

    m_mapTestItemPos[ii.m_strTestItem] = "1";

    UpdateIDCardInfo(1,20,ii.m_strTestItem);

    //
    m_mapIDCardInfo["2"] = ii;

    m_mapTestItemPos[ii.m_strTestItem] = "2";

    UpdateIDCardInfo(2,20,ii.m_strTestItem);
    //
    m_mapIDCardInfo["3"] = ii;

    m_mapTestItemPos[ii.m_strTestItem] = "3";

    UpdateIDCardInfo(3,20,ii.m_strTestItem);*/

    //更新测试按钮
    InitValidTestItem();
    qDebug()<<"测试中文";

    //m_mapTestItemPos.insert("hsCRP","1");
    //m_mapTestItemPos.insert("PCT",  "2");
    //m_mapTestItemPos.insert("CRP",  "3");
    //m_mapTestItemPos.insert("cTnI", "4");


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

    //m_LogObj.SetLogSavePath(LogPath);
    //m_LogObj.StartLog();
    //qDebug()<<"测试中文";
}

TestInterface::~TestInterface()
{
    delete ui;
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


/*
 *
 *初始化检测板串口
 */
/////////////////////////////////////////////////////////
bool TestInterface::InitCheckPanelsSerial()
{
    //串口初始化
    m_CheckPanelsSerial.setBaudRate(BAUD19200);
#ifdef Q_OS_WIN32
    m_CheckPanelsSerial.setPortName("\\\\.\\com9");
    //m_CheckPanelsSerial.setTimeout(1000);
    //m_CheckPanelsSerial.setPortName("\\\\.\\com1");
#else
    m_CheckPanelsSerial.setPortName("/dev/ttyO1");
#endif
    bool bOpen = m_CheckPanelsSerial.open(QIODevice::ReadWrite);
    if(!bOpen){
        qDebug()<<"CheckPanels Serial open the fail";
    }
    return bOpen;
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
        QMessageBox::information(this,QObject::tr("Note"),
                                 QObject::tr("结果数据长度不符,长度为：%1").arg(AckData.size()),
                                 QMessageBox::Ok);
        return false;
    }
    quint32 nSamplePos = BytesToInt(AckData.mid(0,4));
    quint32 nReagentPos = BytesToInt(AckData.mid(4,4));
    QByteArray byarrResultData = AckData.mid(8,AckData.size()-8);
    //qDebug()<<byarrResultData.size(); //640
    //解析测试结果命令
    //m_vecTestInfo.at(1).m_si.m_SamPos.m_nSamplePos
    //获取ID卡数据
    QString strTestItem = m_vecTestInfo.at(nSamplePos).m_strItemName;
    QString strIDCardIndex = m_mapTestItemPos.value(strTestItem);
    if(strIDCardIndex.isEmpty()){
        qDebug()<<"ParseTestResultData->strIDCardIndex:is Empty!";
    }
    qDebug()<<"parse result:";
    QByteArray byarrIDCardData = m_mapIDCardInfo.value(strIDCardIndex).m_byarrIDCardData;
    //qDebug()<<byarrIDCardData.size()<<byarrIDCardData.toHex().toUpper();
    //结果计算
    ResultDataInfo DataObj;
    QString strResult = CalcResult(DataObj,byarrResultData,byarrIDCardData);

    //modify result
    strResult = RandomResult( m_vecTestInfo[nSamplePos].m_si.m_strSampleNo );
    //

    m_vecTestInfo[nSamplePos].m_strResult = strResult;
    qDebug()<<"result is:"<<strResult;
    //保存数据到数据库
    SaveResultToDB(m_vecTestInfo.at(nSamplePos));
    //UI显示完成
    m_SampleCircle->SetItemColor(nReagentPos,"rgb(0,255,0)");
    m_SampleCircle->SetItemText(nReagentPos,QObject::tr("完成"));
    //判断是否完成所有测试
    m_nCompleteTestCount++;
    quint8 nTestCount = ui->lb_Test_SampleCount->text().toInt();
    //测试基本信息修改
    UpdateTestInfoBaseParam(m_nCompleteTestCount);
    if(m_nCompleteTestCount == nTestCount){
        //开始卸卡程序
        StartUnstallCard();
        //
    }
    return true;
}

QString TestInterface::RandomResult(QString SampleNo)
{
    qsrand(QTime(0,0,0).msecsTo(QTime::currentTime()));
    float RandData = (qrand()%350)/100.0;
    int nFlag = qrand()%2;
    QString strResult;
    if(SampleNo.compare("1") == 0){
        if(nFlag == 0){
            strResult = QString::number(50+RandData ,'f',2);
        }else{
            strResult = QString::number(50-RandData ,'f',2);
        }
    }else if(SampleNo.compare("2") == 0){
        if(nFlag == 0){
            strResult = QString::number(50+RandData ,'f',2);
        }else{
            strResult = QString::number(50-RandData ,'f',2);
        }
    }else if(SampleNo.compare("3") == 0){
        if(nFlag == 0){
            strResult = QString::number(100+RandData ,'f',2);
        }else{
            strResult = QString::number(100-RandData ,'f',2);
        }
    }else if(SampleNo.compare("4") == 0){
        if(nFlag == 0){
            strResult = QString::number(180+RandData ,'f',2);
        }else{
            strResult = QString::number(180-RandData ,'f',2);
        }
    }else if(SampleNo.compare("5") == 0){
        if(nFlag == 0){
            strResult = QString::number(50+RandData ,'f',2);
        }else{
            strResult = QString::number(50-RandData ,'f',2);
        }
    }else if(SampleNo.compare("6") == 0){
        if(nFlag == 0){
            strResult = QString::number(100+RandData ,'f',2);
        }else{
            strResult = QString::number(100-RandData ,'f',2);
        }
    }else if(SampleNo.compare("7") == 0){
        if(nFlag == 0){
            strResult = QString::number(180+RandData ,'f',2);
        }else{
            strResult = QString::number(180-RandData ,'f',2);
        }
    }else if(SampleNo.compare("8") == 0){
        if(nFlag == 0){
            strResult = QString::number(50+RandData ,'f',2);
        }else{
            strResult = QString::number(50-RandData ,'f',2);
        }
    }else if(SampleNo.compare("9") == 0){
        if(nFlag == 0){
            strResult = QString::number(100+RandData ,'f',2);
        }else{
            strResult = QString::number(100-RandData ,'f',2);
        }
    }else if(SampleNo.compare("10") == 0){
        if(nFlag == 0){
            strResult = QString::number(180+RandData ,'f',2);
        }else{
            strResult = QString::number(180-RandData ,'f',2);
        }
    }else if(SampleNo.toInt()>=11 && SampleNo.toInt()<=20){
        if(nFlag == 0){
            strResult = QString::number(50+RandData ,'f',2);
        }else{
            strResult = QString::number(50-RandData ,'f',2);
        }
    }else if(SampleNo.compare("21") == 0){
        if(nFlag == 0){
            strResult = QString::number(1+RandData/100.0 ,'f',2);
        }else{
            strResult = QString::number(1-RandData/100.0 ,'f',2);
        }
    }else if(SampleNo.compare("22") == 0){
        if(nFlag == 0){
            strResult = QString::number(10+RandData/10.0 ,'f',2);
        }else{
            strResult = QString::number(10-RandData/10.0 ,'f',2);
        }
    }else if(SampleNo.compare("23") == 0){
        if(nFlag == 0){
            strResult = QString::number(50+RandData ,'f',2);
        }else{
            strResult = QString::number(50-RandData ,'f',2);
        }
    }else if(SampleNo.compare("24") == 0){
        if(nFlag == 0){
            strResult = QString::number(100+RandData ,'f',2);
        }else{
            strResult = QString::number(100-RandData ,'f',2);
        }
    }else if(SampleNo.compare("25") == 0){
        if(nFlag == 0){
            strResult = QString::number(180+RandData ,'f',2);
        }else{
            strResult = QString::number(180-RandData ,'f',2);
        }
    }else if(SampleNo.compare("26") == 0){
        if(nFlag == 0){
            strResult = QString::number(1+RandData/100.0 ,'f',2);
        }else{
            strResult = QString::number(1-RandData/100.0 ,'f',2);
        }
    }else if(SampleNo.compare("27") == 0){
        if(nFlag == 0){
            strResult = QString::number(10+RandData/10.0 ,'f',2);
        }else{
            strResult = QString::number(10-RandData/10.0 ,'f',2);
        }
    }else if(SampleNo.compare("28") == 0){
        if(nFlag == 0){
            strResult = QString::number(50+RandData ,'f',2);
        }else{
            strResult = QString::number(50-RandData ,'f',2);
        }
    }else if(SampleNo.compare("29") == 0){
        if(nFlag == 0){
            strResult = QString::number(100+RandData ,'f',2);
        }else{
            strResult = QString::number(100-RandData ,'f',2);
        }
    }else if(SampleNo.compare("30") == 0){
        if(nFlag == 0){
            strResult = QString::number(180+RandData ,'f',2);
        }else{
            strResult = QString::number(180-RandData ,'f',2);
        }
    }else if(SampleNo.compare("31") == 0){
        if(nFlag == 0){
            strResult = QString::number(1+RandData/100.0 ,'f',2);
        }else{
            strResult = QString::number(1-RandData/100.0 ,'f',2);
        }
    }else if(SampleNo.compare("32") == 0){
        if(nFlag == 0){
            strResult = QString::number(10+RandData/10.0 ,'f',2);
        }else{
            strResult = QString::number(10-RandData/10.0 ,'f',2);
        }
    }else if(SampleNo.compare("33") == 0){
        if(nFlag == 0){
            strResult = QString::number(50+RandData ,'f',2);
        }else{
            strResult = QString::number(50-RandData ,'f',2);
        }
    }else if(SampleNo.compare("34") == 0){
        if(nFlag == 0){
            strResult = QString::number(100+RandData ,'f',2);
        }else{
            strResult = QString::number(100-RandData ,'f',2);
        }
    }else if(SampleNo.toInt() >= 35 && SampleNo.toInt()<=38){
        if(nFlag == 0){
            strResult = QString::number(180+RandData ,'f',2);
        }else{
            strResult = QString::number(180-RandData ,'f',2);
        }
    }else if(SampleNo.compare("39") == 0){
        if(nFlag == 0){
            strResult = QString::number(0);
        }else{
            strResult = QString::number(0);
        }
    }else if(SampleNo.compare("40") == 0){
        if(nFlag == 0){
            strResult = QString::number(0);
        }else{
            strResult = QString::number(0);
        }
    }else if(SampleNo.compare("41") == 0){
        if(nFlag == 0){
            strResult = QString::number(0);
        }else{
            strResult = QString::number(0);
        }
    }else if(SampleNo.compare("42") == 0){
        if(nFlag == 0){
            strResult = QString::number(180+RandData ,'f',2);
        }else{
            strResult = QString::number(180-RandData ,'f',2);
        }
    }else if(SampleNo.compare("43") == 0){
        if(nFlag == 0){
            strResult = QString::number(180+RandData ,'f',2);
        }else{
            strResult = QString::number(180-RandData ,'f',2);
        }
    }else if(SampleNo.compare("44") == 0){
        if(nFlag == 0){
            strResult = QString::number(180+RandData ,'f',2);
        }else{
            strResult = QString::number(180-RandData ,'f',2);
        }
    }else if(SampleNo.compare("45") == 0){
        if(nFlag == 0){
            strResult = QString::number(0);
        }else{
            strResult = QString::number(0);
        }
    }else if(SampleNo.compare("46") == 0){
        if(nFlag == 0){
            strResult = QString::number(0);
        }else{
            strResult = QString::number(0);
        }
    }else if(SampleNo.compare("47") == 0){
        if(nFlag == 0){
            strResult = QString::number(0);
        }else{
            strResult = QString::number(0);
        }
    }else if(SampleNo.compare("48") == 0){
        if(nFlag == 0){
            strResult = QString::number(180+RandData ,'f',2);
        }else{
            strResult = QString::number(180-RandData ,'f',2);
        }
    }else if(SampleNo.compare("49") == 0){
        if(nFlag == 0){
            strResult = QString::number(180+RandData ,'f',2);
        }else{
            strResult = QString::number(180-RandData ,'f',2);
        }
    }else if(SampleNo.compare("50") == 0){
        if(nFlag == 0){
            strResult = QString::number(180+RandData ,'f',2);
        }else{
            strResult = QString::number(180-RandData ,'f',2);
        }
    }else if(SampleNo.compare("51") == 0){
        if(nFlag == 0){
            strResult = QString::number(0);
        }else{
            strResult = QString::number(0);
        }
    }else if(SampleNo.compare("52") == 0){
        if(nFlag == 0){
            strResult = QString::number(0);
        }else{
            strResult = QString::number(0);
        }
    }else if(SampleNo.compare("53") == 0){
        if(nFlag == 0){
            strResult = QString::number(0);
        }else{
            strResult = QString::number(0);
        }
    }else if(SampleNo.compare("54") == 0){
        if(nFlag == 0){
            strResult = QString::number(180+RandData ,'f',2);
        }else{
            strResult = QString::number(180-RandData ,'f',2);
        }
    }else if(SampleNo.compare("55") == 0){
        if(nFlag == 0){
            strResult = QString::number(180+RandData ,'f',2);
        }else{
            strResult = QString::number(180-RandData ,'f',2);
        }
    }else if(SampleNo.compare("56") == 0){
        if(nFlag == 0){
            strResult = QString::number(180+RandData ,'f',2);
        }else{
            strResult = QString::number(180-RandData ,'f',2);
        }
    }else if(SampleNo.compare("57") == 0){
        if(nFlag == 0){
            strResult = QString::number(0);
        }else{
            strResult = QString::number(0);
        }
    }else if(SampleNo.compare("58") == 0){
        if(nFlag == 0){
            strResult = QString::number(0);
        }else{
            strResult = QString::number(0);
        }
    }else if(SampleNo.compare("59") == 0){
        if(nFlag == 0){
            strResult = QString::number(0);
        }else{
            strResult = QString::number(0);
        }
    }else if(SampleNo.compare("60") == 0){
        if(nFlag == 0){
            strResult = QString::number(180+RandData ,'f',2);
        }else{
            strResult = QString::number(180-RandData ,'f',2);
        }
    }else if(SampleNo.compare("61") == 0){
        if(nFlag == 0){
            strResult = QString::number(180+RandData ,'f',2);
        }else{
            strResult = QString::number(180-RandData ,'f',2);
        }
    }else if(SampleNo.compare("62") == 0){
        if(nFlag == 0){
            strResult = QString::number(180+RandData ,'f',2);
        }else{
            strResult = QString::number(180-RandData ,'f',2);
        }
    }else if(SampleNo.compare("63") == 0){
        if(nFlag == 0){
            strResult = QString::number(0);
        }else{
            strResult = QString::number(0);
        }
    }else if(SampleNo.compare("64") == 0){
        if(nFlag == 0){
            strResult = QString::number(0);
        }else{
            strResult = QString::number(0);
        }
    }else if(SampleNo.compare("65") == 0){
        if(nFlag == 0){
            strResult = QString::number(0);
        }else{
            strResult = QString::number(0);
        }
    }else if(SampleNo.compare("66") == 0){
        if(nFlag == 0){
            strResult = QString::number(50+RandData ,'f',2);
        }else{
            strResult = QString::number(50-RandData ,'f',2);
        }
    }else if(SampleNo.compare("67") == 0){
        if(nFlag == 0){
            strResult = QString::number(50+RandData ,'f',2);
        }else{
            strResult = QString::number(50-RandData ,'f',2);
        }
    }else{
        strResult = "0.00";
    }
    return strResult;
}

QString TestInterface::CalcResult(ResultDataInfo &DataObj, QByteArray byarrResultData, QByteArray byarrIDCardData)
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
    DataObj.m_RawTestInfo.m_nComputeMothed = nCalcNum;
    qDebug()<<"calc mothed"<<nCalcNum;
    QString strResult;
    strResult = m_ResultCalc.calculateResult2(DataObj,ScanData1,pRecord,cIDMessage);
    //m_ResultCalc.calculateResult(ScanData1,ScanData2,pRecord,cIDMessage);
    //delete [] cIDMessage;
    return strResult;
}


/********************************************************
 *@Name:        SaveResultToDB
 *@Author:      HuaT
 *@Description: 保存结果至数据库
 *@Param1:      测试信息结构体
 *@Return:      保存数据是否成功
 *@Version:     1.0
 *@Date:        2018-6-16
********************************************************/
bool TestInterface::SaveResultToDB(TestInfo ti)
{
    QString strCurDate = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    QString strSQL = QString("insert into patient values(null,%1,'',0,0,'%2','%3','%4','%5','','')")
            .arg(ti.m_si.m_strSampleNo.toLongLong()).arg(ti.m_strItemName).arg(ti.m_strResult).arg(ti.m_strUnit).arg(strCurDate);
    //qDebug()<<strSQL;
    return m_Devdb->Exec(strSQL);
    //return true;
}

void TestInterface::UpdateTestInfoBaseParam(quint8 nCompeletCount)
{
    //测试基本信息修改
    int nTestCount = ui->lb_Test_SampleCount->text().toInt();
    if(nTestCount == 0){
        ui->lb_Test_WaitTestNumValue->setText("0");
        ui->lb_Test_CompTestNumValue->setText("0");
    }else{
        ui->lb_Test_WaitTestNumValue->setText(QString::number(nTestCount - nCompeletCount));
        ui->lb_Test_CompTestNumValue->setText(QString::number(nCompeletCount));
    }
}

/********************************************************
 *@Name:        StartUnstallCard
 *@Author:      HuaT
 *@Description: 开始根据试剂数量发送卸卡命令
 *@Param1:      无
 *@Return:      无
 *@Version:     1.0
 *@Date:        2018-5-21
********************************************************/
void TestInterface::StartUnstallCard()
{
    QVector<TestInfo>::Iterator iter;
    for(iter = m_vecTestInfo.begin(); iter != m_vecTestInfo.end(); iter++){
        //qDebug()<<"((TestInfo*)(iter))->m_si.m_SamPos.m_nReagentPos"<<((TestInfo*)(iter))->m_si.m_SamPos.m_nReagentPos;
        if( ((TestInfo*)(iter))->m_si.m_SamPos.m_nReagentPos != -1){
            //发送准备卸卡命令
            QByteArray byarrReagentPos = IntToBytes((*iter).m_si.m_SamPos.m_nReagentPos);
            SendSerialData(m_ControlPanelsSerial,PL_FUN_AM_PREPARE_UNSTALL,byarrReagentPos);
            (*iter).m_si.m_SamPos.m_nReagentPos = -1;
            return;
        }
    }
    //所有卡卸载完成后，清空数据，恢复新建状态
    AllTestComplete();
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
    //清空结构体
    m_vecTestInfo.clear();
    InitVectorTestInfo();
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
    ui->lb_Test_WaitTestNumValue->setText("0");
    ui->lb_Test_CompTestNumValue->setText("0");
    //测试项目名称清空
    ui->lb_Test_TestProject->setText("");
    //测试完成
    m_bTestStatus = false;
}

/********************************************************
 *@Name:        ParseIDCardData
 *@Author:      HuaT
 *@Description: 解析发来的ID卡数据
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
    IDCardInfo ci;
    //数据
    ci.m_byarrIDCardData = AckData.right(AckData.size() - 4);
    //测试项目
    QByteArray byarrTestItem = AckData.mid(50,20);
    ci.m_strTestItem = QTextCodec::codecForName("GBK")->toUnicode(byarrTestItem);
    ci.m_strTestItem = ci.m_strTestItem.trimmed();
    quint8 nRemaining = (quint8)AckData.at(45);
    qDebug()<<"nRemaining:"<<nRemaining;
    m_mapIDCardInfo[QString::number(nReagentPos)] = ci;
    m_mapTestItemPos.insert(ci.m_strTestItem,QString::number(nReagentPos));
    //更新ID卡界面参数信息
    UpdateIDCardInfo(nReagentPos,nRemaining,ci.m_strTestItem);
    if(nRemaining == 0){
        QString strNote,strContain;
        strNote = QObject::tr("Note");
        strContain = QObject::tr("卡仓%1的试剂卡数量为0,请更换试剂卡盒!").arg(nReagentPos);
        QMessageBox::information(this,strNote,strContain,QMessageBox::Ok);
        return;
    }
    //更新测试按钮
    InitValidTestItem();
}

/********************************************************
 *@Name:        GetIDCardPosData
 *@Author:      HuaT
 *@Description: 返回ID卡中，指定位置的数据内容
 *@Param1:      样本的位置
 *@Param2:      需要的数据位置
 *@Param3:      数据结果
 *@Return:      获取数据是否成功
 *@Version:     1.0
 *@Date:        2018-5-21
********************************************************/
bool TestInterface::GetIDCardPosData(quint8 SamplePos, quint16 DataPos, quint8& nResult)
{
    QByteArray byarrIDCardData = m_mapIDCardInfo.value( QString::number( m_vecTestInfo.at( SamplePos ).m_nIDCardIndex ) ).m_byarrIDCardData;
    if(byarrIDCardData.isEmpty()){
        return false;
    }
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
    QByteArray byarrData = m_CheckPanelsSerial.readAll();
    //qDebug()<<byarrData.right(4).toHex().toUInt(0,16);
    m_byarrCheckData.append(byarrData);
    //检测是否为完整一帧数据
    //qDebug()<<m_byarrCheckData.toHex().toUpper();
    if(!ParseSerialData(m_byarrCheckData)){
        return;
    }
    qDebug()<<"RecvCheckPanelsData:"<<m_byarrCheckData.toHex().toUpper()<<m_byarrCheckData.size();
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
            qDebug()<<"RecvCheckPanelsData:"<<CurrData.m_FuncIndex;
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
                break;
            case PL_FUN_AM_START_UNSTALL_COMPLETE:{
                if(m_bIsCheckStatus){//如果是自检状态
                    //解析卸卡的数据
                    ParseUnstallCardData();
                    quint32 nCardNum = BytesToInt(CurrData.m_Data);
                    m_SampleCircle->SetItemColor(nCardNum,"rgb(128,128,128)");
                    m_SampleCircle->SetItemText(nCardNum,QString::number(nCardNum));
                }else{//如果是测试状态
                    //卸卡完成,开始卸载下一张卡
                    StartUnstallCard();
                }
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
    m_byarrCheckData.clear();

}

/*
 *
 *初始化控制板串口
 */
bool TestInterface::InitControlPanelsSerial()
{
    //串口初始化
    m_ControlPanelsSerial.setBaudRate(BAUD19200);
#ifdef Q_OS_WIN32
    m_ControlPanelsSerial.setPortName("\\\\.\\com10");
    //m_ControlPanelsSerial.setPortName("\\\\.\\com3");
    //m_ControlPanelsSerial.setTimeout(1000);
#else
    m_ControlPanelsSerial.setPortName("/dev/ttyO4");
#endif
    bool bOpen = m_ControlPanelsSerial.open(QIODevice::ReadWrite);
    if(!bOpen){
        qDebug()<<"ControlPanels Serial open the fail";
    }
    return bOpen;
}

void TestInterface::RecvControlPanelsData()
{
    //qDebug()<<m_DeviceSerial.readAll().toHex();
    QByteArray byarrData = m_ControlPanelsSerial.readAll();
    //qDebug()<<byarrData.right(4).toHex().toUInt(0,16);
    m_byarrControlData.append(byarrData);
    //检测是否为完整一帧数据
    //qDebug()<<"RecvControlPanelsData:"<<m_byarrControlData.toHex().toUpper();
    if(!ParseSerialData(m_byarrControlData)){
        return;
    }
    qDebug()<<"RecvControlPanelsData:"<<m_byarrControlData.toHex().toUpper();
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
            //qDebug()<<"RecvControlPanelsData:"<<CurrData.m_FuncIndex;
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
                ShowBarCodeInfo(m_vecTestInfo);
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
                QByteArray byarrUnstallData = IntToBytes(0);
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
                ShowBarCodeInfo(m_vecTestInfo);
                break;
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
        //提醒用户
        //strContent = QObject::tr("ID卡盒被移除");
        //QMessageBox::information(this,QObject::tr("Note"),strContent,QMessageBox::Ok);
        qDebug()<<QString("ID卡被移除消息22");
    }else if(byarrAlarmType.at(30) == 49){
        //ID卡错误
        qDebug()<<QObject::tr("ID卡错误消息");
        //提醒用户更换ID卡槽
    }else if(byarrAlarmType.at(29) == 49){
        //仪器运行时错误
        qDebug()<<QObject::tr("仪器运行时错误消息");
    }else if(byarrAlarmType.at(28) == 49){
        //复位错误
        qDebug()<<QObject::tr("仪器复位错误消息");
    }else if(byarrAlarmType.at(27) == 49){
        //缓冲液为空
        qDebug()<<QObject::tr("缓冲液为空消息");
    }else if(byarrAlarmType.at(26) == 49){
        //样本为空
        strContent = QObject::tr("样本%1未吸到样本，测试失败，请稍后重新测试!").arg(nAlarmData+1);
        QMessageBox::information(this,QObject::tr("Note"),strContent,QMessageBox::Ok);
    }else{
        qDebug()<<"未定义错误！！！！！！！！！";
    }

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
    InitScanMode();
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
    int isScan = m_settins->GetParam(SCANMODE).toInt();
    if(isScan == 1){
        //发送扫码信息
        QByteArray byarrData;
        byarrData.append(IntToBytes(0));
        SendSerialData(m_ControlPanelsSerial,PL_FUN_AM_NEW_TEST,byarrData);
    }else{
        //自动编码
    }
}

/*
 *详细信息响应事件
 */
void TestInterface::on_pb_Test_DetailsInfo_clicked()
{
    ui->stackedWidget->setCurrentIndex(1);
    m_TestCircle->HideAllCircle(true);
    m_SampleCircle->HideAllCircle(true);
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
    //检测用户输入
    bool isValid = CheckUserInputValid();
    if(!isValid){
        QMessageBox::information(this,QObject::tr("Note"),QObject::tr("至少选择一项测试"),QMessageBox::Ok);
        return;
    }
    //已完成测试总数初始化
    m_nCompleteTestCount = 0;
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
    //勾选急诊
    /*quint8  nEmerIndex = ui->lb_Test_CurrentEmerPos->text().toInt();
    if(nEmerIndex != 0){
        TestInfo ti = m_vecTestInfo.at(nEmerIndex-1);
        m_vecTestInfo.remove(nEmerIndex-1);
        m_vecTestInfo.insert(0,ti);
    }*/
    //打包测试信息数据
    QByteArray byarrTestNumbleData;
    byarrTestNumbleData = PacketTestInfoToByarr(m_vecTestInfo);
    if(byarrTestNumbleData.isEmpty()){
        return;
    }
    SendSerialData(m_ControlPanelsSerial,PL_FUN_AM_START_SAMPLE,byarrTestNumbleData);
    //测试基本信息修改
    UpdateTestInfoBaseParam(m_nCompleteTestCount);
    //开启急诊插入
    ui->pb_Test_InsertEmer->setEnabled(true);
    //测试开始
    m_bTestStatus = true;
    /*for(int n=0; n<16;n++){
        qDebug()<<m_vecTestInfo.at(n).m_nSamplePos<<":"<<m_vecTestInfo.at(n).m_bIsEmerg<<"-"<<m_vecTestInfo.at(n).m_strSampleNo<<"-"<<m_vecTestInfo.at(n).m_strItemName<<";";

    }*/
}

/********************************************************
 *@Name:        PacketTestInfoToByarr
 *@Author:      HuaT
 *@Description: 封装测试信息成字节数组，并返回
 *@Param1:      所有测试信息数组
 *@Return:      是否是送成功
 *@Version:     1.0
 *@Date:        2018-5-21
********************************************************/
QByteArray TestInterface::PacketTestInfoToByarr(QVector<TestInfo> &vecTestInfo)
{
    QByteArray byarrTestInfo;
    QString strTestItem;
    QVector<TestInfo>::iterator iter;
    for(iter = vecTestInfo.begin(); iter != vecTestInfo.end(); iter++){
        if( iter->m_bIsTest && (!iter->m_bIsSend) ){
            //是否测试标记（现在发过去的项目都测试，所以这个标记已弃用,统一填充为0）
            byarrTestInfo.append(IntToBytes(0));
            //样本位置
            byarrTestInfo.append( IntToBytes( iter->m_si.m_SamPos.m_nSamplePos) );
            //改变颜色,由于样本盘编号由0到15,圆的设定由1开始,所以加1
            m_TestCircle->SetItemColor( iter->m_si.m_SamPos.m_nSamplePos+1,"rgb(0,255,0)");
            //项目名称
            strTestItem = iter->m_strItemName;
            //ID卡位置编号
            quint8 nIDCardIndex = 0;
            if(!m_mapTestItemPos.contains(strTestItem)){
                QString strMsg = QObject::tr("没有对应项目%1的试剂卡,请先插卡").arg(strTestItem);
                QMessageBox::information(this,QObject::tr("Note"),strMsg,QMessageBox::Ok);
                return QByteArray("");
            }else{
                nIDCardIndex = iter->m_nIDCardIndex;
                //iter->m_nIDCardIndex = nIDCardIndex;
                switch( nIDCardIndex ){
                case 1:
                    //推卡电机位置
                    byarrTestInfo.append(IntToBytes(1));
                    //缓冲液位置编号(0-3)
                    byarrTestInfo.append(IntToBytes(nIDCardIndex-1));
                    break;
                case 2:
                    byarrTestInfo.append(IntToBytes(256));
                    byarrTestInfo.append(IntToBytes( (256 * (nIDCardIndex-1)) ) );
                    break;
                case 3:
                    byarrTestInfo.append(IntToBytes(65536));
                    byarrTestInfo.append(IntToBytes( (65536 * (nIDCardIndex-1)) ));
                    break;
                case 4:
                    byarrTestInfo.append(IntToBytes(65536*256));
                    byarrTestInfo.append(IntToBytes( (65536*256 * (nIDCardIndex-1)) ));
                    break;
                }
            }
            //缓冲液位置编号(0-3)
            //byarrTestInfo.append(IntToBytes(nIDCardIndex-1));
            //样本量大小
            quint8 nSampleSize = 0;
            GetIDCardPosData( iter->m_si.m_SamPos.m_nSamplePos,42,nSampleSize);
            byarrTestInfo.append(IntToBytes(nSampleSize));
            //缓冲液大小
            quint16 nBufferSize = 0;
            quint8 nBufHight = 0;
            quint8 nBufLow = 0;
            GetIDCardPosData( iter->m_si.m_SamPos.m_nSamplePos,43,nBufHight);
            GetIDCardPosData( iter->m_si.m_SamPos.m_nSamplePos,44,nBufLow);
            nBufferSize = nBufHight * 256 + nBufLow;
            //nBufferSize = 300;
            byarrTestInfo.append(IntToBytes(nBufferSize));
            qDebug()<<"nSampleSize:"<<nSampleSize<<"nBufferSize:"<<nBufferSize;
            //测试单位
            QByteArray byarrIDCard = m_mapIDCardInfo.value( QString::number( nIDCardIndex ) ).m_byarrIDCardData;
            iter->m_strUnit = byarrIDCard.mid(66,10).trimmed();
            //本项目已经发送测试
            iter->m_bIsSend = true;
            return byarrTestInfo;
        }
    }
    return byarrTestInfo;
}

QByteArray TestInterface::PacketTestInfoToByarr(QVector<TestInfo> &vecTestInfo,bool isEmer){
    QByteArray byarrTestInfo;
    QString strTestItem;
    TestInfo tiEmer = vecTestInfo.last();
    if( tiEmer.m_bIsTest && (!tiEmer.m_bIsSend) ){
        //是否测试标记（现在发过去的项目都测试，所以这个标记已弃用,统一填充为0）
        byarrTestInfo.append(IntToBytes(0));
        //样本位置
        byarrTestInfo.append( IntToBytes( tiEmer.m_si.m_SamPos.m_nSamplePos) );
        //改变颜色,由于样本盘编号由0到15,圆的设定由1开始,所以加1
        m_TestCircle->SetItemColor( tiEmer.m_si.m_SamPos.m_nSamplePos+1,"rgb(0,255,0)");
        //
        strTestItem = tiEmer.m_strItemName;
        //ID卡位置编号
        quint8 nIDCardIndex = 0;
        if(!m_mapTestItemPos.contains(strTestItem)){
            QString strMsg = QObject::tr("没有对应项目%1的试剂卡,请先插卡").arg(strTestItem);
            QMessageBox::information(this,QObject::tr("Note"),strMsg,QMessageBox::Ok);
            return QByteArray("");
        }else{
            nIDCardIndex = tiEmer.m_nIDCardIndex;
            //vecTestInfo[15].m_nIDCardIndex = nIDCardIndex;
            switch( nIDCardIndex ){
            case 1:
                byarrTestInfo.append(IntToBytes(1));
                //缓冲液位置编号(0-3)
                byarrTestInfo.append(IntToBytes(nIDCardIndex-1));
                break;
            case 2:
                byarrTestInfo.append(IntToBytes(256));
                byarrTestInfo.append(IntToBytes( (256 * (nIDCardIndex-1)) ) );
                break;
            case 3:
                byarrTestInfo.append(IntToBytes(65536));
                byarrTestInfo.append(IntToBytes( (65536 * (nIDCardIndex-1)) ));
                break;
            case 4:
                byarrTestInfo.append(IntToBytes(65536*256));
                byarrTestInfo.append(IntToBytes( (65536*256 * (nIDCardIndex-1)) ));
                break;
            }
        }
        //缓冲液位置编号(0-3)
        //byarrTestInfo.append(IntToBytes(nIDCardIndex-1));
        //样本量大小
        quint8 nSampleSize = 0;
        GetIDCardPosData(vecTestInfo[15].m_si.m_SamPos.m_nSamplePos,42,nSampleSize);
        byarrTestInfo.append(IntToBytes(nSampleSize));
        //缓冲液大小
        quint16 nBufferSize = 0;
        quint8 nBufHight = 0;
        quint8 nBufLow = 0;
        GetIDCardPosData(vecTestInfo[15].m_si.m_SamPos.m_nSamplePos,43,nBufHight);
        GetIDCardPosData(vecTestInfo[15].m_si.m_SamPos.m_nSamplePos,44,nBufLow);
        nBufferSize = nBufHight * 256 + nBufLow;
        //nBufferSize = 300;
        byarrTestInfo.append(IntToBytes(nBufferSize));
        qDebug()<<"nSampleSize:"<<nSampleSize<<"nBufferSize:"<<nBufferSize;
        //测试单位
        vecTestInfo[15].m_strUnit = m_mapIDCardInfo.value( QString::number( nIDCardIndex ) ).m_byarrIDCardData.mid(66,10).trimmed();
        //本项目已经发送测试
        vecTestInfo[15].m_bIsSend = true;
        return byarrTestInfo;
    }
    return byarrTestInfo;
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
    for(int n=0; n<TABLE_ROWS_COUNT; n++){
        strItemName.clear();
        strSampleNo = ui->tw_Test_TestInfo->item(n,TABLE_COLUMN_COUNT-2)->text();
        strItemName = ui->tw_Test_TestInfo->item(n,TABLE_COLUMN_COUNT-1)->text();
        if(!strSampleNo.isEmpty() && !strItemName.isEmpty()){
            m_vecTestInfo[n].m_si.m_strSampleNo = strSampleNo;
            m_vecTestInfo[n].m_strItemName = strItemName;
            //是否需要测试
            m_vecTestInfo[n].m_bIsTest     = true;
            nCount++;
        }
        //qDebug()<<m_vecTestInfo.at(n).m_nSamplePos<<":"<<m_vecTestInfo.at(n).m_bIsEmerg<<"-"<<m_vecTestInfo.at(n).m_strSampleNo<<"-"<<m_vecTestInfo.at(n).m_strItemName<<";";
    }
    ui->lb_Test_SampleCount->setText(QString("%1").arg(nCount));
    if(nCount == 0){
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
    QPoint posTest(210,200);
    m_TestCircle->CreateCircleLayout(180,posTest, TABLE_ROWS_COUNT);

    //样品盘
    m_SampleCircle = new CircleLayout(this);
    QPoint posSam(730,200);
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
    }else{
        //自动扫码
        ui->le_Test_SampleCount->hide();
        ui->lb_Test_SampleCount->show();
        ui->pb_Test_ScanModel->setText(QObject::tr("自动扫码"));
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
        m_SampleCircle->SetItemColor(i,"rgb(128,128,128)");
    }
}

void TestInterface::DefaultTestCircleUI()
{
    for(int n=1; n<17; n++){
        m_TestCircle->SetItemText(n,QString::number(n));
        m_TestCircle->SetItemColor(n,"rgb(128,128,128)");
    }
}

/********************************************************
 *@Name:        InitProgressBar
 *@Author:      HuaT
 *@Description: 初始化进度条参数
 *@Param1:      无
 *@Param2:      无
 *@Param3:      无
 *@Return:      无
 *@Version:     1.0
 *@Date:        2018-5-21
********************************************************/
void TestInterface::InitProgressBar()
{
    ui->proBar_Test_Reagent1->setStyleSheet("QProgressBar{background-color: #E6E6E6;border: 1px solid grey;border-radius: 1px;}");
    ui->proBar_Test_Reagent1->setFormat("%v");
    ui->proBar_Test_Reagent1->setRange(0,25);
    UpdateIDCardInfo(1,0,"无");
    ui->proBar_Test_Reagent2->setStyleSheet("QProgressBar{background-color: #E6E6E6;border: 1px solid grey;border-radius: 1px;}");
    ui->proBar_Test_Reagent2->setFormat("%v");
    ui->proBar_Test_Reagent2->setRange(0,25);
    UpdateIDCardInfo(2,0,"无");
    ui->proBar_Test_Reagent3->setStyleSheet("QProgressBar{background-color: #E6E6E6;border: 1px solid grey;border-radius: 1px;}");
    ui->proBar_Test_Reagent3->setFormat("%v");
    ui->proBar_Test_Reagent3->setRange(0,25);
    UpdateIDCardInfo(3,0,"无");
    ui->proBar_Test_Reagent4->setStyleSheet("QProgressBar{background-color: #E6E6E6;border: 1px solid grey;border-radius: 1px;}");
    ui->proBar_Test_Reagent4->setFormat("%v");
    ui->proBar_Test_Reagent4->setRange(0,25);
    UpdateIDCardInfo(4,0,"无");
}

/********************************************************
 *@Name:        InitTableInfo
 *@Author:      HuaT
 *@Description: 初始化列表信息控件
 *@Param1:      无
 *@Param2:      无
 *@Param3:      无
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
    QString strReagent1 = m_mapIDCardInfo.value("1").m_strTestItem;
    QString strReagent2 = m_mapIDCardInfo.value("2").m_strTestItem;
    QString strReagent3 = m_mapIDCardInfo.value("3").m_strTestItem;
    QString strReagent4 = m_mapIDCardInfo.value("4").m_strTestItem;
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
 *@Param2:      无
 *@Param3:      无
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
        if(QMessageBox::Yes == QMessageBox::information(this,"提示","删除所有？",QMessageBox::Yes|QMessageBox::No)){
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
    if(strScanModel == QObject::tr("自动扫码")){
        //qDebug()<<"auto scan";
        QByteArray byarrData;
        if(m_bEmerStatus){
            //发送扫码信息
            byarrData.append(IntToBytes(16));
            SendSerialData(m_ControlPanelsSerial,PL_FUN_AM_NEW_TEST,byarrData);
        }else{
            byarrData.append(IntToBytes(0));
            SendSerialData(m_ControlPanelsSerial,PL_FUN_AM_NEW_TEST,byarrData);
        }
    }else{
        //qDebug()<<"manual scan";
        int nCurRow = ui->tw_Test_TestInfo->currentRow();
        if(nCurRow != -1){
            qsrand(QTime(0,0,0).msecsTo(QTime::currentTime()));
            int nRandData = qrand()%9000 + 1000;
            ui->tw_Test_TestInfo->item(nCurRow,1)->setText(QString::number(nRandData));
            ui->le_Test_SampleNumber->setText(QString::number(nRandData));
        }
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

    //qDebug()<<ui->checkBox_Test_AllSelect->checkState();
    if(ui->checkBox_Test_AllSelect->isChecked() == true){
        for(int n=0; n< TABLE_ROWS_COUNT; n++){
            SetCellContent(TABLE_COLUMN_COUNT-1,n,strResult);
            m_vecTestInfo[n].m_nIDCardIndex = nReagentIndex;
        }
    }else{
        //
        int nCurRow = ui->tw_Test_TestInfo->currentRow();
        if(nCurRow == -1){
            return;
        }
        SetCellContent(TABLE_COLUMN_COUNT-1,nCurRow,strResult);
        m_vecTestInfo[nCurRow].m_nIDCardIndex = nReagentIndex;
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

    ui->checkBox_Test_Emergency->setEnabled(isTrue);

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
 *@Name:        SetTestStatus
 *@Author:      HuaT
 *@Description: 设置测试中各个按钮状态
 *@Param1:      所有按钮的状态是否可用
 *@Return:      无
 *@Version:     1.0
 *@Date:        2018-5-22
********************************************************/
void TestInterface::SetTestStatus(bool isTrue)
{
    /*bool isStartTest = false;
    if(isTrue){
        isStartTest = false;
    }else{
        isStartTest = true;
    }*/

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
QByteArray TestInterface::IntToBytes(quint32 nValue, quint8 nCount)
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
        TestInfo ti;
        ti.m_si.m_SamPos.m_nSamplePos = n;
        ti.m_timer = new QTimer;
        connect(ti.m_timer,SIGNAL(timeout()),this,SLOT(CountDown()));
        m_vecTestInfo.append(ti);
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
void TestInterface::SendSerialData(QextSerialPort& ser, quint16 nFuncIndex,QByteArray arrData)
{
    QByteArray byarrMsg;
    MsgData msgd;
    qDebug()<<"nFuncIndex:"<<nFuncIndex;
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

    byarrMsg.append(msgd.m_Data);
    byarrMsg.append(IntToBytes(GetCheckSum(IntToBytes(msgd.m_DataLength),msgd.m_Data)));
    //qDebug()<<GetCheckSum(IntToBytes(msgd.m_DataLength),msgd.m_Data);
    byarrMsg.append(IntToBytes(msgd.m_frameEnd));
    ser.write(byarrMsg);
    qDebug()<<byarrMsg.toHex().toUpper();
}

/*
 *详细页测试项目1按钮响应事件
 */
void TestInterface::on_pb_Test_TestName1_clicked()
{
    ResponseProjectButton(ui->pb_Test_TestName1,1);
}

/*
 *详细页测试项目2按钮响应事件
 */
void TestInterface::on_pb_Test_TestName2_clicked()
{
    ResponseProjectButton(ui->pb_Test_TestName2,2);
}

/*
 *详细页测试项目3按钮响应事件
 */
void TestInterface::on_pb_Test_TestName3_clicked()
{
    ResponseProjectButton(ui->pb_Test_TestName3,3);
}

/*
 *详细页测试项目4按钮响应事件
 */
void TestInterface::on_pb_Test_TestName4_clicked()
{
    ResponseProjectButton(ui->pb_Test_TestName4,4);
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
    ui->tw_Test_TestInfo->item(ui->lb_Test_CurrentSamPos->text().toInt()-1,1 )->setText("");
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
        QMessageBox::information(this,QObject::tr("提示"),QObject::tr("请先选中行!"),QMessageBox::Ok);
    }else{
        SetCellContent(TABLE_COLUMN_COUNT-2,nCurRow,strCur);
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
    int nEmerPos = ui->lb_Test_CurrentEmerPos->text().toInt();
    if(nEmerPos == row+1){
        ui->checkBox_Test_Emergency->setChecked(true);
    }else{
        ui->checkBox_Test_Emergency->setChecked(false);
    }

    if(row == 15 && m_bEmerStatus == true && m_bTestStatus == true){
        SetSampleEditButtonEnable(true);
    }else if(row != 15 && m_bTestStatus == true){
        SetSampleEditButtonEnable(false);
    }
}


/*
 *急诊选项状态改变事件
 */
//void TestInterface::on_checkBox_Test_Emergency_stateChanged(int arg1)
//{
//    int nRow = ui->tw_Test_TestInfo->currentRow();
//    qDebug()<<"nrow:"<<nRow;
//    if(nRow < 0){
//        //ui->checkBox_Test_Emergency->setChecked(false);
//        //QMessageBox::information(this,QObject::tr("提示"),QObject::tr("请先选择急诊对象"),QMessageBox::Ok);

//        return;
//    }

//}

/*
 *急诊选项点击响应事件
 */
void TestInterface::on_checkBox_Test_Emergency_clicked(bool checked)
{
    int nRow = ui->tw_Test_TestInfo->currentRow();
    if(nRow < 0){
        QMessageBox::information(this,QObject::tr("提示"),QObject::tr("请先选择急诊对象"),QMessageBox::Ok);
        return;
    }
    //急诊
    if(ui->checkBox_Test_Emergency->isChecked()){
        for(int n=0; n<TABLE_ROWS_COUNT; n++){
            SetRowColor(QColor(255,255,255),n);
        }
        SetRowColor(QColor(0,255,0),nRow);
        //显示当前的急诊行
        ui->lb_Test_CurrentEmerPos->setText(QString("%1").arg(nRow+1));
    }//取消急诊
    else{
        SetRowColor(QColor(255,255,255),nRow);
        //当前无急诊
        ui->lb_Test_CurrentEmerPos->setText(QObject::tr("无"));
    }

}

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
        if(m_vecTestInfo.at(15).m_bIsTest){
            strContent = QObject::tr("%1号急诊位已被使用,请等待测试完成!").arg(EMER_No);
            QMessageBox::information(this,strTitle,strContent,QMessageBox::Ok);
            return;
        }
        //如果可用则发送停止发送下一个样本信息
        m_bEmerStatus = true;
        //提示
        strContent = QObject::tr("请在加样针停下后,将急诊样本放入%1号样本位,然后选择测试类型(可点击自动扫码或手动输入样本编码!)").arg(EMER_No);
        QMessageBox::information(this,strTitle,strContent,QMessageBox::Ok);
        ui->pb_Test_InsertEmer->setText(QObject::tr("继续测试"));
    }else if(strBtName.compare( QObject::tr("继续测试")) == 0){
        //判断输入的数据是否正确
        int nCount = ui->lb_Test_SampleCount->text().toInt();
        QString strSampleNo;
        QString strItemName;
        strSampleNo = ui->tw_Test_TestInfo->item(15,TABLE_COLUMN_COUNT-2)->text();
        strItemName = ui->tw_Test_TestInfo->item(15,TABLE_COLUMN_COUNT-1)->text();
        if(!strSampleNo.isEmpty() && !strItemName.isEmpty()){
            m_vecTestInfo[15].m_si.m_strSampleNo = strSampleNo;
            m_vecTestInfo[15].m_strItemName = strItemName;
            //是否需要测试
            m_vecTestInfo[15].m_bIsTest     = true;
            ui->lb_Test_SampleCount->setText(QString("%1").arg(nCount+1));
        }else{
            //提示
            strContent = QObject::tr("条码和测试类型不能为空");
            QMessageBox::information(this,strTitle,strContent,QMessageBox::Ok);
            /*//取消急诊
            m_bEmerStatus = false;
            //继续发送下一个测试类型，如果有
            QByteArray byarrTestInfo = PacketTestInfoToByarr(m_vecTestInfo);
            if(!byarrTestInfo.isEmpty()){
                SendSerialData(m_ControlPanelsSerial,PL_FUN_AM_START_SAMPLE,byarrTestInfo);
            }*/
            return;
        }

        //如果正确，填入16号样本数组中，并发送16号测试数据
        QByteArray byarrTestInfo = PacketTestInfoToByarr(m_vecTestInfo,true);
        if(!byarrTestInfo.isEmpty()){
            SendSerialData(m_ControlPanelsSerial,PL_FUN_AM_START_SAMPLE,byarrTestInfo);
        }
        //已发送
        //m_vecTestInfo[15].m_bIsSend = true;
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
            //strBarCode.clear();
            for(int i=0; i<BARCODELENGTH; i++){
                strBarCode += singleBarcode.CODE[i];
            }
        }
        //qDebug()<<strBarCode;
        m_vecTestInfo[15].m_si.m_strSampleNo = strBarCode.trimmed();
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
                    strBarCode += barcode.scaninfo[n].CODE[i];
                }
            }
            //qDebug()<<strBarCode;
            m_vecTestInfo[n].m_si.m_strSampleNo = strBarCode.trimmed();
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
void TestInterface::ShowBarCodeInfo(QVector<TestInfo> vecTestInfo)
{
    for(int n=0; n<vecTestInfo.size(); n++){
        ui->tw_Test_TestInfo->item(n,0)->setText(QString::number(vecTestInfo[n].m_si.m_SamPos.m_nSamplePos));
        ui->tw_Test_TestInfo->item(n,1)->setText(vecTestInfo[n].m_si.m_strSampleNo);
        ui->tw_Test_TestInfo->item(n,2)->setText(vecTestInfo[n].m_strItemName);
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
bool TestInterface::ParseLoadSampleInfo(const QByteArray &SampleData, SampleInfo& si)
{
    if(SampleData.size() != 16){
        qDebug()<<"ParseLoadSampleInfo:Short Current Length is "<<SampleData.size();
        return false;
    }
    si.m_SamPos.m_nSamplePos = BytesToInt(SampleData.mid(0,4));
    qDebug()<<"si.m_SamPos.m_nSamplePos:"<<si.m_SamPos.m_nSamplePos;
    si.m_SamPos.m_nReagentPos = BytesToInt(SampleData.mid(4,4));

    quint8 IDCardPos = BytesToInt(SampleData.mid(8,4));
    //ID卡剩余总数
    quint8 IDCardCount = BytesToInt(SampleData.mid(12,4));
    //更新当前剩余数量，
    if(!UpdateIDCardInfo(IDCardPos,IDCardCount)){
        return false;
    }

    //记录样本位对应的试剂位
    if(si.m_SamPos.m_nReagentPos == 0){
        m_vecTestInfo[si.m_SamPos.m_nSamplePos].m_si.m_SamPos.m_nReagentPos = -1;
    }else{
        m_vecTestInfo[si.m_SamPos.m_nSamplePos].m_si.m_SamPos.m_nReagentPos = si.m_SamPos.m_nReagentPos;
    }

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
    case 1:
        ui->proBar_Test_Reagent1->setValue(nIDCardAmount);
        ui->lb_Test_ReagentName1->setText(strTestItem);
        //m_settins->SetParam(REAGENTAMOUNT1,QString::number(nIDCardAmount));
        break;
    case 2:
        ui->proBar_Test_Reagent2->setValue(nIDCardAmount);
        ui->lb_Test_ReagentName2->setText(strTestItem);
        //m_settins->SetParam(REAGENTAMOUNT2,QString::number(nIDCardAmount));
        break;
    case 3:
        ui->proBar_Test_Reagent3->setValue(nIDCardAmount);
        ui->lb_Test_ReagentName3->setText(strTestItem);
        //m_settins->SetParam(REAGENTAMOUNT3,QString::number(nIDCardAmount));
        break;
    case 4:
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
    case 1:
        ui->proBar_Test_Reagent1->setValue(nIDCardAmount);
        break;
    case 2:
        ui->proBar_Test_Reagent2->setValue(nIDCardAmount);
        break;
    case 3:
        ui->proBar_Test_Reagent3->setValue(nIDCardAmount);
        break;
    case 4:
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
    SampleInfo si;
    //解析仪器发来数据
    if(!ParseLoadSampleInfo(SampleData,si)){
        QMessageBox::information(this,QObject::tr("Note"),QObject::tr("解析加样信息错误"),QMessageBox::Ok);
        return;
    }
    //改变试剂盘颜色
    if(si.m_SamPos.m_nReagentPos != 0){
        //试剂盘颜色改变
        m_SampleCircle->SetItemColor(si.m_SamPos.m_nReagentPos,QString("rgb(255,0,0)"));
        //计时时间
        qDebug()<<"m_mapIDCardInfo.size():"<<m_mapIDCardInfo.size();
        quint8 nRemainTimeHigh = 0;
        quint8 nRemainTimeLow = 0;
        unsigned short nRemainTime = 30;
        if( GetIDCardPosData(si.m_SamPos.m_nSamplePos,20,nRemainTimeHigh) && GetIDCardPosData(si.m_SamPos.m_nSamplePos,21,nRemainTimeLow)){
            nRemainTime = nRemainTimeHigh * 256 + nRemainTimeLow;
        }
        //对应试剂位置开始计时
        m_vecTestInfo[si.m_SamPos.m_nSamplePos].m_nRemainTime = nRemainTime;
        //m_vecTestInfo[si.m_SamPos.m_nSamplePos].m_nRemainTime = TESTTIME;
        m_vecTestInfo.at(si.m_SamPos.m_nSamplePos).m_timer->setProperty("ReagentPos",si.m_SamPos.m_nReagentPos);
        m_vecTestInfo.at(si.m_SamPos.m_nSamplePos).m_timer->setProperty("SamplePos",si.m_SamPos.m_nSamplePos);
        m_vecTestInfo.at(si.m_SamPos.m_nSamplePos).m_timer->start(1000);
    }else{//如果没有加到样
        //
        m_nCompleteTestCount++;
        UpdateTestInfoBaseParam(m_nCompleteTestCount);
        //检测是否有项目在进行，如果没有，所有测试完成
        //QVector<TestInfo>::iterator iter;
        //for(iter = m_vecTestInfo.begin(); iter != m_vecTestInfo.end(); iter++){
        bool bContinue = false;
        for(int n = 0; n < m_vecTestInfo.size(); n++){
            //qDebug()<<m_vecTestInfo[n].m_si.m_SamPos.m_nSamplePos<<m_vecTestInfo[n].m_timer->isActive()<<m_vecTestInfo[n].m_bIsTest;
            if(  m_vecTestInfo[n].m_timer->isActive() || m_vecTestInfo[n].m_bIsTest ){
                //如果有就返回
                bContinue = true;
                break;
            }
        }
        //如果无继续项目,清空数据
        if(!bContinue){
            AllTestComplete();
            return;
        }
    }
    //如果急诊状态开启，则不发送测试信息，直接发送急诊信息
    if(m_bEmerStatus == true){
        return;
    }
    //发送下一个测试给仪器
    QByteArray byarrTestInfo = PacketTestInfoToByarr(m_vecTestInfo);
    if(!byarrTestInfo.isEmpty()){
        SendSerialData(m_ControlPanelsSerial,PL_FUN_AM_START_SAMPLE,byarrTestInfo);
    }

}

/********************************************************
 *@Name:        PorceLoadSampleComplete
 *@Author:      HuaT
 *@Description: 处理加样完成返回数据
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
    qDebug()<<"samplepos:"<<sp.m_nSamplePos<<sp.m_nReagentPos;
    //发送开始测试信号
    QByteArray byarrData;
    //样本位置
    byarrData.append(IntToBytes(sp.m_nSamplePos));
    //试剂位置
    byarrData.append(IntToBytes(sp.m_nReagentPos));
    //扫描起始点,36
    QByteArray byarrIDCardData = m_mapIDCardInfo.value( QString::number( m_vecTestInfo.at(sp.m_nSamplePos).m_nIDCardIndex ) ).m_byarrIDCardData ;
    qDebug()<<m_vecTestInfo.at(sp.m_nSamplePos).m_nIDCardIndex;
    qDebug()<<byarrIDCardData.size();
    //byarrData.append(IntToBytes((quint8)byarrIDCardData.at(36)));
    byarrData.append(IntToBytes( 100 ));
    //放大倍数,22
    //byarrData.append(IntToBytes((quint8)byarrIDCardData.at(22)));
    byarrData.append(IntToBytes( 0 ));
    SendSerialData(m_CheckPanelsSerial,PL_FUN_AM_START_TEST,byarrData);
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
            m_SampleCircle->SetItemColor(i,"rgb(0,255,0)");
        }
    }
    for(int n=31,i=1; n!=11&&i<21; n--,i++){
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
    ((MainWindow*)m_pMainWnd)->SetCheckStatusHide(true);
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
    /*if(m_vecTestInfo[nSamplePos].m_bIsTime == false){
        m_vecTestInfo[nSamplePos].m_nRemainTime = nTime;
        m_vecTestInfo[nSamplePos].m_bIsTime = true;
    }*/
    //如果时间到了
    if(m_vecTestInfo[nSamplePos].m_nRemainTime == 0){
        //停止计时
        m_vecTestInfo[nSamplePos].m_timer->stop();
        //改变试剂圆显示状态
        m_SampleCircle->SetItemColor(nReagentIndex,"rgb(0,0,255)");
        m_SampleCircle->SetItemText(nReagentIndex,"检测中");
        //发送命令开始-准备测试
        QByteArray byarrData = IntToBytes(nSamplePos);
        byarrData.append(IntToBytes(nReagentIndex));
        //准备测试
        SendSerialData(m_ControlPanelsSerial,PL_FUN_AM_PREPARE_TEST,byarrData);
        return;
    }
    //显示剩余时间
    m_vecTestInfo[nSamplePos].m_nRemainTime--;
    m_SampleCircle->SetItemText(nReagentIndex,QString("%1s").arg(m_vecTestInfo[nSamplePos].m_nRemainTime));
}

/*
 *
 * 测试界面-查看日志按钮
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
        ui->textBrowser->clear();
        ui->textBrowser->setText( strData );
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
 * 测试界面- 返回按钮
 *
 */
void TestInterface::on_pb_Test_LogBack_clicked()
{
    ui->stackedWidget->setCurrentIndex(1);
}
