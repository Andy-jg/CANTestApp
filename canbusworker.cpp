#include "canbusworker.h"

CanBusWorker::CanBusWorker(const QString &CanPlugin, const QString &CanItfcName)
    :canPlugin(CanPlugin), canInterfaceName(CanItfcName)
{
    dbgwrp(CanBusDbgEn,
           wdbg("Property Changed")
           dbg("    " _VarTrk(canPlugin))
           dbg("    " _VarTrk(canInterfaceName)));
}

CanBusWorker::~CanBusWorker()
{
    qCanBusDev->disconnectDevice();
    qCanBusDev->deleteLater();
    dbgwrp(CanBusDbgEn,
           wdbg("Destructor Called"));
}

void CanBusWorker::ReConfigCanBusDevice(const QString &CanPlugin, const QString &CanItfcName)
{
    canPlugin = CanPlugin;
    canInterfaceName = CanItfcName;
    dbgwrp(CanBusDbgEn,
           wdbg("Property Changed")
           dbg("    " _VarTrk(canPlugin))
           dbg("    " _VarTrk(canInterfaceName)));
}

void CanBusWorker::doWork()
{
    if (!isDevPickedUp()){
        dbgwrp(CanBusDbgEn,
               wdbg("KO Device Not Obtained"));
        return;
    }
    connectSIGNALSLOT();
    while (isWorking)
    {
        qApp->processEvents();
        while (qCanBusDev->framesAvailable() != 0){
            CanProtocol *CanPtcTmp = new CanProtocol(qCanBusDev->readFrame());
            bufferRead.append(CanPtcTmp);
            dbgwrp(CanBusDbgEn,
                   wdbg("BufferRead Added In While Loop")
                   dbg("    " _VarTrk(bufferRead.last()->GetMsgStr())));
        }
        if (isWritable && (!bufferSend.isEmpty())){
            dbgwrp(CanBusDbgEn,
                   wdbg("A CanProtocol Sent")
                   dbg("    MsgSent="+bufferSend.first()->GetMsgStr()));
            qCanBusDev->writeFrame(bufferSend.takeFirst()->GetMsg());
            isWritable = false;
            dbgwrp(CanBusDbgEn,
                   dbgwrp(!bufferSend.isEmpty(),
                   dbg("    NextMsg="+bufferSend.at(0)->GetMsgStr())));
        }
    }
    dbgwrp(CanBusDbgEn,
           wdbg("Work Stopped"));
    emit canBusWorkerStopped();
}

void CanBusWorker::stopWork()
{
    isWorking = false;
    dbgwrp(CanBusDbgEn,
           wdbg(_VarTrk(isWorking)));
}

void CanBusWorker::addToBufferSend(const CanProtocol &CanPtc)
{
    bufferSend.append(&CanPtc);
    dbgwrp(CanBusDbgEn,
           wdbg("BufferSend Added")
           dbg("    " _VarTrk(bufferSend.last()->GetMsgStr())));
}

void CanBusWorker::addToBufferRead()
{
    bufferRead.append(new CanProtocol(qCanBusDev->readFrame()));
    dbgwrp(CanBusDbgEn,
           wdbg("BufferRead Added")
           dbg("    " _VarTrk(bufferRead.last()->GetMsgStr())));
}

void CanBusWorker::getWriteTicket()
{
    dbgwrp(CanBusDbgEn,
           wdbg(_VarTrk(isWritable)));
    isWritable = true;
}

void CanBusWorker::QCanBusDeviceStateChanged(QCanBusDevice::CanBusDeviceState CanBusDevState)
{
    dbgwrp(CanBusDbgEn,
           wdbg("CHANGE")
           dbg("    " _VarTrk(CanBusDevState)));
}

void CanBusWorker::QCanBusDeviceErrorOccurred(QCanBusDevice::CanBusError CanBusDevError)
{
    dbgwrp(CanBusDbgEn,
           wdbg("KO")
           dbg("    " _VarTrk(CanBusDevError)));
}

void CanBusWorker::aMsgOutRequest()
{
    dbgwrp(CanBusDbgEn,
           wdbg("A Message Emitted")
           dbg("    " _VarTrk(bufferRead.first()->GetMsgStr())));
    emit replyAMsgOutRequest(*bufferRead.takeFirst());
}

void CanBusWorker::allMsgOutRequest()
{
    emit replyAllMsgOutRequest(bufferRead);
    dbgwrp(CanBusDbgEn,
           wdbg("ALL Message Emitted")
           dbg("    " _VarTrk(bufferRead.first()->GetMsgStr())));
    bufferRead = * new AListOfCanProtocol();
}

bool CanBusWorker::isDevPickedUp()
{
    if (!IsCanPluginSupported(canPlugin)){
        dbgwrp(CanBusDbgEn,
               wdbg("Plugin Not Supported")
               dbg("    " _VarTrk(canPlugin)));
        return false;
    }
    QString *QStrTmp = new QString();
    qCanBusDev = QCanBus::instance()->createDevice(canPlugin,canInterfaceName,QStrTmp);
    dbgwrp(CanBusDbgEn,
           adbg(QStrTmp->size()!=0,QStrTmp));
    if (!qCanBusDev)     //alternatively: if (QStrTmp->size()!=0)
        return false;
    qCanBusDev->setConfigurationParameter(QCanBusDevice::LoopbackKey,QVariant("false"));
    if (!(qCanBusDev->connectDevice())){
        dbgwrp(CanBusDbgEn,
               wdbg("Device Not Connected"));
        return false;
    }
    dbgwrp(CanBusDbgEn,wdbg("OK Device Obtained"));
    return true;
}

bool CanBusWorker::IsCanPluginSupported(const QString &CanPlugin)
{
    return QCanBus::instance()->plugins().contains(CanPlugin);
}

void CanBusWorker::connectSIGNALSLOT()
{
    connect(qCanBusDev,SIGNAL(framesReceived()),this,SLOT(addToBufferRead()));
    connect(qCanBusDev,SIGNAL(framesWritten(qint64)),this,SLOT(getWriteTicket()));
    connect(qCanBusDev,SIGNAL(stateChanged(QCanBusDevice::CanBusDeviceState))
            ,this,SLOT(whenQCanBusDeviceStateChanged(QCanBusDevice::CanBusDeviceState)));
    connect(qCanBusDev,SIGNAL(errorOccurred(QCanBusDevice::CanBusError))
            ,this,SLOT(getWriteTicket()));
}
