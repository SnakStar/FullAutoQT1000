﻿#include "setinterface.h"
#include "ui_setinterface.h"
#include "mainwindow.h"
#include "paramdef.h"
#include "msgboxdlg.h"

SetInterface::SetInterface(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SetInterface)
{
    ui->setupUi(this);
    //
    m_pMainWnd = parent;
    //初始化导航栏
    InitNavigation();
    //初始化时间控件
    InitSysTimeCon();
    //初始化图形对象
    m_packPlot = new CPackCustomPlot(ui->wg_Curve_CustomPlot);
    m_packPlot->InitPlotParam(700);
    //初始化曲线界面控件
    InitCurveControl();
    //获取配置文件对象
    m_pSettinsObj = (static_cast<MainWindow*>(m_pMainWnd))->GetSettingsObj();
    //获取测试界面对象
    m_pTestInterface = (static_cast<MainWindow*>(m_pMainWnd))->GetTestInterface();
    //获取数据库对象
    m_devDB = (static_cast<MainWindow*>(m_pMainWnd))->GetDatabaseObj();
    //
    connect(m_pTestInterface,SIGNAL(sendSampleTypeAddSignals()),this,SLOT(RecvSampleTypeAddSignals()) );
    connect(m_pTestInterface,SIGNAL(sendHardwareVersionData(QByteArray)),this,SLOT(RecvHardwareVersion(QByteArray)) );
    //m_pTestInterface->m_mapTestItemPos.key("4","");
    //获取配置参数内容
    //m_SetParam = m_pSettinsObj->ReadSettingsInfoToMap();
    //根据参数显示控件选项
    UpdateUIControl();
    //初始化参考值表框
    InitRenfTable();
    //初始化用户帐户表框
    InitUserAccountTable();
    //更新数据到帐户控件
    UpdateUserTable();
    //获取加样样本类型指针对象
    m_pmapSampleTypeObj = m_pTestInterface->GetSampleTypeObj();
    //初始化样本类型列表框
    InitSampleTypeTable();
    //更新加样样本类型数据到列表框
    UpdateSampleTypeTable();
    //关机按钮
    ui->btn_Set_Poweroff->hide();
    //编辑框输出属性
    ui->le_Set_DebugLoginPwd->setProperty("flag","number");
    //日志输出
    qInstallMsgHandler(myMessageOutput);
    QFile fi(":/res/qss/SettingStyle.qss");
    if(fi.open(QIODevice::ReadOnly)){
        QTextStream ts(&fi);
        QString strStyle = ts.readAll();
        this->setStyleSheet(strStyle);
        fi.close();
    }
}

SetInterface::~SetInterface()
{
    delete ui;
}

bool SetInterface::eventFilter(QObject *obj, QEvent *e)
{
    /*if( obj == ui->le_Set_DebugLoginPwd && (e->type() == QEvent::MouseButtonPress) ){
        m_myInputPanel->showInputPanel();
        return true;
    }*/
    return QObject::eventFilter(obj,e);
}

/********************************************************
 *@Name:        UpdateDbToRenf
 *@Author:      HuaT
 *@Description: 更新参考值数据表内容到表格上
 *@Param:       无
 *@Return:      无
 *@Version:     1.0
 *@Date:        2016-6-13
********************************************************/
void SetInterface::UpdateDbToRenf()
{
    ui->tw_Set_RenfvInfo->setRowCount(0);
    QString querySql = "select * from renfvalue";
    QSqlQuery* query = m_devDB->GetSqlQuery();
    query->exec(querySql);
    quint8 nSex;
    QString strAge;
    while(query->next()){
        ui->tw_Set_RenfvInfo->insertRow(ui->tw_Set_RenfvInfo->rowCount());
        //项目名称
        QTableWidgetItem* item1 = new QTableWidgetItem;
        item1->setText(query->value(0).toString());
        item1->setTextAlignment(Qt::AlignCenter);
        ui->tw_Set_RenfvInfo->setItem(ui->tw_Set_RenfvInfo->rowCount()-1,0,item1);
        //参考值
        QTableWidgetItem* item2 = new QTableWidgetItem;
        item2->setText(query->value(1).toString());
        item2->setTextAlignment(Qt::AlignCenter);
        ui->tw_Set_RenfvInfo->setItem(ui->tw_Set_RenfvInfo->rowCount()-1,1,item2);
        //年龄
        QTableWidgetItem* item3 = new QTableWidgetItem;
        strAge = m_UtilTool.ProcValue(query->value(2).toString(),query->value(3).toString());
        item3->setText(strAge);
        item3->setTextAlignment(Qt::AlignCenter);
        ui->tw_Set_RenfvInfo->setItem(ui->tw_Set_RenfvInfo->rowCount()-1,2,item3);
        //性别
        QTableWidgetItem* item4 = new QTableWidgetItem;
        nSex = query->value(4).toInt();
        if(nSex == 1){
            item4->setText(QObject::tr("男"));
        }else if(nSex == 2){
            item4->setText(QObject::tr("女"));
        }else{
            item4->setText(QObject::tr("男/女"));
        }
        item4->setTextAlignment(Qt::AlignCenter);
        ui->tw_Set_RenfvInfo->setItem(ui->tw_Set_RenfvInfo->rowCount()-1,3,item4);
    }
}

