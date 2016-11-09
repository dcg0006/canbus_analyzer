#ifndef DATAPROCWINDOW_H
#define DATAPROCWINDOW_H

#include <QMainWindow>
#include <QSqlQuery>
#include <QDebug>
#include <QFile>
#include <QFileDialog>
#include <QMessageBox>
#include "addtomasterdiag.h"

namespace Ui {
class dataProcWindow;
}

class dataProcWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit dataProcWindow(QWidget *parent = 0);
    ~dataProcWindow();
    void incompleteDataError();

private:
    Ui::dataProcWindow *ui;
    QString fileName;
    QString pidType;
    void createMenus();
    void createActions();
    QString getFilename();
    QAction *openFileAct;
    QAction *addIDtoMasterListAct;
    QMenu *fileMenu;
    QMenu *tableMenu;
    QMessageBox *errorMsg;
    addToMasterDiag *add2Master;

private slots:
    void updateVehicleList();
    void showData();
    void checkPID();
    void updateComboBoxes();
    void addIDtoMasterList();
    QString getPID();

};

#endif // DATAPROCWINDOW_H
