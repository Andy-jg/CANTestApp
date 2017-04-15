#ifndef CANPROTOCOL_H
#define CANPROTOCOL_H

#include "wadbgconf.h"
#include <QCanBusFrame>
#include <QByteArray>

class CanProtocol : public virtual QCanBusFrame
{
public:
    explicit CanProtocol():QCanBusFrame(){}
    explicit CanProtocol(quint8 identifier, const QByteArray &data)
                :QCanBusFrame(identifier, data){}

    CanProtocol &SetSdcsId(quint8 sdcsid);
    CanProtocol &SetChId(quint8 chid);
    CanProtocol &SetRFID(QByteArray rfid);
    CanProtocol &SetValveOn();
    CanProtocol &SetValveOff();

    quint8 GetSdcsId() const {
        return (frameId()&0xf0)>>4;
    }

    quint8 GetChId() const {
        return frameId()&0x0f;
    }

    const QByteArray &GetRFID() {
        return "";
    }
};

#endif // CANPROTOCOL_H
