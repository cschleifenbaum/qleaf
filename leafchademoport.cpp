#include "leafchademoport.h"

#include <QDebug>

LeafChademoPort::LeafChademoPort(QCanBusDevice* canBusDevice, quint32 frameId, QObject* parent)
    : CanBusNode(canBusDevice, 0, frameId, parent)
{
    qDebug() << "Adding Leaf Chademo port";
}

void LeafChademoPort::receiveFrame(quint32 frameId, const QByteArray& data)
{
}
    
QVector<quint32> LeafChademoPort::receivingFrameIds() const
{
    return { 0x100, 0x101, 0x103, 0x200 };
}
