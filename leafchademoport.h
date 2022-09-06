#ifndef LEAFCHADEMOPORT_H
#define LEAFCHADEMOPORT_H

#include "canbusnode.h"

class LeafChademoPort : public CanBusNode
{
    Q_OBJECT
public:
    LeafChademoPort(QCanBusDevice* canBusDevice, quint32 frameId, QObject* parent = nullptr);
    ~LeafChademoPort() = default;

protected:
    void receiveFrame(quint32 frameId, const QByteArray& data) override;
    QVector<quint32> receivingFrameIds() const override;
};

#endif
