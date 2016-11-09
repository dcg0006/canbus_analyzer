#include "addtomasterdiag.h"
#include "ui_addtomasterdiag.h"

addToMasterDiag::addToMasterDiag(QString ID, QString vehicle, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::addToMasterDiag)
{
    ui->setupUi(this);
    setWindowTitle("PID to Master List Entry");
    alreadyPresent = 0;

    connect(ui->pushButtonOkay, SIGNAL(clicked(bool)), this, SLOT(addIDtoMaster()));
    connect(ui->pushButtonCancel, SIGNAL(clicked(bool)), this, SLOT(reject()));

    ui->lineEditYear->setReadOnly(true);
    ui->lineEditMake->setReadOnly(true);
    ui->lineEditModel->setReadOnly(true);
    ui->lineEditVIN->setReadOnly(true);
    ui->lineEditYear->setInputMask("9999");
    ui->lineEditPID->setInputMask(">HHH"); //3 digit hex required
    ui->lineEditData->setInputMask(">HHhhhhhhhhhhhhhh"); //16 digit Data, 1 hex Byte required
    // not requiring mask for conversion or comments fields currently

    ui->lineEditPID->setText(ID);
    ui->lineEditYear->setText(vehicle.split(" ").at(0));
    ui->lineEditMake->setText(vehicle.split(" ").at(1));
    ui->lineEditModel->setText(vehicle.split(" ").at(2));
    if (vehicle.split(" ").size() > 3){
        //qDebug() << "Had a VIN";
        ui->lineEditVIN->setText(vehicle.split(" ").at(3));
    }
}

addToMasterDiag::~addToMasterDiag(){
    delete ui;
}

void addToMasterDiag::addIDtoMaster(){
    if (checkData()){
        QString tempID = ui->lineEditPID->text();
        QString tempData = ui->lineEditData->text();
        QString tempConversion = ui->lineEditConversion->text();
        QString tempComments = ui->lineEditComments->text();
        QString tempYear = ui->lineEditYear->text();
        QString tempMake = ui->lineEditMake->text();
        QString tempModel = ui->lineEditModel->text();
        QString tempVin = ui->lineEditVIN->text();
        QSqlQuery query;
        query.prepare("INSERT INTO PIDList (ID, data, conversion, comments, year, make, model, vin) "
                  "VALUES (:ID, :data, :conversion, :comments, :year, :make, :model, :vin)");
        query.bindValue(":ID", tempID);
        query.bindValue(":data", tempData);
        query.bindValue(":conversion", tempConversion);
        query.bindValue(":comments", tempComments);
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

bool addToMasterDiag::checkData(){
    alreadyPresent = 0;
    if(ui->lineEditData->hasAcceptableInput() && ui->lineEditPID->hasAcceptableInput()
            && ui->lineEditYear->hasAcceptableInput()){ //will work if there is an ID, Data, and Vehicle info
        QString tempID = ui->lineEditPID->text();
        QString tempYear = ui->lineEditYear->text();
        QString tempMake = ui->lineEditMake->text();
        QString tempModel = ui->lineEditModel->text();
        QString tempVin = ui->lineEditVIN->text();
        QSqlQuery query;
        query.exec("SELECT * FROM PIDList WHERE Year LIKE '"+tempYear+"' AND Make LIKE '"+tempMake+"' AND"
                      " Model LIKE '"+tempModel+"' AND vin LIKE '"+tempVin+"' AND ID LIKE '"+tempID+"'");
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

void addToMasterDiag::alreadyPresentError(){
    errorMsg = new QMessageBox;
    errorMsg->setText(tr("PID Already Present"));
    errorMsg->setDetailedText(tr("The ID and Vehicle source entered already exists in the master table"));
    errorMsg->exec();
}

void addToMasterDiag::incompleteDataError(){
    errorMsg = new QMessageBox;
    errorMsg->setText(tr("Invalid PID Data"));
    errorMsg->setDetailedText(tr("At least the ID, Data, and Vehicle fields must be filled out to proceed"));
    errorMsg->exec();
}
