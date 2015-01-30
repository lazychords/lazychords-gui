#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include "mainwindow.h"
#include <iostream>
#include <QIcon>

int main(int argc, char *argv[])
{
    try
    {
    QApplication a(argc, argv);
    MainWindow w;
    QIcon icon(":/icons/lazyy.ico");
    w.setWindowTitle("LazyChords");
    w.setWindowIcon(icon);
    w.show();
    
    return a.exec();
    }
    catch(const std::exception& e)
    {
        std::cout<<"EXCEPTION : "<<e.what()<<"\n";
    }
}
