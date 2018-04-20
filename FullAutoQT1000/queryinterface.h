#ifndef QUERYINTERFACE_H
#define QUERYINTERFACE_H

#include <QDialog>

namespace Ui {
class QueryInterface;
}

class QueryInterface : public QDialog
{
    Q_OBJECT
    
public:
    explicit QueryInterface(QWidget *parent = 0);
    ~QueryInterface();
    
private:
    Ui::QueryInterface *ui;
};

#endif // QUERYINTERFACE_H
