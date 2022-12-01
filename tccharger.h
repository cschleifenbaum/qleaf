#ifndef TCCHARGER_H
#define TCCHARGER_H

#include "canbusnode.h"

class TcCharger : public CanBusNode
{
    Q_OBJECT
    Q_PROPERTY(double outputVoltage READ outputVoltage NOTIFY outputVoltageChanged)
    Q_PROPERTY(double outputCurrent READ outputCurrent NOTIFY outputCurrentChanged)
    Q_PROPERTY(double maxOutputVoltage READ maxOutputVoltage WRITE setMaxOutputVoltage NOTIFY maxOutputVoltageChanged)
    Q_PROPERTY(double maxOutputCurrent READ maxOutputCurrent WRITE setMaxOutputCurrent NOTIFY maxOutputCurrentChanged)
    Q_PROPERTY(StatusFlags status READ status NOTIFY statusChanged)
public:
    TcCharger(QCanBusDevice* canBusDevice, quint32 frameIdReceiving, QObject* parent = nullptr);
    ~TcCharger() = default;
   
    enum StatusFlag
    {
        Normal = 0x0,
        HardwareFailure = 0x1,
        OverTemperatureProtection = 0x2,
        IncorrectInputVoltage = 0x04,
        BatteryDisconnectedOrReversed = 0x08,
        CommunicationTimeout = 0x10
    };
    Q_DECLARE_FLAGS(StatusFlags, StatusFlag)
    Q_FLAG(StatusFlag)

    double outputVoltage() const;
    double outputCurrent() const;

    double maxOutputVoltage() const;
    void setMaxOutputVoltage(double voltage);

    double maxOutputCurrent() const;
    void setMaxOutputCurrent(double current);

    StatusFlags status() const;

Q_SIGNALS:
    void outputVoltageChanged(double voltage);
    void outputCurrentChanged(double current);
    void maxOutputVoltageChanged(double voltage);
    void maxOutputCurrentChanged(double current);
    void statusChanged(StatusFlags status);

protected:
    void prepareAndSendFrame();
    void receiveFrame(const QByteArray& data) override;

private:
    quint16 m_outputVoltage = 0;
    quint16 m_outputCurrent = 0;
    quint16 m_maxOutputVoltage = 0;
    quint16 m_maxOutputCurrent = 0;

    StatusFlags m_status = CommunicationTimeout;
};

#endif
