#include "gpsparams.h"
#include <stdio.h>
#include <string.h>
#include "device_shadow_types.h"

#include <zephyr/logging/log.h>
#include <zephyr/sys/base64.h>

LOG_MODULE_REGISTER(gpsparams_helper, CONFIG_APP_LOG_LEVEL);

gpsparams_t g_gpsparams;
gpsparams_valid_t g_gpsparams_vld;

/* for each parameter, if it is valid, check if is different from stored one. If different set dest valid flag (used as change flag), othervise clear it */
/* if input parameters is not valid clear dest valid flag (still used as change flag)*/

#define SAVEPAR(param)\
    if (gpsvld->vld##param) {\
        if (gpsparams->param != g_gpsparams.param) {\
            LOG_DBG ("***Param "#param" received value %d is different from stored value %d ***", gpsparams->param, g_gpsparams.param);\
            g_gpsparams.param = gpsparams->param;\
        }\
    }

void GpsParamsUpdateValid(const gpsparams_t * gpsparams, const gpsparams_valid_t * gpsvld)
{
    SAVEPAR(PresenceTimeoutIdleSec);
    SAVEPAR(PresenceTimeoutActSec);
    SAVEPAR(PresenceHysteresisSec);

    SAVEPAR(NotifyPresenceLost);
    SAVEPAR(NotifyAccMove);
    SAVEPAR(NotifySledEvents);
    SAVEPAR(CoaleshTimeIdleMin);

    SAVEPAR(MinGpsStrength);
    SAVEPAR(GpsFixTimeoutSec);
    SAVEPAR(GpsFixDelaySec);

    SAVEPAR(CoaleshTimeActiveMin);
    SAVEPAR(LteTimeoutSec);
    SAVEPAR(LteTimeoutMaxRetry);
    SAVEPAR(LteTimeoutPurgeMins);
    SAVEPAR(LteConnOnNoFix);
    SAVEPAR(GpsAidIntervalH);
    SAVEPAR(GpsAidNumDays);
    SAVEPAR(LteTimeoutDouble);

    SAVEPAR(Ts1Range);
    SAVEPAR(Ts1Dow);
    SAVEPAR(Ts1IntervalIM);

    SAVEPAR(Ts2Range);
    SAVEPAR(Ts2Dow);
    SAVEPAR(Ts2IntervalIM);

    SAVEPAR(Ts3Range);
    SAVEPAR(Ts3Dow);
    SAVEPAR(Ts3IntervalIM);

    SAVEPAR(Ts4Range);
    SAVEPAR(Ts4Dow);
    SAVEPAR(Ts4IntervalIM);

    SAVEPAR(Ts5Range);
    SAVEPAR(Ts5Dow);
    SAVEPAR(Ts5IntervalIM);

    SAVEPAR(Ts6Range);
    SAVEPAR(Ts6Dow);
    SAVEPAR(Ts6IntervalIM);

    SAVEPAR(Ts7Range);
    SAVEPAR(Ts7Dow);
    SAVEPAR(Ts7IntervalIM);

    SAVEPAR(Ts8Range);
    SAVEPAR(Ts8Dow);
    SAVEPAR(Ts8IntervalIM);

    SAVEPAR(OutDoorEnable);

    SAVEPAR(AccMoveDuration);
    SAVEPAR(Ts1IntervalAM);
    SAVEPAR(Ts2IntervalAM);
    SAVEPAR(Ts3IntervalAM);
    SAVEPAR(Ts4IntervalAM);
    SAVEPAR(Ts5IntervalAM);
    SAVEPAR(Ts6IntervalAM);
    SAVEPAR(Ts7IntervalAM);
    SAVEPAR(Ts8IntervalAM);
    SAVEPAR(LogEnable);
}

void GpsParamsGetAll(gpsparams_t * gpsparams, gpsparams_valid_t * gpschgd)
{
    memcpy(gpsparams, &g_gpsparams, sizeof(gpsparams_t));
    memset(gpschgd, 0xff, sizeof(gpsparams_valid_t));
}

/* copy CBOR parameters (with short names)from shadow and store in gpsparams structures gpsparams and gpsvld */

#define CBOR2PAR(srcname,dstname)\
	if (shadow->config.srcname##_present) {\
		gpsvld->vld##dstname = 1;\
		gpsparams->dstname = shadow->config.srcname.srcname;\
		LOG_DBG ("***Param "#srcname"("#dstname") received value %d ***",shadow->config.srcname.srcname);\
	}\
	else\
		gpsvld->vld##dstname = 0;\

void GpsParamsDecodeFromCbor (const struct shadow_object * shadow, gpsparams_t * gpsparams, gpsparams_valid_t * gpsvld)
{
    CBOR2PAR(PTIS, PresenceTimeoutIdleSec);
    CBOR2PAR(PTAC, PresenceTimeoutActSec);
    CBOR2PAR(PHS, PresenceHysteresisSec);
    
    CBOR2PAR(NPS, NotifyPresenceLost);
    CBOR2PAR(NAM, NotifyAccMove);
    CBOR2PAR(NSE, NotifySledEvents);
    CBOR2PAR(CTM, CoaleshTimeIdleMin);
    
    CBOR2PAR(MGS, MinGpsStrength);
    CBOR2PAR(GFTS, GpsFixTimeoutSec);
    CBOR2PAR(GFDS, GpsFixDelaySec);
    
    CBOR2PAR(LMS, CoaleshTimeActiveMin);
    CBOR2PAR(LTS, LteTimeoutSec);
    CBOR2PAR(LODFM, LteTimeoutMaxRetry);
    CBOR2PAR(LODOM, LteTimeoutPurgeMins);
    CBOR2PAR(LCONF, LteConnOnNoFix);
    CBOR2PAR(GAIH, GpsAidIntervalH);
    CBOR2PAR(GAIND, GpsAidNumDays);
    CBOR2PAR(GAOM, LteTimeoutDouble);
    
    CBOR2PAR(TR1, Ts1Range);
    CBOR2PAR(TD1, Ts1Dow);
    CBOR2PAR(TI1, Ts1IntervalIM);

    CBOR2PAR(TR2, Ts2Range);
    CBOR2PAR(TD2, Ts2Dow);
    CBOR2PAR(TI2, Ts2IntervalIM);

    CBOR2PAR(TR3, Ts3Range);
    CBOR2PAR(TD3, Ts3Dow);
    CBOR2PAR(TI3, Ts3IntervalIM);

    CBOR2PAR(TR4, Ts4Range);
    CBOR2PAR(TD4, Ts4Dow);
    CBOR2PAR(TI4, Ts4IntervalIM);

    CBOR2PAR(TR5, Ts5Range);
    CBOR2PAR(TD5, Ts5Dow);
    CBOR2PAR(TI5, Ts5IntervalIM);

    CBOR2PAR(TR6, Ts6Range);
    CBOR2PAR(TD6, Ts6Dow);
    CBOR2PAR(TI6, Ts6IntervalIM);

    CBOR2PAR(TR7, Ts7Range);
    CBOR2PAR(TD7, Ts7Dow);
    CBOR2PAR(TI7, Ts7IntervalIM);

    CBOR2PAR(TR8, Ts8Range);
    CBOR2PAR(TD8, Ts8Dow);
    CBOR2PAR(TI8, Ts8IntervalIM);

    CBOR2PAR(ODE, OutDoorEnable);

    CBOR2PAR(AMD, AccMoveDuration);
    CBOR2PAR(TA1, Ts1IntervalAM);
    CBOR2PAR(TA2, Ts2IntervalAM);
    CBOR2PAR(TA3, Ts3IntervalAM);
    CBOR2PAR(TA4, Ts4IntervalAM); 
    CBOR2PAR(TA5, Ts5IntervalAM);
    CBOR2PAR(TA6, Ts6IntervalAM);
    CBOR2PAR(TA7, Ts7IntervalAM);
    CBOR2PAR(TA8, Ts8IntervalAM);
    CBOR2PAR(LOG, LogEnable);
}



/* copy gpsparams and gpschgd to CBOR shadow (with short names) */

#define PAR2CBOR(srcname,dstname)\
	if (gpsvld->vld##srcname) {\
		shadow->config_present = true;\
		shadow->config.dstname##_present = true;\
		shadow->config.dstname.dstname = gpsparams->srcname;\
		LOG_DBG ("***Param "#srcname"("#dstname") sent value %d ***",shadow->config.dstname.dstname);\
	}\
    else {\
        shadow->config.dstname##_present = false;\
    }

void GpsParamsEncodeToCbor (const gpsparams_t * gpsparams, const gpsparams_valid_t * gpsvld, struct shadow_object * shadow)
{
    PAR2CBOR(PresenceTimeoutIdleSec, PTIS);
    PAR2CBOR(PresenceTimeoutActSec, PTAC);
    PAR2CBOR(PresenceHysteresisSec, PHS);
    
    PAR2CBOR(NotifyPresenceLost, NPS);
    PAR2CBOR(NotifyAccMove, NAM);
    PAR2CBOR(NotifySledEvents, NSE);
    PAR2CBOR(CoaleshTimeIdleMin, CTM);
            
    PAR2CBOR(MinGpsStrength, MGS);
    PAR2CBOR(GpsFixTimeoutSec, GFTS);
    PAR2CBOR(GpsFixDelaySec, GFDS);
    
    PAR2CBOR(CoaleshTimeActiveMin, LMS);
    PAR2CBOR(LteTimeoutSec, LTS);
    PAR2CBOR(LteTimeoutMaxRetry, LODFM);
    PAR2CBOR(LteTimeoutPurgeMins, LODOM);
    PAR2CBOR(LteConnOnNoFix, LCONF);
    PAR2CBOR(GpsAidIntervalH, GAIH);
    PAR2CBOR(GpsAidNumDays, GAIND);
    PAR2CBOR(LteTimeoutDouble, GAOM);
    
    PAR2CBOR(Ts1Range, TR1);
    PAR2CBOR(Ts1Dow, TD1);
    PAR2CBOR(Ts1IntervalIM, TI1);

    PAR2CBOR(Ts2Range, TR2);
    PAR2CBOR(Ts2Dow, TD2);
    PAR2CBOR(Ts2IntervalIM, TI2);

    PAR2CBOR(Ts3Range, TR3);
    PAR2CBOR(Ts3Dow, TD3);
    PAR2CBOR(Ts3IntervalIM, TI3);

    PAR2CBOR(Ts4Range, TR4);
    PAR2CBOR(Ts4Dow, TD4);
    PAR2CBOR(Ts4IntervalIM, TI4);

    PAR2CBOR(Ts5Range, TR5);
    PAR2CBOR(Ts5Dow, TD5);
    PAR2CBOR(Ts5IntervalIM, TI5);

    PAR2CBOR(Ts6Range, TR6);
    PAR2CBOR(Ts6Dow, TD6);
    PAR2CBOR(Ts6IntervalIM, TI6);

    PAR2CBOR(Ts7Range, TR7);
    PAR2CBOR(Ts7Dow, TD7);
    PAR2CBOR(Ts7IntervalIM, TI7);

    PAR2CBOR(Ts8Range, TR8);
    PAR2CBOR(Ts8Dow, TD8);
    PAR2CBOR(Ts8IntervalIM, TI8);

    PAR2CBOR(OutDoorEnable, ODE);

    PAR2CBOR(AccMoveDuration, AMD);
    PAR2CBOR(Ts1IntervalAM, TA1);
    PAR2CBOR(Ts2IntervalAM, TA2);
    PAR2CBOR(Ts3IntervalAM, TA3);
    PAR2CBOR(Ts4IntervalAM, TA4);
    PAR2CBOR(Ts5IntervalAM, TA5);
    PAR2CBOR(Ts6IntervalAM, TA6);
    PAR2CBOR(Ts7IntervalAM, TA7);
    PAR2CBOR(Ts8IntervalAM, TA8);
    PAR2CBOR(LogEnable, LOG);
}

#define DUMP(param,spcs)\
    printf(#param":"spcs" %d %c\n", gpsparams->param, gpsvld->vld##param ? 'C':' ');

void GpsParamsDump(const gpsparams_t * gpsparams, const gpsparams_valid_t * gpsvld)
{
    DUMP(PresenceTimeoutIdleSec, "   ");
    DUMP(PresenceTimeoutActSec, "    ");
    DUMP(PresenceHysteresisSec, "    ");
    printf("\n");

    DUMP(NotifyPresenceLost, "       ");
    DUMP(NotifyAccMove, "            ");
    DUMP(NotifySledEvents, "         ");
    DUMP(CoaleshTimeIdleMin, "       ");
    printf("\n");

    DUMP(MinGpsStrength, "           ");
    DUMP(GpsFixTimeoutSec, "         ");
    DUMP(GpsFixDelaySec, "           ");
    printf("\n");

    DUMP(CoaleshTimeActiveMin, "     ");
    DUMP(LteTimeoutSec, "            ");
    DUMP(LteTimeoutMaxRetry, "       ");
    DUMP(LteTimeoutPurgeMins, "      ");
    DUMP(LteConnOnNoFix, "           ");
    DUMP(GpsAidIntervalH, "          ");
    DUMP(GpsAidNumDays, "            ");
    DUMP(LteTimeoutDouble, "         ");
    printf("\n");

    DUMP(Ts1Range, "                 ");
    DUMP(Ts1Dow, "                   ");
    DUMP(Ts1IntervalIM, "            ");
    DUMP(Ts1IntervalAM, "            ");
    printf("\n");
    DUMP(Ts2Range, "                 ");
    DUMP(Ts2Dow, "                   ");
    DUMP(Ts2IntervalIM, "            ");
    DUMP(Ts2IntervalAM, "            ");
    printf("\n");
    DUMP(Ts3Range, "                 ");
    DUMP(Ts3Dow, "                   ");
    DUMP(Ts3IntervalIM, "            ");
    DUMP(Ts3IntervalAM, "            ");
    printf("\n");
    DUMP(Ts4Range, "                 ");
    DUMP(Ts4Dow, "                   ");
    DUMP(Ts4IntervalIM, "            ");
    DUMP(Ts4IntervalAM, "            ");
    printf("\n");
    DUMP(Ts5Range, "                 ");
    DUMP(Ts5Dow, "                   ");
    DUMP(Ts5IntervalIM, "            ");
    DUMP(Ts5IntervalAM, "            ");
    printf("\n");
    DUMP(Ts6Range, "                 ");
    DUMP(Ts6Dow, "                   ");
    DUMP(Ts6IntervalIM, "            ");
    DUMP(Ts6IntervalAM, "            ");
    printf("\n");
    DUMP(Ts7Range, "                 ");
    DUMP(Ts7Dow, "                   ");
    DUMP(Ts7IntervalIM, "            ");
    DUMP(Ts7IntervalAM, "            ");
    printf("\n");
    DUMP(Ts8Range, "                 ");
    DUMP(Ts8Dow, "                   ");
    DUMP(Ts8IntervalIM, "            ");
    DUMP(Ts8IntervalAM, "            ");
    printf("\n");
    DUMP(OutDoorEnable, "            ");
    
}


#define SET(param,value)\
    gpsparams->param = value;\
    gpsvld->vld##param = 1;

void GpsParamsTestFill(gpsparams_t * gpsparams, gpsparams_valid_t * gpsvld)
{
    SET(PresenceTimeoutIdleSec, 900);
    SET(PresenceTimeoutActSec, 900);
    SET(PresenceHysteresisSec, 300);

    SET(NotifyPresenceLost, 0);
    SET(NotifyAccMove, 0);
    SET(NotifySledEvents, 0);
    SET(CoaleshTimeIdleMin, 5);

    SET(MinGpsStrength, 35);
    SET(GpsFixTimeoutSec, 20);
    SET(GpsFixDelaySec, 10);

    SET(CoaleshTimeActiveMin, 5);
    SET(LteTimeoutSec, 20);
    SET(LteTimeoutMaxRetry, 5);
    SET(LteTimeoutPurgeMins, 2);
    SET(LteConnOnNoFix, 1);
    SET(GpsAidIntervalH, 168);
    SET(GpsAidNumDays, 3);
    SET(LteTimeoutDouble, 0);

    SET(Ts1Range, 0);   
    SET(Ts1Dow, 0);     
    SET(Ts1IntervalIM, 0);

    SET(Ts2Range, 0);   
    SET(Ts2Dow, 0);     
    SET(Ts2IntervalIM, 0);

    SET(Ts3Range, 0);   
    SET(Ts3Dow, 0);     
    SET(Ts3IntervalIM, 0);

    SET(Ts4Range, 0);   
    SET(Ts4Dow, 0);     
    SET(Ts4IntervalIM, 0);

    SET(Ts5Range, 0);   
    SET(Ts5Dow, 0);     
    SET(Ts5IntervalIM, 0);

    SET(Ts6Range, 0);   
    SET(Ts6Dow, 0);     
    SET(Ts6IntervalIM, 0);

    SET(Ts7Range, 0);   
    SET(Ts7Dow, 0);     
    SET(Ts7IntervalIM, 0);

    SET(Ts8Range, 0);   
    SET(Ts8Dow, 0);     
    SET(Ts8IntervalIM, 0);

    SET(OutDoorEnable, 0);

    SET(AccMoveDuration, 15);
    SET(Ts1IntervalAM, 0);
    SET(Ts2IntervalAM, 0);
    SET(Ts3IntervalAM, 0);  
    SET(Ts4IntervalAM, 0);
    SET(Ts5IntervalAM, 0);
    SET(Ts6IntervalAM, 0);
    SET(Ts7IntervalAM, 0);
    SET(Ts8IntervalAM, 0); 
    SET(LogEnable, 0);
}

int GpsParamsBase64Encode(const gpsparams_t * gpsparams,  char * buffer, int maxlen)
{
    // check maxlen is enough to hold base64 encoded gpsparams
    int needed_len = sizeof(gpsparams_t) / 3 * 4 + 1; // base64 encodes each 3 bytes into 4 chars, plus null terminator
    if (maxlen < needed_len) {
        LOG_ERR("Buffer too small for base64 encoding of gpsparams");
        return -1;
    }
    size_t olen;

    int ret = base64_encode(buffer, maxlen, &olen, (const uint8_t *)gpsparams, sizeof(gpsparams_t));
    if (ret < 0) {
        LOG_ERR("Base64 encoding failed");
        return -1;  
    }
    return ret; // return length of encoded string
}

#define IS_VALID(param)\
    if (gpsvld->vld##param)\
        return 1;\

 int GpsParamsHasValids(const gpsparams_valid_t * gpsvld)
{
    IS_VALID(PresenceTimeoutIdleSec)
    IS_VALID(PresenceTimeoutActSec)
    IS_VALID(PresenceHysteresisSec)

    IS_VALID(NotifyPresenceLost)
    IS_VALID(NotifyAccMove)
    IS_VALID(NotifySledEvents)
    IS_VALID(CoaleshTimeIdleMin)

    IS_VALID(MinGpsStrength)
    IS_VALID(GpsFixTimeoutSec)
    IS_VALID(GpsFixDelaySec)

    IS_VALID(CoaleshTimeActiveMin)
    IS_VALID(LteTimeoutSec)
    IS_VALID(LteTimeoutMaxRetry)
    IS_VALID(LteTimeoutPurgeMins)
    IS_VALID(LteConnOnNoFix)
    IS_VALID(GpsAidIntervalH)
    IS_VALID(GpsAidNumDays)
    IS_VALID(LteTimeoutDouble)

    IS_VALID(Ts1Range)
    IS_VALID(Ts1Dow)
    IS_VALID(Ts1IntervalIM)
    
    IS_VALID(Ts2Range)
    IS_VALID(Ts2Dow)
    IS_VALID(Ts2IntervalIM)

    IS_VALID(Ts3Range)
    IS_VALID(Ts3Dow)
    IS_VALID(Ts3IntervalIM)

    IS_VALID(Ts4Range)
    IS_VALID(Ts4Dow)
    IS_VALID(Ts4IntervalIM)

    IS_VALID(Ts5Range)
    IS_VALID(Ts5Dow)
    IS_VALID(Ts5IntervalIM)

    IS_VALID(Ts6Range)
    IS_VALID(Ts6Dow)
    IS_VALID(Ts6IntervalIM)

    IS_VALID(Ts7Range)
    IS_VALID(Ts7Dow)
    IS_VALID(Ts7IntervalIM)

    IS_VALID(Ts8Range)
    IS_VALID(Ts8Dow)
    IS_VALID(Ts8IntervalIM)

    IS_VALID(AccMoveDuration);
    IS_VALID(Ts1IntervalAM);
    IS_VALID(Ts2IntervalAM);
    IS_VALID(Ts3IntervalAM);  
    IS_VALID(Ts4IntervalAM);
    IS_VALID(Ts5IntervalAM);
    IS_VALID(Ts6IntervalAM);
    IS_VALID(Ts7IntervalAM);
    IS_VALID(Ts8IntervalAM); 
    IS_VALID(LogEnable);

    IS_VALID(OutDoorEnable)

    return 0;
}