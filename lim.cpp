#include "lim.h"

#include "openinverter/i3LIM.h"

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

    m_fields[0x2f1] = CanMessageUtils::parseFields(QStringLiteral("SG_ Time_to_FC_SOC : 40|16@1+ (10,0) [0|655320] \"Secs\" Vector__XXX\n"
                                                                  "SG_ Time_to_SOC : 24|16@1+ (10,0) [0|655320] \"Secs\" Vector__XXX\n"
                                                                  "SG_ FC_SOC : 56|8@1+ (0.5,0) [0|100] \"%\" Vector__XXX\n"
                                                                  "SG_ CHG_I_Lim : 16|8@1+ (1,0) [0|254] \"Amps\"  LIM\n"
                                                                  "SG_ CHG_V_Lim : 0|14@1- (0.1,0) [0|1000] \"Volts\" Vector__XXX"));

    m_fields[0x2fa] = CanMessageUtils::parseFields(QStringLiteral("SG_ Chg_End : 18|2@1+ (1,0) [0|1] "" Vector__XXX"
                                                                  "SG_ FC_End : 16|2@1+ (1,0) [0|1] \"\" Vector__XXX\n"
                                                                  "SG_ Target_CHG_Ph : 20|4@1+ (1,0) [0|1] \"\" Vector__XXX\n"
                                                                  "SG_ Time2Go : 0|16@1+ (1,0) [0|65534] \"Mins\" Vector__XXX\n"
                                                                  "SG_ Target_Volts : 24|14@1+ (0.1,0) [0|1000] \"V\"  LIM"));

    QTimer* t = new QTimer(this);
    connect(t, &QTimer::timeout, this, &I3LIM::prepareAndSendFrame10);
    t->start(10);
    t = new QTimer(this);
    connect(t, &QTimer::timeout, this, &I3LIM::prepareAndSendFrame100);
    t->start(100);
    t = new QTimer(this);
    connect(t, &QTimer::timeout, this, &I3LIM::prepareAndSendFrame200);
    t->start(200);
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
//    m_maxPower = maximumPower;
}

int I3LIM::maximumPower() const
{
    return 0;//m_maxPower;
}

void I3LIM::Send(int id, uint32_t* bytes, int length)
{
    QByteArray data(reinterpret_cast<char*>(bytes), length);
    qDebug() << Qt::hex << id << data;
    if (m_fields.contains(id))
    {
        auto fields = m_fields[id];
        for (const auto& field : fields)
        {
           qDebug() << field.name << CanMessageUtils::readField(data, field);
        }
    }
    qDebug() << "";
    sendFrame(id, data);
}

void I3LIM::receiveFrame(quint32 frameId, const QByteArray& data)
{
    qDebug() << Qt::hex << frameId << data;
    if (m_fields.contains(frameId))
    {
        auto fields = m_fields[frameId];
        for (const auto& field : fields)
        {
           qDebug() << field.name << CanMessageUtils::readField(data, field);
        }
    }
    qDebug() << "";
    uint32_t* d = const_cast<uint32_t*>(reinterpret_cast<const uint32_t*>(data.data()));
    switch (frameId)
    {
    case 0x3b4:
        i3LIMClass::handle3B4(d);
        break;
    case 0x272:
        i3LIMClass::handle3B4(d);
        break;
    case 0x2ce:
        i3LIMClass::handle3B4(d);
        break;
    case 0x2b2:
        i3LIMClass::handle3B4(d);
        break;
    case 0x2ef:
        i3LIMClass::handle3B4(d);
        break;
    }
    auto fields = m_fields[frameId];
    for (const auto& field : fields)
    {
       qDebug() << field.name << CanMessageUtils::readField(data, field);
    }
}

