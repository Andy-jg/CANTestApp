#include "mainwindow.h"
#include <QApplication>
#include <QDebug>
#include "andebug.h"
#include "canprotocol.h"
#include "wmiscellany.h"
#include <QTextStream>
#include "assert.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    dbg("=======PROGRAM STARTED=======");
    w.show();

    return a.exec();
}
