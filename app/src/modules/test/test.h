#ifndef _TEST_H_
#define _TEST_H_

#include <zephyr/kernel.h>
#include <zephyr/zbus/zbus.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Module's zbus channel */
ZBUS_CHAN_DECLARE(TEST_CHAN);

/* Module message types */
enum test_msg_type {
    /* Output message types */
    TEST_RESPONSE = 0x1,
    TEST_STARTAPP,

    /* Input message types */
    TEST_REQUEST,

};

/* Module message structure */
struct test_msg {
    enum test_msg_type type;
    int32_t value;
};

#define MSG_TO_TEST_TYPE(_msg) (*(const struct test_msg *)_msg)

#ifdef __cplusplus
}
#endif

#endif /* _DUMMY_H_ */