#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/zbus/zbus.h>
#include <zephyr/task_wdt/task_wdt.h>
#include <zephyr/smf.h>
#include <nrf_modem_at.h>

#include "app_common.h"
#include "test.h"

/* Register log module */
LOG_MODULE_REGISTER(test_module, CONFIG_APP_TEST_LOG_LEVEL);

/* Define module's zbus channel */
ZBUS_CHAN_DEFINE(TEST_CHAN,
                 struct test_msg,
                 NULL,
                 NULL,
                 ZBUS_OBSERVERS_EMPTY,
                 ZBUS_MSG_INIT(0)
);


/* Register zbus subscriber */
ZBUS_MSG_SUBSCRIBER_DEFINE(test);

/* Add subscriber to channel */
ZBUS_CHAN_ADD_OBS(TEST_CHAN, test, 0);

#define MAX_MSG_SIZE sizeof(struct test_msg)

BUILD_ASSERT(CONFIG_APP_TEST_WATCHDOG_TIMEOUT_SECONDS >
             CONFIG_APP_TEST_MSG_PROCESSING_TIMEOUT_SECONDS,
             "Watchdog timeout must be greater than maximum message processing time");

/* State machine states */
enum trdy_module_state {
    STATE_RUNNING,
};

/* Module state structure */
struct test_state {
    /* State machine context (must be first) */
    struct smf_ctx ctx;

    /* Last received zbus channel */
    const struct zbus_channel *chan;

    /* Message buffer */
    uint8_t msg_buf[MAX_MSG_SIZE];

    /* Current counter value */
    int32_t current_value;
};

/* Forward declarations */
static enum smf_state_result state_running_run(void *o);

/* State machine definition */
static const struct smf_state states[] = {
    [STATE_RUNNING] = SMF_CREATE_STATE(NULL, state_running_run, NULL, NULL, NULL),
};

/* Watchdog callback */
static void task_wdt_callback(int channel_id, void *user_data)
{
    LOG_ERR("Watchdog expired, Channel: %d, Thread: %s",
            channel_id, k_thread_name_get((k_tid_t)user_data));

    SEND_FATAL_ERROR_WATCHDOG_TIMEOUT();
}

static void run_at_cmds()
{
   	int err;
    err = nrf_modem_at_printf("AT+CFUN=41");
    if (err != 0) {
        LOG_ERR ("Unable to activate modem for reading info");
        return;
    }
    k_msleep(1000);
    char data[256];
    LOG_INF("***NRF9151 MODEM INFO BEGIN***");
    err = nrf_modem_at_scanf("AT%XICCID", "%%XICCID: %s", data); 
    if (err < 1) {
        LOG_ERR ("Unable to read ICCID rv: %d", err);
        return;
    }
    printf("$NRFRESP$: ICCID=%s\r\n", data);

    err = nrf_modem_at_scanf("AT%ATTESTTOKEN", "%%ATTESTTOKEN: %s", data); 
    if (err < 1) {
        LOG_ERR ("Unable to read TOKEN");
        return;
    }
    printf("$NRFRESP$: TOKEN=%s\r\n", data);

    err = nrf_modem_at_scanf("AT%DEVICEUUID", "%%DEVICEUUID: %s", data); 
    if (err < 1) {
        LOG_ERR ("Unable to read UUID");
        return;
    }
    printf("$NRFRESP$: UUID=%s\r\n", data);

    err = nrf_modem_at_scanf("AT+CGMR", "%s", data); 
    if (err < 1) {
        LOG_ERR ("Unable to read MODEMREV");
        return;
    }
    printf("$NRFRESP$: MODEMREV=%s\r\n", data);

    err = nrf_modem_at_scanf("AT%2DID", "%%2DID: %s", data); 
    if (err < 1) {
        LOG_ERR ("Unable to read 2D ID");
        return;
    }
    printf("$NRFRESP$: 2DID=%s\r\n", data);
    LOG_INF("***NRF9151 MODEM INFO END***");

}

/* State machine handlers */
static enum smf_state_result state_running_run(void *o)
{
    struct test_state *state_object = (struct test_state *)o;

    if (&TEST_CHAN == state_object->chan) {
        struct test_msg msg = MSG_TO_TEST_TYPE(state_object->msg_buf);

        if (msg.type == TEST_REQUEST) {
            LOG_INF("Received test request");
            run_at_cmds();

            state_object->current_value++;

            struct test_msg response = {
                .type = TEST_RESPONSE,
                .value = state_object->current_value
            };

            int err = zbus_chan_pub(&TEST_CHAN, &response, K_NO_WAIT);
            if (err) {
                LOG_ERR("Failed to publish response: %d", err);
                SEND_FATAL_ERROR();
                return SMF_EVENT_PROPAGATE;
            }
        }
    }

    return SMF_EVENT_PROPAGATE;
}

/* Module task function */
static void test_task(void)
{
    int err;
    int task_wdt_id;
    const uint32_t wdt_timeout_ms =
        (CONFIG_APP_TEST_WATCHDOG_TIMEOUT_SECONDS * MSEC_PER_SEC);
    const uint32_t execution_time_ms =
        (CONFIG_APP_TEST_MSG_PROCESSING_TIMEOUT_SECONDS * MSEC_PER_SEC);
    const k_timeout_t zbus_wait_ms = K_MSEC(wdt_timeout_ms - execution_time_ms);
    struct test_state test_state = {
        .current_value = 0
    };

    LOG_DBG("Starting test module task");
    printf ("$NRFRESP$: READY\r\n");

    task_wdt_id = task_wdt_add(wdt_timeout_ms, task_wdt_callback, (void *)k_current_get());

    smf_set_initial(SMF_CTX(&test_state), &states[STATE_RUNNING]);


    while (true) {
        err = task_wdt_feed(task_wdt_id);
        if (err) {
            LOG_ERR("Failed to feed watchdog: %d", err);
            SEND_FATAL_ERROR();
            return;
        }

        err = zbus_sub_wait_msg(&test,
                               &test_state.chan,
                               test_state.msg_buf,
                               zbus_wait_ms);
        if (err == -ENOMSG) {
            continue;
        } else if (err) {
            LOG_ERR("Failed to wait for message: %d", err);
            SEND_FATAL_ERROR();
            return;
        }

        err = smf_run_state(SMF_CTX(&test_state));
        if (err) {
            LOG_ERR("Failed to run state machine: %d", err);
            SEND_FATAL_ERROR();
            return;
        }
    }
}

/* Define module thread */
K_THREAD_DEFINE(test_task_id,
                CONFIG_APP_TEST_THREAD_STACK_SIZE,
                test_task, NULL, NULL, NULL,
                K_LOWEST_APPLICATION_THREAD_PRIO, 0, 0);