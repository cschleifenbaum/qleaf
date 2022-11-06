#ifndef CHARGINGRELAYCONTROLLER_H
#define CHARGINGRELAYCONTROLLER_H

#include "nymea-gpio/gpio.h"

class ChargingRelayController : public QObject
{
public:
    ChargingRelayController(QObject* parent = nullptr);

    enum class ChargingMode
    {
        LIM,
        LeafOBCharger
    };

    ChargingMode chargingMode() const;
    void setChargingMode(ChargingMode mode);

    bool chademoProximity() const;
    void setChademoProximity(bool proximity);

    void setChargeFlapOpen(bool open);
    bool isChargeFlapOpen() const;

    void setChargerStart1(bool set);
    bool isChargerStart1Set() const;

    void setChargerStart2(bool set);
    bool isChargerStart2Set() const;

protected:
    void deductChargingMode();
    void fakeChargeFlap();

private:
    Gpio m_gpio2;   // High -> charge flap open
    Gpio m_gpio3;   // Chademo d1, pin 2 -> 12V
    Gpio m_gpio4;   // Chademo proximity, pin 7 -> GND
    Gpio m_gpio14;  // Chademo d2, pin 10 -> GND
    Gpio m_gpio15;  // PP, CP, CCS plug lock...
    bool m_weldTest = false;
};

#endif