/********************************************************
 *@Name:        InitNavigation
 *@Author:      HuaT
 *@Description: 初始化导航栏
 *@Param1:      无
 *@Return:      无
 *@Version:     1.0
 *@Date:        2018-8-17
********************************************************/
void SetInterface::InitNavigation()
{
    QStringList items;
    items<<QObject::tr("常规设置")<<QObject::tr("参考值设置")<<QObject::tr("曲线显示")
        <<QObject::tr("系统设置")<<QObject::tr("用户管理")<<QObject::tr("系统信息")
        <<QObject::tr("调试设置");
    m_lisIconPath<<":/res/image/Setting/SettingNormal"
                 <<":/res/image/Setting/SettingRefer"
                 <<":/res/image/Setting/SettingSystemCurve"
                 <<":/res/image/Setting/SettingSystemSet"
                 <<":/res/image/Setting/SettingUserManager"
                 <<":/res/image/Setting/SettingSystemInfo"
                 <<":/res/image/Setting/SettingDebug";
    m_lisIconPathPressed<<""<<""<<""<<""<<""<<""<<"";
    for(int n=0; n<items.size(); n++){
        QListWidgetItem* subitem = new QListWidgetItem;
        //subitem->setTextAlignment(Qt::AlignCenter);
        subitem->setText(items.at(n));
        subitem->setSizeHint(QSize(187,80));
        subitem->setIcon(QIcon(m_lisIconPath.at(n)));
        ui->lw_Set_Navigation->addItem(subitem);
    }
    ui->lw_Set_Navigation->setIconSize(QSize(32,32));
    ui->lw_Set_Navigation->item(0)->setSelected(true);
}

/********************************************************
 *@Name:        UpdateUIControl
 *@Author:      HuaT
 *@Description: 初始化界面控件参数
 *              扫描模式 --- 0：自动编码，1：自动扫码
 *@Param1:      无
 *@Return:      无
 *@Version:     1.0
 *@Date:        2018-8-17
********************************************************/
void SetInterface::UpdateUIControl()
{
    if(m_pSettinsObj->GetParam(SCANMODE).toInt()  == 1){
        ui->rb_Set_ScanEncode->setChecked(true);
    }else{
        ui->rb_Set_AutoEncode->setChecked(true);
    }
    if(m_pSettinsObj->GetParam(AUTOLOGIN).toInt() == 1){
        ui->rb_Set_AutoLoginOpen->setChecked(true);
    }else{
        ui->rb_Set_AutoLoginClose->setChecked(true);
    }
    if(m_pSettinsObj->GetParam(LISMODE).toInt() == 1){
        ui->rb_Set_HL7Lis->setChecked(true);
    }else{
        ui->rb_Set_NULLLis->setChecked(true);
    }
}


/********************************************************
 *@Name:        InitUserAccountTable
 *@Author:      HuaT
 *@Description: 初始化用户帐户信息框
 *@Param1:      无
 *@Return:      无
 *@Version:     1.0
 *@Date:        2018-8-17
********************************************************/
void SetInterface::InitUserAccountTable()
{
    ui->tw_Set_UserAccount->setEditTriggers(QTableWidget::NoEditTriggers);
    //ui->tw_Set_UserAccount->setSelectionMode(QTableWidget::SingleSelection);
    ui->tw_Set_UserAccount->setSelectionBehavior(QTableWidget::SelectRows);
    ui->tw_Set_UserAccount->setColumnCount(3);
    ui->tw_Set_UserAccount->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
    ui->tw_Set_UserAccount->verticalHeader()->setHidden(true);

    QStringList listHeader;
    listHeader<<"序号"<<"用户名"<<"密码";
    ui->tw_Set_UserAccount->setHorizontalHeaderLabels(listHeader);
    ui->tw_Set_UserAccount->verticalHeader()->setDefaultSectionSize(42);
}

//初始化系统时间控件
void SetInterface::InitSysTimeCon()
{
    QDate dt = QDate::currentDate();
    QTime tm = QTime::currentTime();
    ui->de_Set_dteDateSet->setDate(dt);
    ui->de_Set_dteTimeSet->setTime(tm);
    //
    ui->de_Set_dteDateSet->setProperty("noinput",true);
    ui->de_Set_dteTimeSet->setProperty("noinput",true);
}

/********************************************************
 *@Name:        InitRenfTable
 *@Author:      HuaT
 *@Description: 初始化参考值列表框
 *@Param1:      无
 *@Return:      无
 *@Version:     1.0
 *@Date:        2018-8-17
********************************************************/
void SetInterface::InitRenfTable()
{
    ui->tw_Set_RenfvInfo->setSelectionBehavior(QTableWidget::SelectRows);
    ui->tw_Set_RenfvInfo->setEditTriggers(QTableWidget::NoEditTriggers);
    //ui->tw_Set_RenfvInfo->setSelectionMode(QTableWidget::SingleSelection);
    ui->tw_Set_RenfvInfo->setColumnCount(4);
    ui->tw_Set_RenfvInfo->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
    ui->tw_Set_RenfvInfo->verticalHeader()->setHidden(true);

    QStringList listHeader;
    listHeader<<"测试名称"<<"参考值"<<"年龄"<<"性别";
    ui->tw_Set_RenfvInfo->setHorizontalHeaderLabels(listHeader);
    ui->tw_Set_RenfvInfo->verticalHeader()->setDefaultSectionSize(42);
}

