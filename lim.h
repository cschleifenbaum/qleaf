#ifndef I3LIM_H
#define I3LIM_H

#include "canbusnode.h"
#include "canmessageutils.h"

#include "openinverter/stm32_can.h"

class I3LIM : public CanBusNode, public CanHardware
{
    Q_OBJECT
    Q_PROPERTY(bool chargeEnabled READ isChargeEnabled WRITE setChargeEnabled);
public:
    I3LIM(QCanBusDevice* canBusDevice, quint32 frameId, QObject* parent = nullptr);
    ~I3LIM();

    bool isChargeEnabled() const;
    void setChargeEnabled(bool enabled);

    void Send(int id, uint32_t* bytes, int length) override;

protected:
    void receiveFrame(quint32 frameId, const QByteArray& data) override;
    QVector<quint32> receivingFrameIds() const override;

    void prepareAndSendFrame10();
    void prepareAndSendFrame100();
    void prepareAndSendFrame200();

private:
    bool m_chargeEnabled = false;
    QHash<quint32, CanMessageUtils::Fields> m_fields;
};

#endif
