#ifndef LEAFHVBATTERY_H
#define LEAFHVBATTERY_H

#include "canbusnode.h"

class LeafHVBattery : public CanBusNode
{
    Q_OBJECT
    Q_PROPERTY(quint32 dischargePowerLimit READ dischargePowerLimit NOTIFY dischargePowerLimitChanged);
    Q_PROPERTY(quint32 chargePowerLimit READ chargePowerLimit NOTIFY chargePowerLimitChanged);
    Q_PROPERTY(qint32 maxPowerForCharger READ maxPowerForCharger NOTIFY maxPowerForCharger);
    Q_PROPERTY(double voltage READ voltage NOTIFY voltageChanged);
    Q_PROPERTY(double current READ current NOTIFY currentChanged);
    Q_PROPERTY(double stateOfCharge READ stateOfCharge NOTIFY stateOfCharge);
public:
    LeafHVBattery(QCanBusDevice* canBusDevice, quint32 frameId, QObject* parent = nullptr);
    ~LeafHVBattery() = default;

    quint32 dischargePowerLimit() const;
    quint32 chargePowerLimit() const;
    qint32 maxPowerForCharger() const;

    double voltage() const;
    double current() const;

    double stateOfCharge() const;

Q_SIGNALS:
    void dischargePowerLimitChanged(quint32 dischargePowerLimit);
    void chargePowerLimitChanged(quint32 chargePowerLimit);
    void maxPowerForChargerChanged(qint32 maxPowerForCharger);
    void voltageChanged(double voltage);
    void currentChanged(double current);
    void stateOfChargeChanged(double stateOfCharge);

protected:
    void receiveFrame(quint32 frameId, const QByteArray& data) override;
    QVector<quint32> receivingFrameIds() const override;

private:
    quint32 m_dischargePowerLimit = 0;
    quint32 m_chargePowerLimit = 0;
    qint32 m_maxPowerForCharger = 0;
    double m_voltage = 0.0;
    double m_current = 0.0;
    double m_stateOfCharge = 0.0;
};

#endif
