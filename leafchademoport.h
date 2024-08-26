#ifndef LEAFCHADEMOPORT_H
#define LEAFCHADEMOPORT_H

#include "canbusnode.h"
#include "canmessageutils.h"

class LeafChademoPort : public CanBusNode
{
    Q_OBJECT
public:
    LeafChademoPort(QCanBusDevice* canBusDevice, quint32 frameId, QObject* parent = nullptr);
    ~LeafChademoPort();

protected:
    void receiveFrame(quint32 frameId, const QByteArray& data) override;
    QVector<quint32> receivingFrameIds() const override;

    void prepareAndSendFrame();

private:
    QHash<quint32, CanMessageUtils::Fields> m_fields;
    quint16 m_thresholdVoltage = 0;
    quint16 m_batteryVoltage = 0;
    quint8 m_chargingCurrentRequest = 0;
    bool m_activeCharging = false;
};

#endif
