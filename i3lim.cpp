#include "i3lim.h"

#include <QDebug>
#include <QTimer>

I3LIM::I3LIM(QCanBusDevice* canBusDevice, quint32 frameId, QObject* parent)
    : CanBusNode(canBusDevice, 0, frameId, parent)
{
    qDebug() << "Adding i3 LIM";

    m_fields[0x3b4] = CanMessageUtils::parseFields(QStringLiteral("SG_ Pilot_Current : 0|8@1+ (1,0) [0|63] \"Amps\" Vector__XXX\n"
                                                                  "SG_ Cable_Current : 8|8@1+ (1,0) [0|63] \"Amps\" Vector__XXX\n"
                                                                  "SG_ FC_Voltage : 56|8@1+ (2,0) [0|500] \"Volts\" Vector__XXX\n"
                                                                  "SG_ Pilot_Status : 32|3@1+ (1,0) [0|1] \"\" Vector__XXX\n"
                                                                  "SG_ PP_Status : 16|1@0+ (1,0) [0|1] \"\" Vector__XXX\n"
                                                                  "SG_ CP_Lock_Status : 26|2@1+ (1,0) [0|1] \"\" Vector__XXX\n"
                                                                  "SG_ CHG_Flap_Status : 24|2@1+ (1,0) [0|4] \"\" Vector__XXX\n"
                                                                  "SG_ Plug_Connection : 37|2@1+ (1,0) [0|4] \"\" Vector__XXX\n"
                                                                  "SG_ Charge_End_Sig : 20|2@1+ (1,0) [0|4] \"\" Vector__XXX\n"
                                                                  "SG_ HW_Charger_EN : 22|2@1+ (1,0) [0|1] \"\" Vector__XXX\n"
                                                                  "SG_ CCS_Plug : 35|2@1+ (1,0) [0|1] \"\" Vector__XXX\n"
                                                                  "SG_ Charging_Type : 48|8@1+ (1,0) [0|1] \"\" Vector__XXX\n"
                                                                  "SG_ DC_Chg_Volts : 39|9@1+ (2,0) [0|1000] \"V\" Vector__XXX"));


    m_fields[0x272] = CanMessageUtils::parseFields(QStringLiteral("SG_ FC_Contactor_Info : 20|4@1+ (1,0) [0|15] \"\" Vector__XXX\n"
                                                                  "SG_ FC_Contactor_State : 18|2@1+ (1,0) [0|3] \"\" Vector__XXX\n"
                                                                  "SG_ ChargeFlap_Status : 16|2@1+ (1,0) [0|4] \"\" Vector__XXX"));
    
    m_fields[0x337] = CanMessageUtils::parseFields(QStringLiteral("SG_ Hook_Pos : 0|2@1+ (1,0) [0|3] \"\" Vector__XXX\n"
                                                                  "SG_ Hook_Lock : 2|2@1+ (1,0) [0|3] \"\" Vector__XXX"));


    m_fields[0x112] = CanMessageUtils::parseFields(QStringLiteral("SG_ Battery_Volts : 16|16@1+ (0.1,0) [0|500] \"Volts\" Vector__XXX\n"
                                                                  "SG_ DC_link : 56|8@1+ (4,0) [0|1000] \"Volts\" Vector__XXX\n"
                                                                  "SG_ Battery_Status : 48|4@1+ (1,0) [0|16] \"\" Vector__XXX\n"
                                                                  "SG_ Battery_Current : 0|16@1+ (0.1,-819.2) [0|819] \"Amps\" Vector__XXX\n"
                                                                  "SG_ HVBatt_SOC : 32|12@1+ (0.1,0) [0|100] \"%\" Vector__XXX"));

    m_fields[0x3e9] = CanMessageUtils::parseFields(QStringLiteral("SG_ FC_Current_Command : 40|10@1- (1,0) [-500|500] \"Amps\" Vector__XXX\n"
                                                                  "SG_ Contactor_Con : 52|4@1+ (1,0) [0|16] \"\" Vector__XXX\n"
                                                                  "SG_ EndOfChg_Time : 56|8@1+ (1,0) [0|255] \"Mins\" Vector__XXX\n"
                                                                  "SG_ Chg_Status_Info : 20|4@1+ (1,0) [0|3] \"\" Vector__XXX\n"
                                                                  "SG_ Chg_Readiness : 24|2@1+ (1,0) [0|3] \"\" Vector__XXX\n"
                                                                  "SG_ DCFC_I_Diff : 26|2@1+ (1,0) [0|3] \"\" Vector__XXX\n"
                                                                  "SG_ DCFC_V_Diff : 50|2@1+ (1,0) [0|3] \"\" Vector__XXX\n"
                                                                  "SG_ Batt_Energy : 0|16@1+ (1,0) [0|65534] \"Wh\" Vector__XXX\n"
                                                                  "SG_ Charge_Req : 16|4@1+ (1,0) [0|16] \"\" Vector__XXX\n"
                                                                  "SG_ Charging_Pwr : 28|12@1+ (25,0) [0|102350] \"Watts\" Vector__XXX"));

    QTimer* t = new QTimer(this);
    connect(t, &QTimer::timeout, this, &I3LIM::prepareAndSendFrame10);
    t->start(10);
    t = new QTimer(this);
    connect(t, &QTimer::timeout, this, &I3LIM::prepareAndSendFrame100);
    t->start(100);
    t = new QTimer(this);
    connect(t, &QTimer::timeout, this, &I3LIM::prepareAndSendFrame200);
    t->start(200);
    t = new QTimer(this);
    connect(t, &QTimer::timeout, this, &I3LIM::prepareAndSendFrame600);
    t->start(600);
}

