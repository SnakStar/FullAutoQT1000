#ifndef DEBUGINTEFACEDLG_H
#define DEBUGINTEFACEDLG_H

#include <QDialog>
#include <QDesktopWidget>

namespace Ui {
class debugIntefaceDlg;
}

class debugIntefaceDlg : public QDialog
{
    Q_OBJECT

public:
    explicit debugIntefaceDlg(QWidget *parent = 0);
    ~debugIntefaceDlg();

private slots:
    void on_lw_debug_Navigate_currentRowChanged(int currentRow);

private:
    Ui::debugIntefaceDlg *ui;
private:
    //初始化导航栏
    void InitNavigate();
    //初始化对话框样式
    void InitDialogStyle();
};

#endif // DEBUGINTEFACEDLG_H
