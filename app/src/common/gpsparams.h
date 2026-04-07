#ifndef _GPSPARAMS_H_
#define _GPSPARAMS_H_


#include <stdint.h>

typedef struct {
    uint16_t PresenceTimeoutIdleSec;
    uint16_t PresenceTimeoutActSec;
    uint16_t PresenceHysteresisSec;

    uint8_t NotifyPresenceLost;
    uint8_t NotifyAccMove;
    uint8_t NotifySledEvents;
    uint8_t CoaleshTimeMin;

    uint8_t MinGpsStrength;
    uint8_t GpsFixTimeoutSec;
    uint8_t GpsFixDelaySec;
    uint8_t dummy1;

    uint8_t LteMinStrenght;
    uint8_t LteTimeoutSec;
    uint16_t LteOnDemandFreqMin;
    uint16_t LteOnDemandOffsetMin;
    uint8_t LteConnOnNoFix;
    uint8_t GpsAidIntervalH;
    uint8_t GpsAidNumDays;
    uint8_t GpsAidOnlyM1;

    uint32_t Ts1Range;    // byte array hex HHMM from HHMM to in hex (ie 0x10452030)
    uint32_t Ts2Range;    // byte array hex HHMM from HHMM to in hex (ie 0x10452030)
    uint32_t Ts3Range;    // byte array hex HHMM from HHMM to in hex (ie 0x10452030)
    uint32_t Ts4Range;    // byte array hex HHMM from HHMM to in hex (ie 0x10452030)
    uint32_t Ts5Range;    // byte array hex HHMM from HHMM to in hex (ie 0x10452030)
    uint32_t Ts6Range;    // byte array hex HHMM from HHMM to in hex (ie 0x10452030)
    uint32_t Ts7Range;    // byte array hex HHMM from HHMM to in hex (ie 0x10452030)
    uint32_t Ts8Range;    // byte array hex HHMM from HHMM to in hex (ie 0x10452030)

    uint8_t Ts1Dow;        // day of week bitmask (bit 0 sun, bit 1 mon etc.)
    uint8_t Ts2Dow;        // day of week bitmask (bit 0 sun, bit 1 mon etc.)
    uint8_t Ts3Dow;        // day of week bitmask (bit 0 sun, bit 1 mon etc.)
    uint8_t Ts4Dow;        // day of week bitmask (bit 0 sun, bit 1 mon etc.)
    uint8_t Ts5Dow;        // day of week bitmask (bit 0 sun, bit 1 mon etc.)
    uint8_t Ts6Dow;        // day of week bitmask (bit 0 sun, bit 1 mon etc.)
    uint8_t Ts7Dow;        // day of week bitmask (bit 0 sun, bit 1 mon etc.)
    uint8_t Ts8Dow;        // day of week bitmask (bit 0 sun, bit 1 mon etc.)

    uint8_t Ts1IntervalM;  // interval of fixes in minutes
    uint8_t Ts2IntervalM;  // interval of fixes in minutes
    uint8_t Ts3IntervalM;  // interval of fixes in minutes
    uint8_t Ts4IntervalM;  // interval of fixes in minutes
    uint8_t Ts5IntervalM;  // interval of fixes in minutes
    uint8_t Ts6IntervalM;  // interval of fixes in minutes
    uint8_t Ts7IntervalM;  // interval of fixes in minutes
    uint8_t Ts8IntervalM;  // interval of fixes in minutes

    uint8_t Ode;

} gpsparams_t;