/********************************************************
 *@Name:        InitSampleTypeTable
 *@Author:      HuaT
 *@Description: 初始化加样样本类型列表框
 *@Param1:      无
 *@Return:      无
 *@Version:     1.0
 *@Date:        2019-6-14
********************************************************/
void SetInterface::InitSampleTypeTable()
{
    ui->tw_Set_GeneralSampleType->setSelectionBehavior(QTableWidget::SelectRows);
    ui->tw_Set_GeneralSampleType->setEditTriggers(QTableWidget::NoEditTriggers);
    ui->tw_Set_GeneralSampleType->setColumnCount(3);
    ui->tw_Set_GeneralSampleType->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
    ui->tw_Set_GeneralSampleType->verticalHeader()->setHidden(true);

    QStringList listHeader;
    listHeader<<"项目名称"<<"血清/血浆"<<"全血";
    ui->tw_Set_GeneralSampleType->setHorizontalHeaderLabels(listHeader);
    ui->tw_Set_GeneralSampleType->verticalHeader()->setDefaultSectionSize(42);
}

/*
 *
 *当前行改变
 */
void SetInterface::on_lw_Set_Navigation_currentRowChanged(int currentRow)
{
    ui->stackedWidget->setCurrentIndex(currentRow);
}

/*
 *
 * 常规界面-删除样本类型
 *
 */
void SetInterface::on_btn_Set_GenDelSampleType_clicked()
{
    int nCurRow = ui->tw_Set_GeneralSampleType->currentRow();
    if(nCurRow == -1){
        return;
    }
    QString strTypeName = ui->tw_Set_GeneralSampleType->item(nCurRow,0)->text();
    //m_pSettinsObj->DelParam(strTypeName);
    //内存删除
    m_pmapSampleTypeObj->remove(strTypeName);
    //控件删除
    ui->tw_Set_GeneralSampleType->removeRow(nCurRow);
    //数据库删除
    QString strSql = QString( "delete from sampletype where itemname='%1' ").arg(strTypeName);
    if(m_devDB->Exec(strSql) == false ){
        qDebug()<<"del sampletype to data failed";
    }
}

/*
 *
 * 常规界面-确定按钮
 *
 */
void SetInterface::on_btn_Set_GenSave_clicked()
{
    //扫码模式
    if(ui->rb_Set_AutoEncode->isChecked()){
        m_pSettinsObj->SetParam(SCANMODE,"0");
    }else{
        m_pSettinsObj->SetParam(SCANMODE,"1");
    }
    if(ui->rb_Set_AutoLoginClose->isChecked()){
        m_pSettinsObj->SetParam(AUTOLOGIN,"0");
    }else{
        m_pSettinsObj->SetParam(AUTOLOGIN,"1");
    }
    if(ui->rb_Set_NULLLis->isChecked()){
        m_pSettinsObj->SetParam(LISMODE,"0");
    }else{
        m_pSettinsObj->SetParam(LISMODE,"1");
    }
    //写入配置文件
    m_pSettinsObj->SaveAllSettingsInfoToFile();
    //保存当前配置数据到样本类型数据库
    int row = ui->tw_Set_GeneralSampleType->rowCount();
    bool bCheck = false;
    for(int n=0; n<row; n++){
        QString strItem1 = ui->tw_Set_GeneralSampleType->item(n,0)->text();
        bCheck = static_cast<QRadioButton*>(ui->tw_Set_GeneralSampleType->cellWidget(n,1) )->isChecked();
        if(bCheck == true){
            //血清
            (*m_pmapSampleTypeObj)[strItem1] = "0";
        }else{
            //全血
            (*m_pmapSampleTypeObj)[strItem1] = "1";
        }
    }
    //map数据对象更新到数据库
    SaveSampleTypeObjToDb();
    emit m_pSettinsObj->emitSettingChange();
    //提示
    MsgBoxDlg msgbox;
    msgbox.ShowMsg("保存成功",0);
    msgbox.exec();
}

/*
 *
 * 常规界面-取消按钮
 *
 */
void SetInterface::on_btn_Set_GenCancel_clicked()
{
    //更新控件
    UpdateUIControl();
}


/*
 *
 * 常规界面-自动编码单选控件
 *
 */
void SetInterface::on_rb_Set_AutoEncode_clicked()
{
    m_pSettinsObj->SetParam(SCANMODE,"0");
}

/*
 *
 * 常规界面-自动扫码单选控件
 *
 */
void SetInterface::on_rb_Set_ScanEncode_clicked()
{
    m_pSettinsObj->SetParam(SCANMODE,"1");
}

/*
 *
 * 参考值界面-增加按钮
 *
 */
void SetInterface::on_btn_Set_RenfAdd_clicked()
{
    RenfInfo ri;
    ri.m_listItem = m_pTestInterface->GetTestItems().keys();
    //
    RenfValue *RenfvDlg = new RenfValue(ri,this);
    connect(RenfvDlg,SIGNAL(RenfInfoAddComplete(RenfInfo&)),this,SLOT(RecvAddRenfValue(RenfInfo&)));
    RenfvDlg->SetShowMode(true);
    RenfvDlg->show();
}

