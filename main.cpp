#include <QCoreApplication>

#include <QDebug>

#include "canbusnodedetector.h"
#include "tccharger.h"
#include "leafobcharger.h"
#include "leafhvbattery.h"

int main(int argc, char* argv[])
{
    QCoreApplication app(argc, argv);

    CanBusNodeDetector nodeDetector;
    nodeDetector.registerCanBusNodeType<TcCharger>(0x18ff50e7); // protocol 998
    nodeDetector.registerCanBusNodeType<TcCharger>(0x18ff50e5); // protocol 1430
    nodeDetector.registerCanBusNodeType<LeafOBCharger>(0x390);
    nodeDetector.registerCanBusNodeType<LeafHVBattery>(0x1db);

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
        qDebug() << "";
    };

    QObject::connect(&nodeDetector, &CanBusNodeDetector::canBusNodeCreated, [&](CanBusNode* node) {
        QObject::connect(node, &CanBusNode::changed, printInfo);
        auto tcChargers = nodeDetector.createdNodeInstances<TcCharger>();
        for (auto charger : tcChargers)
        {
            charger->setMaxOutputVoltage(96 * 4.2);
            charger->setMaxOutputCurrent(20);
        }
    });

    return app.exec();
}
