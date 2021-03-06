﻿#include "qcinterface.h"
#include "ui_qcinterface.h"

QCInterface::QCInterface(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QCInterface)
{
    ui->setupUi(this);

    ui->wg_QC_CustomPlot->legend->setVisible(true);
    ui->wg_QC_CustomPlot->xAxis->setLabel("Raw Data To X");
    ui->wg_QC_CustomPlot->yAxis->setLabel("Raw Data To Y");
    ui->wg_QC_CustomPlot->xAxis->setAutoTickStep(false);
    ui->wg_QC_CustomPlot->xAxis->setTickStep(86400);
    //ui->wg_QC_CustomPlot->xAxis->setRange(0,30);
    ui->wg_QC_CustomPlot->yAxis->setRange(0,100);
    //时间格式
    ui->wg_QC_CustomPlot->xAxis->setTickLabelType(QCPAxis::ltDateTime);
    ui->wg_QC_CustomPlot->xAxis->setDateTimeFormat("MM/\ndd");

    ui->wg_QC_CustomPlot->setBackground(QBrush(QColor(193,214,250)));

    ui->wg_QC_CustomPlot->xAxis->setRange(QDateTime::currentDateTime().toTime_t(),QDateTime::currentDateTime().toTime_t()+84600*30);

    QFile fi(":/res/qss/QCStyle.qss");
    if(fi.open(QIODevice::ReadOnly)){
        QTextStream ts(&fi);
        QString strStyle = ts.readAll();
        this->setStyleSheet(strStyle);
        fi.close();
    }
}

QCInterface::~QCInterface()
{
    delete ui;
}
