#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    createActions();
    createMenus();

    QSqlDatabase db = db.addDatabase("QSQLITE");  //connect to database
    db.setDatabaseName("masterPIDList.db");

    if (!db.open()){
        qDebug() << db.lastError();
        qDebug() <<"Error: Unable to connect";
    }

    tableModel1 = new QSqlTableModel();
    tableModel2 = new QSqlTableModel();    //show master list by default in right window
    tableModel2->setTable("PIDList");
    tableModel2->select();
    ui->tableView2->setModel(tableModel2);
    ui->tableView2->resizeColumnsToContents();
    ui->tableView2->show();
}

MainWindow::~MainWindow(){
    QSqlQuery query;
    delete ui;
}

void MainWindow::createTempTable(int currentView){  //makes a temp table to show in the view
    QSqlQuery query;
    if (currentView == 1){
        query.exec("CREATE TEMP TABLE leftTable(ID TEXT, time TEXT, size TEXT, info TEXT, formula TEXT, conversion TEXT, comments TEXT)");
        tableModel1->clear();
        tableModel1 = new QSqlTableModel();
        tableModel1->setTable("leftTable");
        ui->tableView1->setModel(tableModel1);
        ui->tableView1->show();
    }
    else if (currentView == 2){
        query.exec("CREATE TEMP TABLE rightTable(ID TEXT, time TEXT, size TEXT, info TEXT, formula TEXT, conversion TEXT, comments TEXT)");
        tableModel2->clear();
        tableModel2 = new QSqlTableModel();
        tableModel2->setTable("rightTable");
        ui->tableView2->setModel(tableModel2);
        ui->tableView2->show();
    }
}

void MainWindow::setName(const QString &name){
    ui->IDLeftEdit->setText(name);
    ui->IDRightEdit->setText(name);
}


void MainWindow::createActions(){ //creates actions to be attached to menus
    openF1 = new QAction(tr("Open Left File"), this);
    openF2 = new QAction(tr("Open Right File"), this);
    connect(openF1, SIGNAL(triggered()), this, SLOT(fOpen1()));
    connect(ui->openButton1, SIGNAL(clicked(bool)), this, SLOT(fOpen1() ));
    connect(openF2, SIGNAL(triggered()), this, SLOT(fOpen2()));
    connect(ui->openButton2, SIGNAL(clicked(bool)), this, SLOT(fOpen2()));
    connect(ui->showButton1, SIGNAL(clicked(bool)), this, SLOT(showOnlyIDLeft()));
    connect(ui->showButton2, SIGNAL(clicked(bool)), this, SLOT(showOnlyIDRight()));
}

void MainWindow::showOnlyIDRight(){
    QString tempID = ui->IDRightEdit->text();
    tempID = "'%"+tempID+"'";
    tableModel2->setFilter(QString("ID LIKE %1").arg(tempID));
    qDebug() << tableModel2->filter();
}

void MainWindow::showOnlyIDLeft(){
    QString tempID = ui->IDLeftEdit->text();
    tempID = "'%"+tempID+"'";
    tableModel1->setFilter(QString("ID LIKE %1").arg(tempID));
    qDebug() << tableModel1->filter();
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
        query.prepare("INSERT INTO leftTable (ID, time, size, info) "
                      "VALUES (:id, :time, :size, :info)");
        query.bindValue(":id", temp0);
        query.bindValue(":time", temp1);
        query.bindValue(":size", temp5);
        query.bindValue(":info", temp6);
        query.exec();
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
        query.prepare("INSERT INTO rightTable (ID, time, size, info) "
                      "VALUES (:id, :time, :size, :info)");
        query.bindValue(":id", temp0);
        query.bindValue(":time", temp1);
        query.bindValue(":size", temp5);
        query.bindValue(":info", temp6);
        query.exec();
    }
    tableModel2->select();
    ui->tableView2->resizeColumnsToContents();
    file2.close();
}
