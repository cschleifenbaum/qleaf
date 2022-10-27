#include "leafobcharger.h"

#include "openinverter/params.h"

#include <QDebug>

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
    m_fields[0x390] = CanMessageUtils::parseFields(QStringLiteral("SG_ OBC_Status_AC_Voltage : 27|2@1+ (1,0) [0|3] \"status\" Vector__XXX\n"
                                                                  "SG_ OBC_Flag_QC_Relay_On_Announcemen : 38|1@1+ (1,0) [0|1] \"status\" Vector__XXX\n"
                                                                  "SG_ OBC_Flag_QC_IR_Sensor : 47|1@1+ (1,0) [0|1] \"status\" Vector__XXX\n"
                                                                  "SG_ OBC_Maximum_Charge_Power_Out : 40|9@0+ (0.1,0) [0|50] \"kW\" Vector__XXX\n"
                                                                  "SG_ PRUN_390 : 60|2@1+ (1,0) [0|3] \"PRUN\" Vector__XXX\n"
                                                                  "SG_ OBC_Charge_Status : 41|6@1+ (1,0) [0|0] \"status\" Vector__XXX\n"
                                                                  "SG_ CSUM_390 : 56|4@1+ (1,0) [0|16] \"CSUM\" Vector__XXX\n"
                                                                  "SG_ OBC_Charge_Power : 0|9@0+ (0.1,0) [0|50] \"kW\" Vector__XXX"));
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
    Param::SetInt(Param::OBC_Charge_Status, CanMessageUtils::readField(data, m_fields[0x390]["OBC_Charge_Status"]));
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
