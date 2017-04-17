#include "canbusworker.h"

CanBusWorker::CanBusWorker(const QString &CanPlugin, const QString &CanItfcName)
    :canPlugin(CanPlugin), canInterfaceName(CanItfcName)
{
    anDebugWrap(CanBusDbgEn,
           anqDebug("New Object Created")
           dbg("    " _VarTrk(canPlugin))
           dbg("    " _VarTrk(canInterfaceName)));
}

CanBusWorker::~CanBusWorker()
{
    qCanBusDev->disconnect();
    qCanBusDev->disconnectDevice();
    delete qCanBusDev;
    anDebugWrap(CanBusDbgEn,
           anqDebug("Destructor Called"));
}

void CanBusWorker::ReConfigCanBusDevice(const QString &CanPlugin, const QString &CanItfcName)
{
    canPlugin = CanPlugin;
    canInterfaceName = CanItfcName;
    anDebugWrap(CanBusDbgEn,
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
                anDebugWrap(CanBusDbgEn,
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
        anDebugWrap(CanBusDbgEn,
               anqDebug("Worker ALREADY Started"));
        return;
    }
}

void CanBusWorker::doWork()
{
    if (!isNotStarted)
    {
        anDebugWrap(CanBusDbgEn,
               anqDebug("Worker Started"));
        while (isWorking)
        {
            qApp->processEvents();
            while (qCanBusDev->framesAvailable() != 0){
                CanProtocol *CanPtcTmp = new CanProtocol(qCanBusDev->readFrame());
                bufferRead.append(CanPtcTmp);
                anDebugWrap(CanBusDbgEn,
                       anqDebug("BufferRead Added In While Loop")
                       dbg("    " _VarTrk(bufferRead.last()->GetMsgStr())));
            }
            if (isWritable && (!bufferSend.isEmpty())){
                anDebugWrap(CanBusDbgEn,
                       anqDebug("A CanProtocol Sent")
                       dbg("    MsgSent="+bufferSend.first()->GetMsgStr()));
                qCanBusDev->writeFrame(bufferSend.takeFirst()->GetMsg());
                isWritable = false;
                anDebugWrap(CanBusDbgEn,
                       anDebugWrap(!bufferSend.isEmpty(),
                       dbg("    NextMsg="+bufferSend.at(0)->GetMsgStr())));
            }
        }
        anDebugWrap(CanBusDbgEn,
               anqDebug("Worker Paused"));
    }
    else
    {
        anDebugWrap(CanBusDbgEn,
               anqDebug("Worker Not Started"));
    }
}

void CanBusWorker::stopWork()
{
    pauseWork();
    isWritable = false;
    clearBufferSend();
    isNotStarted = true;
    anDebugWrap(CanBusDbgEn,
           anqDebug(_VarTrk(isNotStarted)));
}

void CanBusWorker::pauseWork()
{
    isWorking = false;
    anDebugWrap(CanBusDbgEn,
           anqDebug(_VarTrk(isWorking)));
}

void CanBusWorker::resumeWork()
{
    isWorking = true;
    anDebugWrap(CanBusDbgEn,
           anqDebug(_VarTrk(isWorking)));
    doWork();
}

void CanBusWorker::addToBufferSend(const CanProtocol &CanPtc)
{
    bufferSend.append(&CanPtc);
    anDebugWrap(CanBusDbgEn,
           anqDebug("BufferSend Added")
           dbg("    " _VarTrk(bufferSend.last()->GetMsgStr())));
}

void CanBusWorker::addToBufferSend4Debug(const CanProtocol &CanPtc)
{
    bufferSend.append(&CanPtc);
    anDebugWrap(CanBusDbgEn,
           anqDebug("BufferSend Added")
           dbg("    " _VarTrk(bufferSend.last()->GetMsgStr())));
    emit bufferSendCount(bufferSend.size());
}

void CanBusWorker::addToBufferRead()
{
    bufferRead.append(new CanProtocol(qCanBusDev->readFrame()));
    anDebugWrap(CanBusDbgEn,
           anqDebug("BufferRead Added")
           dbg("    " _VarTrk(bufferRead.last()->GetMsgStr()))
           dbg("    readReady() emitted"));
    emit readReady();
}

void CanBusWorker::clearBufferSend()
{
    bufferSend = AListOfCanProtocol();
    anDebugWrap(CanBusDbgEn,
           anqDebug("BufferSend Cleared"));
}

void CanBusWorker::getWriteTicket()
{
    isWritable = true;
    emit writeOK();
    anDebugWrap(CanBusDbgEn,
           anqDebug(_VarTrk(isWritable)));
}

//void CanBusWorker::QCanBusDeviceStateChanged(QCanBusDevice::CanBusDeviceState CanBusDevState)
//{
//    anDebugWrap(CanBusDbgEn,
//           anqDebug("CHANGE")
//           dbg("    " _VarTrk(CanBusDevState)));
//}

//void CanBusWorker::QCanBusDeviceErrorOccurred(QCanBusDevice::CanBusError CanBusDevError)
//{
//    anDebugWrap(CanBusDbgEn,
//           anqDebug("KO")
//           dbg("    " _VarTrk(CanBusDevError)));
//}

void CanBusWorker::aMsgOutRequest()
{
    anDebugWrap(CanBusDbgEn,
           anqDebug("A Message Emitted")
           dbg("    " _VarTrk(bufferRead.first()->GetMsgStr())));
    emit replyAMsgOutRequest4Debug(bufferRead.first()->GetMsgStr());
    emit replyAMsgOutRequest(*bufferRead.takeFirst());
}

void CanBusWorker::allMsgOutRequest()
{
    emit replyAllMsgOutRequest(bufferRead);
    anDebugWrap(CanBusDbgEn,
           anqDebug("ALL Message Emitted")
           dbg("    " _VarTrk(bufferRead.first()->GetMsgStr())));
    bufferRead = AListOfCanProtocol();
}

void CanBusWorker::countBufferRead()
{
    emit bufferReadCount(bufferRead.size());
    anDebugWrap(CanBusDbgEn,
           anqDebug("bufferReadCount Emitted"));
}

void CanBusWorker::countBufferSend()
{
    emit bufferSendCount(bufferSend.size());
    anDebugWrap(CanBusDbgEn,
           anqDebug("bufferSendCount Emitted"));
}

bool CanBusWorker::isDevPickedUp()
{
    if (!IsCanPluginSupported(canPlugin)){
        anDebugWrap(CanBusDbgEn,
               anqDebug("Plugin Not Supported")
               dbg("    " _VarTrk(canPlugin)));
        return false;
    }
    QString *QStrTmp = new QString();
    qCanBusDev = QCanBus::instance()->createDevice(canPlugin,canInterfaceName,QStrTmp);
    anDebugWrap(CanBusDbgEn,
           anAssert(QStrTmp->size()!=0,QStrTmp));
    if (!qCanBusDev)     //alternatively: if (QStrTmp->size()!=0)
        return false;
    qCanBusDev->setConfigurationParameter(QCanBusDevice::LoopbackKey,QVariant("false"));
    if (!(qCanBusDev->connectDevice())){
        anDebugWrap(CanBusDbgEn,
               anqDebug("Device Not Connected"));
        return false;
    }
    anDebugWrap(CanBusDbgEn,anqDebug("OK Device Obtained"));
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
        anDebugWrap(CanBusDbgEn,
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
    anDebugWrap(CanBusDbgEn,
           anqDebug("SignalSlot Connected"));
}

CanBusWorker::CanBusWorker()
{
    anDebugWrap(CanBusDbgEn,
           anqDebug("New Object Created")
           dbg("    " _VarTrk(canPlugin))
           dbg("    " _VarTrk(canInterfaceName)));
}
