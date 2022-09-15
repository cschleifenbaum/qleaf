#ifndef PARAMS_H
#define PARAMS_H

#include <QMap>

enum modes
{
    MOD_OFF = 0,
    MOD_RUN,
    MOD_PRECHARGE,
    MOD_PCHFAIL,
    MOD_CHARGE,
    MOD_LAST
};

class Param
{
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
        CCS_ILim,     // maximum I allowed by MS
        CCS_Ireq,
        Voltspnt,
        CP_DOOR,
        udc,          // battery voltage, V
        idc,          // battery current, A
        SOC,          // state of charge, %
        BattCap,      // battery capacity, Wh
        opmode        // vehicle operation mode, see enum modes
	};

	static void SetInt(Value v, int i) { s_map.insert(v, i); }
	static int GetInt(Value v) { return s_map.value(v); }
	static bool GetBool(Value v) { return GetInt(v) != 0; }

    static QMap<Value, int> s_map;
};

#endif
