#ifndef I3LIM_H
#define I3LIM_H

#include "canbusnode.h"
#include "canmessageutils.h"

class I3LIM : public CanBusNode
{
    Q_OBJECT
    Q_PROPERTY(bool chargeEnabled READ isChargeEnabled WRITE setChargeEnabled);
    Q_PROPERTY(int maximumPower READ maximumPower WRITE setMaximumPower);
public:
    I3LIM(QCanBusDevice* canBusDevice, quint32 frameId, QObject* parent = nullptr);
    ~I3LIM() = default;

    bool isChargeEnabled() const;
    void setChargeEnabled(bool enabled);

    void setMaximumPower(int maximumPower);
    int maximumPower() const;

protected:
    void receiveFrame(quint32 frameId, const QByteArray& data) override;
    QVector<quint32> receivingFrameIds() const override;

    void prepareAndSendFrame10();
    void prepareAndSendFrame100();
    void prepareAndSendFrame200();
    void prepareAndSendFrame600();

private:
    bool m_chargeEnabled = false;
    int m_maxPower = 0;
    QHash<quint32, CanMessageUtils::Fields> m_fields;
};

#endif
