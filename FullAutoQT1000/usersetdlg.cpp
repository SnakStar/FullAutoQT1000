#include "usersetdlg.h"
#include "ui_usersetdlg.h"

UserSetDlg::UserSetDlg(CQtProDB *db,int showType,QWidget *parent) :
    QDialog(parent),
    ui(new Ui::UserSetDlg)
{
    ui->setupUi(this);
    ui->pb_User_OK->setFocus();

    m_devDB = db;
    m_nShowType = showType;
    InitShowText();
    ui->lb_User_UserNote->hide();
    ui->le_User_Password->setEchoMode(QLineEdit::PasswordEchoOnEdit);
    ui->le_User_pwConfirm->setEchoMode(QLineEdit::PasswordEchoOnEdit);
}

UserSetDlg::UserSetDlg(CQtProDB *db, int showType, QString strUserName, QWidget *parent):
    QDialog(parent),
    ui(new Ui::UserSetDlg)
{
    ui->setupUi(this);
    ui->pb_User_OK->setFocus();

    m_devDB = db;
    m_strUserName = strUserName;
    m_nShowType = showType;
    InitShowText();
    ui->lb_User_UserNote->hide();
    ui->le_User_Password->setEchoMode(QLineEdit::PasswordEchoOnEdit);
    ui->le_User_pwConfirm->setEchoMode(QLineEdit::PasswordEchoOnEdit);
}

/*UserSetDlg::UserSetDlg(CQtProDB *db, QWidget *parent):
    QDialog(parent),
    ui(new Ui::UserSetDlg)
{
    m_devDB = db;
}*/

UserSetDlg::~UserSetDlg()
{
    delete ui;
}

void UserSetDlg::InitShowText()
{
    //修改
    if(m_nShowType == 2){
        ui->le_User_UserName->setEnabled(false);
        ui->le_User_UserName->setText(m_strUserName);
        ui->lb_User_Password->setText("旧密码");
        ui->lb_User_Confirm->setText("新密码");
    }
}

bool UserSetDlg::CheckInput(QString strUserName, QString strPasswd, QString strConfirm, QString &strRel)
{
    if(strUserName.isEmpty()){
        strRel = ("用户名不能为空");
        return false;
    }else if( strPasswd.isEmpty() ){
        strRel = ("新密码不能为空");
        return false;
    }else if(strConfirm.isEmpty() ){
        strRel = ("确认密码不能为空");
        return false;
    }
    if(strPasswd.compare(strConfirm) != 0){
        strRel = ("两次密码输入不一致,请重新输入");
        return false;
    }
    return true;
}

bool UserSetDlg::CheckInput(QString strOldPasswd, QString strNewPasswd, QString &strRel)
{
    if( strOldPasswd.isEmpty() ){
        strRel = ("旧密码不能为空");
        return false;
    }else if(strNewPasswd.isEmpty() ){
        strRel = ("新密码不能为空");
        return false;
    }
    return true;
}

bool UserSetDlg::AddUserAccount(QString strUserName, QString strPasswd)
{
    QString strSql;
    QStringList listUserName;
    strSql = "select username from UserLogin";
    listUserName = m_devDB->ExecQuery(strSql);
    if(listUserName.contains(strUserName) == true){
        ui->lb_User_UserNote->setText("用户名已存在");
        ui->lb_User_UserNote->show();
        return false;
    }
    strSql = QString("insert into UserLogin(UserName,Password) values('%1','%2')").arg(strUserName).arg(strPasswd);
    qDebug()<<strSql;
    bool re = m_devDB->Exec(strSql);
    QString strTitle = QObject::tr("提示");
    QString strContent;
    if(re == true){
        strContent = QObject::tr("添加成功");
        QMessageBox::information(this,strTitle,strContent,QMessageBox::Ok);
    }else{
        strContent = QObject::tr("添加失败");
        QMessageBox::warning(this,strTitle,strContent,QMessageBox::Ok);
    }
    this->done(re);
}

bool UserSetDlg::ModifyUserPasswd(QString strUserName, QString strOldPasswd, QString strNewPasswd)
{
    QString strSql;
    strSql = QString("select username,Password from UserLogin where username='%1'").arg(strUserName);
    QStringList listUserName;
    listUserName = m_devDB->ExecQuery(strSql);
    if(listUserName.size() == 0){
        ui->lb_User_UserNote->setText("查询指定帐户信息失败");
        ui->lb_User_UserNote->show();
        return false;
    }else{
        if(listUserName.at(1).compare(strOldPasswd) != 0){
            ui->lb_User_UserNote->setText("旧密码输入错误,请重新输入");
            ui->lb_User_UserNote->show();
            ui->le_User_Password->setText("");
            return false;
        }else{
            strSql = QString("update userLogin set password='%1' where username='%2'")
                    .arg(strNewPasswd).arg(strUserName);
            QString strTitle = QObject::tr("提示");
            QString strContent;
            //qDebug()<<strSql;
            bool rel = m_devDB->Exec(strSql);
            this->done(rel);
        }
    }
}

void UserSetDlg::on_pb_User_OK_clicked()
{
    QString strUserName = ui->le_User_UserName->text();
    QString strPasswd = ui->le_User_Password->text();
    QString strConfirm = ui->le_User_pwConfirm->text();
    QString strSql;
    QStringList listUserName;
    QString strRel;
    if(m_nShowType == 1){
        bool rel = CheckInput(strUserName,strPasswd,strConfirm,strRel);
        if(rel == false){
            ui->lb_User_UserNote->setText(strRel);
            ui->lb_User_UserNote->show();
        }else{
            AddUserAccount(strUserName,strPasswd);
        }
    }else if(m_nShowType == 2){
        bool rel = CheckInput(strPasswd,strConfirm,strRel);
        if(rel == false){
            ui->lb_User_UserNote->setText(strRel);
            ui->lb_User_UserNote->show();
        }else{
            ModifyUserPasswd(strUserName,strPasswd,strConfirm);
        }
    }else{
        QMessageBox::warning(this,"错误","帐户数据读取错误",QMessageBox::Ok);
        this->done(-1);
    }
}
