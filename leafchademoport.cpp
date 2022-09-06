#include "leafchademoport.h"

#include <QDebug>

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
}

void LeafChademoPort::receiveFrame(quint32 frameId, const QByteArray& data)
{
}
    
QVector<quint32> LeafChademoPort::receivingFrameIds() const
{
    return { 0x100, 0x101, 0x103, 0x200 };
}
