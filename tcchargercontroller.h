#ifndef TCCHARGERCONTROLLER_H
#define TCCHARGERCONTROLLER_H

#include <QObject>

class QTimer;
class CanBusNode;

class LeafHVBattery;
class LeafOBCharger;
class TcCharger;

class TcChargerController : public QObject
{
public:
    TcChargerController(QObject* parent = nullptr);
    ~TcChargerController();

protected:
    void canBusNodeCreated(CanBusNode* node);
    void canBusNodeDestroyed(CanBusNode* node);

    void controlChargers();
    void shutdown();

private:
    QTimer* m_timer;
    LeafHVBattery* m_battery = nullptr;
    LeafOBCharger* m_stockCharger = nullptr;
    QList<TcCharger*> m_tcChargers;
};

#endif
