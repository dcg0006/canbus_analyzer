#ifndef DATAPROCWINDOW_H
#define DATAPROCWINDOW_H

#include <QMainWindow>
#include <QSqlQuery>
#include <QDebug>

namespace Ui {
class dataProcWindow;
}

class dataProcWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit dataProcWindow(QWidget *parent = 0);
    ~dataProcWindow();

private:
    Ui::dataProcWindow *ui;

private slots:
    void updateVehicleList();

};

#endif // DATAPROCWINDOW_H
