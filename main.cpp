#include "mainwindow.h"
#include <QApplication>
#include <QDebug>
#include "wadbgconf.h"
#include "canprotocol.h"
#include "wmiscellany.h"
#include <QTextStream>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    dbg("=======PROGRAM STARTED=======");
    w.show();

    return a.exec();
}
