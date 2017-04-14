#include "mainwindow.h"
#include <QApplication>
#include <QDebug>
#include "wadbgconf.h"

int main(int argc, char *argv[])
{
    adbg(1<10, "this msg should not be printed out");
//    QApplication a(argc, argv);
//    MainWindow w;
//    w.show();

//    return a.exec();
}
