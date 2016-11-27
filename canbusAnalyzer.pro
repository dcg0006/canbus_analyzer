#-------------------------------------------------
#
# Project created by QtCreator 2016-09-09T22:39:09
#
#-------------------------------------------------

QT       += core gui sql widgets charts

TARGET = canbusAnalyzer
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    newvehiclediag.cpp \
    dataprocwindow.cpp \
    addtomasterdiag.cpp \

HEADERS  += mainwindow.h \
    newvehiclediag.h \
    dataprocwindow.h \
    addtomasterdiag.h \

FORMS    += mainwindow.ui \
    newvehiclediag.ui \
    dataprocwindow.ui \
    addtomasterdiag.ui
