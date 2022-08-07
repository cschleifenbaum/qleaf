#include "canbusnode.h"

#include <QCanBusDevice>

CanBusNode::CanBusNode(QCanBusDevice* canBusDevice, quint32 frameIdSending, quint32 frameIdReceiving, QObject* parent)
    : QObject(parent),
      m_canBusDevice(canBusDevice),
      m_frameIdSending(frameIdSending),
      m_frameIdReceiving(frameIdReceiving)
{
}

void CanBusNode::sendFrame(const QByteArray& data)
{
    QCanBusFrame frame(m_frameIdSending, data);
    m_canBusDevice->writeFrame(frame);
}

void CanBusNode::receiveFrame(const QByteArray& data)
{
    Q_UNUSED(data);
}

void CanBusNode::receiveFrame(quint32 frameId, const QByteArray& data)
{
    Q_UNUSED(frameId);
    Q_UNUSED(data);
}
    
QVector<quint32> CanBusNode::receivingFrameIds() const
{
    return { m_frameIdReceiving };
}
