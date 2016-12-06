#include "dataprocwindow.h"
#include "ui_dataprocwindow.h"

QT_CHARTS_USE_NAMESPACE

dataProcWindow::dataProcWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::dataProcWindow){

    ui->setupUi(this);
    ui->textEdit->setReadOnly(true);
    ui->comboBoxVehicle->setInsertPolicy(QComboBox::InsertAlphabetically);
    updateVehicleList();
    setWindowTitle(tr("PID Data Processing"));
    createActions();
    createMenus();

    ui->lineEditPID->setInputMask(">hhh"); //validates for 3 hexadecimal digits
    ui->comboBoxEndByte->setCurrentIndex(1);

    scene = new QGraphicsScene(this);
    chartSetup();

    //xVector = new QVector();
    //yVector = new QVector();
}

dataProcWindow::~dataProcWindow(){
    delete ui;
}

void dataProcWindow::createActions(){
    openFileAct = new QAction(tr("Open File"), this);
    addIDtoMasterListAct = new QAction(tr("Add ID to Master List"), this);
    saveDataToFileAct = new QAction(tr("Save Data to Text File"), this);

    connect(openFileAct, SIGNAL(triggered(bool)), this, SLOT(checkPID()));
    connect(ui->pushButtonOpen, SIGNAL(clicked(bool)), this, SLOT(checkPID()));
    connect(ui->pushButtonReparse, SIGNAL(clicked(bool)), this, SLOT(checkPID()));
    connect(ui->comboBoxStartByte, SIGNAL(currentTextChanged(QString)), this, SLOT(updateComboBoxes()));
    connect(ui->comboBoxEndByte, SIGNAL(currentTextChanged(QString)), this, SLOT(updateComboBoxes()));
    connect(addIDtoMasterListAct, SIGNAL(triggered(bool)), this, SLOT(addIDtoMasterList()));
    connect(saveDataToFileAct, SIGNAL(triggered(bool)), this, SLOT(saveDataToFile()));
}

void dataProcWindow::createMenus(){
    fileMenu = menuBar()->addMenu(tr("File"));
    fileMenu->addAction(openFileAct);
    fileMenu->addAction(saveDataToFileAct);
    tableMenu = menuBar()->addMenu(tr("Table"));
    tableMenu->addAction(addIDtoMasterListAct);
}

void dataProcWindow::saveDataToFile(){
    if (ui->lineEditPID->text() != "" && ui->lineEditFileSource->text() != ""){
        QString saveFileName = "ID_"+ui->lineEditPID->text()+"_"
            +"Bytes_"+ui->comboBoxStartByte->currentText()+"_To_"
            +ui->comboBoxEndByte->currentText()+"_For_"+ui->comboBoxVehicle->currentText().split(" ").at(0)
            +"_"+ui->comboBoxVehicle->currentText().split(" ").at(1)+"_"
            +ui->comboBoxVehicle->currentText().split(" ").at(2)+".txt";
        qDebug() << saveFileName;
        QFile saveFile(saveFileName);
        if (saveFile.exists()){
            saveFile.remove(); //delete file if already present
        }
        if (!saveFile.open(QIODevice::ReadWrite)){
            qDebug() << saveFile.errorString();
            return;
        }
        QTextStream line(&saveFile);
        for(int i = 0; i < xVector.size(); i++){
            line << xVector[i] << "," << yVector[i] << "\n";

        }
        //qDebug() << line;
        saveFile.close();
    }
}

void dataProcWindow::showEvent(QShowEvent *){
    ui->graphicsView->fitInView(scene->sceneRect(), Qt::IgnoreAspectRatio);
}

void dataProcWindow::resizeEvent(QResizeEvent *){
    ui->graphicsView->fitInView(scene->sceneRect(), Qt::IgnoreAspectRatio);
}

void dataProcWindow::chartSetup(){
    series = new QSplineSeries();
    chart = new QChart();
    scene->addItem(chart);
    ui->graphicsView->setScene(scene);

    chart->setTheme(QChart::ChartThemeBlueCerulean);
    QMargins margin = QMargins(1,1,1,1);
    chart->setMargins(margin);
    chart->legend()->hide();
    chart->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding,
                         QSizePolicy::DefaultType);
    ui->graphicsView->setRenderHint(QPainter::Antialiasing);

    chart->addSeries(series);
    chart->createDefaultAxes();
}

void dataProcWindow::addIDtoMasterList(){
    QString vehicle = ui->comboBoxVehicle->currentText();
    add2Master =  new addToMasterDiag(ui->lineEditPID->text(), vehicle);
    add2Master->exec();
}

QString dataProcWindow::getPID(){
    return ui->lineEditPID->text();
}

void dataProcWindow::updateComboBoxes(){
    int start = ui->comboBoxStartByte->currentText().toInt();
    int end = ui->comboBoxEndByte->currentText().toInt();
    if (start > end){
        ui->comboBoxEndByte->setCurrentIndex(ui->comboBoxStartByte->currentIndex());
    }

}

void dataProcWindow::incompleteDataError(){
    errorMsg = new QMessageBox;
    errorMsg->setText(tr("Invalid PID Data"));
    errorMsg->setDetailedText(tr("Enter a PID to parse data from to proceed"));
    errorMsg->exec();
}


