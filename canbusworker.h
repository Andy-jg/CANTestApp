#ifndef CANBUSWORKER_H
#define CANBUSWORKER_H
#define CanBusDbgEn 1

#include "wadbgconf.h"
#include "canprotocol.h"
#include <QCanBus>
#include <QCanBusDevice>
#include <QObject>
#include <QList>
#include <QtSerialPort>

typedef QList<const CanProtocol *> AListOfCanProtocol;

class CanBusWorker: public QObject
{
    Q_OBJECT
    QString canPlugin = "";
    QString canInterfaceName = "";
    QCanBusDevice *qCanBusDev = nullptr;
    AListOfCanProtocol bufferSend;
    AListOfCanProtocol bufferRead;
    bool isDevPickedUp();
    bool isSignalSlotConnected = false;
    bool isNotStarted = true;
    bool isWorking = false;
    bool isWritable = false;

    static bool IsCanPluginSupported(const QString &CanPlugin);
    void doWork();
    void connectSIGNALSLOT();
public:
    CanBusWorker();
    CanBusWorker(const QString &CanPlugin, const QString &CanItfcName);
    ~CanBusWorker();
public slots:
    void ReConfigCanBusDevice(const QString &CanPlugin, const QString &CanItfcName);
    void startWork();
    void stopWork();
    void pauseWork();
    void resumeWork();
    void addToBufferSend(const CanProtocol &CanPtc);

    void addToBufferSend4Debug(const CanProtocol &CanPtc);

    void addToBufferRead();
    void clearBufferSend();
    void getWriteTicket();
//    void QCanBusDeviceStateChanged(QCanBusDevice::CanBusDeviceState CanBusDevState);
//    void QCanBusDeviceErrorOccurred(QCanBusDevice::CanBusError CanBusDevError);

    void aMsgOutRequest();
    void allMsgOutRequest();
    void countBufferRead();
    void countBufferSend();
signals:
    void bufferReadCount(int);
    void bufferSendCount(int);
    void readReady();
    void writeOK();
    void replyAMsgOutRequest(const CanProtocol &);
    void replyAllMsgOutRequest(const AListOfCanProtocol &);

    void replyAMsgOutRequest4Debug(const QString &);

};

#endif // CANBUSWORKER_H

