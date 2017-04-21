#include "canbusworker.h"

CanBusWorker::CanBusWorker(const QString &CanPlugin, const QString &CanItfcName)
    :canPlugin(CanPlugin), canInterfaceName(CanItfcName)
{
    anDebugWrap(CanBusDbgEn,
           anDebug("New Object Created")
           anqDebug("    " _VarView(canPlugin))
           anqDebug("    " _VarView(canInterfaceName)));
}

CanBusWorker::~CanBusWorker()
{
    qCanBusDev->disconnect();
    qCanBusDev->disconnectDevice();
    delete qCanBusDev;
    anDebugWrap(CanBusDbgEn,
           anDebug("Destructor Called"));
}

void CanBusWorker::ReConfigCanBusDevice(const QString &CanPlugin, const QString &CanItfcName)
{
    canPlugin = CanPlugin;
    canInterfaceName = CanItfcName;
    anDebugWrap(CanBusDbgEn,
           anDebug("Property Changed")
           anqDebug("    " _VarView(canPlugin))
           anqDebug("    " _VarView(canInterfaceName)));
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
                       anDebug("KO Device Not Obtained"));
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
               anDebug("Worker ALREADY Started"));
        return;
    }
}

void CanBusWorker::doWork()
{
    if (!isNotStarted)
    {
        anDebugWrap(CanBusDbgEn,
               anDebug("Worker Started"));
        while (isWorking)
        {
            qApp->processEvents();
            while (qCanBusDev->framesAvailable() != 0){
                CanProtocol *CanPtcTmp = new CanProtocol(qCanBusDev->readFrame());
                bufferRead.append(CanPtcTmp);
                anDebugWrap(CanBusDbgEn,
                       anDebug("BufferRead Added In While Loop")
                       anqDebug("    " _VarView(bufferRead.last()->GetMsgStr())));
            }
            if (isWritable && (!bufferSend.isEmpty())){
                anDebugWrap(CanBusDbgEn,
                       anDebug("A CanProtocol Sent")
                       anqDebug("    MsgSent="+bufferSend.first()->GetMsgStr()));
                qCanBusDev->writeFrame(bufferSend.takeFirst()->GetMsg());
                isWritable = false;
                anDebugWrap(CanBusDbgEn,
                       anDebugWrap(!bufferSend.isEmpty(),
                       anqDebug("    NextMsg="+bufferSend.at(0)->GetMsgStr())));
            }
        }
        anDebugWrap(CanBusDbgEn,
               anDebug("Worker Paused"));
    }
    else
    {
        anDebugWrap(CanBusDbgEn,
               anDebug("Worker Not Started"));
    }
}

void CanBusWorker::stopWork()
{
    pauseWork();
    isWritable = false;
    clearBufferSend();
    isNotStarted = true;
    anDebugWrap(CanBusDbgEn,
           anDebug(_VarView(isNotStarted)));
}

void CanBusWorker::pauseWork()
{
    isWorking = false;
    anDebugWrap(CanBusDbgEn,
           anDebug(_VarView(isWorking)));
}

void CanBusWorker::resumeWork()
{
    isWorking = true;
    anDebugWrap(CanBusDbgEn,
           anDebug(_VarView(isWorking)));
    doWork();
}

void CanBusWorker::addToBufferSend(const CanProtocol &CanPtc)
{
    bufferSend.append(&CanPtc);
    anDebugWrap(CanBusDbgEn,
           anDebug("BufferSend Added")
           anqDebug("    " _VarView(bufferSend.last()->GetMsgStr())));
}

void CanBusWorker::addToBufferSend4Debug(const CanProtocol &CanPtc)
{
    bufferSend.append(&CanPtc);
    anDebugWrap(CanBusDbgEn,
           anDebug("BufferSend Added")
           anqDebug("    " _VarView(bufferSend.last()->GetMsgStr())));
    emit bufferSendCount(bufferSend.size());
}

void CanBusWorker::addToBufferRead()
{
    bufferRead.append(new CanProtocol(qCanBusDev->readFrame()));
    anDebugWrap(CanBusDbgEn,
           anDebug("BufferRead Added")
           anqDebug("    " _VarView(bufferRead.last()->GetMsgStr()))
           anqDebug("    readReady() emitted"));
    emit readReady();
}

void CanBusWorker::clearBufferSend()
{
    bufferSend = AListOfCanProtocol();
    anDebugWrap(CanBusDbgEn,
           anDebug("BufferSend Cleared"));
    emit bufferSendCount(0);
}

void CanBusWorker::getWriteTicket()
{
    isWritable = true;
    emit writeOK();
    anDebugWrap(CanBusDbgEn,
           anDebug(_VarView(isWritable)));
}

//void CanBusWorker::QCanBusDeviceStateChanged(QCanBusDevice::CanBusDeviceState CanBusDevState)
//{
//    anDebugWrap(CanBusDbgEn,
//           anDebug("CHANGE")
//           anqDebug("    " _VarView(CanBusDevState)));
//}

//void CanBusWorker::QCanBusDeviceErrorOccurred(QCanBusDevice::CanBusError CanBusDevError)
//{
//    anDebugWrap(CanBusDbgEn,
//           anDebug("KO")
//           anqDebug("    " _VarView(CanBusDevError)));
//}

void CanBusWorker::aMsgOutRequest()
{
    anDebugWrap(CanBusDbgEn,
           anDebug("A Message Emitted")
           anqDebug("    " _VarView(bufferRead.first()->GetMsgStr())));
    emit replyAMsgOutRequest4Debug(bufferRead.first()->GetMsgStr());
    emit replyAMsgOutRequest(*bufferRead.takeFirst());
}

void CanBusWorker::allMsgOutRequest()
{
    emit replyAllMsgOutRequest(bufferRead);
    anDebugWrap(CanBusDbgEn,
           anDebug("ALL Message Emitted")
           anqDebug("    " _VarView(bufferRead.first()->GetMsgStr())));
    bufferRead = AListOfCanProtocol();
}

void CanBusWorker::countBufferRead()
{
    emit bufferReadCount(bufferRead.size());
    anDebugWrap(CanBusDbgEn,
           anDebug("bufferReadCount Emitted"));
}

void CanBusWorker::countBufferSend()
{
    emit bufferSendCount(bufferSend.size());
    anDebugWrap(CanBusDbgEn,
           anDebug("bufferSendCount Emitted"));
}

bool CanBusWorker::isDevPickedUp()
{
    if (!IsCanPluginSupported(canPlugin)){
        anDebugWrap(CanBusDbgEn,
               anDebug("Plugin Not Supported")
               anqDebug("    " _VarView(canPlugin)));
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
               anDebug("Device Not Connected"));
        return false;
    }
    anDebugWrap(CanBusDbgEn,anDebug("OK Device Obtained"));
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
               anDebug("SignalSlot ALREADY Connected"));
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
           anDebug("SignalSlot Connected"));
}

CanBusWorker::CanBusWorker()
{
    anDebugWrap(CanBusDbgEn,
           anDebug("New Object Created")
           anqDebug("    " _VarView(canPlugin))
           anqDebug("    " _VarView(canInterfaceName)));
}
