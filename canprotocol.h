#ifndef CANPROTOCOL_H
#define CANPROTOCOL_H
#define CanPtcDbgEn 1

#include "wadbgconf.h"
#include "wmiscellany.h"
#include <QCanBusFrame>
#include <QByteArray>

class CanProtocol : public QCanBusFrame
{
public:

    explicit CanProtocol();
    explicit CanProtocol(quint8 identifier, const QByteArray &data);
    explicit CanProtocol(const QCanBusFrame &CanMsg);

    void operator=(const QCanBusFrame &CanMsg);

    bool IsOfRightFormat() const;

    CanProtocol &SetSdcsId(quint8 sdcsid);
    CanProtocol &SetChId(quint8 chid);
    CanProtocol &SetRFID(const QByteArray &rfid);
    CanProtocol &SetValveControl(bool SetBit, bool ResetBit);
    CanProtocol &SetValveOn();
    CanProtocol &SetValveOff();

    const QCanBusFrame &GetMsg() const;
    const QString GetMsgStr() const;

    quint8 GetSdcsId() const;
    quint8 GetChId() const;
    const QByteArray GetRFID() const;
    quint8 GetValveControl() const;
    quint8 GetValveStatus() const;

    static const CanProtocol &PresentRequest;
};

#endif // CANPROTOCOL_H
