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
    int currentView;

private slots:
    void fOpen1();
    void fOpen2();
    void showOnlyIDLeft();
    void showOnlyIDRight();
};

#endif // MAINWINDOW_H
