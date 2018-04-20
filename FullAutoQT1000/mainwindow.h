/*******************************************************************
 *  Copyright(c) 2000-2020 Company Name
 *  All rights reserved.
 *
 *  �ļ�����: mainwindow.h
 *  ��Ҫ����: �����ͷ�ļ������ж�������
 *
 *  ��������: 2018-4-20
 *  ����: HuaT
 *  ˵��:
 *
 *  �޸�����:
 *  ����:
 *  ˵��:
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
