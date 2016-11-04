#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    createActions();  //adds the actions and sets up signal connections
    createMenus();    //adds menus
    statusBarLabel1 = new QLabel;
    statusBarLabel2 = new QLabel;
    statusBarLabel1->setAlignment(Qt::AlignCenter);
    statusBarLabel2->setAlignment(Qt::AlignCenter);
    statusBarLabel2->setText(tr("Showing Master PID List"));
    statusBar()->addWidget(statusBarLabel1, 1);
    statusBar()->addWidget(statusBarLabel2, 1);

    QSqlDatabase db = db.addDatabase("QSQLITE");  //connect to database
    db.setDatabaseName("masterPIDList.db");       //set to database expected to hold known PID value table
    if (!db.open()){                //display error if DB not found
        qDebug() << db.lastError();
        qDebug() <<"Error: Unable to connect";
    }
    QSqlQuery checkTable;
    checkTable.exec("CREATE TABLE IF NOT EXISTS PIDList (ID TEXT, data TEXT, time TEXT, size TEXT, "
                    "info TEXT, formula TEXT, conversion TEXT, comments TEXT, make TEXT, model TEXT, "
                    "year TEXT, vin TEXT)");

    tableModel1 = new QSqlTableModel();
    tableModel2 = new QSqlTableModel();    //show master list by default in right window
    tableModel2->setTable("PIDList");
    tableModel2->select();
    ui->tableView2->setModel(tableModel2);
    ui->tableView2->resizeColumnsToContents(); //makes the table easier to read
    ui->tableView2->show();
    filterOutFlag2 = 0;  //nothing yet filtered by default
    tableCounter = 0;    //used to ensure new temp tables have different names only
}

MainWindow::~MainWindow(){
    QSqlQuery query;
    delete ui;
}

void MainWindow::createTempTable(int currentView){  //makes a temp table to show in the view
    QSqlQuery query;
    tableCounter++;
    if (currentView == 1){  // 1 = left window
        leftTableName = QString("leftTable%1").arg(tableCounter);
        //qDebug() << leftTableName;
        query.exec("CREATE TEMP TABLE "+ leftTableName + "(ID TEXT, data TEXT, time TEXT, size TEXT, info TEXT,"
                                                         " formula TEXT, conversion TEXT, comments TEXT,"
                                                         " make TEXT, model TEXT, year TEXT, vin TEXT)");
        //qDebug() << query.lastQuery();
        tableModel1->clear();
        tableModel1 = new QSqlTableModel();
        tableModel1->setTable(leftTableName);
        ui->tableView1->setModel(tableModel1);
        ui->tableView1->show();
        filterOutFlag1 = 0;
        statusBarLabel1->clear();
    }
    else if (currentView == 2){  // 2 = right window
        rightTableName = QString("rightTable%1").arg(tableCounter);
        tableModel2->clear();
        tableModel2 = new QSqlTableModel();
        query.exec("CREATE TEMP TABLE "+rightTableName+ "(ID TEXT, data TEXT, time TEXT, size TEXT, info TEXT,"
                                                        " formula TEXT, conversion TEXT, comments TEXT,"
                                                        " make TEXT, model TEXT, year TEXT, vin TEXT)");
        tableModel2->setTable(rightTableName);
        ui->tableView2->setModel(tableModel2);
        ui->tableView2->show();
        filterOutFlag2 = 0;
        statusBarLabel2->clear();
    }
}

void MainWindow::setName(const QString &name){
    ui->IDLeftEdit->setText(name);
    ui->IDRightEdit->setText(name);
}


