#include "leafchademoport.h"

#include <QDebug>
#include <QTimer>

#include "openinverter/params.h"

LeafChademoPort::LeafChademoPort(QCanBusDevice* canBusDevice, quint32 frameId, QObject* parent)
    : CanBusNode(canBusDevice, 0, frameId, parent)
{
    qDebug() << "Adding Leaf Chademo port";

    m_fields[0x100] = CanMessageUtils::parseFields(QStringLiteral("SG_ MaximumBatteryVoltage : 39|16@0+ (1,0) [0|600] \"V\" Vector__XXX\n"
                                                                  "SG_ ConstantOfChargingRateIndication : 48|8@1+ (1,0) [0|100] \"%\" Vector__XXX\n"
                                                                  "SG_ MinimumChargeCurrent : 0|8@1+ (1,0) [0|255] \"A\" Vector__XXX"));

    m_fields[0x101] = CanMessageUtils::parseFields(QStringLiteral("SG_ MaxChargingTime10sBit : 8|8@1+ (10,0) [0|2540] \"seconds\" Vector__XXX\n"
                                                                  "SG_ MaxChargingTime1minBit : 16|8@1+ (1,0) [0|255] \"minutes\" Vector__XXX\n"
                                                                  "SG_ EstimatedChargingTime : 24|8@1+ (1,0) [0|254] \"minutes\" Vector__XXX\n"
                                                                  "SG_ RatedBatteryCapacity : 47|16@0+ (0.11,0) [0|100] \"kWh\" Vector__XXX"));

    m_fields[0x102] = CanMessageUtils::parseFields(QStringLiteral("SG_ ControlProtocolNumberEV : 0|8@1+ (1,0) [0|255] \"-\" Vector__XXX\n"
                                                                  "SG_ TargetBatteryVoltage : 15|16@0+ (1,0) [0|600] \"V\" Vector__XXX\n"
                                                                  "SG_ ChargingCurrentRequest : 24|8@1+ (1,0) [0|255] \"A\" Vector__XXX\n"
                                                                  "SG_ FaultBatteryVoltageDeviation : 36|1@1+ (1,0) [0|1] \"status\" Vector__XXX\n"
                                                                  "SG_ FaultHighBatteryTemperature : 35|1@1+ (1,0) [0|1] \"status\" Vector__XXX\n"
                                                                  "SG_ FaultBatteryCurrentDeviation : 34|1@1+ (1,0) [0|1] \"status\" Vector__XXX\n"
                                                                  "SG_ FaultBatteryUndervoltage : 33|1@1+ (1,0) [0|1] \"status\" Vector__XXX\n"
                                                                  "SG_ FaultBatteryOvervoltage : 32|1@1+ (1,0) [0|1] \"status\" Vector__XXX\n"
                                                                  "SG_ StatusNormalStopRequest : 44|1@1+ (1,0) [0|1] \"status\" Vector__XXX\n"
                                                                  "SG_ StatusVehicle : 43|1@1+ (1,0) [0|1] \"status\" Vector__XXX\n"
                                                                  "SG_ StatusChargingSystem : 42|1@1+ (1,0) [0|1] \"status\" Vector__XXX\n"
                                                                  "SG_ StatusVehicleShifterPosition : 41|1@1+ (1,0) [0|1] \"status\" Vector__XXX\n"
                                                                  "SG_ StatusVehicleCharging : 40|1@1+ (1,0) [0|1] \"status\" Vector__XXX\n"
                                                                  "SG_ ChargingRate : 48|8@1+ (1,0) [0|100] \"%\" Vector__XXX"));

    m_fields[0x108] = CanMessageUtils::parseFields(QStringLiteral("SG_ EVContactorWeldingDetection : 0|8@1+ (1,0) [0|1] "" Vector__XXX\n"
                                                                  "SG_ AvailableOutputVoltage : 15|16@0+ (1,0) [0|600] \"V\" Vector__XXX\n"
                                                                  "SG_ AvailableOutputCurrent : 24|8@1+ (1,0) [0|255] \"A\" Vector__XXX\n"
                                                                  "SG_ ThresholdVoltage : 39|16@0+ (1,0) [0|600] \"V\" Vector__XXX"));

    m_fields[0x109] = CanMessageUtils::parseFields(QStringLiteral("SG_ ControlProtocolNumberQC : 0|8@1+ (1,0) [0|255] \"-\" Vector__XXX\n"
                                                                  "SG_ OutputVoltage : 15|16@0+ (1,0) [0|600] \"V\" Vector__XXX\n"
                                                                  "SG_ OutputCurrent : 24|8@1+ (1,0) [0|255] \"A\" Vector__XXX\n"
                                                                  "SG_ StatusChargerStopControl : 45|1@1+ (1,0) [0|1] \"status\" Vector__XXX\n"
                                                                  "SG_ FaultChargingSystemMalfunction : 44|1@1+ (1,0) [0|1] \"status\" Vector__XXX\n"
                                                                  "SG_ FaultBatteryIncompatibility : 43|1@1+ (1,0) [0|1] \"status\" Vector__XXX\n"
                                                                  "SG_ StatusVehicleConnectorLock : 42|1@1+ (1,0) [0|1] \"status\" Vector__XXX\n"
                                                                  "SG_ FaultStationMalfunction : 41|1@1+ (1,0) [0|1] \"status\" Vector__XXX\n"
                                                                  "SG_ StatusStation : 40|1@1+ (1,0) [0|1] \"status\" Vector__XXX\n"
                                                                  "SG_ RemainingChargingTime10sBit : 48|8@1+ (10,0) [0|2540] \"seconds\" Vector__XXX\n"
                                                                  "SG_ RemainingChargingTime1minBit : 56|8@1+ (1,0) [0|255] \"minutes\" Vector__XXX"));

    QTimer* t = new QTimer(this);
    connect(t, &QTimer::timeout, this, &LeafChademoPort::prepareAndSendFrame);
    t->start(100);
}

