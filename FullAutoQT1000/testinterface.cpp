#include "testinterface.h"
#include "ui_testinterface.h"

TestInterface::TestInterface(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TestInterface)
{
    ui->setupUi(this);
}

TestInterface::~TestInterface()
{
    delete ui;
}
