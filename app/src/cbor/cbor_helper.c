/*
 * Copyright (c) 2025 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */
#include <errno.h>
#include <string.h>
#include <zcbor_encode.h>

#include "cbor_helper.h"
#include "device_shadow_types.h"
#include "device_shadow_decode.h"
#include "device_shadow_encode.h"

#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(cbor_helper, CONFIG_APP_LOG_LEVEL);

#define CBOR2PAR(srcname,dstname)\
	if (shadow.config.srcname##_present) {\
		config->gpschgd->chgd##dstname = 1;\
		config->gpsparams->dstname = shadow.config.srcname.srcname;\
	}\
	else\
		config->gpschgd->chgd##dstname = 0;\

#define PAR2CBOR(srcname,dstname)\
	if (config->gpschgd->chgd##srcname) {\
		shadow.config_present = true;\
		shadow.config.dstname##_present = true;\
		shadow.config.dstname.dstname = config->gpsparams->srcname;\
	}

int decode_shadow_parameters_from_cbor(const uint8_t *cbor,
				       size_t len,
				       struct gps_config_params *config,
				       uint32_t *command_type,
				       uint32_t *command_id)
{
	int err;
	struct shadow_object shadow = { 0 };
	size_t decode_len = len;

	if (!cbor || !config || !command_type || len == 0) {
		LOG_ERR("Invalid input");
		return -EINVAL;
	}

	err = cbor_decode_shadow_object(cbor, decode_len, &shadow, &decode_len);
	if (err) {
		LOG_ERR("cbor_decode_shadow_object, error: %d", err);
		LOG_HEXDUMP_ERR(cbor, len, "Unexpected CBOR data");
		return -EFAULT;
	}

	if (shadow.config_present) {
		if (shadow.config.update_interval_present) {
			config->update_interval = shadow.config.update_interval.update_interval;
			LOG_DBG("Configuration: Decoded update_interval = %d seconds",
				config->update_interval);
		}

		if (shadow.config.sample_interval_present) {
			config->sample_interval = shadow.config.sample_interval.sample_interval;
			LOG_DBG("Configuration: Decoded sample_interval = %d seconds",
				config->sample_interval);
		}

		if (shadow.config.buffer_mode_present) {
			config->buffer_mode = shadow.config.buffer_mode.buffer_mode;
			config->buffer_mode_valid = true;
			LOG_DBG("Configuration: Decoded buffer_mode = %s",
				config->buffer_mode ? "enabled" : "disabled");
		}

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

	}

	if (shadow.command_present) {
		*command_type = shadow.command.type;
		*command_id = shadow.command.id;

		LOG_DBG("Command parameter present: type=%u, id=%u",
			*command_type, *command_id);

	}

	return 0;
}

int encode_shadow_parameters_to_cbor(const struct gps_config_params *config,
				     uint32_t command_type,
				     uint32_t command_id,
				     uint8_t *buffer,
				     size_t buffer_size,
				     size_t *encoded_len)
{
	int err;
	struct shadow_object shadow = { 0 };
	size_t encode_len;

	if (!config || !buffer || !encoded_len || buffer_size == 0) {
		return -EINVAL;
	}

	/* Build shadow object with config section */
	if (config->update_interval > 0) {
		shadow.config_present = true;
		shadow.config.update_interval_present = true;
		shadow.config.update_interval.update_interval = config->update_interval;
	}

	if (config->sample_interval > 0) {
		shadow.config_present = true;
		shadow.config.sample_interval_present = true;
		shadow.config.sample_interval.sample_interval = config->sample_interval;
	}

	if (config->buffer_mode_valid) {
		shadow.config_present = true;
		shadow.config.buffer_mode_present = true;
		shadow.config.buffer_mode.buffer_mode = config->buffer_mode;
	}

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


	/* Build shadow object with command section */
	if (command_type > 0) {
		shadow.command_present = true;
		shadow.command.type = command_type;
		shadow.command.id = command_id;
	}

	/* Encode the shadow object to CBOR */
	err = cbor_encode_shadow_object(buffer, buffer_size, &shadow, &encode_len);
	if (err) {
		LOG_ERR("cbor_encode_shadow_object, error: %d", err);
		return (err == ZCBOR_ERR_NO_PAYLOAD) ? -ENOMEM : -EFAULT;
	}

	*encoded_len = encode_len;

	return 0;
}