void MainWindow::createActions(){ //creates actions to be attached to menus and connects signals to slots
    openF1 = new QAction(tr("Open Left File"), this);
    openF2 = new QAction(tr("Open Right File"), this);
    filterNonUnique = new QAction(tr("Filter Out Non-Unique IDs"), this);
    showCommonAct = new QAction(tr("Show IDs in Common"), this);
    addVehicleAct = new QAction(tr("Add New Vehicle"), this);
    showMaster1Act = new QAction(tr("Show Master List in Left Window"), this);
    showMaster2Act = new QAction(tr("Show Master List in Right Window"), this);
    showUnique1Act = new QAction(tr("Show Unique IDs Left"), this);
    showUnique2Act = new QAction(tr("Show Unique IDs Right"), this);
    startDataProc1Act = new QAction(tr("Process PID Indicated"), this);
    //startDataProc2Act = new QAction(tr("Process Right PID Indicated"), this);

    connect(openF1, SIGNAL(triggered()), this, SLOT(fOpen1()));
    connect(ui->openButton1, SIGNAL(clicked(bool)), this, SLOT(fOpen1() ));
    connect(openF2, SIGNAL(triggered()), this, SLOT(fOpen2()));
    connect(ui->openButton2, SIGNAL(clicked(bool)), this, SLOT(fOpen2()));
    connect(ui->showButton1, SIGNAL(clicked(bool)), this, SLOT(showOnlyIDLeft()));
    connect(ui->showButton2, SIGNAL(clicked(bool)), this, SLOT(showOnlyIDRight()));
    connect(ui->filterButton1, SIGNAL(clicked(bool)), this, SLOT(filterOutLeft()));
    connect(ui->filterButton2, SIGNAL(clicked(bool)), this, SLOT(filterOutRight()));
    connect(showUnique1Act, SIGNAL(triggered(bool)), this, SLOT(showUnique1()));
    connect(showUnique2Act, SIGNAL(triggered(bool)), this, SLOT(showUnique2()));
    connect(filterNonUnique, SIGNAL(triggered(bool)), this, SLOT(filterOutNonUnique()));
    connect(showCommonAct, SIGNAL(triggered(bool)), this, SLOT(showCommonID()));
    connect(showMaster1Act, SIGNAL(triggered(bool)), this, SLOT(showMaster1()));
    connect(showMaster2Act, SIGNAL(triggered(bool)), this, SLOT(showMaster2()));
    connect(addVehicleAct, SIGNAL(triggered(bool)), this, SLOT(addNewVehicle()));
    connect(startDataProc1Act, SIGNAL(triggered(bool)), this, SLOT(startDataProc1()));
    //connect(startDataProc2Act, SIGNAL(triggered(bool)), this, SLOT(startDataProc2()));
}

void MainWindow::createMenus(){ //adds menu items for holding actions
    file = menuBar()->addMenu(tr("Files"));
    file->addSection(tr("Both Files"));
    tables = menuBar()->addMenu(tr("Tables"));
    data = menuBar()->addMenu(tr("Data"));
    file->addAction(filterNonUnique);
    file->addAction(showCommonAct);
    file->addSection(tr("Left File"));
    file->addAction(openF1);
    file->addAction(showUnique1Act);
    file->addSection(tr("Right File"));
    file->addAction(openF2);
    file->addAction(showUnique2Act);
    tables->addSection(tr("Master PID Table"));
    tables->addAction(showMaster1Act);
    tables->addAction(showMaster2Act);
    tables->addSection(tr("Vehicle Table"));
    tables->addAction(addVehicleAct);
    data->addSection(tr("Data Parsing"));
    data->addAction(startDataProc1Act);
    //data->addSection(tr("Right Window"));
    //data->addAction(startDataProc2Act);
}
/*
void MainWindow::dataProc(QString fileName, QString pidType){
    dataProcess =  new dataProcWindow;
    //dataProcess->updateText(fileName, pidType);
    dataProcess->show();
}*/

void MainWindow::startDataProc1(){
    //QString tempFileName = ui->leftFileName->text();
    //QString tempPIDType = ui->IDLeftEdit->text();
    //dataProc(tempFileName, tempPIDType);
    dataProcess =  new dataProcWindow;
    //dataProcess->updateText(fileName, pidType);
    dataProcess->show();
}
/*
void MainWindow::startDataProc2(){
    QString tempFileName = ui->rightFileName->text();
    QString tempPIDType = ui->IDRightEdit->text();
    dataProc(tempFileName, tempPIDType);
}*/

void MainWindow::addNewVehicle(){
    addVehicleDiag = new newVehicleDiag;
    //addVehicleDiag->setWindowTitle("Add New Vehicle");
   // QLabel yearLabel = "Year:";
   // QBoxLayout layout = new QBoxLayout;
   // layout.addItem(yearLabel);
    addVehicleDiag->exec();
}

void MainWindow::showMaster1(){
    tableModel1->clear();
    tableModel1 = new QSqlTableModel();
    tableModel1->setTable("PIDList");
    tableModel1->select();
    ui->tableView1->setModel(tableModel1);
    ui->tableView1->resizeColumnsToContents();
    filterOutFlag1 = 0;
    statusBarLabel1->setText("Showing Master PID List");
}

void MainWindow::showMaster2(){
    tableModel2->clear();
    tableModel2 = new QSqlTableModel();
    tableModel2->setTable("PIDList");
    tableModel2->select();
    ui->tableView2->setModel(tableModel2);
    ui->tableView2->resizeColumnsToContents();
    filterOutFlag2 = 0;
    statusBarLabel2->setText("Showing Master PID List");
}

