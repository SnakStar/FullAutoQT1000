/*******************************************************************
 *  Copyright(c) 2000-2020 Company Name
 *  All rights reserved.
 *
 *  文件名称: mainwindow.h
 *  简要描述: 主框架头文件，进行对象声明
 *
 *  创建日期: 2018-4-20
 *  作者: HuaT
 *  说明:
 *
 *  修改日期:
 *  作者:
 *  说明:
 ******************************************************************/
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include"testinterface.h"
#include"queryinterface.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_pb_Main_Test_clicked();

    void on_pb_Main_Query_clicked();

    void on_pb_Main_QC_clicked();

    void on_pb_Main_Setting_clicked();

private:
    void InitMainInterface();

private:
    TestInterface* m_TestInterface;
    QueryInterface* m_QueryInterface;
    
private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
