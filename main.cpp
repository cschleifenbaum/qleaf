#include <QCoreApplication>

#include <QDebug>
#include <QTimer>

#include "canbusnodedetector.h"
#include "tccharger.h"
#include "leafchademoport.h"
#include "leafobcharger.h"
#include "leafhvbattery.h"
#include "lim.h"

#include "openinverter/params.h"

int main(int argc, char* argv[])
{
    QCoreApplication app(argc, argv);

    Param::SetInt(Param::BattCap, 40080);

    CanBusNodeDetector nodeDetector;
    nodeDetector.registerCanBusNodeType<TcCharger>(0x18ff50e7); // protocol 998
    nodeDetector.registerCanBusNodeType<TcCharger>(0x18ff50e5); // protocol 1430
    nodeDetector.registerCanBusNodeType<LeafChademoPort>(0x100);
    nodeDetector.registerCanBusNodeType<LeafOBCharger>(0x390);
    nodeDetector.registerCanBusNodeType<LeafHVBattery>(0x1db);
    nodeDetector.registerCanBusNodeType<I3LIM>(0x3b4);

    auto printInfo = [&]()
    {
        for (auto charger : nodeDetector.createdNodeInstances<LeafOBCharger>())
        {
            qDebug() << "Maximum output power:  " << (charger->maxOutputPower() / 1000.) << "kW";
            qDebug() << "Output power:          " << (charger->outputPower() / 1000.0) << "kW";
        }
        for (auto battery : nodeDetector.createdNodeInstances<LeafHVBattery>())
        {
            qDebug() << "State of charge:       " << battery->stateOfCharge() << "%";
            qDebug() << "Discharge Power limit: " << (battery->dischargePowerLimit() / 1000.) << "kW";
            qDebug() << "Charge Power limit:    " << (battery->chargePowerLimit() / 1000.) << "kW";
            qDebug() << "Max. Power for charger:" << (battery->maxPowerForCharger() / 1000.) << "kW";
            qDebug() << "Battery voltage:       " << battery->voltage() << "V";
            qDebug() << "Battery current:       " << battery->current() << "A";
        }
        auto tcChargers = nodeDetector.createdNodeInstances<TcCharger>();
        for (auto charger : tcChargers)
        {
            qDebug() << charger->outputVoltage() << "V";
            qDebug() << charger->outputCurrent() << "A";
            //charger->setMaxOutputVoltage(96 * 4.2);
            //charger->setMaxOutputCurrent(20);
        }
        qDebug() << "";
    };

    auto controlTCChargers = [&]()
    {
        const auto tcChargers = nodeDetector.createdNodeInstances<TcCharger>();
        const auto onboardChargers = nodeDetector.createdNodeInstances<LeafOBCharger>();
        const auto batteries = nodeDetector.createdNodeInstances<LeafHVBattery>();

        if (batteries.isEmpty())
            qDebug() << "No signal from HV battery.";
        if (onboardChargers.isEmpty())
            qDebug() << "No signal from stock onboard charger.";
        if (tcChargers.isEmpty())
            qDebug() << "No signal from TC chargers.";

        if (batteries.isEmpty() || onboardChargers.isEmpty())
        {
            for (auto charger : tcChargers)
            {
                charger->setMaxOutputVoltage(0);
                charger->setMaxOutputCurrent(0);
            }
//            if (tcChargers.isEmpty())
//                return;
        }

        const quint32 maxPower = 20000;//batteries.first()->maxPowerForCharger();
        const quint32 onboardChargerPower = 3300;//charger->outputPower();
        const quint32 totalTcChargerPower = maxPower - onboardChargerPower;
        if (!tcChargers.isEmpty())
        {
            const quint32 tcChargerPower = std::min(onboardChargerPower, totalTcChargerPower / tcChargers.count());
            for (auto charger : tcChargers)
            {
                charger->setMaxOutputVoltage(403.2);
                charger->setMaxOutputCurrent(tcChargerPower / charger->maxOutputVoltage());
                qDebug() << 403.2 << (tcChargerPower / charger->maxOutputVoltage());
            }
        }
    };

    QObject::connect(&nodeDetector, &CanBusNodeDetector::canBusNodeCreated, [&](CanBusNode*) {
    });

    QTimer t;
    QObject::connect(&t, &QTimer::timeout, [&] { printInfo(); });
    QObject::connect(&t, &QTimer::timeout, [&] { controlTCChargers(); });
    t.start(1000);

    return app.exec();
}
