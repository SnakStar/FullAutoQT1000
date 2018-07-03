#ifndef QUERYINTERFACE_H
#define QUERYINTERFACE_H

#include <QDialog>
#include <QStringList>
#include <QDebug>
#include <QTableWidget>
#include "cqtprodb.h"

#define RESULT_ROW_COUNT 12

namespace Ui {
class QueryInterface;
}

class QueryInterface : public QDialog
{
    Q_OBJECT
    
public:
    explicit QueryInterface(QWidget *parent = 0);
    ~QueryInterface();
    
private slots:
    void on_pb_Query_Query_clicked();

private:
    Ui::QueryInterface *ui;
private:
    //主界面指针
    QWidget* m_pMainWnd;
    //
    CQtProDB* m_Devdb;
private:
    //初始化查询列表
    void InitQueryTable();
    //初始化日期控件
    void InitDateControl();
};

#endif // QUERYINTERFACE_H
