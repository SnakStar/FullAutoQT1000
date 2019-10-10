#ifndef MSGBOXDLG_H
#define MSGBOXDLG_H

#include <QDialog>
#include <QLabel>

namespace Ui {
class MsgBoxDlg;
}

class MsgBoxDlg : public QDialog
{
    Q_OBJECT

public:
    explicit MsgBoxDlg(QWidget *parent = NULL);
    ~MsgBoxDlg();

public:
    void SetText(QString strContent);
    void ShowMsg(QString strMsg,int nStyle=0);

private slots:
    void on_pb_Close_clicked();

    void on_pb_OK_clicked();

    void on_pb_Cancel_clicked();

private:
    Ui::MsgBoxDlg *ui;
    QLabel* m_lbNoteTitle;
};

#endif // MSGBOXDLG_H
