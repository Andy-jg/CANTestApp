#include "mainwindow.h"
#include <QApplication>
#include <QDebug>
#include "wadbgconf.h"
#include "canprotocol.h"

int main(int argc, char *argv[])
{
    int a = 0b1101101110010;
    a &=0xf0;
    qDebug() << (a>>4);
//    QApplication a(argc, argv);
//    MainWindow w;
//    w.show();

//    return a.exec();
}
