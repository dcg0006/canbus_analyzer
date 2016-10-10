#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFile>
#include <QDebug>
#include <QSqlTableModel>
#include <QTableView>
#include <QSqlQuery>
#include <QSqlError>
#include <QRegExp>
#include <QSet>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

void setName(const QString &name);


private:
    Ui::MainWindow *ui;
    void createMenus();
    void createActions();
    void createTempTable(int currentView);
    QAction *openF1;
    QAction *openF2;
    QMenu *file;
    QTableView *view1;
    QTableView *view2;
    QSqlTableModel *tableModel1;
    QSqlTableModel *tableModel2;
    int filterOutFlag1; //to keep track of when AND is needed in filterOut
    int filterOutFlag2;
    int tableCounter;
    QString leftTableName;
    QString rightTableName;

private slots:
    void fOpen1();
    void fOpen2();
    void showOnlyIDLeft();
    void showOnlyIDRight();
    void filterOutLeft();
    void filterOutRight();
    void showUnique1();
    void showUnique2();
    void filterOutNonUnique();
    void showMaster1();
    void showMaster2();
};

#endif // MAINWINDOW_H
