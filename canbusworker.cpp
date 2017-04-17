#include "canbusworker.h"

CanBusWorker::CanBusWorker(const QString &CanPlugin, const QString &CanItfcName)
    :canPlugin(CanPlugin), canInterfaceName(CanItfcName)
{
    dbgwrp(CanBusDbgEn,
           anqDebug("New Object Created")
           dbg("    " _VarTrk(canPlugin))
           dbg("    " _VarTrk(canInterfaceName)));
}

CanBusWorker::~CanBusWorker()
{
    qCanBusDev->disconnect();
    qCanBusDev->disconnectDevice();
    delete qCanBusDev;
    dbgwrp(CanBusDbgEn,
           anqDebug("Destructor Called"));
}

void CanBusWorker::ReConfigCanBusDevice(const QString &CanPlugin, const QString &CanItfcName)
{
    canPlugin = CanPlugin;
    canInterfaceName = CanItfcName;
    dbgwrp(CanBusDbgEn,
           anqDebug("Property Changed")
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
                       anqDebug("KO Device Not Obtained"));
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
               anqDebug("Worker ALREADY Started"));
        return;
    }
}

void CanBusWorker::doWork()
{
    if (!isNotStarted)
    {
        dbgwrp(CanBusDbgEn,
               anqDebug("Worker Started"));
        while (isWorking)
        {
            qApp->processEvents();
            while (qCanBusDev->framesAvailable() != 0){
                CanProtocol *CanPtcTmp = new CanProtocol(qCanBusDev->readFrame());
                bufferRead.append(CanPtcTmp);
                dbgwrp(CanBusDbgEn,
                       anqDebug("BufferRead Added In While Loop")
                       dbg("    " _VarTrk(bufferRead.last()->GetMsgStr())));
            }
            if (isWritable && (!bufferSend.isEmpty())){
                dbgwrp(CanBusDbgEn,
                       anqDebug("A CanProtocol Sent")
                       dbg("    MsgSent="+bufferSend.first()->GetMsgStr()));
                qCanBusDev->writeFrame(bufferSend.takeFirst()->GetMsg());
                isWritable = false;
                dbgwrp(CanBusDbgEn,
                       dbgwrp(!bufferSend.isEmpty(),
                       dbg("    NextMsg="+bufferSend.at(0)->GetMsgStr())));
            }
        }
        dbgwrp(CanBusDbgEn,
               anqDebug("Worker Paused"));
    }
    else
    {
        dbgwrp(CanBusDbgEn,
               anqDebug("Worker Not Started"));
    }
}

void CanBusWorker::stopWork()
{
    pauseWork();
    isWritable = false;
    clearBufferSend();
    isNotStarted = true;
    dbgwrp(CanBusDbgEn,
           anqDebug(_VarTrk(isNotStarted)));
}

void CanBusWorker::pauseWork()
{
    isWorking = false;
    dbgwrp(CanBusDbgEn,
           anqDebug(_VarTrk(isWorking)));
}

void CanBusWorker::resumeWork()
{
    isWorking = true;
    dbgwrp(CanBusDbgEn,
           anqDebug(_VarTrk(isWorking)));
    doWork();
}

void CanBusWorker::addToBufferSend(const CanProtocol &CanPtc)
{
    bufferSend.append(&CanPtc);
    dbgwrp(CanBusDbgEn,
           anqDebug("BufferSend Added")
           dbg("    " _VarTrk(bufferSend.last()->GetMsgStr())));
}

void CanBusWorker::addToBufferSend4Debug(const CanProtocol &CanPtc)
{
    bufferSend.append(&CanPtc);
    dbgwrp(CanBusDbgEn,
           anqDebug("BufferSend Added")
           dbg("    " _VarTrk(bufferSend.last()->GetMsgStr())));
    emit bufferSendCount(bufferSend.size());
}

void CanBusWorker::addToBufferRead()
{
    bufferRead.append(new CanProtocol(qCanBusDev->readFrame()));
    dbgwrp(CanBusDbgEn,
           anqDebug("BufferRead Added")
           dbg("    " _VarTrk(bufferRead.last()->GetMsgStr()))
           dbg("    readReady() emitted"));
    emit readReady();
}

void CanBusWorker::clearBufferSend()
{
    bufferSend = AListOfCanProtocol();
    dbgwrp(CanBusDbgEn,
           anqDebug("BufferSend Cleared"));
}

void CanBusWorker::getWriteTicket()
{
    isWritable = true;
    emit writeOK();
    dbgwrp(CanBusDbgEn,
           anqDebug(_VarTrk(isWritable)));
}

//void CanBusWorker::QCanBusDeviceStateChanged(QCanBusDevice::CanBusDeviceState CanBusDevState)
//{
//    dbgwrp(CanBusDbgEn,
//           anqDebug("CHANGE")
//           dbg("    " _VarTrk(CanBusDevState)));
//}

//void CanBusWorker::QCanBusDeviceErrorOccurred(QCanBusDevice::CanBusError CanBusDevError)
//{
//    dbgwrp(CanBusDbgEn,
//           anqDebug("KO")
//           dbg("    " _VarTrk(CanBusDevError)));
//}

void CanBusWorker::aMsgOutRequest()
{
    dbgwrp(CanBusDbgEn,
           anqDebug("A Message Emitted")
           dbg("    " _VarTrk(bufferRead.first()->GetMsgStr())));
    emit replyAMsgOutRequest4Debug(bufferRead.first()->GetMsgStr());
    emit replyAMsgOutRequest(*bufferRead.takeFirst());
}

void CanBusWorker::allMsgOutRequest()
{
    emit replyAllMsgOutRequest(bufferRead);
    dbgwrp(CanBusDbgEn,
           anqDebug("ALL Message Emitted")
           dbg("    " _VarTrk(bufferRead.first()->GetMsgStr())));
    bufferRead = AListOfCanProtocol();
}

void CanBusWorker::countBufferRead()
{
    emit bufferReadCount(bufferRead.size());
    dbgwrp(CanBusDbgEn,
           anqDebug("bufferReadCount Emitted"));
}

void CanBusWorker::countBufferSend()
{
    emit bufferSendCount(bufferSend.size());
    dbgwrp(CanBusDbgEn,
           anqDebug("bufferSendCount Emitted"));
}

bool CanBusWorker::isDevPickedUp()
{
    if (!IsCanPluginSupported(canPlugin)){
        dbgwrp(CanBusDbgEn,
               anqDebug("Plugin Not Supported")
               dbg("    " _VarTrk(canPlugin)));
        return false;
    }
    QString *QStrTmp = new QString();
    qCanBusDev = QCanBus::instance()->createDevice(canPlugin,canInterfaceName,QStrTmp);
    dbgwrp(CanBusDbgEn,
           anAssert(QStrTmp->size()!=0,QStrTmp));
    if (!qCanBusDev)     //alternatively: if (QStrTmp->size()!=0)
        return false;
    qCanBusDev->setConfigurationParameter(QCanBusDevice::LoopbackKey,QVariant("false"));
    if (!(qCanBusDev->connectDevice())){
        dbgwrp(CanBusDbgEn,
               anqDebug("Device Not Connected"));
        return false;
    }
    dbgwrp(CanBusDbgEn,anqDebug("OK Device Obtained"));
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
               anqDebug("SignalSlot ALREADY Connected"));
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
           anqDebug("SignalSlot Connected"));
}

CanBusWorker::CanBusWorker()
{
    dbgwrp(CanBusDbgEn,
           anqDebug("New Object Created")
           dbg("    " _VarTrk(canPlugin))
           dbg("    " _VarTrk(canInterfaceName)));
}
