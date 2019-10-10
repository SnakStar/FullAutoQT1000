﻿#include "msgboxdlg.h"
#include "ui_msgboxdlg.h"

MsgBoxDlg::MsgBoxDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MsgBoxDlg)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::FramelessWindowHint|Qt::Dialog);
    int nWidth = this->width();
    m_lbNoteTitle = new QLabel(this);
    m_lbNoteTitle->lower();
    m_lbNoteTitle->setGeometry(0,0,nWidth,30);
    m_lbNoteTitle->setStyleSheet("QLabel{color:white;background-color:rgb(60,183,198);}");
    //lbTitle->setStyleSheet("QLabel{color:white;background-color:rgb(243,189,79);}");
    ui->pb_Close->setIcon(QIcon(":/res/image/close.png"));
    this->setStyleSheet("QPushButton{background-color:white;border:2px solid;"
                        "border-color: rgb(44,158,247);color:rgb(60,183,198);"
                        "border-radius:8px;}"
                        "QPushButton:pressed{border-color:#2857ad;padding-left:8px;"
                        "padding-top:8px;}"
                        "QWidget:focus{outline: none;}");
    ui->pb_Close->setStyleSheet("QPushButton#pb_Close{border:none;border-radius:0px;"
                                "background-color:rgb(60,183,198);}"
                                "QPushButton#pb_Close::pressed{border:none;border-radius:0px;"
                                "background-color:rgb(250,85,85);padding-left:0px;"
                                "padding-top:0px;}");
    ui->lb_MsgContent->setStyleSheet("font-size:16px;color:rgb(0,135,106);");

    ui->lb_MsgContent->setWordWrap(true);
}

MsgBoxDlg::~MsgBoxDlg()
{
    delete ui;
}

void MsgBoxDlg::ShowMsg(QString strMsg, int nStyle)
{
    if(nStyle == 0){
        ui->pb_OK->move(160,170);
        ui->lb_Icon->setStyleSheet("border-image:url(:/res/image/Note);");
        m_lbNoteTitle->setText("  提示");
        ui->pb_Cancel->hide();
    }else if(nStyle == 1){
        ui->pb_OK->move(80,170);
        ui->lb_Icon->setStyleSheet("border-image:url(:/res/image/Warning);");
        m_lbNoteTitle->setText("  警告");
        ui->pb_Cancel->show();
    }
    ui->lb_MsgContent->setText(strMsg);
}

void MsgBoxDlg::on_pb_Close_clicked()
{
    ui->pb_Close->setIcon(QIcon(":/res/image/close-hover.png"));
    this->close();
}

void MsgBoxDlg::on_pb_OK_clicked()
{
    this->accept();
}

void MsgBoxDlg::on_pb_Cancel_clicked()
{
    this->close();
}