void MainWindow::showCommonID(){ //show what IDs are common to both files
    QString leftFile = ui->leftFileName->text();
    QString rightFile = ui->rightFileName->text();
    QFile file1(leftFile);
    if (!file1.open(QIODevice::ReadOnly)){
        qDebug() << file1.errorString();
        return;
    }
    QFile file2(rightFile);
    if (!file2.open(QIODevice::ReadOnly)){
        qDebug() << file2.errorString();
        return;
    }
    QSet<QString> leftIDset;
    QSet<QString> rightIDset;
    createTempTable(1);
    createTempTable(2);
    QSqlQuery query;
    QString tempID;
    QString tempInfo;
    QString tempTime;
    QString tempSize;
    QString tempData;
    QByteArray line1;
    QByteArray line2;
    //create sets of IDs for left and right file
    if (ui->radioLeftWire->isChecked()){
        line1 = file1.readLine(); //skip first line if Wireshark format
        while (!file1.atEnd()){
            line1 = file1.readLine();
            tempID = line1.split(',').at(6).split('"').at(1).split(' ').at(1);
            tempID.remove(0, 7);
            if (!leftIDset.contains(tempID)){  //create a set of IDs from left file
                leftIDset.insert(tempID);
            }
        }
    }
    else if (ui->radioLeftCandump->isChecked()){
        //no need to skip first line in candump format
        while (!file1.atEnd()){
            line1 = file1.readLine();
            tempID = line1.split(' ').at(2).split('#').at(0);
            if (!leftIDset.contains(tempID)){
                leftIDset.insert(tempID);
            }
        }
    }

    if (ui->radioRightWire->isChecked()){
        line2 = file2.readLine(); //skip first line if Wireshark format
        while (!file2.atEnd()){
            line2 = file2.readLine();
            tempID = line2.split(',').at(6).split('"').at(1).split(' ').at(1);
            tempID.remove(0, 7);
            if (!rightIDset.contains(tempID)){ //create a set of IDs from right file
                rightIDset.insert(tempID);
            }
        }
    }
    else if (ui->radioRightCandump->isChecked()){
        //no need to skip first line in candump format
        while (!file2.atEnd()){
            line2 = file2.readLine();
            tempID = line2.split(' ').at(2).split('#').at(0);
            if (!rightIDset.contains(tempID)){
                rightIDset.insert(tempID);
            }
        }
    }

    file1.seek(0); //start files back at beginning
    file2.seek(0);
    QSet<QString> uniqueSet;
    //fill in left side with shared IDs
    if (ui->radioLeftWire->isChecked()){
        line1 = file1.readLine(); //skip first line for wireshark format
        while (!file1.atEnd()){
            line1 = file1.readLine();
            tempID = line1.split(',').at(6).split('"').at(1).split(' ').at(1);
            tempID.remove(0, 7);
            tempTime = line1.split(',').at(1).split('"').at(1);
            tempSize = line1.split(',').at(5).split('"').at(1);
            tempInfo = line1.split(',').at(6).split('"').at(1);
            tempData = line1.split(',').at(6).split('"').at(1);
            tempData.remove(0, 18).remove(QChar(' '), Qt::CaseInsensitive);
            if (leftIDset.contains(tempID) && rightIDset.contains(tempID)){  //add to model if in file2
                if (!uniqueSet.contains(tempID)){ //but only add it once
                    uniqueSet.insert(tempID);
                    query.prepare("INSERT INTO "+leftTableName+" (ID, time, size, info, data) "
                              "VALUES (:id, :time, :size, :info, :data)");
                    query.bindValue(":id", tempID);
                    query.bindValue(":time", tempTime);
                    query.bindValue(":size", tempSize);
                    query.bindValue(":info", tempInfo);
                    query.bindValue(":data", tempData);
                    query.exec();
                }
            }
        }
    }
    else if (ui->radioLeftCandump->isChecked()){ //begin parsing candump formatted .csv file
        //no need to skip first line
        QByteArray line;
        while (!file1.atEnd()){
            line1 = file1.readLine();
            tempID = line1.split(' ').at(2).split('#').at(0);
            tempTime = line1.split(' ').at(0).split('.').at(1).split(')').at(0);
            tempInfo = line1.split(' ').at(2);
            tempData = line1.split(' ').at(2).split('#').at(1);
            if (leftIDset.contains(tempID) && rightIDset.contains(tempID)){  //add to model if in file2
                if (!uniqueSet.contains(tempID)){ //but only add it once
                    uniqueSet.insert(tempID);
                    query.prepare("INSERT INTO "+leftTableName+" (ID, time, info, data) "
                                  "VALUES (:id, :time, :info, :data)");
                    query.bindValue(":id", tempID);
                    query.bindValue(":time", tempTime);
                    query.bindValue(":info", tempInfo);
                    query.bindValue(":data", tempData);
                    query.exec();
                }
            }
        }
    }
    //now for the right side
    uniqueSet.clear();
    if (ui->radioRightWire->isChecked()){
        line2 = file2.readLine(); //skip first line for wireshark format
        while (!file2.atEnd()){
            line2 = file2.readLine();
            tempID = line2.split(',').at(6).split('"').at(1).split(' ').at(1);
            tempID.remove(0, 7);
            tempTime = line2.split(',').at(1).split('"').at(1);
            tempSize = line2.split(',').at(5).split('"').at(1);
            tempInfo = line2.split(',').at(6).split('"').at(1);
            tempData = line2.split(',').at(6).split('"').at(1);
            tempData.remove(0, 18).remove(QChar(' '), Qt::CaseInsensitive);
            if (rightIDset.contains(tempID) && leftIDset.contains(tempID)){  //add to model if in file1
                if (!uniqueSet.contains(tempID)){ //but only add it once
                    uniqueSet.insert(tempID);
                    query.prepare("INSERT INTO "+rightTableName+" (ID, time, size, info, data) "
                              "VALUES (:id, :time, :size, :info, :data)");
                    query.bindValue(":id", tempID);
                    query.bindValue(":time", tempTime);
                    query.bindValue(":size", tempSize);
                    query.bindValue(":info", tempInfo);
                    query.bindValue(":data", tempData);
                    query.exec();
                }
            }
        }
    }
    else if (ui->radioRightCandump->isChecked()){ //begin parsing candump formatted .csv file
        //no need to skip first line
        QByteArray line;
        while (!file2.atEnd()){
            line2 = file2.readLine();
            tempID = line2.split(' ').at(2).split('#').at(0);
            tempTime = line2.split(' ').at(0).split('.').at(1).split(')').at(0);
            tempInfo = line2.split(' ').at(2);
            tempData = line2.split(' ').at(2).split('#').at(1);
            if (rightIDset.contains(tempID) && leftIDset.contains(tempID)){  //add to model if in file1
                if (!uniqueSet.contains(tempID)){ //but only add it once
                    uniqueSet.insert(tempID);
                    query.prepare("INSERT INTO "+rightTableName+" (ID, time, info, data) "
                                  "VALUES (:id, :time, :info, :data)");
                    query.bindValue(":id", tempID);
                    query.bindValue(":time", tempTime);
                    query.bindValue(":info", tempInfo);
                    query.bindValue(":data", tempData);
                    query.exec();
                }
            }
        }
    }

    tableModel1->sort(0, Qt::AscendingOrder);
    tableModel1->select();
    ui->tableView1->resizeColumnsToContents();
    file1.close();
    filterOutFlag1 = 0;
    tableModel2->sort(0, Qt::AscendingOrder);
    tableModel2->select();
    ui->tableView2->resizeColumnsToContents();
    file2.close();
    filterOutFlag2 = 0;
    leftIDset.clear();
    rightIDset.clear();
    uniqueSet.clear();
    statusBarLabel1->setText(tr("Showing IDs shared with right file"));
    statusBarLabel2->setText(tr("Showing IDs shared with left file"));

}

