/*******************************************************************
 *  Copyright(c) 2000-2020 Company Name
 *  All rights reserved.
 *
 *  文件名称: mainwindow.cpp
 *  简要描述: 主框架实现文件，进行主框架功能实现
 *
 *  创建日期: 2018-4-20
 *  作者: HuaT
 *  说明:
 *
 *  修改日期:
 *  作者:
 *  说明:
 ******************************************************************/

#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    //设置指定宽高
    this->setFixedSize(this->width(),this->height());
    //初始化主界面
    InitMainInterface();
    //连接数据库
    m_devdb = new CQtProDB();
    m_devdb->ConnectDB("/home/root/qt200//autodb.db");
    //初始化配置文件
    m_SetParam = m_settings.ReadSettingsInfoToMap();

}

/********************************************************
 *@Name:~MainWindow
 *@Author:HuaT
 *@Description:主框架析构函数
 *@Param:无
 *@Return:无
 *@Version:1.0
 *@Date:2018-4-20
 ********************************************************/
MainWindow::~MainWindow()
{
    delete ui;
}

/********************************************************
 *@Name:GetDatabaseObj
 *@Author:HuaT
 *@Description:获取数据库对象
 *@Param:无
 *@Return:无
 *@Version:1.0
 *@Date:2018-4-20
 ********************************************************/
CQtProDB* MainWindow::GetDatabaseObj()
{
    return m_devdb;
}

/********************************************************
 *@Name:GetSettingsObj
 *@Author:HuaT
 *@Description:获取配置文件对象
 *@Param:无
 *@Return:无
 *@Version:1.0
 *@Date:2018-4-20
 ********************************************************/
CUtilSettings *MainWindow::GetSettingsObj()
{
    return &m_settings;
}

/********************************************************
 *@Name:InitMainInterface
 *@Author:HuaT
 *@Description:初始化主界面相关控件
 *@Param:无
 *@Return:无
 *@Version:1.0
 *@Date:2018-4-20
 ********************************************************/
void MainWindow::InitMainInterface()
{
    m_TestInterface = new TestInterface(this);
    m_QueryInterface = new QueryInterface(this);
    m_QCInterface    = new QCInterface(this);
    m_SetInterface   = new SetInterface(this);
    ui->stackedWidget->addWidget(m_TestInterface);
    ui->stackedWidget->addWidget(m_QueryInterface);
    ui->stackedWidget->addWidget(m_QCInterface);
    ui->stackedWidget->addWidget(m_SetInterface);

    ui->stackedWidget->setCurrentIndex(0);
}

/*
 *测试响应事件
 */
void MainWindow::on_pb_Main_Test_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
}

/*
 *查询响应事件
 */
void MainWindow::on_pb_Main_Query_clicked()
{
    ui->stackedWidget->setCurrentIndex(1);
}

/*
 *质控响应事件
 */
void MainWindow::on_pb_Main_QC_clicked()
{
    ui->stackedWidget->setCurrentIndex(2);
}

/*
 *设置响应事件
 */
void MainWindow::on_pb_Main_Setting_clicked()
{
    ui->stackedWidget->setCurrentIndex(3);
}
