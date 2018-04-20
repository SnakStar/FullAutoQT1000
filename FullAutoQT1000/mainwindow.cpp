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

    //
    this->setFixedSize(this->width(),this->height());
    //
    InitMainInterface();
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
    m_TestInterface = new TestInterface;
    m_QueryInterface = new QueryInterface;
    ui->stackedWidget->addWidget(m_TestInterface);
    ui->stackedWidget->addWidget(m_QueryInterface);

    ui->stackedWidget->setCurrentIndex(0);
}

void MainWindow::on_pb_Main_Test_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
}

void MainWindow::on_pb_Main_Query_clicked()
{
    ui->stackedWidget->setCurrentIndex(1);
}

void MainWindow::on_pb_Main_QC_clicked()
{
    ui->stackedWidget->setCurrentIndex(2);
}

void MainWindow::on_pb_Main_Setting_clicked()
{
    ui->stackedWidget->setCurrentIndex(3);
}
