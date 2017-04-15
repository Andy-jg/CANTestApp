#include "mainwindow.h"
#include <QApplication>
#include <QDebug>
#include "wadbgconf.h"
#include "canprotocol.h"
#include "wmiscellany.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}
