#include "mainwindow.h"
#include "asyncreader.h"
#include "valueinput.h"
#include <QObject>
#include <iostream>

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    ValueInput v;
    if(v.exec() == QDialog::Accepted){
        a.quit();
    }
    MainWindow m;
    m.show();
    return a.exec();
}
