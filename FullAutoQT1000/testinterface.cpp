#include "testinterface.h"
#include "ui_testinterface.h"

TestInterface::TestInterface(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TestInterface)
{
    ui->setupUi(this);
    //隐藏急诊选项
    //ui->checkBox_Test_Emergency->hide();
    //初始化圆界面
    InitCircleUI();
    //进度条控件
    InitProgressBar();
    //列表框
    InitTableInfo();
    //初始化测试信息
    InitVectorTestInfo();
    //初始化设备串口
    InitDeviceSerial();
    connect(&m_DeviceSerial,SIGNAL(readyRead()), this, SLOT(ReadDeviceData()) );
    //

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

}

TestInterface::~TestInterface()
{
    delete ui;
}


/////////////////////////////////////////////////////////
bool TestInterface::InitDeviceSerial()
{
    //串口初始化
    m_DeviceSerial.setBaudRate(BAUD9600);
#ifdef Q_OS_WIN32
    m_DeviceSerial.setPortName("\\\\.\\com1");
#else
    m_DeviceSerial.setPortName("/dev/ttyO2");
#endif
    bool bOpen = m_DeviceSerial.open(QIODevice::ReadWrite);
    if(!bOpen){
        qDebug()<<"Device Serial open the fail";
    }
    return bOpen;
}

/********************************************************
 *@Name:        ReadDeviceData
 *@Author:      HuaT
 *@Description: 接收仪器串口数据
 *@Param1:      无
 *@Return:      无
 *@Version:     1.0
 *@Date:        2018-5-21
********************************************************/
void TestInterface::ReadDeviceData()
{
    //qDebug()<<m_DeviceSerial.readAll().toHex();
    QByteArray byarrData = m_DeviceSerial.readAll();
    MsgData CurrData;
    //qDebug()<<byarrData.right(4).toHex().toUInt(0,16);
    if(CurrData.m_frameHeader == byarrData.mid(0,4).toHex().toUInt(0,16) && CurrData.m_frameEnd == byarrData.right(4).toHex().toUInt(0,16)){
        //QByteArray byarrDataContent = byarrData.mid(16,CurrData.m_DataLength);
        //CurrData.m_CRC = byarrData.mid(byarrData.size()-8,4).toHex().toUInt(0,16);
        int nDataCount = byarrData.size();
        quint8 nContentLen = BytesToInt(byarrData.mid(12,4));
        CurrData.m_CRC = BytesToInt(byarrData.mid(nDataCount-8,4));
        //qDebug()<<"nContentLen"<<nContentLen;
        quint32 nCRC = GetCheckSum(byarrData.mid(12,4),byarrData.mid(16,nContentLen*4));
        qDebug()<<"my check code:"<<nCRC<<" currdata.m_crc:"<<CurrData.m_CRC;
        if(nCRC == CurrData.m_CRC ){
            CurrData.m_ProductID = BytesToInt( byarrData.mid(4,4) );
            CurrData.m_DeviceAdd = BytesToInt( byarrData.mid(8,2) );
            CurrData.m_FuncIndex = BytesToInt( byarrData.mid(10,2) );
            CurrData.m_DataLength = BytesToInt( byarrData.mid(12,4) );
            CurrData.m_Data = byarrData.mid(16,CurrData.m_DataLength*4) ;
            //MsgData msgd;
            switch(CurrData.m_FuncIndex){
            case PL_FUN_ACK:
                //返回ACK
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
            case PL_FUN_AM_TEST_RESULT :
                //收到测试结果
                qDebug()<<"PL_FUN_AM_TEST_RESULT ";
                break;
            case PL_FUN_AM_PREPARE_UNSTALL_COMPLETE:
                //卸卡准备完成
                break;
            case PL_FUN_AM_START_UNSTALL_COMPLETE:
                //卸卡完成
                break;
            default:
                qDebug()<<"command is invalid!";
            }

        }

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
    ProceCountDownEvent(nReagentPos,nSamplePos);
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
    bool isScan = true;
    if(isScan){
        //发送扫码信息
        QByteArray byarrData;
        SendSerialData(m_DeviceSerial,PL_FUN_AM_NEW_TEST,byarrData);
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
    m_nTestIndex = 0;
    //检测用户输入
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
            m_vecTestInfo[n].m_bIsTest     = true;
            nCount++;
        }
        //qDebug()<<m_vecTestInfo.at(n).m_nSamplePos<<":"<<m_vecTestInfo.at(n).m_bIsEmerg<<"-"<<m_vecTestInfo.at(n).m_strSampleNo<<"-"<<m_vecTestInfo.at(n).m_strItemName<<";";
    }
    ui->lb_Test_SampleCount->setText(QString("%1").arg(nCount));

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
    //发送首个测试信息给仪器
    quint8  nEmerIndex = ui->lb_Test_CurrentEmerPos->text().toInt();
    if(nEmerIndex != 0){
        TestInfo ti = m_vecTestInfo.at(nEmerIndex-1);
        m_vecTestInfo.remove(nEmerIndex-1);
        m_vecTestInfo.insert(0,ti);
    }
    //一次性发送测试编号数据
    QByteArray arrTestNumbleData;
    arrTestNumbleData.append(IntToBytes(1));
    arrTestNumbleData.append(IntToBytes(1));
    SendSerialData(m_DeviceSerial,PL_FUN_AM_START_SAMPLE,arrTestNumbleData);
    /*for(int n=0; n<16;n++){
        qDebug()<<m_vecTestInfo.at(n).m_nSamplePos<<":"<<m_vecTestInfo.at(n).m_bIsEmerg<<"-"<<m_vecTestInfo.at(n).m_strSampleNo<<"-"<<m_vecTestInfo.at(n).m_strItemName<<";";

    }*/
}


