#ifndef TESTINTERFACE_H
#define TESTINTERFACE_H

#include <QDialog>

namespace Ui {
class TestInterface;
}

class TestInterface : public QDialog
{
    Q_OBJECT
    
public:
    explicit TestInterface(QWidget *parent = 0);
    ~TestInterface();
    
private:
    Ui::TestInterface *ui;
};

#endif // TESTINTERFACE_H
