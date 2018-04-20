/*******************************************************************
 *  Copyright(c) 2000-2020 Company Name
 *  All rights reserved.
 *
 *  �ļ�����: mainwindow.cpp
 *  ��Ҫ����: �����ʵ���ļ�����������ܹ���ʵ��
 *
 *  ��������: 2018-4-20
 *  ����: HuaT
 *  ˵��:
 *
 *  �޸�����:
 *  ����:
 *  ˵��:
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
 *@Description:�������������
 *@Param:��
 *@Return:��
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
 *@Description:��ʼ����������ؿؼ�
 *@Param:��
 *@Return:��
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