/*
 *
 * 参考值界面-修改按钮
 *
 */
void SetInterface::on_btn_Set_RenfModify_clicked()
{
    //参考值对话框
    RenfInfo ri;
    QString Title = QObject::tr("提示");
    QString Msg = QObject::tr("请先选择需要修改的数据");
    int CurrentRow = ui->tw_Set_RenfvInfo->currentRow();
    if(CurrentRow == -1){
        MsgBoxDlg msgbox;
        msgbox.ShowMsg(Msg,0);
        msgbox.exec();
        return;
    }
    QStringList strList = GetRowValue(ui->tw_Set_RenfvInfo);
    //项目名称
    ri.m_Item = strList.at(0);
    //项目列表
    //QString strCMD = "select item from itemtype";
    ri.m_listItem = m_pTestInterface->GetTestItems().keys();
    //qDebug()<<ri.m_listItem;
    //参考值
    ri.m_Value = strList.at(1);
    //年龄
    QStringList listAge = m_UtilTool.ParseValue(strList.at(2));
    ri.m_AgeLow = listAge.at(0);
    ri.m_AgeHight = listAge.at(1);
    //性别
    ri.m_Sex = strList.at(3);
    RenfValue *RenfvDlg = new RenfValue(ri,this);
    connect(RenfvDlg,SIGNAL(RenfInfoModifyComplete(RenfInfo&)),this,SLOT(RecvModifyRenfValue(RenfInfo&)));
    RenfvDlg->SetShowMode(false);
    RenfvDlg->show();
}

/*
 *
 * 参考值界面-删除按钮
 *
 */
void SetInterface::on_btn_Set_RenfDelete_clicked()
{
    QString Title = QObject::tr("提示");
    QString Msg = QObject::tr("请选择需要删除的数据");
    int CurrentRow = ui->tw_Set_RenfvInfo->currentRow();
    if(CurrentRow == -1){
        MsgBoxDlg msgbox;
        msgbox.ShowMsg(Msg,0);
        msgbox.exec();
        return;
    }
    QString strTitle,strContent;
    strTitle = QObject::tr("提示");
    strContent = QObject::tr("确认删除此数据?");

    MsgBoxDlg msgbox;
    msgbox.ShowMsg(strContent,1);
    msgbox.exec();
    if(1 == msgbox.exec()){
        QStringList stringList = GetRowValue(ui->tw_Set_RenfvInfo);
        //更新数据库
        QString deleteSql;
        QStringList listAge;
        listAge = m_UtilTool.ParseValue(stringList.at(2));
        quint8 nSex;
        if(stringList.at(3).compare(QObject::tr("Male")) == 0){
            nSex = 1;
        }else if(stringList.at(3).compare(QObject::tr("Female")) == 0){
            nSex = 2;
        }else{
            nSex = 0;
        }
        deleteSql = QString("delete from renfvalue where \
                            item='%1' and value='%2' and agelow='%3' and agehight='%4'and sex=%5 ")\
                .arg(stringList.at(0)).arg(stringList.at(1)).arg(listAge.at(0))
                .arg(listAge.at(1)).arg(nSex);
                m_devDB->Exec(deleteSql);
        //qDebug()<<deleteSql;
        //更新控件
        ui->tw_Set_RenfvInfo->removeRow(CurrentRow);
    }
}

/********************************************************
 *@Name:        RecvAddRenfValue
 *@Author:      HuaT
 *@Description: 接收新增参考值信息
 *@Param:       需要新增的参考值内容
 *@Return:      无
 *@Version:     1.0
 *@Date:        2016-5-13
********************************************************/
void SetInterface::RecvAddRenfValue(RenfInfo &ri)
{
    //插入数据库
    //用户性别，0为男/女，1为男，2为女，Male / Female
    quint8 nSex;
    if(ri.m_Sex.compare(QObject::tr("男")) == 0){
        nSex = 1;
    }else if(ri.m_Sex.compare(QObject::tr("女")) == 0){
        nSex = 2;
    }else{
        nSex = 0;
    }
    QString insertSql;
    insertSql = QString("insert into renfvalue values('%1','%2','%3','%4',%5);")\
            .arg(ri.m_Item).arg(ri.m_Value).arg(ri.m_AgeLow).arg(ri.m_AgeHight).arg(nSex);
    m_devDB->Exec(insertSql);
    //插入表单控件
    ui->tw_Set_RenfvInfo->insertRow(ui->tw_Set_RenfvInfo->rowCount());
    //项目名称
    QTableWidgetItem* item = new QTableWidgetItem;
    item->setText(ri.m_Item);
    ui->tw_Set_RenfvInfo->setItem(ui->tw_Set_RenfvInfo->rowCount()-1,0,item);
    //值
    QTableWidgetItem* value = new QTableWidgetItem;
    value->setText(ri.m_Value);
    ui->tw_Set_RenfvInfo->setItem(ui->tw_Set_RenfvInfo->rowCount()-1,1,value);
    //年龄
    QTableWidgetItem* age = new QTableWidgetItem;
    age->setText(m_UtilTool.ProcValue(ri.m_AgeLow,ri.m_AgeHight));
    ui->tw_Set_RenfvInfo->setItem(ui->tw_Set_RenfvInfo->rowCount()-1,2,age);
    //性别
    QTableWidgetItem* sex = new QTableWidgetItem;
    sex->setText(ri.m_Sex);
    ui->tw_Set_RenfvInfo->setItem(ui->tw_Set_RenfvInfo->rowCount()-1,3,sex);
}


