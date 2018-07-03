#include "queryinterface.h"
#include "ui_queryinterface.h"
#include "mainwindow.h"

QueryInterface::QueryInterface(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QueryInterface)
{
    ui->setupUi(this);
    //
    m_pMainWnd = parent;
    m_Devdb = ((MainWindow*)m_pMainWnd)->GetDatabaseObj();
    //初始化查询列表
    InitQueryTable();
    //初始化时间控件
    InitDateControl();
}

QueryInterface::~QueryInterface()
{
    delete ui;
}

/********************************************************
 *@Name:        InitQueryTable
 *@Author:      HuaT
 *@Description: 初始化查询列表字段信息
 *@Param1:      无
 *@Return:      无
 *@Version:     1.0
 *@Date:        2018-6-29
********************************************************/
void QueryInterface::InitQueryTable()
{
    QStringList listHeader;
    listHeader<<QObject::tr("样本编号")<<QObject::tr("测试项目")<<QObject::tr("结果")
                <<QObject::tr("测试时间")<<QObject::tr("参考值");

    ui->tw_Query_QueryInfo->setColumnCount(listHeader.size());
    ui->tw_Query_QueryInfo->setRowCount(RESULT_ROW_COUNT);

    ui->tw_Query_QueryInfo->setHorizontalHeaderLabels(listHeader);
    ui->tw_Query_QueryInfo->setEditTriggers(QTableWidget::NoEditTriggers);
    ui->tw_Query_QueryInfo->setSelectionBehavior(QTableWidget::SelectRows);

    ui->tw_Query_QueryInfo->setColumnWidth(0,200);
    ui->tw_Query_QueryInfo->setColumnWidth(1,200);
    ui->tw_Query_QueryInfo->setColumnWidth(2,150);
    ui->tw_Query_QueryInfo->setColumnWidth(3,180);
    ui->tw_Query_QueryInfo->horizontalHeader()->setStretchLastSection(true);

    ui->tw_Query_QueryInfo->verticalHeader()->setDefaultSectionSize(42);
}

/********************************************************
 *@Name:        InitDateControl
 *@Author:      HuaT
 *@Description: 初始化日期控件
 *@Param1:      无
 *@Return:      无
 *@Version:     1.0
 *@Date:        2018-6-29
********************************************************/
void QueryInterface::InitDateControl()
{
    QDate dateCur = QDate::currentDate();
    ui->de_Query_DateStart->setDate(dateCur);
    ui->de_Query_DateEnd->setDate(dateCur);
}

/*
 *查询按钮事件
 */
void QueryInterface::on_pb_Query_Query_clicked()
{
    QString strStartDate = ui->de_Query_DateStart->date().toString("yyyy-MM-dd");
    QString strEndDate = ui->de_Query_DateEnd->date().toString("yyyy-MM-dd");
    QString strSql = QString("select * from patient where date(testdate) between '%1' and '%2' ").arg(strStartDate).arg(strEndDate);
    qDebug()<<strSql;
    qDebug()<<m_Devdb->ExecQuery(strSql);
}
