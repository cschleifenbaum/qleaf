#include "leafobcharger.h"

namespace
{
double readField(const QByteArray& data, int bitpos, int length, double factor = 1.0, int offset = 0)
{
    int result = 0;

    int bitsread = 0;

    while (length > 0) {
        int byte = bitpos / 8;
        int localbitpos = bitpos % 8;
        int bitstoread = std::min(length, 8 - localbitpos);
        uchar value = data[byte];
        uchar mask = uchar(0xff << 8 >> bitstoread) >> (8 - bitstoread) << localbitpos;
        int shift = localbitpos - bitsread;
        result += (int(value & mask) >> std::max(0, shift) << std::max(0, -shift));

        bitsread += bitstoread;
        length -= bitstoread;
        bitpos += bitstoread - 16;
    }

    return result * factor + offset;
}
}

LeafOBCharger::LeafOBCharger(QCanBusDevice* canBusDevice, quint32 frameId, QObject* parent)
    : CanBusNode(canBusDevice, 0, frameId, parent)
{
}

quint32 LeafOBCharger::outputPower() const
{
    return m_outputPower;
}

quint32 LeafOBCharger::maxOutputPower() const
{
    return m_maxOutputPower;
}

void LeafOBCharger::receiveFrame(const QByteArray& data)
{
    quint32 maximumPower = readField(data, 48, 9, 100);
    quint32 power = readField(data, 8, 9, 100);
    bool changedValue = false;
    if (maximumPower != m_maxOutputPower)
    {
        m_maxOutputPower = maximumPower;
	changedValue = true;
        Q_EMIT maxOutputPowerChanged(maximumPower);
    }
    if (power != m_outputPower)
    {
        m_outputPower = power;
	changedValue = true;
        Q_EMIT outputPowerChanged(power);
    }
    if (changedValue)
        Q_EMIT changed();
}
