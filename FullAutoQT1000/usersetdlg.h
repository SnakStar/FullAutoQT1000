#ifndef USERSETDLG_H
#define USERSETDLG_H

#include <QDialog>
#include <QMessageBox>
#include "cqtprodb.h"

namespace Ui {
class UserSetDlg;
}

class UserSetDlg : public QDialog
{
    Q_OBJECT

public:
    explicit UserSetDlg(CQtProDB *db,int showType,QWidget *parent = NULL);
    explicit UserSetDlg(CQtProDB* db,int showType,QString strUserName,QWidget* parent = NULL);
    ~UserSetDlg();

private slots:
    void on_pb_User_OK_clicked();

private:
    void InitShowText();
    bool CheckInput(QString strUserName,QString strPasswd,QString strConfirm,QString& strRel);
    bool CheckInput(QString strOldPasswd, QString strNewPasswd, QString& strRel);
    bool AddUserAccount(QString strUserName, QString strPasswd);
    bool ModifyUserPasswd(QString strUserName,QString strOldPasswd, QString strNewPasswd);

private:
    Ui::UserSetDlg *ui;
    CQtProDB* m_devDB;
    int m_nShowType;
    QString m_strUserName;
};

#endif // USERSETDLG_H