void MainWindow::filterOutNonUnique(){ //show what IDs are unique to each file
    QString leftFile = ui->leftFileName->text();
    QString rightFile = ui->rightFileName->text();
    QFile file1(leftFile);
    if (!file1.open(QIODevice::ReadOnly)){
        qDebug() << file1.errorString();
        return;
    }
    QFile file2(rightFile);
    if (!file2.open(QIODevice::ReadOnly)){
        qDebug() << file2.errorString();
        return;
    }
    QSet<QString> leftIDset;
    QSet<QString> rightIDset;
    createTempTable(1);
    createTempTable(2);
    QSqlQuery query;
    QString tempID;
    QString tempInfo;
    QString tempTime;
    QString tempSize;
    QString tempData;
    QByteArray line1;
    QByteArray line2;
    //create sets of IDs for left and right file
    if (ui->radioLeftWire->isChecked()){
        line1 = file1.readLine(); //skip first line if Wireshark format
        while (!file1.atEnd()){
            line1 = file1.readLine();
            tempID = line1.split(',').at(6).split('"').at(1).split(' ').at(1);
            tempID.remove(0, 7);
            if (!leftIDset.contains(tempID)){  //create a set of IDs from left file
                leftIDset.insert(tempID);
            }
        }
    }
    else if (ui->radioLeftCandump->isChecked()){
        //no need to skip first line in candump format
        while (!file1.atEnd()){
            line1 = file1.readLine();
            tempID = line1.split(' ').at(2).split('#').at(0);
            if (!leftIDset.contains(tempID)){
                leftIDset.insert(tempID);
            }
        }
    }

    if (ui->radioRightWire->isChecked()){
        line2 = file2.readLine(); //skip first line if Wireshark format
        while (!file2.atEnd()){
            line2 = file2.readLine();
            tempID = line2.split(',').at(6).split('"').at(1).split(' ').at(1);
            tempID.remove(0, 7);
            if (!rightIDset.contains(tempID)){ //create a set of IDs from right file
                rightIDset.insert(tempID);
            }
        }
    }
    else if (ui->radioRightCandump->isChecked()){
        //no need to skip first line in candump format
        while (!file2.atEnd()){
            line2 = file2.readLine();
            tempID = line2.split(' ').at(2).split('#').at(0);
            if (!rightIDset.contains(tempID)){
                rightIDset.insert(tempID);
            }
        }
    }

    file1.seek(0); //start files back at beginning
    file2.seek(0);
    QSet<QString> uniqueSet;
    //fill in left side with unique IDs
    if (ui->radioLeftWire->isChecked()){
        line1 = file1.readLine(); //skip first line for wireshark format
        while (!file1.atEnd()){
            line1 = file1.readLine();
            tempID = line1.split(',').at(6).split('"').at(1).split(' ').at(1);
            tempID.remove(0, 7);
            tempTime = line1.split(',').at(1).split('"').at(1);
            tempSize = line1.split(',').at(5).split('"').at(1);
            tempInfo = line1.split(',').at(6).split('"').at(1);
            tempData = line1.split(',').at(6).split('"').at(1);
            tempData.remove(0, 18).remove(QChar(' '), Qt::CaseInsensitive);
            if (leftIDset.contains(tempID) && !rightIDset.contains(tempID)){  //add to model if not in file2
                if (!uniqueSet.contains(tempID)){ //but only add it once
                    uniqueSet.insert(tempID);
                    query.prepare("INSERT INTO "+leftTableName+" (ID, time, size, info, data) "
                              "VALUES (:id, :time, :size, :info, :data)");
                    query.bindValue(":id", tempID);
                    query.bindValue(":time", tempTime);
                    query.bindValue(":size", tempSize);
                    query.bindValue(":info", tempInfo);
                    query.bindValue(":data", tempData);
                    query.exec();
                }
            }
        }
    }
    else if (ui->radioLeftCandump->isChecked()){ //begin parsing candump formatted .csv file
        //no need to skip first line
        QByteArray line;
        while (!file1.atEnd()){
            line1 = file1.readLine();
            tempID = line1.split(' ').at(2).split('#').at(0);
            tempTime = line1.split(' ').at(0).split('.').at(1).split(')').at(0);
            tempInfo = line1.split(' ').at(2);
            tempData = line1.split(' ').at(2).split('#').at(1);
            if (leftIDset.contains(tempID) && !rightIDset.contains(tempID)){  //add to model if not in file2
                if (!uniqueSet.contains(tempID)){ //but only add it once
                    uniqueSet.insert(tempID);
                    query.prepare("INSERT INTO "+leftTableName+" (ID, time, info, data) "
                                  "VALUES (:id, :time, :info, :data)");
                    query.bindValue(":id", tempID);
                    query.bindValue(":time", tempTime);
                    query.bindValue(":info", tempInfo);
                    query.bindValue(":data", tempData);
                    query.exec();
                }
            }
        }
    }
    //now for the right side
    if (ui->radioRightWire->isChecked()){
        line2 = file2.readLine(); //skip first line for wireshark format
        while (!file2.atEnd()){
            line2 = file2.readLine();
            tempID = line2.split(',').at(6).split('"').at(1).split(' ').at(1);
            tempID.remove(0, 7);
            tempTime = line2.split(',').at(1).split('"').at(1);
            tempSize = line2.split(',').at(5).split('"').at(1);
            tempInfo = line2.split(',').at(6).split('"').at(1);
            tempData = line2.split(',').at(6).split('"').at(1);
            tempData.remove(0, 18).remove(QChar(' '), Qt::CaseInsensitive);
            if (rightIDset.contains(tempID) && !leftIDset.contains(tempID)){  //add to model if not in file1
                if (!uniqueSet.contains(tempID)){ //but only add it once
                    uniqueSet.insert(tempID);
                    query.prepare("INSERT INTO "+rightTableName+" (ID, time, size, info, data) "
                              "VALUES (:id, :time, :size, :info, :data)");
                    query.bindValue(":id", tempID);
                    query.bindValue(":time", tempTime);
                    query.bindValue(":size", tempSize);
                    query.bindValue(":info", tempInfo);
                    query.bindValue(":data", tempData);
                    query.exec();
                }
            }
        }
    }
    else if (ui->radioRightCandump->isChecked()){ //begin parsing candump formatted .csv file
        //no need to skip first line
        QByteArray line;
        while (!file2.atEnd()){
            line2 = file2.readLine();
            tempID = line2.split(' ').at(2).split('#').at(0);
            tempTime = line2.split(' ').at(0).split('.').at(1).split(')').at(0);
            tempInfo = line2.split(' ').at(2);
            tempData = line2.split(' ').at(2).split('#').at(1);
            if (rightIDset.contains(tempID) && !leftIDset.contains(tempID)){  //add to model if not in file1
                if (!uniqueSet.contains(tempID)){ //but only add it once
                    uniqueSet.insert(tempID);
                    query.prepare("INSERT INTO "+rightTableName+" (ID, time, info, data) "
                                  "VALUES (:id, :time, :info, :data)");
                    query.bindValue(":id", tempID);
                    query.bindValue(":time", tempTime);
                    query.bindValue(":info", tempInfo);
                    query.bindValue(":data", tempData);
                    query.exec();
                }
            }
        }
    }



    tableModel1->sort(0, Qt::AscendingOrder);
    tableModel1->select();
    ui->tableView1->resizeColumnsToContents();
    file1.close();
    filterOutFlag1 = 0;
    tableModel2->sort(0, Qt::AscendingOrder);
    tableModel2->select();
    ui->tableView2->resizeColumnsToContents();
    file2.close();
    filterOutFlag2 = 0;
    leftIDset.clear();
    rightIDset.clear();
    uniqueSet.clear();
    statusBarLabel1->setText(tr("Showing IDs not in right file"));
    statusBarLabel2->setText(tr("Showing IDs not in left File"));
}

