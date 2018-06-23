#include "setinterface.h"
#include "ui_setinterface.h"

SetInterface::SetInterface(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SetInterface)
{
    ui->setupUi(this);
}

SetInterface::~SetInterface()
{
    delete ui;
}