void dataProcWindow::checkPID(){
    if (ui->lineEditPID->text() == ""){
        qDebug() << "no Pid entered\n";
        incompleteDataError();
    }
    else if (ui->lineEditFileSource->text() == ""){
        QString name = getFilename();
        ui->lineEditFileSource->setText(name);
        showData();
    }
    else showData();
}

QString dataProcWindow::getFilename(){
    QString name = QFileDialog::getOpenFileName(this, tr("Open CSV File"), "", "");
    return name;
}

void dataProcWindow::showData(){  //show data from selected ID
    QString name = ui->lineEditFileSource->text();// = QFileDialog::getOpenFileName(this, tr("Open CSV File"), "", "");
    //this->ui->lineEditFileSource->setText(name);
    pidType = ui->lineEditPID->text();
    QFile file(name);
    if (!file.open(QIODevice::ReadOnly)){
        qDebug() << file.errorString();
        return;
    }
    //int byteDifference = 0;
    int startPosition = ui->comboBoxStartByte->currentText().toInt();
    int endPosition = ui->comboBoxEndByte->currentText().toInt();
    //byteDifference = endPosition - startPosition + 1;
    //qDebug() << byteDifference;
    ui->textEdit->clear();
    ui->textEdit->append("// Time \t\t Data \t\t Conversion");
    QString tempTime;
    QString tempData;
    QString tempID;
    int hex = -1;
    bool ok = false;
    bool wireShark = false;
    QByteArray line = file.readLine();
    tempTime = line.at(0);
    if (tempTime == "\""){
        //qDebug() << "This is Wireshark data\n";
        line = file.readLine(); //skips first line of wireshark data
        wireShark = true;
    }
    chart->removeAllSeries();     //deletes prior series object
    series = new QSplineSeries(); //refresh series values for chart
    xVector.clear();
    yVector.clear();
    do {
        if (wireShark){
            tempID = line.split(',').at(6).split('"').at(1).split(' ').at(1);
            tempID.remove(0, 7);
            tempTime = line.split(',').at(1).split('"').at(1);
            tempData = line.split(',').at(6).split('"').at(1);
            tempData.remove(0, 18).remove(QChar(' '), Qt::CaseInsensitive);
            if (pidType == tempID && tempData.size() >= startPosition * 2){ // if the start position is not beyond length of data
                tempData.truncate(endPosition*2 + 2);
                tempData.remove(0, startPosition*2); //remove data from before start position
                hex = tempData.toInt(&ok, 16); //convert hex value to decimal
                if (ui->comboBoxModifier->currentIndex() == 1){
                    hex /= 4;
                }
                else if(ui->comboBoxModifier->currentIndex() == 2){
                    hex /= 100;
                }
                else if(ui->comboBoxModifier->currentIndex() == 3){
                    hex -= 10000;
                }
                else if(ui->comboBoxModifier->currentIndex() == 4){
                    hex = (hex - 10000)/100 * .621371;
                }
                ui->textEdit->append(" "+tempTime+"\t\t "+tempData+"\t\t"+ QString::number(hex, 10));
                series->append(tempTime.toDouble()*100, hex);  //shows time in milliseconds
                xVector.append(tempTime.toDouble()*100);
                yVector.append(hex);
            }
        }
        else {
            tempID = line.split(' ').at(2).split('#').at(0);
            tempTime = line.split(' ').at(0).split(')').at(0);
            tempTime.remove(0, 1);
            tempData = line.split(' ').at(2).split('#').at(1);
            tempData.remove(QChar('\n'), Qt::CaseInsensitive);
            //qDebug() << tempID;
            if (pidType == tempID && tempData.size() >= startPosition * 2){
                tempData.truncate(endPosition*2 + 2);
                tempData.remove(0, startPosition*2); //remove data from before start position
                hex = tempData.toInt(&ok, 16); //convert hex value to decimal
                if (ui->comboBoxModifier->currentIndex() == 1){
                    hex /= 4;
                }
                else if(ui->comboBoxModifier->currentIndex() == 2){
                    hex /= 100;
                }
                else if(ui->comboBoxModifier->currentIndex() == 3){
                    hex -= 10000;
                }
                else if(ui->comboBoxModifier->currentIndex() == 4){
                    hex = (hex - 10000)/100 * .621371;
                }
                ui->textEdit->append(" "+tempTime+"\t "+tempData+"\t\t"+ QString::number(hex, 10));
                tempTime.remove(0, 7);  //just to make axis point labels smaller
                series->append(tempTime.toDouble()*100, hex);  //shows time in milliseconds
                xVector.append(tempTime.toDouble()*100);
                yVector.append(hex);
            }
        }
        line = file.readLine();
    } while (!file.atEnd());

    file.close();

    chart->addSeries(series);
    chart->createDefaultAxes();
}


void dataProcWindow::updateVehicleList(){  //add known vehicles to combo box (user updated)
    QStringList vehicleList;
    QSqlQuery query;
    query.exec("SELECT * FROM vehicleList");
    QString tempVehicle;
    while (query.next()){
        tempVehicle = query.value(0).toString()+" "+query.value(1).toString()+" "
                +query.value(2).toString()+" "+query.value(3).toString();
        vehicleList.append(tempVehicle);
        //qDebug() << tempVehicle << "\n";
    }
    vehicleList.sort();
    ui->comboBoxVehicle->insertItems(0,vehicleList);
}
