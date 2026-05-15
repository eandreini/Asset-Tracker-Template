#ifndef _GPSPARAMS_H_
#define _GPSPARAMS_H_


#include <stdint.h>

typedef struct {
    uint16_t PresenceTimeoutIdleSec;
    uint16_t PresenceTimeoutActSec;
    uint16_t PresenceHysteresisSec;

    uint8_t NotifyPresenceLost;
    uint8_t NotifyAccMove;
    uint8_t AccMoveDuration;
    uint8_t NotifySledEvents;
    uint8_t CoaleshTimeIdleMin;

    uint8_t MinGpsStrength;
    uint8_t GpsFixTimeoutSec;
    uint8_t GpsFixDelaySec;

    uint8_t CoaleshTimeActiveMin;
    uint8_t LteTimeoutSec;
    uint16_t LteTimeoutMaxRetry;
    uint16_t LteTimeoutPurgeMins;
    uint8_t LteConnOnNoFix;
    uint8_t GpsAidIntervalH;
    uint8_t GpsAidNumDays;
    uint8_t LteTimeoutDouble;

    uint32_t Ts1Range;    // byte array hex HHMM from HHMM to in hex (ie 0x10452030)
    uint8_t Ts1Dow;        // day of week bitmask (bit 0 sun, bit 1 mon etc.)
    uint8_t Ts1IntervalIM;  // interval of fixes in minutes idle
    uint8_t Ts1IntervalAM;  // interval of fixes in minutes active

    uint32_t Ts2Range;    // byte array hex HHMM from HHMM to in hex (ie 0x10452030)
    uint8_t Ts2Dow;        // day of week bitmask (bit 0 sun, bit 1 mon etc.)
    uint8_t Ts2IntervalIM;  // interval of fixes in minutes idle
    uint8_t Ts2IntervalAM;  // interval of fixes in minutes active

    uint32_t Ts3Range;    // byte array hex HHMM from HHMM to in hex (ie 0x10452030)
    uint8_t Ts3Dow;        // day of week bitmask (bit 0 sun, bit 1 mon etc.)
    uint8_t Ts3IntervalIM;  // interval of fixes in minutes idle
    uint8_t Ts3IntervalAM;  // interval of fixes in minutes active

    uint32_t Ts4Range;    // byte array hex HHMM from HHMM to in hex (ie 0x10452030)
    uint8_t Ts4Dow;        // day of week bitmask (bit 0 sun, bit 1 mon etc.)
    uint8_t Ts4IntervalIM;  // interval of fixes in minutes idle
    uint8_t Ts4IntervalAM;  // interval of fixes in minutes active

    uint32_t Ts5Range;    // byte array hex HHMM from HHMM to in hex (ie 0x10452030)
    uint8_t Ts5Dow;        // day of week bitmask (bit 0 sun, bit 1 mon etc.)
    uint8_t Ts5IntervalIM;  // interval of fixes in minutes idle
    uint8_t Ts5IntervalAM;  // interval of fixes in minutes active

    uint32_t Ts6Range;    // byte array hex HHMM from HHMM to in hex (ie 0x10452030)
    uint8_t Ts6Dow;        // day of week bitmask (bit 0 sun, bit 1 mon etc.)
    uint8_t Ts6IntervalIM;  // interval of fixes in minutes idle
    uint8_t Ts6IntervalAM;  // interval of fixes in minutes active

    uint32_t Ts7Range;    // byte array hex HHMM from HHMM to in hex (ie 0x10452030)
    uint8_t Ts7Dow;        // day of week bitmask (bit 0 sun, bit 1 mon etc.)
    uint8_t Ts7IntervalIM;  // interval of fixes in minutes idle
    uint8_t Ts7IntervalAM;  // interval of fixes in minutes active

    uint32_t Ts8Range;    // byte array hex HHMM from HHMM to in hex (ie 0x10452030)
    uint8_t Ts8Dow;        // day of week bitmask (bit 0 sun, bit 1 mon etc.)
    uint8_t Ts8IntervalIM;  // interval of fixes in minutes idle
    uint8_t Ts8IntervalAM;  // interval of fixes in minutes active

    uint8_t OutDoorEnable;
    uint8_t LogEnable;

    uint8_t GpsParamsVersion;

} gpsparams_t;

