#include "canbusnode.h"

#include <QCanBusDevice>
#include <QTimer>

CanBusNode::CanBusNode(QCanBusDevice* canBusDevice, quint32 frameIdSending, quint32 frameIdReceiving, QObject* parent)
    : QObject(parent),
      m_canBusDevice(canBusDevice),
      m_frameIdSending(frameIdSending),
      m_frameIdReceiving(frameIdReceiving),
      m_timeOutTimer(new QTimer(this))
{
    setTimeoutInterval(5000);
    connect(m_timeOutTimer, &QTimer::timeout, this, &CanBusNode::timeout);
}

void CanBusNode::sendFrame(quint32 frameId, const QByteArray& data)
{
    QCanBusFrame frame(frameId, data);
    m_canBusDevice->writeFrame(frame);
}

void CanBusNode::sendFrame(const QByteArray& data)
{
    sendFrame(m_frameIdSending, data);
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

void CanBusNode::receiveFrame(const QCanBusFrame& frame)
{
    m_timeOutTimer->start();
    receiveFrame(frame.payload());
    receiveFrame(frame.frameId(), frame.payload());
}

QVector<quint32> CanBusNode::receivingFrameIds() const
{
    return { m_frameIdReceiving };
}

void CanBusNode::setTimeoutInterval(int interval)
{
    m_timeOutTimer->setInterval(interval);
}
