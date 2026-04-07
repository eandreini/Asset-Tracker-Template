#include "gpsparams.h"
#include <stdio.h>
#include <string.h>
#include "device_shadow_types.h"

#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(gpsparams_helper, CONFIG_APP_LOG_LEVEL);

gpsparams_t g_gpsparams;
gpsparams_chgd_t g_gpsparams_chgd;

#define DUMP(param,spcs)\
    printf(#param":"spcs" %d %c\n", g_gpsparams.param, g_gpsparams_chgd.chgd##param ? 'C':' ');

#define SET(param,value)\
    g_gpsparams.param = value;\
    g_gpsparams_chgd.chgd##param = 1;


#define CBOR2PAR(srcname,dstname)\
	if (shadow->config.srcname##_present) {\
		gpschgd->chgd##dstname = 1;\
		gpsparams->dstname = shadow->config.srcname.srcname;\
		LOG_DBG ("***Param "#srcname"("#dstname") received value %d ***",shadow->config.srcname.srcname);\
	}\
	else\
		gpschgd->chgd##dstname = 0;\

#define PAR2CBOR(srcname,dstname)\
	if (gpschgd->chgd##srcname) {\
		shadow->config_present = true;\
		shadow->config.dstname##_present = true;\
		shadow->config.dstname.dstname = gpsparams->srcname;\
		LOG_DBG ("***Param "#srcname"("#dstname") sent value %d ***",shadow->config.dstname.dstname);\
	}\
    else {\
        shadow->config.dstname##_present = false;\
    }


#define SAVEPAR(param)\
    if (gpschgd->chgd##param) {\
        g_gpsparams.param = gpsparams->param;\
        g_gpsparams_chgd.chgd##param = 1;\
    }\
    else {\
        g_gpsparams_chgd.chgd##param = 0;\
    }

#define LOADPAR(param)\
    gpsparams->param = g_gpsparams.param;\
    gpschgd->chgd##param = g_gpsparams_chgd.chgd##param;
    
void GpsParamsSetChanged(const gpsparams_t * gpsparams, const gpsparams_chgd_t * gpschgd)
{
    SAVEPAR(PresenceTimeoutIdleSec);
    SAVEPAR(PresenceTimeoutActSec);
    SAVEPAR(PresenceHysteresisSec);

    SAVEPAR(NotifyPresenceLost);
    SAVEPAR(NotifyAccMove);
    SAVEPAR(NotifySledEvents);
    SAVEPAR(CoaleshTimeMin);

    SAVEPAR(MinGpsStrength);
    SAVEPAR(GpsFixTimeoutSec);
    SAVEPAR(GpsFixDelaySec);

    SAVEPAR(LteMinStrenght);
    SAVEPAR(LteTimeoutSec);
    SAVEPAR(LteOnDemandFreqMin);
    SAVEPAR(LteOnDemandOffsetMin);
    SAVEPAR(LteConnOnNoFix);
    SAVEPAR(GpsAidIntervalH);
    SAVEPAR(GpsAidNumDays);
    SAVEPAR(GpsAidOnlyM1);

    SAVEPAR(Ts1Range);
    SAVEPAR(Ts1Dow);
    SAVEPAR(Ts1IntervalM);

    SAVEPAR(Ts2Range);
    SAVEPAR(Ts2Dow);
    SAVEPAR(Ts2IntervalM);

    SAVEPAR(Ts3Range);
    SAVEPAR(Ts3Dow);
    SAVEPAR(Ts3IntervalM);

    SAVEPAR(Ts4Range);
    SAVEPAR(Ts4Dow);
    SAVEPAR(Ts4IntervalM);

    SAVEPAR(Ts5Range);
    SAVEPAR(Ts5Dow);
    SAVEPAR(Ts5IntervalM);

    SAVEPAR(Ts6Range);
    SAVEPAR(Ts6Dow);
    SAVEPAR(Ts6IntervalM);

    SAVEPAR(Ts7Range);
    SAVEPAR(Ts7Dow);
    SAVEPAR(Ts7IntervalM);

    SAVEPAR(Ts8Range);
    SAVEPAR(Ts8Dow);
    SAVEPAR(Ts8IntervalM);

    SAVEPAR(Ode);
}
void GpsParamsGetChanged(gpsparams_t * gpsparams, gpsparams_chgd_t * gpschgd)
{
    LOADPAR(PresenceTimeoutIdleSec);
    LOADPAR(PresenceTimeoutActSec);
    LOADPAR(PresenceHysteresisSec);

    LOADPAR(NotifyPresenceLost);
    LOADPAR(NotifyAccMove);
    LOADPAR(NotifySledEvents);
    LOADPAR(CoaleshTimeMin);

    LOADPAR(MinGpsStrength);
    LOADPAR(GpsFixTimeoutSec);
    LOADPAR(GpsFixDelaySec);

    LOADPAR(LteMinStrenght);
    LOADPAR(LteTimeoutSec);
    LOADPAR(LteOnDemandFreqMin);
    LOADPAR(LteOnDemandOffsetMin);
    LOADPAR(LteConnOnNoFix);
    LOADPAR(GpsAidIntervalH);
    LOADPAR(GpsAidNumDays);
    LOADPAR(GpsAidOnlyM1);

    LOADPAR(Ts1Range);
    LOADPAR(Ts1Dow);
    LOADPAR(Ts1IntervalM);

    LOADPAR(Ts2Range);
    LOADPAR(Ts2Dow);
    LOADPAR(Ts2IntervalM);

    LOADPAR(Ts3Range);
    LOADPAR(Ts3Dow);
    LOADPAR(Ts3IntervalM);

    LOADPAR(Ts4Range);
    LOADPAR(Ts4Dow);
    LOADPAR(Ts4IntervalM);

    LOADPAR(Ts5Range);
    LOADPAR(Ts5Dow);
    LOADPAR(Ts5IntervalM);

    LOADPAR(Ts6Range);
    LOADPAR(Ts6Dow);
    LOADPAR(Ts6IntervalM);

    LOADPAR(Ts7Range);
    LOADPAR(Ts7Dow);
    LOADPAR(Ts7IntervalM);

    LOADPAR(Ts8Range);
    LOADPAR(Ts8Dow);
    LOADPAR(Ts8IntervalM);

    LOADPAR(Ode);
}

void GpsParamsGetAll(gpsparams_t * gpsparams, gpsparams_chgd_t * gpschgd)
{
    memcpy(gpsparams, &g_gpsparams, sizeof(gpsparams_t));
    memset(gpschgd, 0xff, sizeof(gpsparams_chgd_t));
}



void GpsParamsDecodeFromCbor (const struct shadow_object * shadow, gpsparams_t * gpsparams, gpsparams_chgd_t * gpschgd)
{
    CBOR2PAR(PTIS, PresenceTimeoutIdleSec);
    CBOR2PAR(PTAC, PresenceTimeoutActSec);
    CBOR2PAR(PHS, PresenceHysteresisSec);
    
    CBOR2PAR(NPS, NotifyPresenceLost);
    CBOR2PAR(NAM, NotifyAccMove);
    CBOR2PAR(NSE, NotifySledEvents);
    CBOR2PAR(CTM, CoaleshTimeMin);
    
    CBOR2PAR(MGS, MinGpsStrength);
    CBOR2PAR(GFTS, GpsFixTimeoutSec);
    CBOR2PAR(GFDS, GpsFixDelaySec);
    
    CBOR2PAR(LMS, LteMinStrenght);
    CBOR2PAR(LTS, LteTimeoutSec);
    CBOR2PAR(LODFM, LteOnDemandFreqMin);
    CBOR2PAR(LODOM, LteOnDemandOffsetMin);
    CBOR2PAR(LCONF, LteConnOnNoFix);
    CBOR2PAR(GAIH, GpsAidIntervalH);
    CBOR2PAR(GAIND, GpsAidNumDays);
    CBOR2PAR(GAOM, GpsAidOnlyM1);
    
    CBOR2PAR(TR1, Ts1Range);
    CBOR2PAR(TD1, Ts1Dow);
    CBOR2PAR(TI1, Ts1IntervalM);

    CBOR2PAR(TR2, Ts2Range);
    CBOR2PAR(TD2, Ts2Dow);
    CBOR2PAR(TI2, Ts2IntervalM);

    CBOR2PAR(TR3, Ts3Range);
    CBOR2PAR(TD3, Ts3Dow);
    CBOR2PAR(TI3, Ts3IntervalM);

    CBOR2PAR(TR4, Ts4Range);
    CBOR2PAR(TD4, Ts4Dow);
    CBOR2PAR(TI4, Ts4IntervalM);

    CBOR2PAR(TR5, Ts5Range);
    CBOR2PAR(TD5, Ts5Dow);
    CBOR2PAR(TI5, Ts5IntervalM);

    CBOR2PAR(TR6, Ts6Range);
    CBOR2PAR(TD6, Ts6Dow);
    CBOR2PAR(TI6, Ts6IntervalM);

    CBOR2PAR(TR7, Ts7Range);
    CBOR2PAR(TD7, Ts7Dow);
    CBOR2PAR(TI7, Ts7IntervalM);

    CBOR2PAR(TR8, Ts8Range);
    CBOR2PAR(TD8, Ts8Dow);
    CBOR2PAR(TI8, Ts8IntervalM);

    CBOR2PAR(ODE, Ode);

}
    
void GpsParamsEncodeToCbor (const gpsparams_t * gpsparams, const gpsparams_chgd_t * gpschgd, struct shadow_object * shadow)
{
    PAR2CBOR(PresenceTimeoutIdleSec, PTIS);
    PAR2CBOR(PresenceTimeoutActSec, PTAC);
    PAR2CBOR(PresenceHysteresisSec, PHS);
    
    PAR2CBOR(NotifyPresenceLost, NPS);
    PAR2CBOR(NotifyAccMove, NAM);
    PAR2CBOR(NotifySledEvents, NSE);
    PAR2CBOR(CoaleshTimeMin, CTM);
            
    PAR2CBOR(MinGpsStrength, MGS);
    PAR2CBOR(GpsFixTimeoutSec, GFTS);
    PAR2CBOR(GpsFixDelaySec, GFDS);
    
    PAR2CBOR(LteMinStrenght, LMS);
    PAR2CBOR(LteTimeoutSec, LTS);
    PAR2CBOR(LteOnDemandFreqMin, LODFM);
    PAR2CBOR(LteOnDemandOffsetMin, LODOM);
    PAR2CBOR(LteConnOnNoFix, LCONF);
    PAR2CBOR(GpsAidIntervalH, GAIH);
    PAR2CBOR(GpsAidNumDays, GAIND);
    PAR2CBOR(GpsAidOnlyM1, GAOM);
    
    PAR2CBOR(Ts1Range, TR1);
    PAR2CBOR(Ts1Dow, TD1);
    PAR2CBOR(Ts1IntervalM, TI1);

    PAR2CBOR(Ts2Range, TR2);
    PAR2CBOR(Ts2Dow, TD2);
    PAR2CBOR(Ts2IntervalM, TI2);

    PAR2CBOR(Ts3Range, TR3);
    PAR2CBOR(Ts3Dow, TD3);
    PAR2CBOR(Ts3IntervalM, TI3);

    PAR2CBOR(Ts4Range, TR4);
    PAR2CBOR(Ts4Dow, TD4);
    PAR2CBOR(Ts4IntervalM, TI4);

    PAR2CBOR(Ts5Range, TR5);
    PAR2CBOR(Ts5Dow, TD5);
    PAR2CBOR(Ts5IntervalM, TI5);

    PAR2CBOR(Ts6Range, TR6);
    PAR2CBOR(Ts6Dow, TD6);
    PAR2CBOR(Ts6IntervalM, TI6);

    PAR2CBOR(Ts7Range, TR7);
    PAR2CBOR(Ts7Dow, TD7);
    PAR2CBOR(Ts7IntervalM, TI7);

    PAR2CBOR(Ts8Range, TR8);
    PAR2CBOR(Ts8Dow, TD8);
    PAR2CBOR(Ts8IntervalM, TI8);

    PAR2CBOR(Ode, ODE);
}

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
    DUMP(Ode, "                      ");
    
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

    SET(Ode, 0);
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

    CHECK_PAR(name, Ode, value)

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
    if (gpschgd->chgd##param)\
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
    
    GET_CHANGED(Ode)    

    if (ptr != buffer) {
        *ptr = 0;\
        cb(buffer);\
    }

    return changed;
}
int GpsParamsIsChanged(const gpsparams_chgd_t * gpschgd)
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
    
    IS_CHANGED(Ode)

    return 0;
}
int GpsParamsClearChanged()
{
    memset(&g_gpsparams_chgd, 0, sizeof(gpsparams_chgd_t));
    return 0;
}