typedef struct {
    uint32_t vldPresenceTimeoutIdleSec : 1;
    uint32_t vldPresenceTimeoutActSec : 1;
    uint32_t vldPresenceHysteresisSec : 1;

    uint32_t vldNotifyPresenceLost : 1;
    uint32_t vldNotifyAccMove : 1;
    uint32_t vldAccMoveDuration : 1;
    uint32_t vldNotifySledEvents : 1;
    uint32_t vldCoaleshTimeIdleMin : 1;

    uint32_t vldMinGpsStrength : 1;
    uint32_t vldGpsFixTimeoutSec : 1;
    uint32_t vldGpsFixDelaySec : 1;

    uint32_t vldCoaleshTimeActiveMin : 1;
    uint32_t vldLteTimeoutSec : 1;
    uint32_t vldLteTimeoutMaxRetry : 1;
    uint32_t vldLteTimeoutPurgeMins : 1;
    uint32_t vldLteConnOnNoFix : 1;
    uint32_t vldGpsAidIntervalH : 1;
    uint32_t vldGpsAidNumDays : 1;
    uint32_t vldLteTimeoutDouble : 1;

    uint32_t vldTs1Range : 1;    // byte array hex HHMM from HHMM to in hex (ie 0x10452030)
    uint32_t vldTs1Dow : 1;        // day of week bitmask (bit 0 sun, bit 1 mon etc.)
    uint32_t vldTs1IntervalIM : 1;  // interval of fixes in minutes idle
    uint32_t vldTs1IntervalAM : 1;  // interval of fixes in minutes active

    uint32_t vldTs2Range : 1;    // byte array hex HHMM from HHMM to in hex (ie 0x10452030)
    uint32_t vldTs2Dow : 1;        // day of week bitmask (bit 0 sun, bit 1 mon etc.)
    uint32_t vldTs2IntervalIM : 1;  // interval of fixes in minutes idle
    uint32_t vldTs2IntervalAM : 1;  // interval of fixes in minutes active

    uint32_t vldTs3Range : 1;    // byte array hex HHMM from HHMM to in hex (ie 0x10452030)
    uint32_t vldTs3Dow : 1;        // day of week bitmask (bit 0 sun, bit 1 mon etc.)
    uint32_t vldTs3IntervalIM : 1;  // interval of fixes in minutes idle
    uint32_t vldTs3IntervalAM : 1;  // interval of fixes in minutes active

    uint32_t vldTs4Range : 1;    // byte array hex HHMM from HHMM to in hex (ie 0x10452030)
    uint32_t vldTs4Dow : 1;        // day of week bitmask (bit 0 sun, bit 1 mon etc.)
    uint32_t vldTs4IntervalIM : 1;  // interval of fixes in minutes idle
    uint32_t vldTs4IntervalAM : 1;  // interval of fixes in minutes active

    uint32_t vldTs5Range : 1;    // byte array hex HHMM from HHMM to in hex (ie 0x10452030)
    uint32_t vldTs5Dow : 1;        // day of week bitmask (bit 0 sun, bit 1 mon etc.)
    uint32_t vldTs5IntervalIM : 1;  // interval of fixes in minutes idle
    uint32_t vldTs5IntervalAM : 1;  // interval of fixes in minutes active

    uint32_t vldTs6Range : 1;    // byte array hex HHMM from HHMM to in hex (ie 0x10452030)
    uint32_t vldTs6Dow : 1;        // day of week bitmask (bit 0 sun, bit 1 mon etc.)
    uint32_t vldTs6IntervalIM : 1;  // interval of fixes in minutes idle
    uint32_t vldTs6IntervalAM : 1;  // interval of fixes in minutes active

    uint32_t vldTs7Range : 1;    // byte array hex HHMM from HHMM to in hex (ie 0x10452030)
    uint32_t vldTs7Dow : 1;        // day of week bitmask (bit 0 sun, bit 1 mon etc.)
    uint32_t vldTs7IntervalIM : 1;  // interval of fixes in minutes idle
    uint32_t vldTs7IntervalAM : 1;  // interval of fixes in minutes active

    uint32_t vldTs8Range : 1;    // byte array hex HHMM from HHMM to in hex (ie 0x10452030)
    uint32_t vldTs8Dow : 1;        // day of week bitmask (bit 0 sun, bit 1 mon etc.)
    uint32_t vldTs8IntervalIM : 1;  // interval of fixes in minutes idle
    uint32_t vldTs8IntervalAM : 1;  // interval of fixes in minutes active

    uint32_t vldOutDoorEnable : 1;
    uint32_t vldLogEnable : 1;

    uint32_t vldGpsParamsVersion : 1;

} gpsparams_valid_t;


struct shadow_object;


void GpsParamsUpdateValid(const gpsparams_t * gpsparams, const gpsparams_valid_t * gpsvld);
void GpsParamsGetAll(gpsparams_t * gpsparams, gpsparams_valid_t * gpsvld);
void GpsParamsDecodeFromCbor (const struct shadow_object * shadow, gpsparams_t * gpsparams, gpsparams_valid_t * gpsvld);
void GpsParamsEncodeToCbor (const gpsparams_t * gpsparams, const gpsparams_valid_t * gpsvld, struct shadow_object * shadow);

void GpsParamsDump(const gpsparams_t * gpsparams, const gpsparams_valid_t * gpsvld);
void GpsParamsTestFill(gpsparams_t * gpsparams, gpsparams_valid_t * gpsvld);

int GpsParamsHasValids(const gpsparams_valid_t * gpsvld);
int GpsParamsBase64Encode(const gpsparams_t * gpsparams,  char * buffer, int maxlen);


extern gpsparams_t g_gpsparams;
extern gpsparams_valid_t g_gpsparams_vld;

#endif //_GPSPARAMS_H_