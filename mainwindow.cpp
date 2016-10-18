#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    createActions();  //adds the actions and sets up signal connections
    createMenus();    //adds menus

    QSqlDatabase db = db.addDatabase("QSQLITE");  //connect to database
    db.setDatabaseName("masterPIDList.db");       //set to database expected to hold known PID value table
                                                  //should be created prior to running this program currently
    if (!db.open()){                //display error if DB not found
        qDebug() << db.lastError();
        qDebug() <<"Error: Unable to connect";
    }

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
        qDebug() << leftTableName;
        query.exec("CREATE TEMP TABLE "+ leftTableName + "(ID TEXT, data TEXT, time TEXT, size TEXT, info TEXT,"
                                                         " formula TEXT, conversion TEXT, comments TEXT,"
                                                         " make TEXT, model TEXT, year TEXT, vin TEXT)");
        qDebug() << query.lastQuery();
        tableModel1->clear();
        tableModel1 = new QSqlTableModel();
        tableModel1->setTable(leftTableName);
        ui->tableView1->setModel(tableModel1);
        ui->tableView1->show();
        filterOutFlag1 = 0;
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
    }
}

void MainWindow::setName(const QString &name){
    ui->IDLeftEdit->setText(name);
    ui->IDRightEdit->setText(name);
}


void MainWindow::createActions(){ //creates actions to be attached to menus and connects signals to slots
    openF1 = new QAction(tr("Open Left File"), this);
    openF2 = new QAction(tr("Open Right File"), this);
    connect(openF1, SIGNAL(triggered()), this, SLOT(fOpen1()));
    connect(ui->openButton1, SIGNAL(clicked(bool)), this, SLOT(fOpen1() ));
    connect(openF2, SIGNAL(triggered()), this, SLOT(fOpen2()));
    connect(ui->openButton2, SIGNAL(clicked(bool)), this, SLOT(fOpen2()));
    connect(ui->showButton1, SIGNAL(clicked(bool)), this, SLOT(showOnlyIDLeft()));
    connect(ui->showButton2, SIGNAL(clicked(bool)), this, SLOT(showOnlyIDRight()));
    connect(ui->filterButton1, SIGNAL(clicked(bool)), this, SLOT(filterOutLeft()));
    connect(ui->filterButton2, SIGNAL(clicked(bool)), this, SLOT(filterOutRight()));
    connect(ui->showUnique1, SIGNAL(clicked(bool)), this, SLOT(showUnique1()));
    connect(ui->showUnique2, SIGNAL(clicked(bool)), this, SLOT(showUnique2()));
    connect(ui->filterNonUnique, SIGNAL(clicked(bool)),this,SLOT(filterOutNonUnique()));
    connect(ui->showMaster1, SIGNAL(clicked(bool)), this, SLOT(showMaster1()));
    connect(ui->showMaster2, SIGNAL(clicked(bool)), this, SLOT(showMaster2()));
}

void MainWindow::showMaster1(){
    tableModel1->clear();
    tableModel1 = new QSqlTableModel();
    tableModel1->setTable("PIDList");
    tableModel1->select();
    ui->tableView1->setModel(tableModel1);
    ui->tableView1->resizeColumnsToContents();
    filterOutFlag1 = 0;
}

void MainWindow::showMaster2(){
    tableModel2->clear();
    tableModel2 = new QSqlTableModel();
    tableModel2->setTable("PIDList");
    tableModel2->select();
    ui->tableView2->setModel(tableModel2);
    ui->tableView2->resizeColumnsToContents();
    filterOutFlag2 = 0;
}

