#include "lim.h"

#include "Stm32-vcu/include/i3LIM.h"

#include <QDebug>
#include <QTimer>


I3LIM::I3LIM(QCanBusDevice* canBusDevice, quint32 frameId, QObject* parent)
    : CanBusNode(canBusDevice, 0, frameId, parent),
      m_sleepTimer(new QTimer(this))
{
    qDebug() << "Adding i3 LIM";

    setChargeEnabled(true);

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

    connect(m_sleepTimer, &QTimer::timeout, this,  [&]{ m_sleep = true; });
    m_sleepTimer->start(60000);
}

I3LIM::~I3LIM()
{
    setChargeEnabled(false);
}

bool I3LIM::isChargeEnabled() const
{
    return m_chargeEnabled;
}

void I3LIM::setChargeEnabled(bool enabled)
{
    m_chargeEnabled = enabled;
}

void I3LIM::Send(int id, uint32_t* bytes, int length)
{
    QByteArray data(reinterpret_cast<char*>(bytes), length);
    sendFrame(id, data);
}

void I3LIM::receiveFrame(quint32 frameId, const QByteArray& data)
{
    uint32_t* d = const_cast<uint32_t*>(reinterpret_cast<const uint32_t*>(data.data()));
    switch (frameId)
    {
    case 0x3b4:
        i3LIMClass::handle3B4(d);
        if (Param::GetBool(Param::PlugDet)) {
            m_sleep = false;
            m_sleepTimer->start();
        }
        break;
    case 0x272:
        i3LIMClass::handle272(d);
        break;
    case 0x29e:
        i3LIMClass::handle29E(d);
        break;
    case 0x2b2:
        i3LIMClass::handle2B2(d);
        break;
    case 0x2ef:
        i3LIMClass::handle2EF(d);
        break;
    }
}

void I3LIM::prepareAndSendFrame10()
{
    if (!m_sleep)
        i3LIMClass::Send10msMessages(this);
}

void I3LIM::prepareAndSendFrame100()
{
    if (!m_sleep)
    {
        i3LIMClass::Send100msMessages(this);
        i3LIMClass::Control_Charge(m_chargeEnabled);
    }
}

void I3LIM::prepareAndSendFrame200()
{
    if (!m_sleep)
        i3LIMClass::Send200msMessages(this);
}

QVector<quint32> I3LIM::receivingFrameIds() const
{
    return { 0x3b4, 0x29e, 0x2b2, 0x2ef, 0x272 };
}
