#include "gpsparams.h"
#include <stdio.h>


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