#ifndef CANBUSNODE_H
#define CANBUSNODE_H

#include <QObject>

class QCanBusDevice;
class QCanBusFrame;

class QTimer;

class CanBusNode : public QObject
{
    Q_OBJECT
protected:
    CanBusNode(QCanBusDevice* canBusDevice, quint32 frameIdSending, quint32 frameIdReceiving, QObject* parent = nullptr);

public:
    ~CanBusNode() = default;

Q_SIGNALS:
    void changed();
    void timeout();

protected:
    void sendFrame(const QByteArray& data);
    virtual void receiveFrame(quint32 frameId, const QByteArray& data);
    virtual void receiveFrame(const QByteArray& data);

    void receiveFrame(const QCanBusFrame& frame);

    virtual QVector<quint32> receivingFrameIds() const;

private:
    friend class CanBusNodeDetector;
    QCanBusDevice* m_canBusDevice = nullptr;
    quint32 m_frameIdSending = 0x0;
    quint32 m_frameIdReceiving = 0x0;
    QTimer* m_timeOutTimer = nullptr;
};


#endif
