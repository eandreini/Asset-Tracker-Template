#ifndef _GMTRACK_H_
#define _GMTRACKH_

#include <zephyr/kernel.h>
#include <zephyr/zbus/zbus.h>
#include "network.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Module's zbus channel */
ZBUS_CHAN_DECLARE(GMTRACK_CHAN);

/* Module message types */
enum gmtrack_msg_type {
    /* Output message types */
    GMTRACK_RESPONSE = 0x1,

    /* Input message types */
    GMTRACK_REQUEST,
    GMTRACK_CONFIG_CHG,

    /* internal events*/
    GMTRACK_SUSPEND_FLASH,
    GMTRACK_RESUME_FLASH,

    GMTRACK_MSG_TYPE_COUNT,

    GMTRACK_LOCGNSS_PGPS_REQUESTED,
    GMTRACK_LOCGNSS_PGPS_READY,

};

/* Module message structure */
struct gmtrack_msg {
    enum gmtrack_msg_type type;
    int32_t value;
};

enum gmtrack_poll_type {
    gmpoll_empty = 0,
    gmpoll_text = 1,
    gmpoll_config = 2,
    gmpoll_ready = 3,
    gmpoll_step = 4,
};

enum gmtrack_channel_p1 {
    gmstep_cloud = 1,
    gmstep_fota = 2,
    gmstep_location = 3,
    gmstep_network = 4,
    gmstep_storage = 5,
    gmstep_gmtrack = 6,
};



struct gmtrack_poll_msg {
    uint8_t type;
    uint8_t len;
    uint8_t p1;
    uint8_t p2;
    char data[60];
};


typedef struct gmtrack_info {
    uint64_t sled;
    uint64_t mac;
    int battlevel;
} gmtrack_info_t;

extern gmtrack_info_t g_gmtrack_info;

int gmtrack_uart_disable(int uart);
int gmtrack_uart_enable(int uart);
void gmtrack_led_set(int value);
void gmtrack_flush_cfgchg();
int gmtrack_flash_enable(bool enable);
int gmtrack_send_network_message(enum network_msg_type type);


#ifdef __cplusplus
}
#endif

#endif /* _DUMMY_H_ */