void I3LIM::prepareAndSendFrame10()
{
    i3LIMClass::Send10msMessages(this);
#if 0
    auto data = QByteArray(8, '\x00');
    CanMessageUtils::writeField(data, m_fields[0x112]["Battery_Current"], -0.7);
    CanMessageUtils::writeField(data, m_fields[0x112]["Battery_Volts"], 372.3);
    CanMessageUtils::writeField(data, m_fields[0x112]["DC_link"], 372.3);
    CanMessageUtils::writeField(data, m_fields[0x112]["HVBatt_SOC"], 42.2);
    CanMessageUtils::writeField(data, m_fields[0x112]["Battery_Status"], 5);
    data[6] = 0x65; // 5 is Battery_Status, but the 6?
    sendFrame(0x112, data);
#endif
}

void I3LIM::prepareAndSendFrame100()
{
    i3LIMClass::Control_Charge(m_chargeEnabled);
    i3LIMClass::Send100msMessages(this);
#if 0
    // vehicle status message
    sendFrame(0x03c, QByteArray("\xff\x5f\x00\x00\x00\x00\xff\xff", 8));

    QByteArray data(8, '\x00');
    CanMessageUtils::writeField(data, m_fields[0x3e9]["FC_Current_Command"], FC_Cur);
    CanMessageUtils::writeField(data, m_fields[0x3e9]["Contactor_Con"], CONT_Ctrl;);
    CanMessageUtils::writeField(data, m_fields[0x3e9]["EndOfChg_Time"], m_chargeEnabled ? 0xfe : 0x00);
    CanMessageUtils::writeField(data, m_fields[0x3e9]["Chg_Status_Info"], CHG_Status);
    CanMessageUtils::writeField(data, m_fields[0x3e9]["Chg_Readiness"], CHG_Ready);
    CanMessageUtils::writeField(data, m_fields[0x3e9]["DCFC_I_Diff"], 0);
    CanMessageUtils::writeField(data, m_fields[0x3e9]["DCFC_V_Diff"], 0);
    CanMessageUtils::writeField(data, m_fields[0x3e9]["Batt_Energy"], 40080);
    CanMessageUtils::writeField(data, m_fields[0x3e9]["Charge_Req"], CHG_Req);
    CanMessageUtils::writeField(data, m_fields[0x3e9]["Charging_Pwr"], CHG_Pwr);
    sendFrame(0x3e9, data);

    // LIM need to see this but doesn't control anything...
    sendFrame(0x431, QByteArray("\xca\xff\x0b\x02\x69\x26\xf3\x4b", 8));

    data = QByteArray("\xf5\x28\x00\x00\x1d\xf1\x35\x30\x80", 8);
    data[2] = 0x86; // ignition off 0x86, otherwise 0x8a
    sendFrame(0x12f, data);

    data = QByteArray(8, '\x00');
    CanMessageUtils::writeField(data, m_fields[0x2f1]["Time_to_FC_SOC"], Bulk_SOCt);
    CanMessageUtils::writeField(data, m_fields[0x2f1]["Time_to_SOC"], Full_SOCt);
    CanMessageUtils::writeField(data, m_fields[0x2f1]["FC_SOC"], 100);
    CanMessageUtils::writeField(data, m_fields[0x2f1]["CHG_I_Lim"], 125);
    CanMessageUtils::writeField(data, m_fields[0x2f1]["CHG_V_Lim"], 435);
    sendFrame(0x2f1, data);
#endif
}

void I3LIM::prepareAndSendFrame200()
{
    i3LIMClass::Send200msMessages(this);
#if 0
    QByteArray data(8, '\x00');
    CanMessageUtils::writeField(data, m_fields[0x2f1]["Chg_End"], 435); //????
    CanMessageUtils::writeField(data, m_fields[0x2f1]["FC_End"], 435);  // ????
    CanMessageUtils::writeField(data, m_fields[0x2f1]["Target_CHG_Ph"], Chg_Phase);
    CanMessageUtils::writeField(data, m_fields[0x2f1]["Time2Go"], 435); //???
    CanMessageUtils::writeField(data, m_fields[0x2f1]["Target_Volts"], 435); //???
    sendFrame(0x2fa, data);
#endif
}

QVector<quint32> I3LIM::receivingFrameIds() const
{
    return { 0x3b4, 0x29e, 0x2b2, 0x2ef, 0x272 };
}
