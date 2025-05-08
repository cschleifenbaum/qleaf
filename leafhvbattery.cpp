#include "leafhvbattery.h"

#include "openinverter/params.h"

#include <QDebug>

namespace
{
double readSignedField(const QByteArray& data, int bitpos, int length, double factor = 1.0, int offset = 0)
{
    qint32 result = 0;

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

    bool msbit = result & (1 << (bitsread - 1));
    if (msbit) {
        for (int i = bitsread; i < 32; ++i)
            result |= (1 << i);
    }

    return result * factor + offset;
}
double readField(const QByteArray& data, int bitpos, int length, double factor = 1.0, int offset = 0)
{
    qint32 result = 0;

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

LeafHVBattery::LeafHVBattery(QCanBusDevice* canBusDevice, quint32 frameId, QObject* parent)
    : CanBusNode(canBusDevice, 0, frameId, parent)
{
    qDebug() << "Adding Leaf HV battery";
    Param::SetInt(Param::BattCap, 40080);
    Param::SetInt(Param::Voltspnt, 435);
}

quint32 LeafHVBattery::dischargePowerLimit() const
{
    return m_dischargePowerLimit;
}

quint32 LeafHVBattery::chargePowerLimit() const
{
    return m_chargePowerLimit;
}

qint32 LeafHVBattery::maxPowerForCharger() const
{
    return m_maxPowerForCharger;
}

double LeafHVBattery::voltage() const
{
    return m_voltage;
}

double LeafHVBattery::current() const
{
    return m_current;
}

double LeafHVBattery::stateOfCharge() const
{
    return m_stateOfCharge;
}

void LeafHVBattery::receiveFrame(quint32 frameId, const QByteArray& data)
{
    bool changedValue = false;
    switch (frameId)
    {
    case 0x1db:
    {
        double stateOfCharge = data[4];
        if (m_stateOfCharge != stateOfCharge)
        {
            m_stateOfCharge = stateOfCharge;
            Param::SetInt(Param::SOC, stateOfCharge);
            Param::SetInt(Param::SOCFC, stateOfCharge);
            changedValue = true;
            Q_EMIT stateOfChargeChanged(m_stateOfCharge);
        }
        double voltage = readField(data, 30, 10, 0.5);
        if (voltage != m_voltage)
        {
            m_voltage = voltage;
            changedValue = true;
            Q_EMIT voltageChanged(m_voltage);
        }
        double current = readSignedField(data, 13, 11, 0.5);
        if (current != m_current)
        {
            m_current = current;
            Param::SetInt(Param::idc, current);
            changedValue = true;
            Q_EMIT currentChanged(m_current);
        }
        Param::SetInt(Param::opmode, Param::GetInt(Param::CCS_State) == 0 && current != 0 && !Param::GetBool(Param::PlugDet) ? MOD_RUN : MOD_OFF);
        Param::SetInt(Param::udc, Param::GetBool(Param::PlugDet) || current != 0 ? voltage : 0);
        break;
    }
    case 0x2db:
    {
        double stateOfCharge = data[4];
        double voltage = readField(data, 30, 10, 0.5);
        double current = readSignedField(data, 13, 11, 0.5);
        Param::SetInt(Param::SOC_bat0, stateOfCharge);
        Param::SetInt(Param::idc_bat0, current);
        Param::SetInt(Param::udc_bat0, voltage);
        break;
    }
    case 0x3db:
    {
        double stateOfCharge = data[4];
        double voltage = readField(data, 30, 10, 0.5);
        double current = readSignedField(data, 13, 11, 0.5);
        Param::SetInt(Param::SOC_bat1, stateOfCharge);
        Param::SetInt(Param::idc_bat1, current);
        Param::SetInt(Param::udc_bat1, voltage);
        break;
    }
    case 0x1dc:
    {
        quint32 dischargePowerLimit = readField(data, 14, 10, 250);
        if (dischargePowerLimit != m_dischargePowerLimit)
        {
            m_dischargePowerLimit = dischargePowerLimit;
            changedValue = true;
            Q_EMIT dischargePowerLimitChanged(m_dischargePowerLimit);
        }
        quint32 chargePowerLimit = readField(data, 20, 10, 250);
        if (chargePowerLimit != m_chargePowerLimit)
        {
            m_chargePowerLimit = chargePowerLimit;
            changedValue = true;
            Q_EMIT chargePowerLimitChanged(m_chargePowerLimit);
        }
        qint32 maxPowerForCharger = readField(data, 26, 10, 100, -10000);
        if (maxPowerForCharger != m_maxPowerForCharger)
        {
            m_maxPowerForCharger = maxPowerForCharger;
            changedValue = true;
            Q_EMIT maxPowerForChargerChanged(m_maxPowerForCharger);
            Param::SetInt(Param::idc_max, m_maxPowerForCharger / m_voltage);
        }
        int chademoLimit = Param::GetInt(Param::CHAdeMO_Ireq) > 0 ? Param::GetInt(Param::CHAdeMO_Ireq) : 125;
        chademoLimit = std::min(10, chademoLimit);
        Param::SetInt(Param::CCS_ILim, std::min<double>(m_maxPowerForCharger / m_voltage, chademoLimit));
        break;
    }
    case 0x2dc:
    {
        qint32 maxPowerForCharger = readField(data, 26, 10, 100, -10000);
        Param::SetInt(Param::idc_max_bat0, maxPowerForCharger / m_voltage);
        break;
    }
    case 0x3dc:
    {
        qint32 maxPowerForCharger = readField(data, 26, 10, 100, -10000);
        Param::SetInt(Param::idc_max_bat1, maxPowerForCharger / m_voltage);
        break;
    }
    }
    if (changedValue)
        Q_EMIT changed();
}
    
QVector<quint32> LeafHVBattery::receivingFrameIds() const
{
    return { 0x1dc, 0x2dc, 0x3dc, 0x1db, 0x2db, 0x3db, 0x55b };
}
