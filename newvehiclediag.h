#ifndef NEWVEHICLEDIAG_H
#define NEWVEHICLEDIAG_H

#include <QDialog>
#include <QMessageBox>
#include <QSqlQuery>
#include <QDebug>

namespace Ui {
class newVehicleDiag;
}

class newVehicleDiag : public QDialog
{
    Q_OBJECT

public:
    explicit newVehicleDiag(QWidget *parent = 0);
    ~newVehicleDiag();
    bool checkVehicleData();
    void alreadyPresentError();
    void incompleteDataError();

private:
    Ui::newVehicleDiag *ui;
    int alreadyPresent;
    QMessageBox *errorMsg;

private slots:
    void insertVehicleData();
};

#endif // NEWVEHICLEDIAG_H