/********************************************************
 *@Name:        RecvModifyRenfValue
 *@Author:      HuaT
 *@Description: 接收修改参考值信息
 *@Param:       需要修改的参考值内容
 *@Return:      无
 *@Version:     1.0
 *@Date:        2016-5-13
********************************************************/
void SetInterface::RecvModifyRenfValue(RenfInfo &ri)
{
    int nCurrentRow = ui->tw_Set_RenfvInfo->currentRow();
    QStringList strList = GetRowValue(ui->tw_Set_RenfvInfo);
    //
    quint8 nSex,nOldSex;
    if(ri.m_Sex.compare(QObject::tr("Male")) == 0){
        nSex = 1;
    }else if(ri.m_Sex.compare(QObject::tr("Female")) == 0){
        nSex = 2;
    }else{
        nSex = 0;
    }
    if(strList.at(3).compare(QObject::tr("Male")) == 0){
        nOldSex = 1;
    }else if(strList.at(3).compare(QObject::tr("Female")) == 0){
        nOldSex = 2;
    }else{
        nOldSex = 0;
    }
    QStringList listOleAge;
    //0：下限，没有则插入空
    //1：上限，没有则插入空
    listOleAge = m_UtilTool.ParseValue(strList.at(2));
    QString UpdateSql;
    //更新数据库内容
    UpdateSql = QString("update renfvalue set item='%1',value='%2',AgeLow='%3',AgeHight='%4'\
                        ,sex=%5 where item='%6' and value='%7' and AgeLow='%8' and AgeHight='%9' and sex=%10 ")\
            .arg(ri.m_Item).arg(ri.m_Value).arg(ri.m_AgeLow).arg(ri.m_AgeHight)
            .arg(nSex).arg(strList.at(0)).arg(strList.at(1))
            .arg(listOleAge.at(0)).arg(listOleAge.at(1)).arg(nOldSex);
    //qDebug()<<UpdateSql;
    if(!m_devDB->Exec(UpdateSql)){
        qDebug()<<"RecvModifyRenfValue: update renfvalue error";
    }
    //更新控件内容
    //项目名称
    QTableWidgetItem* item = new QTableWidgetItem;
    item->setText(ri.m_Item);
    ui->tw_Set_RenfvInfo->setItem(nCurrentRow,0,item);
    //新值
    QTableWidgetItem* Value = new QTableWidgetItem;
    Value->setText(ri.m_Value);
    ui->tw_Set_RenfvInfo->setItem(nCurrentRow,1,Value);
    //新的年龄
    QTableWidgetItem* Age = new QTableWidgetItem;
    Age->setText(m_UtilTool.ProcValue(ri.m_AgeLow,ri.m_AgeHight));
    ui->tw_Set_RenfvInfo->setItem(nCurrentRow,2,Age);
    //新的性别
    QTableWidgetItem* Sex = new QTableWidgetItem;
    Sex->setText(ri.m_Sex);
    ui->tw_Set_RenfvInfo->setItem(nCurrentRow,3,Sex);
}

/********************************************************
 *@Name:        RecvSampleTypeAddSignals
 *@Author:      HuaT
 *@Description: 接收加样样本增加信号
 *@Param:       无
 *@Return:      无
 *@Version:     1.0
 *@Date:        2016-5-13
********************************************************/
void SetInterface::RecvSampleTypeAddSignals()
{
    UpdateSampleTypeTable();
}

/********************************************************
 *@Name:        RecvHardwareVersion
 *@Author:      HuaT
 *@Description: 接收硬件版本数据并显示
 *@Param:       无
 *@Return:      无
 *@Version:     1.0
 *@Date:        2019-9-18
********************************************************/
void SetInterface::RecvHardwareVersion(QByteArray baData)
{
    quint32 nMainV = 0;
    quint32 nSubV = 0;
    quint32 nNumber = 0;
    if(baData.size() != 12){
        ui->lb_Set_HardNum->setText("V0.0.0");
        return;
    }
    nMainV = m_pTestInterface->BytesToInt(baData.mid(0,4));
    nSubV = m_pTestInterface->BytesToInt(baData.mid(4,4));
    nNumber = m_pTestInterface->BytesToInt(baData.mid(8,4));
    QString strHardVersion = QString("V%1.%2.%3").arg(nMainV).arg(nSubV).arg(nNumber);
    ui->lb_Set_HardNum->setText(strHardVersion);
}

/********************************************************
 *@Name:        GetRowValue
 *@Author:      HuaT
 *@Description: 获取指定表格控件的一行数据
 *@Param:       指定的表格控件对象
 *@Return:      一行的数据内容，未选择一行则返回空
 *@Version:     1.0
 *@Date:        2016-6-13
********************************************************/
QStringList SetInterface::GetRowValue(QTableWidget *tw)
{
    QStringList strList;
    if(tw->currentRow() == -1){
        return strList;
    }
    for(int i=0; i<tw->columnCount(); i++){
        strList.append(tw->item(tw->currentRow(),i)->text());
    }
    return strList;
}

