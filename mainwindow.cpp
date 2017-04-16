#include "mainwindow.h"
#include "ui_mainwindow.h"

void MainWindow::MessageSent()
{
    ui->textEdit_Send->append("OK Successfully Sent");
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QThread *AThrd = new QThread();
    CanBusWorker *CanBusWkr = new CanBusWorker();
    CanBusWkr->moveToThread(AThrd);

    qRegisterMetaType<CanProtocol>("CanProtocol");

    connect(ui->pushButton_Stop,SIGNAL(clicked(bool))
            ,ui->pushButton_Resume,SLOT(setEnabled(bool)));

    connect(ui->pushButton_Stop,SIGNAL(clicked(bool))
            ,ui->pushButton_Pause,SLOT(setEnabled(bool)));

    connect(ui->pushButton_Start,SIGNAL(clicked(bool))
            ,ui->pushButton_Pause,SLOT(setDisabled(bool)));

    connect(ui->pushButton_Pause,SIGNAL(clicked(bool))
            ,ui->pushButton_Resume,SLOT(setDisabled(bool)));

    connect(ui->pushButton_Pause,SIGNAL(clicked(bool))
            ,ui->pushButton_Pause,SLOT(setEnabled(bool)));

    connect(ui->pushButton_Resume,SIGNAL(clicked(bool))
            ,ui->pushButton_Pause,SLOT(setDisabled(bool)));

    connect(ui->pushButton_Resume,SIGNAL(clicked(bool))
            ,ui->pushButton_Resume,SLOT(setEnabled(bool)));

    connect(ui->pushButton_Stop,SIGNAL(clicked(bool))
            ,ui->pushButton_Start,SLOT(setDisabled(bool)));

    connect(ui->pushButton_Stop,SIGNAL(clicked(bool))
            ,ui->pushButton_Stop,SLOT(setEnabled(bool)));

    connect(ui->pushButton_Start,SIGNAL(clicked(bool))
            ,ui->pushButton_Stop,SLOT(setDisabled(bool)));

    connect(ui->pushButton_Start,SIGNAL(clicked(bool))
            ,ui->pushButton_Start,SLOT(setEnabled(bool)));

    connect(ui->pushButton_clearBufferSend, SIGNAL(clicked())
            ,CanBusWkr,SLOT(clearBufferSend()));

    connect(ui->pushButton_Start,SIGNAL(clicked())
            ,CanBusWkr,SLOT(startWork()));
    connect(ui->pushButton_Stop,SIGNAL(clicked())
            ,CanBusWkr,SLOT(stopWork()));
    connect(ui->pushButton_Resume,SIGNAL(clicked())
            ,CanBusWkr,SLOT(resumeWork()));
    connect(ui->pushButton_Pause,SIGNAL(clicked())
            ,CanBusWkr,SLOT(pauseWork()));


    connect(CanBusWkr,SIGNAL(replyAMsgOutRequest4Debug(QString)),
            ui->textEdit_Read,SLOT(append(QString)));
    connect(CanBusWkr,SIGNAL(readReady()),
            CanBusWkr,SLOT(aMsgOutRequest()));

    connect(CanBusWkr,SIGNAL(bufferReadCount(int)),
            ui->label_bufferReadCount,SLOT(setNum(int)));
    connect(CanBusWkr,SIGNAL(readReady()),
            CanBusWkr,SLOT(countBufferRead()));

    connect(CanBusWkr,SIGNAL(bufferSendCount(int)),
            ui->label_bufferSendCount,SLOT(setNum(int)));
    connect(CanBusWkr,SIGNAL(writeOK()),
            CanBusWkr,SLOT(countBufferSend()));
    connect(CanBusWkr,SIGNAL(writeOK()),
            this,SLOT(MessageSent()));

    connect(this, SIGNAL(writeAMessage(CanProtocol)),
            CanBusWkr,SLOT(addToBufferSend4Debug(CanProtocol)));

    AThrd->start();
}

MainWindow::~MainWindow()
{

    delete ui;
}

void MainWindow::on_pushButton_Send_clicked()
{
    CanProtocol CanPtcTmp(quint8(qrand()%0xff)
                          ,QByteArray::number(qrand()%0xffffffffffffffff));
    ui->textEdit_Send->append(CanPtcTmp.GetMsgStr());
    emit writeAMessage(CanPtcTmp);
}
