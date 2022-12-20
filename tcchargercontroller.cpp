#include "tcchargercontroller.h"

#include "canbusnodedetector.h"
#include "leafhvbattery.h"
#include "leafobcharger.h"
#include "tccharger.h"

#include "openinverter/params.h"

#include <QDebug>
#include <QTimer>

TcChargerController::TcChargerController(QObject* parent)
    : QObject(parent),
      m_timer(new QTimer(this))
{
    connect(CanBusNodeDetector::instance(), &CanBusNodeDetector::canBusNodeCreated, this, &TcChargerController::canBusNodeCreated);
    connect(m_timer, &QTimer::timeout, this, &TcChargerController::controlChargers);
}

TcChargerController::~TcChargerController()
{
    shutdown();
}

void TcChargerController::canBusNodeCreated(CanBusNode* node)
{
    connect(node, &QObject::destroyed, this, [this,node]{ canBusNodeDestroyed(node); });

    if (auto battery = dynamic_cast<LeafHVBattery*>(node))
        m_battery = battery;
    else if (auto charger = dynamic_cast<LeafOBCharger*>(node))
        m_stockCharger = charger;
    else if (auto charger = dynamic_cast<TcCharger*>(node))
        m_tcChargers.push_back(charger);

    if (!m_tcChargers.isEmpty() && m_stockCharger && m_battery)
        m_timer->start(100);
}

void TcChargerController::canBusNodeDestroyed(CanBusNode* node)
{
    if (node == m_battery)
        m_battery = nullptr;
    else if (node == m_stockCharger)
        m_stockCharger = nullptr;
    m_tcChargers.removeAll(reinterpret_cast<TcCharger*>(node));
    
    if (m_tcChargers.isEmpty() || !m_stockCharger || !m_battery)
        shutdown();
}

void TcChargerController::shutdown()
{
    m_timer->stop();
    for (auto charger : m_tcChargers)
    {
        charger->setMaxOutputVoltage(0);
        charger->setMaxOutputCurrent(0);
    }
}

void TcChargerController::controlChargers()
{
    if (m_tcChargers.isEmpty() || !m_stockCharger || !m_battery)
    {
        shutdown();
        return;
    }

    const quint32 maxPower = std::min(m_battery->maxPowerForCharger(), m_battery->chargePowerLimit());
    const quint32 onboardChargerPower = m_stockCharger->outputPower();
    const quint32 totalTcChargerPower = onboardChargerPower > maxPower ? 0 : maxPower - onboardChargerPower;

    const quint32 tcChargerPower = std::min(static_cast<quint32>(onboardChargerPower * 1.05), totalTcChargerPower / m_tcChargers.count());

    for (auto charger : m_tcChargers)
    {
        if (charger->status() != TcCharger::Normal)
            qDebug() << "Charger state not normal:" << charger << charger->status();
        charger->setMaxOutputVoltage(Param::GetInt(Param::Voltspnt));
        charger->setMaxOutputCurrent(tcChargerPower / m_battery->voltage());
    }
}
