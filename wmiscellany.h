#ifndef WMISCELLANY_H
#define WMISCELLANY_H

#include <QByteArray>

inline QByteArray &operator <<(QByteArray &QBArr, const quint8 Dat)
{
    QBArr.append(Dat);
    return QBArr;
}

inline QByteArray &operator <<(QByteArray &QBArr, const quint16 Dat)
{
    return QBArr<<quint8(Dat>>8)<<quint8(Dat);
}

inline QByteArray &operator <<(QByteArray &QBArr, const quint32 Dat)
{
    return QBArr<<quint16(Dat>>16)<<quint16(Dat);
}

inline QByteArray &operator <<(QByteArray &QBArr, const quint64 Dat)
{
    return QBArr<<quint32(Dat>>32)<<quint32(Dat);
}

inline QByteArray &operator <<(QByteArray &QBArr, const QByteArray &Dat)
{
    QBArr.append(Dat);
    return QBArr;
}

#endif // WMISCELLANY_H