void MainWindow::showUnique1(){
    QString leftFile = ui->leftFileName->text();

    QFile file1(leftFile);
    if (!file1.open(QIODevice::ReadOnly)){
        qDebug() << file1.errorString();
        return;
    }
    QSet<QString> idList;
    createTempTable(1);  //create temp table to hold file data
    QSqlQuery query;
    QString tempID;
    QString tempTime;
    QString tempSize;
    QString tempInfo;
    QString tempData;
    if (ui->radioLeftWire->isChecked()){// begin parsing wireshark format .csv file
        QByteArray line = file1.readLine(); //skip first line
        while (!file1.atEnd()){
            line = file1.readLine();
            tempID = line.split(',').at(6).split('"').at(1).split(' ').at(1);
            tempID.remove(0, 7);
            tempTime = line.split(',').at(1).split('"').at(1);
            tempSize = line.split(',').at(5).split('"').at(1);
            tempInfo = line.split(',').at(6).split('"').at(1);
            tempData = line.split(',').at(6).split('"').at(1);
            tempData.remove(0, 18).remove(QChar(' '), Qt::CaseInsensitive);
            if (!idList.contains(tempID)){
                idList.insert(tempID);
                query.prepare("INSERT INTO "+leftTableName+" (ID, time, size, info, data) "
                              "VALUES (:id, :time, :size, :info, :data)");
                query.bindValue(":id", tempID);
                query.bindValue(":time", tempTime);
                query.bindValue(":size", tempSize);
                query.bindValue(":info", tempInfo);
                query.bindValue(":data", tempData);
                query.exec();
            }
        }
    }
    else if (ui->radioLeftCandump->isChecked()){ //begin parsing candump formatted .csv file
        //no need to skip first line
        QByteArray line;
        while (!file1.atEnd()){
            line = file1.readLine();
            tempID = line.split(' ').at(2).split('#').at(0);
            tempTime = line.split(' ').at(0).split('.').at(1).split(')').at(0);
            tempInfo = line.split(' ').at(2);
            tempData = line.split(' ').at(2).split('#').at(1);
            if (!idList.contains(tempID)){
                idList.insert(tempID);
                query.prepare("INSERT INTO "+leftTableName+" (ID, time, info, data) "
                              "VALUES (:id, :time, :info, :data)");
                query.bindValue(":id", tempID);
                query.bindValue(":time", tempTime);
                query.bindValue(":info", tempInfo);
                query.bindValue(":data", tempData);
                query.exec();
            }
        }
    }
    tableModel1->sort(0, Qt::AscendingOrder);
    tableModel1->select();
    ui->tableView1->resizeColumnsToContents();
    file1.close();
    filterOutFlag1 = 0;
    statusBarLabel1->setText(tr("Showing Unique IDs from CSV File"));
}

