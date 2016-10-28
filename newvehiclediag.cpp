#include "newvehiclediag.h"
#include "ui_newvehiclediag.h"

newVehicleDiag::newVehicleDiag(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::newVehicleDiag)
{
    ui->setupUi(this);
    setWindowTitle("New Vehicle Entry");
    alreadyPresent = 0;

    connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(insertVehicleData()));
    connect(ui->buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

    ui->lineEditYear->setInputMask("9999");
    ui->lineEditMake->setInputMask(">A<aaaaaaaaaaaaaaaaaaaaaaaaaaaaa");
    ui->lineEditModel->setInputMask(">N<nnnnnnnnnnnnnnnnnnnnnnnnnnnnnn");
    ui->lineEditVIN->setInputMask(">Nnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn");
}

newVehicleDiag::~newVehicleDiag()
{
    delete ui;
}

bool newVehicleDiag::checkVehicleData(){
    alreadyPresent = 0;
    if(ui->lineEditYear->hasAcceptableInput() && ui->lineEditMake->hasAcceptableInput()
            && ui->lineEditModel->hasAcceptableInput() ){ //not worrying about VIN yet
        QString tempYear = ui->lineEditYear->text();
        QString tempMake = ui->lineEditMake->text();
        QString tempModel = ui->lineEditModel->text();
        //QString tempVin = ui->lineEditVIN->text();
        QSqlQuery query;
        query.exec("SELECT * FROM vehicleList WHERE Year LIKE '"+tempYear+"' AND Make LIKE '"+tempMake+"' AND"
                      " Model LIKE '"+tempModel+"'");
        qDebug() << query.lastQuery();
        query.next();
        if (query.at() < 0){
            query.clear();
            return true;
        }
        else {
            //qDebug() << "Query not valid";
            query.clear();
            alreadyPresentError();
            alreadyPresent = 1;
            return false;
        }
    }
    else return false;
}

void newVehicleDiag::insertVehicleData(){
    if (checkVehicleData()){
        QString tempYear = ui->lineEditYear->text();
        QString tempMake = ui->lineEditMake->text();
        QString tempModel = ui->lineEditModel->text();
        QString tempVin = ui->lineEditVIN->text();
        QSqlQuery query;
        query.prepare("INSERT INTO vehicleList (Year, Make, Model, VIN) "
                  "VALUES (:year, :make, :model, :vin)");
        query.bindValue(":year", tempYear);
        query.bindValue(":make", tempMake);
        query.bindValue(":model", tempModel);
        query.bindValue(":vin", tempVin);
        query.exec();
        //qDebug() << query.lastQuery();
        accept();
    }
    else if(alreadyPresent == 0){
        incompleteDataError();
    }
}

void newVehicleDiag::incompleteDataError(){
    errorMsg = new QMessageBox;
    errorMsg->setText(tr("Invalid Vehicle Data"));
    errorMsg->setDetailedText(tr("The four fields must be filled out to proceed"));
    errorMsg->exec();
}

void newVehicleDiag::alreadyPresentError(){
    errorMsg = new QMessageBox;
    errorMsg->setText(tr("Vehicle Data Already Present"));
    errorMsg->setDetailedText(tr("The vehicle information entered already exists in the vehicle table"));
    errorMsg->exec();
}