/*
 *初始化测试圆和样品圆界面
 */
void TestInterface::InitCircleUI()
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
    ui->proBar_Test_Reagent2->setStyleSheet("QProgressBar{background-color: #E6E6E6;border: 1px solid grey;border-radius: 1px;}");
    ui->proBar_Test_Reagent3->setStyleSheet("QProgressBar{background-color: #E6E6E6;border: 1px solid grey;border-radius: 1px;}");

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

}

/********************************************************
 *@Name:        TransformProjectName
 *@Author:      HuaT
 *@Description: 项目名称转化,hsCPR在hsCRP/PCT经过转化后为PCT
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
 *@Return:      无
 *@Version:     1.0
 *@Date:        2018-5-22
********************************************************/
void TestInterface::ResponseProjectButton(QPushButton *butObj)
{
    QString strResult = "";
    QString strCurName = butObj->text();
    QString strNameRecord = ui->lb_Test_TestProject->text();

    strResult = TransformProjectName(strCurName,strNameRecord);

    //qDebug()<<ui->checkBox_Test_AllSelect->checkState();
    if(ui->checkBox_Test_AllSelect->isChecked() == true){
        for(int n=0; n< TABLE_ROWS_COUNT; n++){
            SetCellContent(TABLE_COLUMN_COUNT-1,n,strResult);
        }
    }else{
        //
        int nCurRow = ui->tw_Test_TestInfo->currentRow();
        if(nCurRow == -1){
            return;
        }
        SetCellContent(TABLE_COLUMN_COUNT-1,nCurRow,strResult);
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

    ui->pb_Test_TestName1->setEnabled(isTrue);
    ui->pb_Test_TestName2->setEnabled(isTrue);
    ui->pb_Test_TestName3->setEnabled(isTrue);
    ui->pb_Test_TestName4->setEnabled(isTrue);

    ui->le_Test_SampleNumber->setEnabled(isTrue);

    ui->checkBox_Test_Emergency->setEnabled(isTrue);

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
    for(int n=1; n<17; n++){
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
    ResponseProjectButton(ui->pb_Test_TestName1);
}

/*
 *详细页测试项目2按钮响应事件
 */
void TestInterface::on_pb_Test_TestName2_clicked()
{
    ResponseProjectButton(ui->pb_Test_TestName2);
}

/*
 *详细页测试项目3按钮响应事件
 */
void TestInterface::on_pb_Test_TestName3_clicked()
{
    ResponseProjectButton(ui->pb_Test_TestName3);
}

/*
 *详细页测试项目4按钮响应事件
 */
void TestInterface::on_pb_Test_TestName4_clicked()
{
    ResponseProjectButton(ui->pb_Test_TestName4);
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
        //当前行为急诊
        m_vecTestInfo[nRow].m_bIsEmerg= true;
        //显示当前的急诊行
        ui->lb_Test_CurrentEmerPos->setText(QString("%1").arg(nRow+1));
    }//取消急诊
    else{
        SetRowColor(QColor(255,255,255),nRow);
        //取消当前行为急诊
        m_vecTestInfo[nRow].m_bIsEmerg = false;
        //当前无急诊
        ui->lb_Test_CurrentEmerPos->setText(QObject::tr("无"));
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
void TestInterface::ParseScanBarCodeInfo(const QByteArray &ScanData)
{
    if(ScanData.size() != 320){
        return;
    }
    ScanBarCodeInfo barcode;
    memcpy_s(&barcode, SAMPLECOUNT * sizeof(ScanBarCodeInfo), ScanData, ScanData.size());
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
        m_vecTestInfo[n].m_si.m_strSampleNo = strBarCode;
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
 *@Return:      无
 *@Version:     1.0
 *@Date:        2018-6-16
********************************************************/
void TestInterface::ParseLoadSampleInfo(const QByteArray &SampleData, SampleInfo& si)
{
    if(SampleData.size() != 8){
        qDebug()<<"ParseLoadSampleInfo:Short Current Length is "<<SampleData.size();
        return ;
    }
    si.m_SamPos.m_nSamplePos = MAKEFOURWORD(SampleData.at(0),SampleData.at(1),SampleData.at(2),SampleData.at(3));
    si.m_SamPos.m_nReagentPos = MAKEFOURWORD(SampleData.at(4),SampleData.at(5),SampleData.at(6),SampleData.at(7));
    //记录样本位对应的试剂位
    m_vecTestInfo[si.m_SamPos.m_nSamplePos].m_si.m_SamPos.m_nReagentPos = si.m_SamPos.m_nReagentPos;
}

/********************************************************
 *@Name:        ProceStartLoadSample
 *@Author:      HuaT
 *@Description: 处理加样信息
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
    ParseLoadSampleInfo(SampleData,si);
    //试剂盘颜色改变
    m_SampleCircle->SetItemColor(si.m_SamPos.m_nReagentPos,QString("rgb(255,0,0)"));
    //对应试剂位置开始计时
    m_vecTestInfo[si.m_SamPos.m_nSamplePos].m_nRemainTime = 10;
    m_vecTestInfo.at(si.m_SamPos.m_nSamplePos).m_timer->setProperty("ReagentPos",si.m_SamPos.m_nReagentPos);
    m_vecTestInfo.at(si.m_SamPos.m_nSamplePos).m_timer->setProperty("SamplePos",si.m_SamPos.m_nSamplePos);
    m_vecTestInfo.at(si.m_SamPos.m_nSamplePos).m_timer->start(1000);
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
    memcpy_s(&sp,AckData.size(),AckData,AckData.size());
    //qDebug()<<"samplepos:"<<sp.m_nSamplePos<<sp.m_nReagentPos;
    //发送开始测试信号
    QByteArray byarrData;
    byarrData.append(IntToBytes(sp.m_nSamplePos));
    byarrData.append(IntToBytes(sp.m_nReagentPos));
    SendSerialData(m_DeviceSerial,PL_FUN_AM_START_TEST,byarrData);
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
        SendSerialData(m_DeviceSerial,PL_FUN_AM_PREPARE_TEST,byarrData);
        return;
    }
    //显示剩余时间
    m_vecTestInfo[nSamplePos].m_nRemainTime--;
    m_SampleCircle->SetItemText(nReagentIndex,QString("%1s").arg(m_vecTestInfo[nSamplePos].m_nRemainTime));
}


