#include "canbusnodedetector.h"

#include "canbusnode.h"

#include <QCanBus>
#include <QDebug>

CanBusNodeDetector::CanBusNodeDetector(QObject* parent)
    : QObject(parent)
{
    auto canbus = QCanBus::instance();
    for (auto plugin : canbus->plugins())
    {
        auto devices = canbus->availableDevices(plugin);
        for (auto deviceInfo : devices)
        {
            auto device = canbus->createDevice(plugin, deviceInfo.name());
            if (!device)
                continue;
            device->connectDevice();
            QObject::connect(device, &QCanBusDevice::framesReceived, this, [device,this]{
                auto frames = device->readAllFrames();
                for (auto frame : frames)
                    frameReceived(device, frame);
            });
        }
    }

}
    
void CanBusNodeDetector::frameReceived(QCanBusDevice* device, const QCanBusFrame& frame)
{
    if (!m_nodes.contains(frame.frameId()))
    {
        if (frame.payload().size() != 8)
            return;
        if (!m_factories.contains(frame.frameId()))
            return;
        if (auto node = m_factories[frame.frameId()](device, frame.frameId(), this))
        {
            addNode(node);
            qDebug() << Qt::hex << frame.frameId() << frame.payload();
            Q_EMIT canBusNodeCreated(node);
        }
    }
    auto node = m_nodes.value(frame.frameId());
    Q_ASSERT(node);
    if (node->m_canBusDevice == device)
        node->receiveFrame(frame);
}

void CanBusNodeDetector::addNode(CanBusNode* node)
{
    for (auto frameId : node->receivingFrameIds())
        m_nodes.insert(frameId, node);
    connect(node, &CanBusNode::timeout, this, [this, node]{ removeNode(node); });
}

void CanBusNodeDetector::removeNode(CanBusNode* node)
{
    for (auto frameId : node->receivingFrameIds())
        m_nodes.remove(frameId);
    node->deleteLater();
}