void MainWindow::showUnique2(){
    QString rightFile = ui->rightFileName->text();

    QFile file2(rightFile);
    if (!file2.open(QIODevice::ReadOnly)){
        qDebug() << file2.errorString();
        //ui->tableView2->append(file2.errorString());
        return;
    }
    QSet<QString> idList;
    createTempTable(2);  //create temp table to hold file data
    QSqlQuery query;
    QString tempID;
    QString tempTime;
    QString tempSize;
    QString tempInfo;
    QString tempData;
    if (ui->radioRightWire->isChecked()){// begin parsing wireshark format .csv file
        QByteArray line = file2.readLine(); //skip first line
        while (!file2.atEnd()){
            line = file2.readLine();
            tempID = line.split(',').at(6).split('"').at(1).split(' ').at(1);
            tempID.remove(0, 7);
            tempTime = line.split(',').at(1).split('"').at(1);
            tempSize = line.split(',').at(5).split('"').at(1);
            tempInfo = line.split(',').at(6).split('"').at(1);
            tempData = line.split(',').at(6).split('"').at(1);
            tempData.remove(0, 18).remove(QChar(' '), Qt::CaseInsensitive);
            if (!idList.contains(tempID)){
                idList.insert(tempID);
                query.prepare("INSERT INTO "+rightTableName+" (ID, time, size, info, data) "
                              "VALUES (:id, :time, :size, :info, :data)");
                query.bindValue(":id", tempID);
                query.bindValue(":time", tempTime);
                query.bindValue(":size", tempSize);
                query.bindValue(":info", tempInfo);
                query.bindValue(":data", tempData);
                query.exec();
            }
        }
    }
    else if (ui->radioRightCandump->isChecked()){ //begin parsing candump formatted .csv file
        //no need to skip first line
        QByteArray line;
        while (!file2.atEnd()){
            line = file2.readLine();
            tempID = line.split(' ').at(2).split('#').at(0);
            tempTime = line.split(' ').at(0).split('.').at(1).split(')').at(0);
            tempInfo = line.split(' ').at(2);
            tempData = line.split(' ').at(2).split('#').at(1);
            if (!idList.contains(tempID)){
                idList.insert(tempID);
                query.prepare("INSERT INTO "+rightTableName+" (ID, time, info, data) "
                              "VALUES (:id, :time, :info, :data)");
                query.bindValue(":id", tempID);
                query.bindValue(":time", tempTime);
                query.bindValue(":info", tempInfo);
                query.bindValue(":data", tempData);
                query.exec();
            }
        }
    }
    tableModel2->sort(0, Qt::AscendingOrder);
    tableModel2->select();
    ui->tableView2->resizeColumnsToContents();
    file2.close();
    filterOutFlag2 = 0;
    statusBarLabel2->setText(tr("Showing Unique IDs from CSV File"));
}