bool I3LIM::isChargeEnabled() const
{
    return m_chargeEnabled;
}

void I3LIM::setChargeEnabled(bool enabled)
{
    m_chargeEnabled = enabled;
}

void I3LIM::setMaximumPower(int maximumPower)
{
    m_maxPower = maximumPower;
}

int I3LIM::maximumPower() const
{
    return m_maxPower;
}

void I3LIM::receiveFrame(quint32 frameId, const QByteArray& data)
{
    qDebug() << Qt::hex << frameId << data;
    auto fields = m_fields[frameId];
    for (const auto& field : fields)
    {
//        qDebug() << field.name << CanMessageUtils::readField(data, field);
    }
}

void I3LIM::prepareAndSendFrame10()
{
    auto data = QByteArray("\xf9\x1f\x8b\x0e\xa6\x71\x65\x5d", 8);
    auto fields = m_fields[0x112];
    for (const auto& field : fields)
        qDebug() << field.name << CanMessageUtils::readField(data, field);
    sendFrame(0x112, data);
}

void I3LIM::prepareAndSendFrame100()
{
//    sendFrame(0x109, QByteArray(8, '\0'));
    sendFrame(0x12f, QByteArray("\xf5\x28\x88\x1d\xf1\x35\x30\x80", 8));
}

void I3LIM::prepareAndSendFrame200()
{
    m_chargeEnabled = true;
    m_maxPower = 1000;
    quint16 Batt_Wh = 40080;
    quint8 CHG_Status = m_chargeEnabled ? 2 : 0;
    quint8 CHG_Req = m_chargeEnabled ? 1 : 0;
    quint16 CHG_Pwr = m_chargeEnabled ? m_maxPower / 25 : 0;
    quint8 CHG_Ready = m_chargeEnabled ? 1 : 0;
    quint8 EOC_Time = m_chargeEnabled ? 0xfe : 0x00;
    quint16 FC_Cur = m_chargeEnabled ? 125 : 0;

    auto highByte = [](quint16 data) -> quint8 {
        return (data >> 8) & 0xff;
    };
    auto lowByte = [](quint16 data) -> quint8 {
        return data & 0xff;
    };
    QByteArray data(8, '\0');
    data[0]=lowByte(Batt_Wh);  
    data[1]=highByte(Batt_Wh); 
    data[2]=((CHG_Status<<4)|(CHG_Req));  //charge status in bits 4-7.goes to 1 then 2.8 secs later to 2. Plug locking???. Charge request in lower nibble. 1 when charging. 0 when not charging.
    data[3]=((CHG_Pwr<<4)|CHG_Ready); //charge readiness in bits 0 and 1. 1 = ready to charge.upper nibble is LSB of charge power.
    data[4]=CHG_Pwr>>4;  //MSB of charge power.in this case 0x28 = 40x25 = 1000W. Probably net DC power into the Batt.
    data[5]=0x00;  //LSB of the DC ccs current command
    data[6]=0x00;  //bits 0 and 1 MSB of the DC ccs current command.Upper nibble is DC ccs contactor control. Observed in DC fc logs only.
                                      //transitions from 0 to 2 and start of charge but 2 to 1 to 0 at end. Status and Ready operate the same as in AC logs.
    data[7]=EOC_Time;// end of charge timer.
    auto fields = m_fields[0x3e9];
    for (const auto& field : fields)
        qDebug() << field.name << CanMessageUtils::readField(data, field);
    sendFrame(0x03c, QByteArray("\xff\x5f\x00\x00\x00\x00\xff\xff", 8));
    sendFrame(0x3e9, data);
    sendFrame(0x431, QByteArray("\xca\xff\x0b\x02\x69\x26\xf3\x4b", 8));
}

void I3LIM::prepareAndSendFrame600()
{
//    sendFrame(0x108, QByteArray(8, '\0'));
    sendFrame(0x560, QByteArray("\x00\x00\x00\x00\xfe\x00\x00\x60", 8));
//    sendFrame(0x109, QByteArray(8, '\0'));
}

QVector<quint32> I3LIM::receivingFrameIds() const
{
    return { 0x3b4, 0x272, 0x337  };
}