LeafChademoPort::~LeafChademoPort()
{
    Param::SetInt(Param::CHAdeMO_Ireq, 0);
}

void LeafChademoPort::receiveFrame(quint32 frameId, const QByteArray& data)
{
    //qDebug() << Qt::hex << frameId << data;
    auto fields = m_fields[frameId];
    for (const auto& field : fields)
    {
    //    qDebug() << field.name << CanMessageUtils::readField(data, field);
    }

    if (frameId == 0x100)
    {
        m_thresholdVoltage = CanMessageUtils::readField(data, m_fields[0x100]["MaximumBatteryVoltage"]);
    }
    else if (frameId == 0x102)
    {
        m_chargingCurrentRequest = CanMessageUtils::readField(data, m_fields[0x102]["ChargingCurrentRequest"]);
        Param::SetInt(Param::CHAdeMO_Ireq, m_chargingCurrentRequest);
    }
}

void LeafChademoPort::prepareAndSendFrame()
{
    if (!Param::GetBool(Param::PlugDet)) // no CCS plug - this is a real Chademo charge - keep our fingers away
        return;

    int ccs_state = Param::GetInt(Param::CCS_State);

    bool statusChargerStopControl = ccs_state > 6;  // stop if true
    bool statusStation = true; // Charging
    bool statusVehicleConnectorLock = ccs_state < 8; // Locked if true

    quint16 availableOutputVoltage = 500; //Param::GetInt(Param::CCS_V_Avail);
    quint8 availableOutputCurrent = 125;//statusChargerStopControl ? 0 : 125; //Param::GetInt(Param::CCS_I_Avail);

    quint16 batteryVoltage = Param::GetInt(Param::udc);
    if (batteryVoltage != 0)
        m_batteryVoltage = batteryVoltage;
    quint16 outputVoltage = statusVehicleConnectorLock ? m_batteryVoltage : 0;
    quint8 outputCurrent = m_chargingCurrentRequest; // Param::GetInt(Param::CCS_I);

    QByteArray data(8, '\0');
    data[0] = 1; // EVContactorWeldingDetection: Supporting vehicle welding detection
    data[1] = availableOutputVoltage & 0xff;
    data[2] = availableOutputVoltage >> 8;
    data[3] = availableOutputCurrent;
    data[4] = m_thresholdVoltage & 0xff;
    data[5] = m_thresholdVoltage >> 8;
    sendFrame(0x108, data);

    data = QByteArray(8, '\0');
    data[0] = 2; // ControlProtocolNumberQC
    data[1] = outputVoltage & 0xff;
    data[2] = outputVoltage >> 8;
    data[3] = outputCurrent;
    data[4] = 0;
    data[5] = (statusChargerStopControl ? 0x20 : 0) |
              (statusStation ? 0x01 : 0) |
              (statusVehicleConnectorLock ? 4 : 0);
    data[6] = 0; // remaining charge time 10s
    data[7] = 60; // remaining charge time min

    sendFrame(0x109, data);
}

QVector<quint32> LeafChademoPort::receivingFrameIds() const
{
    return { 0x100, 0x101, 0x102, 0x200 };
}