void MainWindow::filterOutLeft(){
    QString tempFilter;
    if (!filterOutFlag1){
        tempFilter = "'%"+ui->IDLeftEdit->text()+"'";
        tableModel1->setFilter(QString("ID NOT LIKE %1").arg(tempFilter));
        filterOutFlag1 = 1;
    }
    else {
        tempFilter = tableModel1->filter()+" AND ID NOT LIKE '%"+ui->IDLeftEdit->text()+"'";
        tableModel1->setFilter(QString(tempFilter));
        qDebug() << tableModel1->filter();
    }
    statusBarLabel1->setText(tr("Showing Filtered CSV File"));
}

void MainWindow::filterOutRight(){
    QString tempFilter;
    if (!filterOutFlag2){
        tempFilter = "'%"+ui->IDRightEdit->text()+"'";
        tableModel2->setFilter(QString("ID NOT LIKE %1").arg(tempFilter));
        filterOutFlag2 = 1;
    }
    else {
        tempFilter = tableModel2->filter()+" AND ID NOT LIKE '%"+ui->IDRightEdit->text()+"'";
        tableModel2->setFilter(QString(tempFilter));
        qDebug() << tableModel2->filter();
    }
    statusBarLabel2->setText(tr("Showing Filtered CSV File"));
}

void MainWindow::showOnlyIDRight(){ //set filter on model to show only 1 ID
    QString tempID = ui->IDRightEdit->text();
    tempID = "'%"+tempID+"'";
    tableModel2->setFilter(QString("ID LIKE %1").arg(tempID));
    qDebug() << tableModel2->filter();
    filterOutFlag2 = 0;
    statusBarLabel2->setText(tr("Showing Filtered CSV File"));
}

void MainWindow::showOnlyIDLeft(){  //set filter on model to show only 1 ID
    QString tempID = ui->IDLeftEdit->text();
    tempID = "'%"+tempID+"'";
    tableModel1->setFilter(QString("ID LIKE %1").arg(tempID));
    qDebug() << tableModel1->filter();
    filterOutFlag1 = 0;
    statusBarLabel1->setText(tr("Showing Filtered CSV File"));
}

