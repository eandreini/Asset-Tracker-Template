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

    uint8_t LteMinStrenght;
    uint8_t LteTimeoutSec;
    uint16_t LteOnDemandFreqMin;
    uint16_t LteOnDemandOffsetMin;
    uint8_t LteConnOnNoFix;
    uint8_t GpsAidIntervalH;
    uint8_t GpsAidNumDays;
    uint8_t GpsAidOnlyM1;

    uint32_t Ts1Range;    // byte array hex HHMM from HHMM to in hex (ie 0x10452030)
    uint8_t Ts1Dow;        // day of week bitmask (bit 0 sun, bit 1 mon etc.)
    uint8_t Ts1IntervalM;  // interval of fixes in minutes

    uint32_t Ts2Range;    // byte array hex HHMM from HHMM to in hex (ie 0x10452030)
    uint8_t Ts2Dow;        // day of week bitmask (bit 0 sun, bit 1 mon etc.)
    uint8_t Ts2IntervalM;  // interval of fixes in minutes

    uint32_t Ts3Range;    // byte array hex HHMM from HHMM to in hex (ie 0x10452030)
    uint8_t Ts3Dow;        // day of week bitmask (bit 0 sun, bit 1 mon etc.)
    uint8_t Ts3IntervalM;  // interval of fixes in minutes

    uint32_t Ts4Range;    // byte array hex HHMM from HHMM to in hex (ie 0x10452030)
    uint8_t Ts4Dow;        // day of week bitmask (bit 0 sun, bit 1 mon etc.)
    uint8_t Ts4IntervalM;  // interval of fixes in minutes

    uint32_t Ts5Range;    // byte array hex HHMM from HHMM to in hex (ie 0x10452030)
    uint8_t Ts5Dow;        // day of week bitmask (bit 0 sun, bit 1 mon etc.)
    uint8_t Ts5IntervalM;  // interval of fixes in minutes

    uint32_t Ts6Range;    // byte array hex HHMM from HHMM to in hex (ie 0x10452030)
    uint8_t Ts6Dow;        // day of week bitmask (bit 0 sun, bit 1 mon etc.)
    uint8_t Ts6IntervalM;  // interval of fixes in minutes

    uint32_t Ts7Range;    // byte array hex HHMM from HHMM to in hex (ie 0x10452030)
    uint8_t Ts7Dow;        // day of week bitmask (bit 0 sun, bit 1 mon etc.)
    uint8_t Ts7IntervalM;  // interval of fixes in minutes

    uint32_t Ts8Range;    // byte array hex HHMM from HHMM to in hex (ie 0x10452030)
    uint8_t Ts8Dow;        // day of week bitmask (bit 0 sun, bit 1 mon etc.)
    uint8_t Ts8IntervalM;  // interval of fixes in minutes

} gpsparams_t;

typedef struct {
    uint8_t chgdPresenceTimeoutIdleSec;
    uint8_t chgdPresenceTimeoutActSec;
    uint8_t chgdPresenceHysteresisSec;

    uint8_t chgdNotifyPresenceLost;
    uint8_t chgdNotifyAccMove;
    uint8_t chgdNotifySledEvents;
    uint8_t chgdCoaleshTimeMin;

    uint8_t chgdMinGpsStrength;
    uint8_t chgdGpsFixTimeoutSec;
    uint8_t chgdGpsFixDelaySec;

    uint8_t chgdLteMinStrenght;
    uint8_t chgdLteTimeoutSec;
    uint8_t chgdLteOnDemandFreqMin;
    uint8_t chgdLteOnDemandOffsetMin;
    uint8_t chgdLteConnOnNoFix;
    uint8_t chgdGpsAidIntervalH;
    uint8_t chgdGpsAidNumDays;
    uint8_t chgdGpsAidOnlyM1;

    uint8_t chgdTs1Range;    // byte array hex HHMM from HHMM to in hex (ie 0x10452030)
    uint8_t chgdTs1Dow;        // day of week bitmask (bit 0 sun, bit 1 mon etc.)
    uint8_t chgdTs1IntervalM;  // interval of fixes in minutes

    uint8_t chgdTs2Range;    // byte array hex HHMM from HHMM to in hex (ie 0x10452030)
    uint8_t chgdTs2Dow;        // day of week bitmask (bit 0 sun, bit 1 mon etc.)
    uint8_t chgdTs2IntervalM;  // interval of fixes in minutes

    uint8_t chgdTs3Range;    // byte array hex HHMM from HHMM to in hex (ie 0x10452030)
    uint8_t chgdTs3Dow;        // day of week bitmask (bit 0 sun, bit 1 mon etc.)
    uint8_t chgdTs3IntervalM;  // interval of fixes in minutes

    uint8_t chgdTs4Range;    // byte array hex HHMM from HHMM to in hex (ie 0x10452030)
    uint8_t chgdTs4Dow;        // day of week bitmask (bit 0 sun, bit 1 mon etc.)
    uint8_t chgdTs4IntervalM;  // interval of fixes in minutes

    uint8_t chgdTs5Range;    // byte array hex HHMM from HHMM to in hex (ie 0x10452030)
    uint8_t chgdTs5Dow;        // day of week bitmask (bit 0 sun, bit 1 mon etc.)
    uint8_t chgdTs5IntervalM;  // interval of fixes in minutes

    uint8_t chgdTs6Range;    // byte array hex HHMM from HHMM to in hex (ie 0x10452030)
    uint8_t chgdTs6Dow;        // day of week bitmask (bit 0 sun, bit 1 mon etc.)
    uint8_t chgdTs6IntervalM;  // interval of fixes in minutes

    uint8_t chgdTs7Range;    // byte array hex HHMM from HHMM to in hex (ie 0x10452030)
    uint8_t chgdTs7Dow;        // day of week bitmask (bit 0 sun, bit 1 mon etc.)
    uint8_t chgdTs7IntervalM;  // interval of fixes in minutes

    uint8_t chgdTs8Range;    // byte array hex HHMM from HHMM to in hex (ie 0x10452030)
    uint8_t chgdTs8Dow;        // day of week bitmask (bit 0 sun, bit 1 mon etc.)
    uint8_t chgdTs8IntervalM;  // interval of fixes in minutes

} gpsparams_chgd_t;


void GpsParamsDump();
void GpsParamsTestFill();

extern gpsparams_t g_gpsparams;
extern gpsparams_chgd_t g_gpsparams_chgd;

#endif //_GPSPARAMS_H_