#ifndef ADDTOMASTERDIAG_H
#define ADDTOMASTERDIAG_H

#include <QDialog>
#include <QMessageBox>
#include <QSqlQuery>
#include <QDebug>

namespace Ui {
class addToMasterDiag;
}

class addToMasterDiag : public QDialog
{
    Q_OBJECT

public:
    explicit addToMasterDiag(QString ID, QString vehicle, QWidget *parent = 0);
    ~addToMasterDiag();
    bool checkData();
    void alreadyPresentError();
    void incompleteDataError();

private:
    Ui::addToMasterDiag *ui;
    int alreadyPresent;
    QMessageBox *errorMsg;

private slots:
    void addIDtoMaster();
};

#endif // ADDTOMASTERDIAG_H