void MainWindow::fOpen1(){  //opens a file to display in left window
    //QString leftFile = ui->leftFileName->text();
    QString leftFile = QFileDialog::getOpenFileName(this, tr("Open CSV File"), "", "");
    ui->leftFileName->setText(leftFile);
    QFile file1(leftFile);
    if (!file1.open(QIODevice::ReadOnly)){
        qDebug() << file1.errorString();
        return;
    }
    createTempTable(1);  //create temp table to hold file data
    QSqlQuery query;
    QString tempID;
    QString tempTime;
    QString tempSize;
    QString tempInfo;
    QString tempData;
    if (ui->radioLeftWire->isChecked()){// begin parsing wireshark format .csv file
        QByteArray line = file1.readLine(); //skip first line
        while (!file1.atEnd()){
            line = file1.readLine();
            tempID = line.split(',').at(6).split('"').at(1).split(' ').at(1);
            tempID.remove(0, 7);
            tempTime = line.split(',').at(1).split('"').at(1);
            tempSize = line.split(',').at(5).split('"').at(1);
            tempInfo = line.split(',').at(6).split('"').at(1);
            tempData = line.split(',').at(6).split('"').at(1);
            tempData.remove(0, 18).remove(QChar(' '), Qt::CaseInsensitive);
            query.prepare("INSERT INTO "+leftTableName+" (ID, time, size, info, data) "
                      "VALUES (:id, :time, :size, :info, :data)");
            query.bindValue(":id", tempID);
            query.bindValue(":time", tempTime);
            query.bindValue(":size", tempSize);
            query.bindValue(":info", tempInfo);
            query.bindValue(":data", tempData);
            query.exec();
        }
    }
    else if (ui->radioLeftCandump->isChecked()){ //begin parsing candump formatted .csv file
        //no need to skip first line
        QByteArray line;
        while (!file1.atEnd()){
            line = file1.readLine();
            tempID = line.split(' ').at(2).split('#').at(0);
            tempTime = line.split(' ').at(0).split('.').at(1).split(')').at(0);
            tempInfo = line.split(' ').at(2);
            tempData = line.split(' ').at(2).split('#').at(1);
            query.prepare("INSERT INTO "+leftTableName+" (ID, time, info, data) "
                          "VALUES (:id, :time, :info, :data)");
            query.bindValue(":id", tempID);
            query.bindValue(":time", tempTime);
            query.bindValue(":info", tempInfo);
            query.bindValue(":data", tempData);
            query.exec();

        }
    }
    tableModel1->select();
    ui->tableView1->resizeColumnsToContents();
    file1.close();
    statusBarLabel1->setText(tr("Showing CSV File"));
}

void MainWindow::fOpen2(){  //opens a file to display in the right window
    //QString rightFile = ui->rightFileName->text();
    QString rightFile = QFileDialog::getOpenFileName(this, tr("Open CSV File"), "", "");
    ui->rightFileName->setText(rightFile);
    QFile file2(rightFile);
    if (!file2.open(QIODevice::ReadOnly)){
        qDebug() << file2.errorString();
        //ui->tableView2->append(file2.errorString());
        return;
    }
    createTempTable(2);  //create temp table to hold file data
    QSqlQuery query;
    QString tempID;
    QString tempTime;
    QString tempSize;
    QString tempInfo;
    QString tempData;
    if (ui->radioRightWire->isChecked()){// begin parsing wireshark format .csv file
        QByteArray line = file2.readLine(); //skip first line
        while (!file2.atEnd()){
            line = file2.readLine();
            tempID = line.split(',').at(6).split('"').at(1).split(' ').at(1);
            tempID.remove(0, 7);
            tempTime = line.split(',').at(1).split('"').at(1);
            tempSize = line.split(',').at(5).split('"').at(1);
            tempInfo = line.split(',').at(6).split('"').at(1);
            tempData = line.split(',').at(6).split('"').at(1);
            tempData.remove(0, 18).remove(QChar(' '), Qt::CaseInsensitive);
            query.prepare("INSERT INTO "+rightTableName+" (ID, time, size, info, data) "
                      "VALUES (:id, :time, :size, :info, :data)");
            query.bindValue(":id", tempID);
            query.bindValue(":time", tempTime);
            query.bindValue(":size", tempSize);
            query.bindValue(":info", tempInfo);
            query.bindValue(":data", tempData);
            query.exec();
        }
    }
    else if (ui->radioRightCandump->isChecked()){ //begin parsing candump formatted .csv file
        //no need to skip first line
        QByteArray line;
        while (!file2.atEnd()){
            line = file2.readLine();
            tempID = line.split(' ').at(2).split('#').at(0);
            tempTime = line.split(' ').at(0).split('.').at(1).split(')').at(0);
            tempInfo = line.split(' ').at(2);
            tempData = line.split(' ').at(2).split('#').at(1);
            query.prepare("INSERT INTO "+rightTableName+" (ID, time, info, data) "
                          "VALUES (:id, :time, :info, :data)");
            query.bindValue(":id", tempID);
            query.bindValue(":time", tempTime);
            query.bindValue(":info", tempInfo);
            query.bindValue(":data", tempData);
            query.exec();

        }
    }
    //qDebug() << query.lastQuery();
    tableModel2->select();
    ui->tableView2->resizeColumnsToContents();
    file2.close();
    statusBarLabel2->setText(tr("Showing CSV File"));
}
