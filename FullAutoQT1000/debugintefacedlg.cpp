#include "debugintefacedlg.h"
#include "ui_debugintefacedlg.h"

debugIntefaceDlg::debugIntefaceDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::debugIntefaceDlg)
{
    ui->setupUi(this);
    //初始化对话框样式
    InitDialogStyle();
    //初始化导航栏
    InitNavigate();

}

debugIntefaceDlg::~debugIntefaceDlg()
{
    delete ui;
}

/********************************************************
 *@Name:        InitNavigate
 *@Author:      HuaT
 *@Description: 初始化调试界面导航栏
 *@Param1:      无
 *@Return:      无
 *@Version:     1.0
 *@Date:        2019-3-4
********************************************************/
void debugIntefaceDlg::InitNavigate()
{
    QStringList listNavigate;
    listNavigate<<"常规调试"<<"参数设置"<<"数据库操作"<<"退出";
    for(int n=0; n<listNavigate.size(); n++){
        QListWidgetItem* item = new QListWidgetItem(listNavigate.at(n));
        item->setSizeHint(QSize(187,80));
        ui->lw_debug_Navigate->addItem(item);
    }
}

/********************************************************
 *@Name:        InitDialogStyle
 *@Author:      HuaT
 *@Description: 初始化对话框样式
 *@Param1:      无
 *@Return:      无
 *@Version:     1.0
 *@Date:        2019-3-4
********************************************************/
void debugIntefaceDlg::InitDialogStyle()
{
    this->setWindowFlags(Qt::CustomizeWindowHint|Qt::FramelessWindowHint);
    QDesktopWidget* dw = qApp->desktop();
    this->move( (dw->width() - this->width())/2 , (dw->height() - this->height())/2);
}

/*
 *
 * 导航项目更改事件
 *
 */
void debugIntefaceDlg::on_lw_debug_Navigate_currentRowChanged(int currentRow)
{
    switch(currentRow){
    case 0:
        break;
    case 1:
        break;
    case 2:
        break;
    case 3:
        this->close();
        break;
    default:
        break;
    }
}
