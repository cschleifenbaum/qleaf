#ifndef CANBUSNODEDETECTOR_H
#define CANBUSNODEDETECTOR_H

#include <QObject>
#include <QMap>

class QCanBusDevice;
class QCanBusFrame;

class CanBusNode;

class CanBusNodeDetector : public QObject
{
    Q_OBJECT
public:
    CanBusNodeDetector(QObject* parent = nullptr);
    ~CanBusNodeDetector() = default;

    template<typename T>
    void registerCanBusNodeType(quint32 frameId)
    {
        auto factory = &CanBusNodeDetector::create<T>;
        m_factories[frameId] = factory;
    }

    template<typename T>
    QList<T*> createdNodeInstances()
    {
        QList<T*> instances;
        for (auto node : m_nodes.values())
        {
            if (auto instance = dynamic_cast<T*>(node))
                instances.push_back(instance);
        }
        return instances;
    }

Q_SIGNALS:
    void canBusNodeCreated(CanBusNode* canBusNode);

protected:
    template<typename T>
    static CanBusNode* create(QCanBusDevice* device, quint32 frameId, QObject* parent)
    {
        return new T(device, frameId, parent);
    }

    void frameReceived(QCanBusDevice* device, const QCanBusFrame& frame);

private:
    typedef std::function<CanBusNode*(QCanBusDevice*, quint32, QObject*)> FactoryFunc;
    QMap<quint32, FactoryFunc> m_factories;
    QMap<quint32, CanBusNode*> m_nodes;
};

#endif
