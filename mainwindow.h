#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "canbusworker.h"
#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_pushButton_Send_clicked();
    void MessageSent();
signals:
    void writeAMessage(const CanProtocol &);
private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
