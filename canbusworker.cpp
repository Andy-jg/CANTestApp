#include "canbusworker.h"

CanBusWorker::CanBusWorker(const QString &CanPlugin, const QString &CanItfcName)
    :canPlugin(CanPlugin), canInterfaceName(CanItfcName)
{
    dbgwrp(CanBusDbgEn,
           wdbg("New Object Created")
           dbg("    " _VarTrk(canPlugin))
           dbg("    " _VarTrk(canInterfaceName)));
}

CanBusWorker::~CanBusWorker()
{
    qCanBusDev->disconnect();
    qCanBusDev->disconnectDevice();
    delete qCanBusDev;
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
    stopWork();
    qCanBusDev->disconnect();
    qCanBusDev->disconnectDevice();
    qCanBusDev = nullptr;
    isSignalSlotConnected = false;
}

void CanBusWorker::startWork()
{
    if (isNotStarted)
    {
        if (!isSignalSlotConnected)
        {
            if (!isDevPickedUp()){
                dbgwrp(CanBusDbgEn,
                       wdbg("KO Device Not Obtained"));
                return;
            }
            connectSIGNALSLOT();
        }
        isNotStarted = false;
        isWorking = true;
        isWritable = true;
        doWork();
    }
    else{
        dbgwrp(CanBusDbgEn,
               wdbg("Worker ALREADY Started"));
        return;
    }
}

void CanBusWorker::doWork()
{
    if (!isNotStarted)
    {
        dbgwrp(CanBusDbgEn,
               wdbg("Worker Started"));
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
               wdbg("Worker Paused"));
    }
    else
    {
        dbgwrp(CanBusDbgEn,
               wdbg("Worker Not Started"));
    }
}

void CanBusWorker::stopWork()
{
    pauseWork();
    isWritable = false;
    clearBufferSend();
    isNotStarted = true;
    dbgwrp(CanBusDbgEn,
           wdbg(_VarTrk(isNotStarted)));
}

void CanBusWorker::pauseWork()
{
    isWorking = false;
    dbgwrp(CanBusDbgEn,
           wdbg(_VarTrk(isWorking)));
}

void CanBusWorker::resumeWork()
{
    isWorking = true;
    dbgwrp(CanBusDbgEn,
           wdbg(_VarTrk(isWorking)));
    doWork();
}

void CanBusWorker::addToBufferSend(const CanProtocol &CanPtc)
{
    bufferSend.append(&CanPtc);
    dbgwrp(CanBusDbgEn,
           wdbg("BufferSend Added")
           dbg("    " _VarTrk(bufferSend.last()->GetMsgStr())));
}

void CanBusWorker::addToBufferSend4Debug(const CanProtocol &CanPtc)
{
    bufferSend.append(&CanPtc);
    dbgwrp(CanBusDbgEn,
           wdbg("BufferSend Added")
           dbg("    " _VarTrk(bufferSend.last()->GetMsgStr())));
    emit bufferSendCount(bufferSend.size());
}

void CanBusWorker::addToBufferRead()
{
    bufferRead.append(new CanProtocol(qCanBusDev->readFrame()));
    dbgwrp(CanBusDbgEn,
           wdbg("BufferRead Added")
           dbg("    " _VarTrk(bufferRead.last()->GetMsgStr()))
           dbg("    readReady() emitted"));
    emit readReady();
}

void CanBusWorker::clearBufferSend()
{
    bufferSend = AListOfCanProtocol();
    dbgwrp(CanBusDbgEn,
           wdbg("BufferSend Cleared"));
}

void CanBusWorker::getWriteTicket()
{
    isWritable = true;
    emit writeOK();
    dbgwrp(CanBusDbgEn,
           wdbg(_VarTrk(isWritable)));
}

//void CanBusWorker::QCanBusDeviceStateChanged(QCanBusDevice::CanBusDeviceState CanBusDevState)
//{
//    dbgwrp(CanBusDbgEn,
//           wdbg("CHANGE")
//           dbg("    " _VarTrk(CanBusDevState)));
//}

//void CanBusWorker::QCanBusDeviceErrorOccurred(QCanBusDevice::CanBusError CanBusDevError)
//{
//    dbgwrp(CanBusDbgEn,
//           wdbg("KO")
//           dbg("    " _VarTrk(CanBusDevError)));
//}

void CanBusWorker::aMsgOutRequest()
{
    dbgwrp(CanBusDbgEn,
           wdbg("A Message Emitted")
           dbg("    " _VarTrk(bufferRead.first()->GetMsgStr())));
    emit replyAMsgOutRequest4Debug(bufferRead.first()->GetMsgStr());
    emit replyAMsgOutRequest(*bufferRead.takeFirst());
}

void CanBusWorker::allMsgOutRequest()
{
    emit replyAllMsgOutRequest(bufferRead);
    dbgwrp(CanBusDbgEn,
           wdbg("ALL Message Emitted")
           dbg("    " _VarTrk(bufferRead.first()->GetMsgStr())));
    bufferRead = AListOfCanProtocol();
}

void CanBusWorker::countBufferRead()
{
    emit bufferReadCount(bufferRead.size());
    dbgwrp(CanBusDbgEn,
           wdbg("bufferReadCount Emitted"));
}

void CanBusWorker::countBufferSend()
{
    emit bufferSendCount(bufferSend.size());
    dbgwrp(CanBusDbgEn,
           wdbg("bufferSendCount Emitted"));
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
    if (isSignalSlotConnected)
    {
        dbgwrp(CanBusDbgEn,
               wdbg("SignalSlot ALREADY Connected"));
        return;
    }
    connect(qCanBusDev,SIGNAL(framesReceived()),this,SLOT(addToBufferRead()));
    connect(qCanBusDev,SIGNAL(framesWritten(qint64)),this,SLOT(getWriteTicket()));
    connect(qCanBusDev,SIGNAL(stateChanged(QCanBusDevice::CanBusDeviceState))
            ,this,SLOT(whenQCanBusDeviceStateChanged(QCanBusDevice::CanBusDeviceState)));
    connect(qCanBusDev,SIGNAL(errorOccurred(QCanBusDevice::CanBusError))
            ,this,SLOT(getWriteTicket()));
    isSignalSlotConnected = true;
    dbgwrp(CanBusDbgEn,
           wdbg("SignalSlot Connected"));
}

CanBusWorker::CanBusWorker()
{
    dbgwrp(CanBusDbgEn,
           wdbg("New Object Created")
           dbg("    " _VarTrk(canPlugin))
           dbg("    " _VarTrk(canInterfaceName)));
}