typedef struct {
    uint32_t chgdPresenceTimeoutIdleSec : 1;
    uint32_t chgdPresenceTimeoutActSec : 1;
    uint32_t chgdPresenceHysteresisSec : 1;

    uint32_t chgdNotifyPresenceLost : 1;
    uint32_t chgdNotifyAccMove : 1;
    uint32_t chgdNotifySledEvents : 1;
    uint32_t chgdCoaleshTimeMin : 1;

    uint32_t chgdMinGpsStrength : 1;
    uint32_t chgdGpsFixTimeoutSec : 1;
    uint32_t chgdGpsFixDelaySec : 1;

    uint32_t chgdLteMinStrenght : 1;
    uint32_t chgdLteTimeoutSec : 1;
    uint32_t chgdLteOnDemandFreqMin : 1;
    uint32_t chgdLteOnDemandOffsetMin : 1;
    uint32_t chgdLteConnOnNoFix : 1;
    uint32_t chgdGpsAidIntervalH : 1;
    uint32_t chgdGpsAidNumDays : 1;
    uint32_t chgdGpsAidOnlyM1 : 1;

    uint32_t chgdTs1Range : 1;    // byte array hex HHMM from HHMM to in hex (ie 0x10452030)
    uint32_t chgdTs1Dow : 1;        // day of week bitmask (bit 0 sun, bit 1 mon etc.)
    uint32_t chgdTs1IntervalM : 1;  // interval of fixes in minutes

    uint32_t chgdTs2Range : 1;    // byte array hex HHMM from HHMM to in hex (ie 0x10452030)
    uint32_t chgdTs2Dow : 1;        // day of week bitmask (bit 0 sun, bit 1 mon etc.)
    uint32_t chgdTs2IntervalM : 1;  // interval of fixes in minutes

    uint32_t chgdTs3Range : 1;    // byte array hex HHMM from HHMM to in hex (ie 0x10452030)
    uint32_t chgdTs3Dow : 1;        // day of week bitmask (bit 0 sun, bit 1 mon etc.)
    uint32_t chgdTs3IntervalM : 1;  // interval of fixes in minutes

    uint32_t chgdTs4Range : 1;    // byte array hex HHMM from HHMM to in hex (ie 0x10452030)
    uint32_t chgdTs4Dow : 1;        // day of week bitmask (bit 0 sun, bit 1 mon etc.)
    uint32_t chgdTs4IntervalM : 1;  // interval of fixes in minutes

    uint32_t chgdTs5Range : 1;    // byte array hex HHMM from HHMM to in hex (ie 0x10452030)
    uint32_t chgdTs5Dow : 1;        // day of week bitmask (bit 0 sun, bit 1 mon etc.)
    uint32_t chgdTs5IntervalM : 1;  // interval of fixes in minutes

    uint32_t chgdTs6Range : 1;    // byte array hex HHMM from HHMM to in hex (ie 0x10452030)
    uint32_t chgdTs6Dow : 1;        // day of week bitmask (bit 0 sun, bit 1 mon etc.)
    uint32_t chgdTs6IntervalM : 1;  // interval of fixes in minutes

    uint32_t chgdTs7Range : 1;    // byte array hex HHMM from HHMM to in hex (ie 0x10452030)
    uint32_t chgdTs7Dow : 1;        // day of week bitmask (bit 0 sun, bit 1 mon etc.)
    uint32_t chgdTs7IntervalM : 1;  // interval of fixes in minutes

    uint32_t chgdTs8Range : 1;    // byte array hex HHMM from HHMM to in hex (ie 0x10452030)
    uint32_t chgdTs8Dow : 1;        // day of week bitmask (bit 0 sun, bit 1 mon etc.)
    uint32_t chgdTs8IntervalM : 1;  // interval of fixes in minutes

    uint32_t chgdOde : 1;

} gpsparams_chgd_t;


struct shadow_object;


void GpsParamsDecodeFromCbor (const struct shadow_object * shadow, gpsparams_t * gpsparams, gpsparams_chgd_t * gpschgd);
void GpsParamsEncodeToCbor (const gpsparams_t * gpsparams, const gpsparams_chgd_t * gpschgd, struct shadow_object * shadow);
void GpsParamsSetChanged(const gpsparams_t * gpsparams, const gpsparams_chgd_t * gpschgd);
void GpsParamsGetChanged(gpsparams_t * gpsparams, gpsparams_chgd_t * gpschgd);
void GpsParamsGetAll(gpsparams_t * gpsparams, gpsparams_chgd_t * gpschgd);

void GpsParamsDump();
void GpsParamsTestFill();
int GpsParamsSetValue(const char * name, int value);

typedef void gpsparams_flush_cb_t(const char * str);
int GpsParamsFlushChanged(char * buffer, int maxlen, gpsparams_flush_cb_t cb);
int GpsParamsIsChanged(const gpsparams_chgd_t * gpschgd);
int GpsParamsClearChanged();


extern gpsparams_t g_gpsparams;
extern gpsparams_chgd_t g_gpsparams_chgd;

#endif //_GPSPARAMS_H_