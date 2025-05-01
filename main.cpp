#include <QCoreApplication>

#include <QDebug>
#include <QTimer>

#include "canbusnodedetector.h"
#include "chargingrelaycontroller.h"
#include "tccharger.h"
#include "tcchargercontroller.h"
#include "leafchademoport.h"
#include "leafobcharger.h"
#include "leafhvbattery.h"
#include "lim.h"

#include "openinverter/params.h"

int main(int argc, char* argv[])
{
    QCoreApplication app(argc, argv);

    ChargingRelayController relays;

    CanBusNodeDetector nodeDetector;
    nodeDetector.registerCanBusNodeType<TcCharger>(0x18ff50e7); // protocol 998
    nodeDetector.registerCanBusNodeType<TcCharger>(0x18ff50e8);
    nodeDetector.registerCanBusNodeType<TcCharger>(0x18ff50e5); // protocol 1430
    nodeDetector.registerCanBusNodeType<LeafChademoPort>(0x100);
    nodeDetector.registerCanBusNodeType<LeafOBCharger>(0x390);
    nodeDetector.registerCanBusNodeType<LeafHVBattery>(0x1db);
    nodeDetector.registerCanBusNodeType<I3LIM>(0x3b4);

    TcChargerController tcController;

    return app.exec();
}
