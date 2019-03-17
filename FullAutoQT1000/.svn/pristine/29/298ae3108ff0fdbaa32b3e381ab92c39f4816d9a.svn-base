#include "logindlg.h"
#include "ui_logindlg.h"
#include "mainwindow.h"

LoginDlg::LoginDlg(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LoginDlg)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::CustomizeWindowHint|Qt::WindowStaysOnTopHint);
    ui->le_Login_Pwd->setEchoMode(QLineEdit::Password);
    ui->le_Login_Pwd->setProperty("flag","number");
}

LoginDlg::~LoginDlg()
{
    delete ui;
}

void LoginDlg::on_pb_Login_Ok_clicked()
{
    this->close();
}
