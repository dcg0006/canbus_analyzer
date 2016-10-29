#include "dataprocwindow.h"
#include "ui_dataprocwindow.h"

dataProcWindow::dataProcWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::dataProcWindow)
{
    ui->setupUi(this);
}

dataProcWindow::~dataProcWindow()
{
    delete ui;
}

void dataProcWindow::updateVehicleList(){
    QStringList vehicleList;
    QSqlQuery query;
    query.exec("SELECT * FROM vehicleList");
    QString tempVehicle;
    while (query.next()){
        tempVehicle = query.value(0).toString()+" "+query.value(1).toString()+" "
                +query.value(2).toString()+" "+query.value(3).toString();
        vehicleList.append(tempVehicle);
        qDebug() << tempVehicle << "\n";
    }
}
