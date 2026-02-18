#include "gpsparams.h"
#include <stdio.h>
#include <string.h>


#define DUMP(param,spcs)\
    printf(#param":"spcs" %d %c\n", g_gpsparams.param, g_gpsparams_chgd.chgd##param ? 'C':' ');

#define SET(param,value)\
    g_gpsparams.param = value;\
    g_gpsparams_chgd.chgd##param = 1;

void GpsParamsDump()
{
    DUMP(PresenceTimeoutIdleSec, "   ");
    DUMP(PresenceTimeoutActSec, "    ");
    DUMP(PresenceHysteresisSec, "    ");
    printf("\n");

    DUMP(NotifyPresenceLost, "       ");
    DUMP(NotifyAccMove, "            ");
    DUMP(NotifySledEvents, "         ");
    DUMP(CoaleshTimeMin, "           ");
    printf("\n");

    DUMP(MinGpsStrength, "           ");
    DUMP(GpsFixTimeoutSec, "         ");
    DUMP(GpsFixDelaySec, "           ");
    printf("\n");

    DUMP(LteMinStrenght, "           ");
    DUMP(LteTimeoutSec, "            ");
    DUMP(LteOnDemandFreqMin, "       ");
    DUMP(LteOnDemandOffsetMin, "     ");
    DUMP(LteConnOnNoFix, "           ");
    DUMP(GpsAidIntervalH, "          ");
    DUMP(GpsAidNumDays, "            ");
    DUMP(GpsAidOnlyM1, "             ");
    printf("\n");

    DUMP(Ts1Range, "                 ");
    DUMP(Ts1Dow, "                   ");
    DUMP(Ts1IntervalM, "             ");
    printf("\n");
    DUMP(Ts2Range, "                 ");
    DUMP(Ts2Dow, "                   ");
    DUMP(Ts2IntervalM, "             ");
    printf("\n");
    DUMP(Ts3Range, "                 ");
    DUMP(Ts3Dow, "                   ");
    DUMP(Ts3IntervalM, "             ");
    printf("\n");
    DUMP(Ts4Range, "                 ");
    DUMP(Ts4Dow, "                   ");
    DUMP(Ts4IntervalM, "             ");
    printf("\n");
    DUMP(Ts5Range, "                 ");
    DUMP(Ts5Dow, "                   ");
    DUMP(Ts5IntervalM, "             ");
    printf("\n");
    DUMP(Ts6Range, "                 ");
    DUMP(Ts6Dow, "                   ");
    DUMP(Ts6IntervalM, "             ");
    printf("\n");
    DUMP(Ts7Range, "                 ");
    DUMP(Ts7Dow, "                   ");
    DUMP(Ts7IntervalM, "             ");
    printf("\n");
    DUMP(Ts8Range, "                 ");
    DUMP(Ts8Dow, "                   ");
    DUMP(Ts8IntervalM, "             ");
    printf("\n");
    
}
void GpsParamsTestFill()
{
    SET(PresenceTimeoutIdleSec, 900);
    SET(PresenceTimeoutActSec, 900);
    SET(PresenceHysteresisSec, 300);

    SET(NotifyPresenceLost, 0);
    SET(NotifyAccMove, 0);
    SET(NotifySledEvents, 0);
    SET(CoaleshTimeMin, 0);

    SET(MinGpsStrength, 35);
    SET(GpsFixTimeoutSec, 10);
    SET(GpsFixDelaySec, 7);

    SET(LteMinStrenght, 110);
    SET(LteTimeoutSec, 10);
    SET(LteOnDemandFreqMin, 0);
    SET(LteOnDemandOffsetMin, 0);
    SET(LteConnOnNoFix, 1);
    SET(GpsAidIntervalH, 168);
    SET(GpsAidNumDays, 3);
    SET(GpsAidOnlyM1, 0);

    SET(Ts1Range, 0);   
    SET(Ts1Dow, 0);     
    SET(Ts1IntervalM, 0);

    SET(Ts2Range, 0);   
    SET(Ts2Dow, 0);     
    SET(Ts2IntervalM, 0);

    SET(Ts3Range, 0);   
    SET(Ts3Dow, 0);     
    SET(Ts3IntervalM, 0);

    SET(Ts4Range, 0);   
    SET(Ts4Dow, 0);     
    SET(Ts4IntervalM, 0);

    SET(Ts5Range, 0);   
    SET(Ts5Dow, 0);     
    SET(Ts5IntervalM, 0);

    SET(Ts6Range, 0);   
    SET(Ts6Dow, 0);     
    SET(Ts6IntervalM, 0);

    SET(Ts7Range, 0);   
    SET(Ts7Dow, 0);     
    SET(Ts7IntervalM, 0);

    SET(Ts8Range, 0);   
    SET(Ts8Dow, 0);     
    SET(Ts8IntervalM, 0);

}


#define CHECK_PAR(name,field,value)\
    if (strcmp(name, #field) == 0) {\
        g_gpsparams.field = value;\
        g_gpsparams_chgd.chgd##field = 1;\
        return 0;\
    }


int GpsParamsSetValue(const char * name, int value)
{
    CHECK_PAR(name, PresenceTimeoutIdleSec, value)
    CHECK_PAR(name, PresenceTimeoutActSec, value)
    CHECK_PAR(name, PresenceHysteresisSec, value)

    CHECK_PAR(name, NotifyPresenceLost, value)
    CHECK_PAR(name, NotifyAccMove, value)
    CHECK_PAR(name, NotifySledEvents, value)
    CHECK_PAR(name, CoaleshTimeMin, value)

    CHECK_PAR(name, MinGpsStrength, value)
    CHECK_PAR(name, GpsFixTimeoutSec, value)
    CHECK_PAR(name, GpsFixDelaySec, value)

    CHECK_PAR(name, LteMinStrenght, value)
    CHECK_PAR(name, LteTimeoutSec, value)
    CHECK_PAR(name, LteOnDemandFreqMin, value)
    CHECK_PAR(name, LteOnDemandOffsetMin, value)
    CHECK_PAR(name, LteConnOnNoFix, value)
    CHECK_PAR(name, GpsAidIntervalH, value)
    CHECK_PAR(name, GpsAidNumDays, value)
    CHECK_PAR(name, GpsAidOnlyM1, value)

    CHECK_PAR(name, Ts1Range, value)
    CHECK_PAR(name, Ts1Dow, value)
    CHECK_PAR(name, Ts1IntervalM, value)

    CHECK_PAR(name, Ts2Range, value)
    CHECK_PAR(name, Ts2Dow, value)
    CHECK_PAR(name, Ts2IntervalM, value)

    CHECK_PAR(name, Ts3Range, value)
    CHECK_PAR(name, Ts3Dow, value)
    CHECK_PAR(name, Ts3IntervalM, value)

    CHECK_PAR(name, Ts4Range, value)
    CHECK_PAR(name, Ts4Dow, value)
    CHECK_PAR(name, Ts4IntervalM, value)

    CHECK_PAR(name, Ts5Range, value)
    CHECK_PAR(name, Ts5Dow, value)
    CHECK_PAR(name, Ts5IntervalM, value)

    CHECK_PAR(name, Ts6Range, value)
    CHECK_PAR(name, Ts6Dow, value)
    CHECK_PAR(name, Ts6IntervalM, value)

    CHECK_PAR(name, Ts7Range, value)
    CHECK_PAR(name, Ts7Dow, value)
    CHECK_PAR(name, Ts7IntervalM, value)

    CHECK_PAR(name, Ts8Range, value)
    CHECK_PAR(name, Ts8Dow, value)
    CHECK_PAR(name, Ts8IntervalM, value)

    return -1;

}

#define GET_CHANGED(param)\
    if (g_gpsparams_chgd.chgd##param) {\
        changed++;\
        sprintf(tmp,#param"=%d,", g_gpsparams.param);\
        int ln = strlen(tmp);\
        if (ln >= space) {\
            *ptr = 0;\
            cb(buffer);\
            ptr = buffer;\
            space = maxlen;\
        }\
        memcpy(ptr, tmp, ln);\
        ptr += ln;\
        space -= ln;\
    }

#define IS_CHANGED(param)\
    if (g_gpsparams_chgd.chgd##param)\
        return 1;\

int GpsParamsFlushChanged(char * buffer, int maxlen, gpsparams_flush_cb_t cb)
{
    char tmp[32];
    char * ptr = buffer;
    int space = maxlen;
    int changed = 0;

    GET_CHANGED(PresenceTimeoutIdleSec)
    GET_CHANGED(PresenceTimeoutActSec)
    GET_CHANGED(PresenceHysteresisSec)

    GET_CHANGED(NotifyPresenceLost)
    GET_CHANGED(NotifyAccMove)
    GET_CHANGED(NotifySledEvents)
    GET_CHANGED(CoaleshTimeMin)

    GET_CHANGED(MinGpsStrength)
    GET_CHANGED(GpsFixTimeoutSec)
    GET_CHANGED(GpsFixDelaySec)

    GET_CHANGED(LteMinStrenght)
    GET_CHANGED(LteTimeoutSec)
    GET_CHANGED(LteOnDemandFreqMin)
    GET_CHANGED(LteOnDemandOffsetMin)
    GET_CHANGED(LteConnOnNoFix)
    GET_CHANGED(GpsAidIntervalH)
    GET_CHANGED(GpsAidNumDays)
    GET_CHANGED(GpsAidOnlyM1)

    GET_CHANGED(Ts1Range)
    GET_CHANGED(Ts1Dow)
    GET_CHANGED(Ts1IntervalM)
    
    GET_CHANGED(Ts2Range)
    GET_CHANGED(Ts2Dow)
    GET_CHANGED(Ts2IntervalM)

    GET_CHANGED(Ts3Range)
    GET_CHANGED(Ts3Dow)
    GET_CHANGED(Ts3IntervalM)

    GET_CHANGED(Ts4Range)
    GET_CHANGED(Ts4Dow)
    GET_CHANGED(Ts4IntervalM)

    GET_CHANGED(Ts5Range)
    GET_CHANGED(Ts5Dow)
    GET_CHANGED(Ts5IntervalM)

    GET_CHANGED(Ts6Range)
    GET_CHANGED(Ts6Dow)
    GET_CHANGED(Ts6IntervalM)

    GET_CHANGED(Ts7Range)
    GET_CHANGED(Ts7Dow)
    GET_CHANGED(Ts7IntervalM)

    GET_CHANGED(Ts8Range)
    GET_CHANGED(Ts8Dow)
    GET_CHANGED(Ts8IntervalM)

    if (ptr != buffer) {
        *ptr = 0;\
        cb(buffer);\
    }

    return changed;
}
int GpsParamsIsChanged()
{
    IS_CHANGED(PresenceTimeoutIdleSec)
    IS_CHANGED(PresenceTimeoutActSec)
    IS_CHANGED(PresenceHysteresisSec)

    IS_CHANGED(NotifyPresenceLost)
    IS_CHANGED(NotifyAccMove)
    IS_CHANGED(NotifySledEvents)
    IS_CHANGED(CoaleshTimeMin)

    IS_CHANGED(MinGpsStrength)
    IS_CHANGED(GpsFixTimeoutSec)
    IS_CHANGED(GpsFixDelaySec)

    IS_CHANGED(LteMinStrenght)
    IS_CHANGED(LteTimeoutSec)
    IS_CHANGED(LteOnDemandFreqMin)
    IS_CHANGED(LteOnDemandOffsetMin)
    IS_CHANGED(LteConnOnNoFix)
    IS_CHANGED(GpsAidIntervalH)
    IS_CHANGED(GpsAidNumDays)
    IS_CHANGED(GpsAidOnlyM1)

    IS_CHANGED(Ts1Range)
    IS_CHANGED(Ts1Dow)
    IS_CHANGED(Ts1IntervalM)
    
    IS_CHANGED(Ts2Range)
    IS_CHANGED(Ts2Dow)
    IS_CHANGED(Ts2IntervalM)

    IS_CHANGED(Ts3Range)
    IS_CHANGED(Ts3Dow)
    IS_CHANGED(Ts3IntervalM)

    IS_CHANGED(Ts4Range)
    IS_CHANGED(Ts4Dow)
    IS_CHANGED(Ts4IntervalM)

    IS_CHANGED(Ts5Range)
    IS_CHANGED(Ts5Dow)
    IS_CHANGED(Ts5IntervalM)

    IS_CHANGED(Ts6Range)
    IS_CHANGED(Ts6Dow)
    IS_CHANGED(Ts6IntervalM)

    IS_CHANGED(Ts7Range)
    IS_CHANGED(Ts7Dow)
    IS_CHANGED(Ts7IntervalM)

    IS_CHANGED(Ts8Range)
    IS_CHANGED(Ts8Dow)
    IS_CHANGED(Ts8IntervalM)

    return 0;
}
