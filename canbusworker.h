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
    bool isWorking = false;
    bool isWritable = true;

    static bool IsCanPluginSupported(const QString &CanPlugin);
    void connectSIGNALSLOT();
public:
    CanBusWorker(const QString &CanPlugin, const QString &CanItfcName);
    ~CanBusWorker();
    const QString &GetCanPlugin()const{ return canPlugin;}
    const QString &GetCanInterfaceName()const{ return canInterfaceName;}
public slots:
    void ReConfigCanBusDevice(const QString &CanPlugin, const QString &CanItfcName);
    void doWork();
    void stopWork();
    void addToBufferSend(const CanProtocol &CanPtc);
    void addToBufferRead();
    void getWriteTicket();
    void QCanBusDeviceStateChanged(QCanBusDevice::CanBusDeviceState CanBusDevState);
    void QCanBusDeviceErrorOccurred(QCanBusDevice::CanBusError CanBusDevError);

    void aMsgOutRequest();
    void allMsgOutRequest();
signals:
    void canBusWorkerStopped();
    void replyAMsgOutRequest(const CanProtocol &);
    void replyAllMsgOutRequest(const AListOfCanProtocol &);
};

#endif // CANBUSWORKER_H

