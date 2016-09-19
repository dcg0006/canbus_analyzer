#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;

    w.setName("Enter Desired ID");
    w.setWindowTitle("Canbus Analyzer");

    w.show();

    return a.exec();
}
