#ifndef PARAMS_H
#define PARAMS_H

#include <QDebug>
#include <QMap>
#include <QTimer>

enum modes
{
    MOD_OFF = 0,
    MOD_RUN,
    MOD_PRECHARGE,
    MOD_PCHFAIL,
    MOD_CHARGE,
    MOD_LAST
};

class Param : public QObject
{
    Q_OBJECT
public:
    enum Value
    {
        PilotLim,
        CableLim,
        PlugDet,
        PilotTyp,
        CCS_V_Con,
        CCS_V_Avail,
        CCS_I_Avail,
        CCS_COND,
        CCS_V,
        CCS_I,
        CCS_V_Min,
        CCS_Contactor,
        CCS_State,
        CCS_ILim,     // maximum I allowed by BMS
        CCS_Ireq,
        Voltspnt,
        CP_DOOR,
        udc,          // battery voltage, V
        idc,          // battery current, A
        SOC,          // state of charge, %
        BattCap,      // battery capacity, Wh
        opmode        // vehicle operation mode, see enum modes
    };
    Q_ENUM(Value)

    static void SetInt(Value v, int i)
    {
        static QTimer* t = nullptr;
        if (!t)
        {
            t = new QTimer;
            QObject::connect(t, &QTimer::timeout, [&]()
            {
                qDebug() << s_map;
            });
            t->start(1000);
        }

        s_map.insert(v, i);
    }
    static int GetInt(Value v)
    { 
        if (!s_map.contains(v))
            qDebug() << "Params::GetInt: Not value set for" << v;
        return s_map.value(v);
    }
    static bool GetBool(Value v)
    {
        return GetInt(v) != 0;
    }

    static QMap<Value, int> s_map;
};

#endif
