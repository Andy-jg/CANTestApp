#ifndef CANPROTOCOL_H
#define CANPROTOCOL_H

#include "wadbgconf.h"
#include "wmiscellany.h"
#include <QCanBusFrame>
#include <QByteArray>

class CanProtocol : public QCanBusFrame
{

public:

    explicit CanProtocol(){}

    explicit CanProtocol(quint8 identifier, const QByteArray &data)
                :QCanBusFrame(  identifier, data){}

    explicit CanProtocol(const QCanBusFrame &CanMsg);

    void operator=(const QCanBusFrame &CanMsg);

    bool IsOfRightFormat() const;

    CanProtocol &SetSdcsId(quint8 sdcsid);
    CanProtocol &SetChId(quint8 chid);
    CanProtocol &SetRFID(const QByteArray &rfid);
    CanProtocol &SetValveControl(bool SetBit, bool ResetBit);
    CanProtocol &SetValveOn();
    CanProtocol &SetValveOff();

    quint8 GetSdcsId() const;
    quint8 GetChId() const;
    const QByteArray GetRFID() const;
    quint8 GetValveControl() const;
    quint8 GetValveStatus() const;
};

#endif // CANPROTOCOL_H
