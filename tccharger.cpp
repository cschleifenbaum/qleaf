#include "tccharger.h"

#include <QTimer>

namespace
{
quint32 matchSendingFrameId(quint32 frameIdReceiving)
{
    if (frameIdReceiving == 0x18ff50e7)      // protocol 998
	    return 0x1806e7f4;
    else if (frameIdReceiving == 0x18ff50e5) // protocol 1430
        return 0x1806e5f4;
    return 0x0;
}
}

TcCharger::TcCharger(QCanBusDevice* canBusDevice, quint32 frameIdReceiving, QObject* parent)
    : CanBusNode(canBusDevice, matchSendingFrameId(frameIdReceiving), frameIdReceiving, parent)
{
    QTimer* t = new QTimer(this);
    connect(t, &QTimer::timeout, this, &TcCharger::prepareAndSendFrame);
    t->start(1000);
}

double TcCharger::outputVoltage() const
{
    return m_outputVoltage / 10.0;
}

double TcCharger::outputCurrent() const
{
    return m_outputCurrent / 10.0;
}

double TcCharger::maxOutputVoltage() const
{
    return m_maxOutputVoltage / 10.0;
}

void TcCharger::setMaxOutputVoltage(double voltage)
{
    quint16 max = voltage * 10;
    if (max == m_maxOutputVoltage)
        return;
    m_maxOutputVoltage = max;
    Q_EMIT maxOutputVoltageChanged(maxOutputVoltage());
}

double TcCharger::maxOutputCurrent() const
{
    return m_maxOutputCurrent / 10.;
}

void TcCharger::setMaxOutputCurrent(double current)
{
    quint16 max = current * 10;
    if (max == m_maxOutputCurrent)
        return;
    m_maxOutputCurrent = max;
    Q_EMIT maxOutputCurrentChanged(maxOutputCurrent());
}

void TcCharger::prepareAndSendFrame()
{
    auto highByte = [](quint16 data) -> quint8 {
        return (data >> 8) & 0xff;
    };
    auto lowByte = [](quint16 data) -> quint8 {
        return data & 0xff;
    };
    QByteArray data(8, '\0');
    data[0] = highByte(m_maxOutputVoltage);
    data[1] = lowByte(m_maxOutputVoltage);
    data[2] = highByte(m_maxOutputCurrent);
    data[3] = lowByte(m_maxOutputCurrent);
    sendFrame(data);
}

void TcCharger::receiveFrame(const QByteArray& data)
{
    quint16 newOutputVoltage = data[0] * 255 + data[1];
    quint16 newOutputCurrent = data[2] * 255 + data[3];
    if (newOutputVoltage != m_outputVoltage)
    {
        m_outputVoltage = newOutputVoltage;
        Q_EMIT outputVoltageChanged(outputVoltage());
    }
    if (newOutputCurrent != m_outputCurrent)
    {
        m_outputCurrent = newOutputCurrent;
        Q_EMIT outputCurrentChanged(outputCurrent());
    }
}
