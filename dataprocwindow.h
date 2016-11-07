#ifndef DATAPROCWINDOW_H
#define DATAPROCWINDOW_H

#include <QMainWindow>
#include <QSqlQuery>
#include <QDebug>
#include <QFile>
#include <QFileDialog>
#include <QMessageBox>

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
    QMenu *fileMenu;
    QMessageBox *errorMsg;

private slots:
    void updateVehicleList();
    void showData();
    void checkPID();
    void updateComboBoxes();

};

#endif // DATAPROCWINDOW_H
