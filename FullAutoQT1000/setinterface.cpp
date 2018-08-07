#include "setinterface.h"
#include "ui_setinterface.h"

SetInterface::SetInterface(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SetInterface)
{
    ui->setupUi(this);
    //初始化导航栏
    InitNavigation();
}

SetInterface::~SetInterface()
{
    delete ui;
}

void SetInterface::InitNavigation()
{
    QStringList items;
    items<<QObject::tr("常规设置")<<QObject::tr("参考值设置")<<QObject::tr("参数设置")
        <<QObject::tr("系统设置")<<QObject::tr("系统信息")<<QObject::tr("调试设置");
    for(int n=0; n<items.size(); n++){
        QListWidgetItem* subitem = new QListWidgetItem;
        subitem->setText(items.at(n));
        subitem->setSizeHint(QSize(187,80));
        ui->lw_Set_Navigation->addItem(subitem);
    }
}

/*
 *
 *当前行改变
 */
void SetInterface::on_lw_Set_Navigation_currentRowChanged(int currentRow)
{
    ui->stackedWidget->setCurrentIndex(currentRow);
}
