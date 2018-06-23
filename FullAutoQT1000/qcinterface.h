#ifndef QCINTERFACE_H
#define QCINTERFACE_H

#include <QDialog>

namespace Ui {
class QCInterface;
}

class QCInterface : public QDialog
{
    Q_OBJECT
    
public:
    explicit QCInterface(QWidget *parent = 0);
    ~QCInterface();
    
private:
    Ui::QCInterface *ui;
};

#endif // QCINTERFACE_H