/********************************************************
 *@Name:        on_btn_Set_SetDateSave_clicked
 *@Author:      HuaT
 *@Description: 时间设置按钮
 *@Param:       无
 *@Return:      无
 *@Version:     1.0
 *@Date:        2018-9-9
********************************************************/
void SetInterface::on_btn_Set_SetDateSave_clicked()
{
    QDate date = ui->de_Set_dteDateSet->date();
    QTime time = ui->de_Set_dteTimeSet->time();
    SetSysDateTime(date,time);
    QString msgContent = QObject::tr("时间保存成功");
    MsgBoxDlg msgbox;
    msgbox.ShowMsg(msgContent,0);
    msgbox.exec();
}

/********************************************************
 *@Name:        SetSysDateTime
 *@Author:      HuaT
 *@Description: 设置操作系统时间
 *@Param:
 *@Return:
 *@Version:     1.0
 *@Date:        2016-5-13
********************************************************/
void SetInterface::SetSysDateTime(QDate& date, QTime time)
{

    //qDebug()<<date<<time;
#ifdef Q_OS_WIN32
    SYSTEMTIME st;
    //getsystemtime函数使用的是UTC时间，设置时会有时间上的偏差
    //GetSystemTime(&st);
    GetLocalTime(&st);
    st.wYear = date.year();
    st.wMonth = date.month();
    st.wDay = date.day();

    st.wHour = time.hour();
    st.wMinute = time.minute();
    st.wSecond = time.second();
    //qDebug()<<date.year()<<date.month()<<date.day()<<time.hour()<<time.minute()<<time.second();
    //SetSystemTime(&st);
    SetLocalTime(&st);

#elif defined(Q_OS_LINUX)
#include<time.h>
    QString WriteTime;
    WriteTime = QString("date -s \"%1 %2\" ").arg(date.toString("yyyy-MM-dd")).arg(time.toString("HH:mm:ss"));
    system(WriteTime.toAscii());
    system("hwclock -w");

#else
//#ifdef __arm__
#include<time.h>
    QString WriteTime;
    WriteTime = QString("date -s \"%1 %2\" ").arg(date.toString("yyyy-MM-dd")).arg(time.toString("HH:mm:ss"));
    system(WriteTime.toAscii());
    system("hwclock -w");
#endif


}

/********************************************************
 *@Name:        UpdateUserTable
 *@Author:      HuaT
 *@Description: 更新用户帐户信息到表格控件
 *@Param:       无
 *@Return:      无
 *@Version:     1.0
 *@Date:        2018-9-19
********************************************************/
void SetInterface::UpdateUserTable()
{
    ui->tw_Set_UserAccount->setRowCount(0);
    QStringList listData;
    QString strSql;
    strSql = "select * from userlogin";
    listData = m_devDB->ExecQuery(strSql);

    for(int n=0; n<listData.size(); n+=3){
        ui->tw_Set_UserAccount->insertRow( ui->tw_Set_UserAccount->rowCount() );
        QTableWidgetItem* item1 = new QTableWidgetItem;
        item1->setText(listData.at(n));
        ui->tw_Set_UserAccount->setItem(ui->tw_Set_UserAccount->rowCount()-1,0,item1);
        //
        QTableWidgetItem* item2 = new QTableWidgetItem;
        item2->setText(listData.at(n+1));
        ui->tw_Set_UserAccount->setItem(ui->tw_Set_UserAccount->rowCount()-1,1,item2);
        //
        QTableWidgetItem* item3 = new QTableWidgetItem;
        item3->setText("******");
        ui->tw_Set_UserAccount->setItem(ui->tw_Set_UserAccount->rowCount()-1,2,item3);
    }
    //qDebug()<<listData;
}

/********************************************************
 *@Name:        UpdateSampleTypeTable
 *@Author:      HuaT
 *@Description: 更新加样样本类型数据到列表控件
 *@Param:       无
 *@Return:      无
 *@Version:     1.0
 *@Date:        2018-9-19
********************************************************/
void SetInterface::UpdateSampleTypeTable()
{
    ui->tw_Set_GeneralSampleType->setRowCount(0);
    m_pmapSampleTypeObj->clear();

    QString strSql = QString("select itemname,bloodtype from sampletype ");
    QStringList listRe = m_devDB->ExecQuery(strSql);
    for(int n=0; n<listRe.size(); n+=2){
        QButtonGroup* bg = new QButtonGroup(this);
        ui->tw_Set_GeneralSampleType->insertRow(ui->tw_Set_GeneralSampleType->rowCount());
        QTableWidgetItem* item = new QTableWidgetItem();
        item->setText(listRe.at(n));
        ui->tw_Set_GeneralSampleType->setItem(ui->tw_Set_GeneralSampleType->rowCount()-1,0,item);
        ui->tw_Set_GeneralSampleType->item(ui->tw_Set_GeneralSampleType->rowCount()-1,0)->setTextAlignment(Qt::AlignCenter);
        QRadioButton* rb1 = new QRadioButton("                                                 ");
        ui->tw_Set_GeneralSampleType->setCellWidget(ui->tw_Set_GeneralSampleType->rowCount()-1,1,rb1);
        ui->tw_Set_GeneralSampleType->item(ui->tw_Set_GeneralSampleType->rowCount()-1,0)->setTextAlignment(Qt::AlignCenter);
        bg->addButton(rb1);
        QRadioButton* rb2 = new QRadioButton("                                                 ");
        ui->tw_Set_GeneralSampleType->setCellWidget(ui->tw_Set_GeneralSampleType->rowCount()-1,2,rb2);
        ui->tw_Set_GeneralSampleType->item(ui->tw_Set_GeneralSampleType->rowCount()-1,0)->setTextAlignment(Qt::AlignCenter);
        bg->addButton(rb2);

        m_pmapSampleTypeObj->insert(listRe.at(n),listRe.at(n+1));
        if(listRe.at(n+1).compare("0") == 0){
            rb1->setChecked(true);
        }else{
            rb2->setChecked(true);
        }
    }
}

