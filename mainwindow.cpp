#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    createActions();
    createMenus();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setName(const QString &name)
{
    ui->lineEdit->setText(name);
    ui->lineEdit_2->setText(name);
}


void MainWindow::createActions() //creates actions to be attached to menus
{
    openF1 = new QAction(tr("Open Left File"), this);
    openF2 = new QAction(tr("Open Right File"), this);
    connect(openF1, SIGNAL(triggered()), this, SLOT(fOpen1()));
    connect(openF2, SIGNAL(triggered()), this, SLOT(fOpen2()));
}

void MainWindow::createMenus() //adds menu items for holding actions
{
    file = menuBar()->addMenu(tr("File"));
    file->addAction(openF1);
    file->addAction(openF2);
}

void MainWindow::fOpen1(){  //opens a file to display in left window
    QString leftFile = ui->leftFileName->text();

    QFile file1(leftFile);
    if (!file1.open(QIODevice::ReadOnly)){
        qDebug() << file1.errorString();
        ui->textBrowser->append(file1.errorString());
        return;
    }

    while (!file1.atEnd()){
        QByteArray line = file1.readLine();
        ui->textBrowser->append(line);
    }
    file1.close();
}

void MainWindow::fOpen2(){  //opens a file to display in the right window
    QString rightFile = ui->rightFileName->text();

    QFile file2(rightFile);
    if (!file2.open(QIODevice::ReadOnly)){
        qDebug() << file2.errorString();
        ui->textBrowser_2->append(file2.errorString());
        return;
    }

    while (!file2.atEnd()){
        QByteArray line = file2.readLine();
        ui->textBrowser_2->append(line);
    }
    file2.close();
}
