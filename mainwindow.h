#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFile>
#include <QDebug>

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
    QAction *openF1;
    QAction *openF2;
    QMenu *file;

private slots:
    void fOpen1();
    void fOpen2();

};

#endif // MAINWINDOW_H