/********************************************************
 *@Name:        on_btn_Set_Poweroff_clicked
 *@Author:      HuaT
 *@Description: 关闭仪器
 *@Param:       无
 *@Return:      无
 *@Version:     1.0
 *@Date:        2018-9-19
********************************************************/
void SetInterface::on_btn_Set_Poweroff_clicked()
{
    QString strNote,strContent;
    strNote = QObject::tr("提示");
    strContent = QObject::tr("确定关闭仪器？");
    MsgBoxDlg msgbox;
    msgbox.ShowMsg(strContent,1);
    if(1 == msgbox.exec()){
#ifdef Q_OS_LINUX
        //sleep(3);
        //qApp->exit(0);
        system("poweroff");
#endif
    }

}

/********************************************************
 *@Name:        on_btn_Set_DebugOk_clicked
 *@Author:      HuaT
 *@Description: 调试登陆事件
 *@Param:       无
 *@Return:      无
 *@Version:     1.0
 *@Date:        2019-3-4
********************************************************/
void SetInterface::on_btn_Set_DebugOk_clicked()
{
    QString strPw = ui->le_Set_DebugLoginPwd->text();
    bool re = CheckPassword(strPw);
    //re = true;
    if(re == true){
        //qDebug()<<strPw;
        ui->le_Set_DebugLoginPwd->clear();
        QextSerialPort* ControlSerial = static_cast<MainWindow*>(m_pMainWnd)->GetControlPanelsSerial();
        QextSerialPort* CheckSerial = static_cast<MainWindow*>(m_pMainWnd)->GetCheckPanelsSerial();
        debugIntefaceDlg* di = new debugIntefaceDlg(m_devDB,m_pSettinsObj,ControlSerial,CheckSerial,m_pTestInterface);
        di->show();
    }else{
        MsgBoxDlg msgbox;
        msgbox.ShowMsg("密码输入错误",0);
        msgbox.exec();
    }
}

/********************************************************
 *@Name:        CheckPassword
 *@Author:      HuaT
 *@Description: 检验调试界面登陆密码
 *@Param:       无
 *@Return:      无
 *@Version:     1.0
 *@Date:        2019-3-4
********************************************************/
bool SetInterface::CheckPassword(QString strPw)
{
    QDate dt = QDate::currentDate();
    QString strCurDate = QString("%1%2").arg("1996").arg(dt.toString("MMdd"));
    if(strCurDate.compare(strPw) == 0){
        return true;
    }else{
        return false;
    }
}

/********************************************************
 *@Name:        SaveSampleTypeObjToDb
 *@Author:      HuaT
 *@Description: 保存加样样本数据内容到数据库
 *@Param:       无
 *@Return:      无
 *@Version:     1.0
 *@Date:        2019-6-15
********************************************************/
void SetInterface::SaveSampleTypeObjToDb()
{
    QString strSql;
    QStringList listKeys = m_pmapSampleTypeObj->keys();
    QString strKey,strValue;
    bool bRet;
    for(int n=0; n<listKeys.size(); n++){
        strKey = listKeys.at(n);
        strValue = m_pmapSampleTypeObj->value(listKeys.at(n));
        strSql = QString("update SampleType set bloodtype='%1' where itemname='%2' ").arg(strValue).arg(strKey);
        bRet = m_devDB->Exec(strSql);
        if(bRet == false){
            qDebug()<<"UpdateSampleTypeObjToDb is Failed ,";
        }
    }
}

/*
 *设置界面-用户增加事件
 * 1-新增用户
 */
void SetInterface::on_btn_Set_UserAdd_clicked()
{
    UserSetDlg us(m_devDB,ADDUSER,this);
    us.setWindowModality(Qt::WindowModal);
    if(us.exec() == true){
        UpdateUserTable();
    }
}

/*
 *设置界面-用户修改事件
 * 2-修改用户
 */
