#include "chargingrelaycontroller.h"

#include "canbusnodedetector.h"
#include "leafobcharger.h"
#include "lim.h"

#include "openinverter/params.h"

#include <QDir>
#include <QLoggingCategory>
#include <QTimer>

void initGpio(Gpio& pin, Gpio::Direction direction = Gpio::DirectionOutput)
{
    if (!QDir(pin.gpioDirectory()).exists())
        pin.exportGpio();
    if (pin.direction() != direction)
        pin.setDirection(direction);
    pin.setValue(Gpio::ValueLow);
}

ChargingRelayController::ChargingRelayController(QObject* parent)
    : QObject(parent),
      m_gpio2(2),
      m_gpio3(3),
      m_gpio4(4),
      m_gpio14(14),
      m_gpio15(15)
{
    QLoggingCategory::setFilterRules("Gpio.debug=false");
    initGpio(m_gpio2);
    initGpio(m_gpio3);
    initGpio(m_gpio4);
    initGpio(m_gpio14);
    initGpio(m_gpio15);

    QTimer* t = new QTimer(this);
    connect(t, &QTimer::timeout, this, &ChargingRelayController::deductChargingMode);
    connect(t, &QTimer::timeout, this, &ChargingRelayController::fakeChargeFlap);
    t->start(250);
}

void ChargingRelayController::deductChargingMode()
{
    bool lim_plug_det = Param::GetBool(Param::PlugDet);
    int obc_charge_status = Param::GetInt(Param::OBC_Charge_Status);
    bool obc_plug_det = obc_charge_status == 2 || obc_charge_status == 4 || obc_charge_status == 12;
    int pilot_typ = Param::GetInt(Param::PilotTyp);
    int pilot_lim = Param::GetInt(Param::PilotLim);
    int cable_lim = Param::GetInt(Param::CableLim);
    bool pilot_typ_ac = pilot_typ == 1 || pilot_typ == 2;

    int ccs_contactor = Param::GetInt(Param::CCS_Contactor);
    int ccs_state = Param::GetInt(Param::CCS_State);

    if (ccs_contactor > 1 && ccs_state < 4)
    {
        setChargingMode(ChargingMode::LeafOBCharger);
        setChargeFlapOpen(false);
        m_weldTest = true;
        return;
    }

    if (m_weldTest)
        setChargeFlapOpen(true);

    m_weldTest = false;

    // LIM says plug detected -> state 0 is AC - DC otherwise
    if (lim_plug_det || pilot_typ_ac)
        setChargingMode(pilot_typ_ac ? ChargingMode::LeafOBCharger : ChargingMode::LIM);

    // onboard charger says it could charge or not
    else
        setChargingMode(obc_plug_det ? ChargingMode::LeafOBCharger : ChargingMode::LIM);

    setChademoProximity(lim_plug_det && !pilot_typ_ac && !pilot_lim && !cable_lim);
}

ChargingRelayController::ChargingMode ChargingRelayController::chargingMode() const
{
    return const_cast<ChargingRelayController*>(this)->m_gpio15.value() == Gpio::ValueHigh ? ChargingMode::LIM : ChargingMode::LeafOBCharger;
}

void ChargingRelayController::setChargingMode(ChargingMode mode)
{
    if (chargingMode() == mode)
        return;
    bool lim_plug_det = Param::GetBool(Param::PlugDet);
    int obc_charge_status = Param::GetInt(Param::OBC_Charge_Status);
    bool obc_plug_det = obc_charge_status == 2 || obc_charge_status == 4 || obc_charge_status == 12;
    int lim_state = Param::GetInt(Param::CCS_State);

    qDebug() << "lim_plug_det" << lim_plug_det << "obc_charge_status" << obc_charge_status << "obc_plug_det" << obc_plug_det << "lim_state" << lim_state << "mode" << (mode == ChargingMode::LIM ? "LIM" : "OBC");

    m_gpio15.setValue(mode == ChargingMode::LIM ? Gpio::ValueHigh : Gpio::ValueLow);
}

void ChargingRelayController::fakeChargeFlap()
{
    if (m_weldTest)
        return;
    bool lim_plug_det = Param::GetBool(Param::PlugDet);
    int battery_I = Param::GetInt(Param::idc);
    int obc_charge_status = Param::GetInt(Param::OBC_Charge_Status);
    bool obc_plug_det = obc_charge_status == 2 || obc_charge_status == 4 || obc_charge_status == 12;
    if (lim_plug_det || obc_plug_det)
        setChargeFlapOpen(true);  // plug inserted... well.. must be open
    if (battery_I == 0)
        setChargeFlapOpen(true);  // car is standing still -> set it to open; maybe we want to charge
    if (battery_I < -20)
        setChargeFlapOpen(false); // we are driving -> charge flag should be closed. Now run your weld test, LIM!
}

bool ChargingRelayController::chademoProximity() const
{
    return const_cast<ChargingRelayController*>(this)->m_gpio3.value() == Gpio::ValueHigh;
}

void ChargingRelayController::setChademoProximity(bool proximity)
{
    if (proximity == chademoProximity())
        return;
//    qDebug() << "set Chademo proximity" << proximity;
    m_gpio3.setValue(proximity ? Gpio::ValueHigh : Gpio::ValueLow);
}

void ChargingRelayController::setChargeFlapOpen(bool open)
{
    if (isChargeFlapOpen() == open)
        return;
    m_gpio2.setValue(open ? Gpio::ValueHigh : Gpio::ValueLow);
}

bool ChargingRelayController::isChargeFlapOpen() const
{
    return const_cast<ChargingRelayController*>(this)->m_gpio2.value() == Gpio::ValueHigh;
}