void MainWindow::filterOutNonUnique(){
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
    QString temp0;
    QString temp1;
    QString temp5;
    QString temp6;
    QByteArray line1 = file1.readLine(); //skip first line
    QByteArray line2 = file2.readLine(); //skip first line
    while (!file1.atEnd()){
        line1 = file1.readLine();
        temp0 = line1.split(',').at(6).split('"').at(1).split(' ').at(1);
        temp1 = line1.split(',').at(1).split('"').at(1);
        temp5 = line1.split(',').at(5).split('"').at(1);
        temp6 = line1.split(',').at(6).split('"').at(1);
        if (!leftIDset.contains(temp0)){  //create a set of IDs from left file
            leftIDset.insert(temp0);
        }
    }
    while (!file2.atEnd()){
        line2 = file2.readLine();
        temp0 = line2.split(',').at(6).split('"').at(1).split(' ').at(1);
        temp1 = line2.split(',').at(1).split('"').at(1);
        temp5 = line2.split(',').at(5).split('"').at(1);
        temp6 = line2.split(',').at(6).split('"').at(1);
        if (!rightIDset.contains(temp0)){ //create a set of IDs from right file
            rightIDset.insert(temp0);
        }
    }
    file1.seek(0); //start files back at beginning
    file2.seek(0);
    line1 = file1.readLine(); //skip first line
    line2 = file2.readLine(); //skip first line
    QSet<QString> uniqueSet;
    while (!file1.atEnd()){
        line1 = file1.readLine();
        temp0 = line1.split(',').at(6).split('"').at(1).split(' ').at(1);
        temp1 = line1.split(',').at(1).split('"').at(1);
        temp5 = line1.split(',').at(5).split('"').at(1);
        temp6 = line1.split(',').at(6).split('"').at(1);
        if (leftIDset.contains(temp0) && !rightIDset.contains(temp0)){  //add to model if not in file2
            if (!uniqueSet.contains(temp0)){
                uniqueSet.insert(temp0);
                query.prepare("INSERT INTO "+leftTableName+" (ID, time, size, info) "
                          "VALUES (:id, :time, :size, :info)");
                query.bindValue(":id", temp0);
                query.bindValue(":time", temp1);
                query.bindValue(":size", temp5);
                query.bindValue(":info", temp6);
                query.exec();
            }
        }
    }
    while (!file2.atEnd()){
        line2 = file2.readLine();
        temp0 = line2.split(',').at(6).split('"').at(1).split(' ').at(1);
        temp1 = line2.split(',').at(1).split('"').at(1);
        temp5 = line2.split(',').at(5).split('"').at(1);
        temp6 = line2.split(',').at(6).split('"').at(1);
        if (rightIDset.contains(temp0) && !leftIDset.contains(temp0)){ //add to model if not in file1
            if (!uniqueSet.contains(temp0)){
                uniqueSet.insert(temp0);
                query.prepare("INSERT INTO "+rightTableName+" (ID, time, size, info) "
                          "VALUES (:id, :time, :size, :info)");
                query.bindValue(":id", temp0);
                query.bindValue(":time", temp1);
                query.bindValue(":size", temp5);
                query.bindValue(":info", temp6);
                query.exec();
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
    QString temp0;
    QString temp1;
    QString temp5;
    QString temp6;
    QByteArray line = file1.readLine(); //skip first line
    while (!file1.atEnd()){
        line = file1.readLine();
        temp0 = line.split(',').at(6).split('"').at(1).split(' ').at(1);
        temp1 = line.split(',').at(1).split('"').at(1);
        temp5 = line.split(',').at(5).split('"').at(1);
        temp6 = line.split(',').at(6).split('"').at(1);

        if (!idList.contains(temp0)){
            idList.insert(temp0);
            query.prepare("INSERT INTO "+leftTableName+" (ID, time, size, info) "
                          "VALUES (:id, :time, :size, :info)");
            query.bindValue(":id", temp0);
            query.bindValue(":time", temp1);
            query.bindValue(":size", temp5);
            query.bindValue(":info", temp6);
            query.exec();
        }

    }
    tableModel1->sort(0, Qt::AscendingOrder);
    tableModel1->select();
    ui->tableView1->resizeColumnsToContents();
    file1.close();
    filterOutFlag1 = 0;
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
    QString temp0;
    QString temp1;
    QString temp5;
    QString temp6;
    QByteArray line = file2.readLine(); //skip first line
    while (!file2.atEnd()){
        line = file2.readLine();
        temp0 = line.split(',').at(6).split('"').at(1).split(' ').at(1);
        temp1 = line.split(',').at(1).split('"').at(1);
        temp5 = line.split(',').at(5).split('"').at(1);
        temp6 = line.split(',').at(6).split('"').at(1);
        if (!idList.contains(temp0)){
            idList.insert(temp0);
            query.prepare("INSERT INTO "+rightTableName+" (ID, time, size, info) "
                          "VALUES (:id, :time, :size, :info)");
            query.bindValue(":id", temp0);
            query.bindValue(":time", temp1);
            query.bindValue(":size", temp5);
            query.bindValue(":info", temp6);
            query.exec();
        }
    }
    tableModel2->sort(0, Qt::AscendingOrder);
    tableModel2->select();
    ui->tableView2->resizeColumnsToContents();
    file2.close();
    filterOutFlag2 = 0;
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
}

void MainWindow::showOnlyIDRight(){ //set filter on model to show only 1 ID
    QString tempID = ui->IDRightEdit->text();
    tempID = "'%"+tempID+"'";
    tableModel2->setFilter(QString("ID LIKE %1").arg(tempID));
    qDebug() << tableModel2->filter();
    filterOutFlag2 = 0;
}

void MainWindow::showOnlyIDLeft(){  //set filter on model to show only 1 ID
    QString tempID = ui->IDLeftEdit->text();
    tempID = "'%"+tempID+"'";
    tableModel1->setFilter(QString("ID LIKE %1").arg(tempID));
    qDebug() << tableModel1->filter();
    filterOutFlag1 = 0;
}

void MainWindow::createMenus(){ //adds menu items for holding actions
    file = menuBar()->addMenu(tr("File"));
    file->addAction(openF1);
    file->addAction(openF2);
}

void MainWindow::fOpen1(){  //opens a file to display in left window
    QString leftFile = ui->leftFileName->text();

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
            tempTime = line.split(',').at(1).split('"').at(1);
            tempSize = line.split(',').at(5).split('"').at(1);
            tempInfo = line.split(',').at(6).split('"').at(1);
            query.prepare("INSERT INTO "+leftTableName+" (ID, time, size, info) "
                      "VALUES (:id, :time, :size, :info)");
            query.bindValue(":id", tempID);
            query.bindValue(":time", tempTime);
            query.bindValue(":size", tempSize);
            query.bindValue(":info", tempInfo);
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
}

void MainWindow::fOpen2(){  //opens a file to display in the right window
    QString rightFile = ui->rightFileName->text();

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
            tempTime = line.split(',').at(1).split('"').at(1);
            tempSize = line.split(',').at(5).split('"').at(1);
            tempInfo = line.split(',').at(6).split('"').at(1);
            query.prepare("INSERT INTO "+rightTableName+" (ID, time, size, info) "
                      "VALUES (:id, :time, :size, :info)");
            query.bindValue(":id", tempID);
            query.bindValue(":time", tempTime);
            query.bindValue(":size", tempSize);
            query.bindValue(":info", tempInfo);
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
}