void SetInterface::on_btn_Set_UserModify_clicked()
{
    int nCurrRow = ui->tw_Set_UserAccount->currentRow();
    QString strTitle = QObject::tr("提示");
    QString strContent;
    if(nCurrRow == -1){
        strContent = QObject::tr("请先选择要修改的帐户");
        MsgBoxDlg msgbox;
        msgbox.ShowMsg(strContent,0);
        msgbox.exec();
        return;
    }else{
        QString strUserName = ui->tw_Set_UserAccount->item(nCurrRow,1)->text();
        UserSetDlg us(m_devDB,MODIFYUSER,strUserName,this);
        us.setWindowModality(Qt::WindowModal);
        if(us.exec() == true){
            strContent = QObject::tr("密码修改成功");
            MsgBoxDlg msgbox;
            msgbox.ShowMsg(strContent,0);
            msgbox.exec();
        }
    }
}

/*
 *设置界面-用户删除事件
 */
void SetInterface::on_btn_Set_UserDelete_clicked()
{
    int nCurrRow = ui->tw_Set_UserAccount->currentRow();
    QString strTitle = QObject::tr("提示");
    QString strContent;
    if(nCurrRow == -1){
        strContent = QObject::tr("请先选择要删除的帐户");
        MsgBoxDlg msgbox;
        msgbox.ShowMsg(strContent,0);
        msgbox.exec();
        return;
    }else{
        QString strSql;
        QString strIndex;
        QString strUserName;
        strUserName = ui->tw_Set_UserAccount->item(nCurrRow,1)->text();
        if(strUserName.compare("Admin") == 0){
            strContent = QObject::tr("不能删除默认用户Admin");
            MsgBoxDlg msgbox;
            msgbox.ShowMsg(strContent,0);
            msgbox.exec();
            return;
        }
        strIndex = ui->tw_Set_UserAccount->item(nCurrRow,0)->text();
        strSql = QString("delete from userLogin where ID='%1'").arg(strIndex.toInt());
        bool rel = m_devDB->Exec(strSql);
        if(rel == true){
            strContent = QObject::tr("用户删除成功");
            UpdateUserTable();
        }else{
            strContent = QObject::tr("用户删除失败");
        }
        MsgBoxDlg msgbox;
        msgbox.ShowMsg(strContent,0);
        msgbox.exec();
    }
}

void SetInterface::on_pb_Set_DebugLoginShow_pressed()
{
    ui->le_Set_DebugLoginPwd->setEchoMode(QLineEdit::Normal);
}

void SetInterface::on_pb_Set_DebugLoginShow_released()
{
    ui->le_Set_DebugLoginPwd->setEchoMode(QLineEdit::Password);
}

/*
 *
 * 初始化结果列表控件
 *
 */
void SetInterface::InitResultTableWidget()
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

    /*ui->tw_Curve_ResultInfo->verticalScrollBar()->setStyleSheet("QScrollBar:vertical{"
                                                                "background-color:rgb(240,240,240);"
                                                                "border: 1px solid white;"
                                                                "width:40px;}");
    ui->tw_Curve_ResultInfo->horizontalScrollBar()->setStyleSheet("QScrollBar:horizontal{"
                                                                  "background-color:rgb(240,240,240);"
                                                                  "border: 1px solid white;"
                                                                  "height:40px;}");*/

}

/*
 *
 *设置界面曲线显示初始化
 *
 */
void SetInterface::InitCurveControl()
{
    //初始化单选控件
    ui->rb_Curve_NoOverlay->setChecked(true);
    ui->rb_Curve_RawData->setChecked(true);
    //初始化表格控件
    InitResultTableWidget();
    //初始化日期控件
    QDate dt = QDate::currentDate();
    ui->de_Curve_StartTime->setDate(dt);
    ui->de_Curve_EndTime->setDate(dt);
    //输入属性
    ui->de_Curve_StartTime->setProperty("noinput",true);
    ui->de_Curve_EndTime->setProperty("noinput",true);
    //表头样式
}

/*
 *
 *设置界面查询数据事件
 *
 */
void SetInterface::on_pb_Curve_QueryData_clicked()
{
    ui->tw_Curve_ResultInfo->setRowCount(0);
    QString strStartDate = QString("%1 00:00:00").arg(ui->de_Curve_StartTime->date().toString("yyyy-MM-dd"));
    QString strEndDate = QString("%2 23:59:59").arg(ui->de_Curve_EndTime->date().toString("yyyy-MM-dd"));
    QString strSql;
    strSql = QString("select * from testinfo where testdate between '%1' and '%2' order by testdate desc")
             .arg(strStartDate).arg(strEndDate);
    QStringList strlistRe;
    strlistRe = m_devDB->ExecQuery(strSql);
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
void SetInterface::InsertTestInfoToTab(QString strNumber, QString strArea1, QString strArea2, QString strRatio,
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
void SetInterface::on_tw_Curve_ResultInfo_cellPressed(int row, int column)
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
void SetInterface::low_passfilter(QVector<double>& oldData)
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
void SetInterface::on_pb_Curve_ClearCurve_clicked()
{
    m_packPlot->ClearGraph();
}

/*
 *
 *转换640个高低数据为320个点数据
 *
 */
QVector<double> SetInterface::ConvertGraphData(QByteArray byteGraphData)
{
    QVector<double> vecData;
    for(int n=0; n<byteGraphData.size(); n+=2){
        vecData.append(byteGraphData.at(n) + byteGraphData.at(n+1)*256);
    }
    return vecData;
}
