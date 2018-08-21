#ifndef SETINTERFACE_H
#define SETINTERFACE_H

#include <QDialog>
#include <QListWidgetItem>
#include "cutilsettings.h"
#include "testinterface.h"
#include "cqtprodb.h"
#include "cutiltool.h"
#include "renfvalue.h"

namespace Ui {
class SetInterface;
}

class SetInterface : public QDialog
{
    Q_OBJECT
    
public:
    explicit SetInterface(QWidget *parent = 0);
    ~SetInterface();

public:
    //更新数据库内参考值内容到表格上
    void UpdateDbToRenf();
    
private slots:
    void on_lw_Set_Navigation_currentRowChanged(int currentRow);

    void on_btn_Set_GenSave_clicked();

    void on_btn_Set_GenCancel_clicked();

    void on_rb_Set_AutoEncode_clicked();

    void on_rb_Set_ScanEncode_clicked();

    void on_btn_Set_RenfAdd_clicked();

    void on_btn_Set_RenfModify_clicked();

    void on_btn_Set_RenfDelete_clicked();

private slots:
    void RecvAddRenfValue(RenfInfo& ri);
    void RecvModifyRenfValue(RenfInfo& ri);

private:
    Ui::SetInterface *ui;

private:
    //设置界面导航对象
    QStringList m_strlistContent;
    //主界面窗口对象
    QWidget* m_pMainWnd;
    //配置文件对象
    CUtilSettings* m_pSettinsObj;
    //测试界面
    TestInterface* m_pTestInterface;
    //数据库对象
    CQtProDB* m_devDB;
    //工具对象
    CUtilTool m_UtilTool;
    //配置参数对象
    //QMap<QString,QString> m_SetParam;
    //获取一行数据
    QStringList GetRowValue(QTableWidget *tw);
private:
    //初始化列表导航栏
    void InitNavigation();
    //根据配置参数显示控件选项
    void UpdateUIControl();
    //初始化参考值信息框
    void InitRenfTable();
};

#endif // SETINTERFACE_H
