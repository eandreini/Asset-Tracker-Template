#ifndef _GMTRACK_H_
#define _GMTRACKH_

#include <zephyr/kernel.h>
#include <zephyr/zbus/zbus.h>

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
};

struct gmtrack_poll_msg {
    uint8_t type;
    uint8_t len;
    uint8_t p1;
    uint8_t p2;
    char data[60];
};

#define MSG_TO_GMTRACK_TYPE(_msg) (*(const struct gmtrack_msg *)_msg)

#ifdef __cplusplus
}
#endif

#endif /* _DUMMY_H_ */