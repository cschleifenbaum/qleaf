#ifndef LEAFOBCHARGER_H
#define LEAFOBCHARGER_H

#include "canbusnode.h"

class LeafOBCharger : public CanBusNode
{
    Q_OBJECT
    Q_PROPERTY(quint32 outputPower READ outputPower NOTIFY outputPowerChanged)
    Q_PROPERTY(quint32 maxOutputPower READ maxOutputPower NOTIFY maxOutputPowerChanged)
public:
    LeafOBCharger(QCanBusDevice* canBusDevice, quint32 frameId, QObject* parent = nullptr);
    ~LeafOBCharger() = default;

    quint32 outputPower() const;
    quint32 maxOutputPower() const;

Q_SIGNALS:
    void outputPowerChanged(quint32 outputPower);
    void maxOutputPowerChanged(quint32 maxOutputPower);

protected:
    void receiveFrame(const QByteArray& data) override;

private:
    quint32 m_outputPower = 0;
    quint32 m_maxOutputPower = 0;
};

#endif
