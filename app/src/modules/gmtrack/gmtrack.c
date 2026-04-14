#include <stdint.h>
#include <stdlib.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/zbus/zbus.h>
#include <zephyr/task_wdt/task_wdt.h>
#include <zephyr/smf.h>
#include <nrf_modem_at.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/shell/shell.h>
#include "app_common.h"
#include "gmtrack.h"
#include <zephyr/pm/device.h>
#include "gpsparams.h"
#include <date_time.h>
#include "network.h"
#include "cloud.h"
#include "location.h"   
#include "fota.h"
#include "storage.h"
#include "button.h"

void method_gnss_register_notify(void (*notify_fn)(int type, const char * msg));
void date_time_core_store(int64_t curr_time_ms, enum date_time_evt_type time_source);

#include <zephyr/pm/device.h>
#include <zephyr/pm/device_runtime.h>
#include <zephyr/pm/pm.h>

gmtrack_info_t g_gmtrack_info;

#define DISABLE_UART1_AT_POWERUP 0

/* functions added in main to trigger timer messages */

void start_timer_fun(struct k_timer *timer_id);
void poll_test_fun(struct k_timer *timer_id);
static void signal_ready();
static bool g_enable_event_send = false;


K_TIMER_DEFINE(g_start_timer, start_timer_fun, NULL);
K_MSGQ_DEFINE(g_poll_msgq, sizeof(struct gmtrack_poll_msg), 32, 4);
K_TIMER_DEFINE(g_test_timer, poll_test_fun, NULL);


static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(DT_ALIAS(led0), gpios);
static const struct device *gpio_dev = DEVICE_DT_GET(DT_NODELABEL(gpio0));
static const struct device *flash_dev = DEVICE_DT_GET(DT_NODELABEL(gd25wb256));


static struct gpio_callback gp14_event_cb;
static const struct device *const uart0_dev = DEVICE_DT_GET(DT_NODELABEL(uart0));
static const struct device *const uart1_dev = DEVICE_DT_GET(DT_NODELABEL(uart1));



/* Register log module */
LOG_MODULE_REGISTER(gmtrack_module, CONFIG_APP_GMTRACK_LOG_LEVEL);

/* Define module's zbus channel */
ZBUS_CHAN_DEFINE(GMTRACK_CHAN,
                 struct gmtrack_msg,
                 NULL,
                 NULL,
                 ZBUS_OBSERVERS_EMPTY,
                 ZBUS_MSG_INIT(0));

/* Register zbus subscriber */
ZBUS_MSG_SUBSCRIBER_DEFINE(gmtrack);


/* Add subscriber to channel */
ZBUS_CHAN_ADD_OBS(GMTRACK_CHAN, gmtrack, 0);

ZBUS_CHAN_ADD_OBS(network_chan, gmtrack, 0);
ZBUS_CHAN_ADD_OBS(cloud_chan, gmtrack, 0);
ZBUS_CHAN_ADD_OBS(location_chan, gmtrack, 0);
ZBUS_CHAN_ADD_OBS(fota_chan, gmtrack, 0);
ZBUS_CHAN_ADD_OBS(storage_chan, gmtrack, 0);


#define MAX_MSG_SIZE sizeof(struct gmtrack_msg)

BUILD_ASSERT(CONFIG_APP_GMTRACK_WATCHDOG_TIMEOUT_SECONDS >
                 CONFIG_APP_GMTRACK_MSG_PROCESSING_TIMEOUT_SECONDS,
             "Watchdog timeout must be greater than maximum message processing time");

/* State machine states */
enum trdy_module_state
{
    STATE_RUNNING,
};

/* Module state structure */
struct gmtrack_state
{
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

static void send_gmtrack_message(enum gmtrack_msg_type type, int32_t value)
{
    const struct gmtrack_msg msg = {
        .type = type,
        .value = value
    };

    int err = zbus_chan_pub(&GMTRACK_CHAN, &msg, K_NO_WAIT);
    if (err) {
        LOG_ERR("zbus_chan_pub, error: %d", err);
    }

}
static void release_msg_flag()
{
    gpio_pin_set(gpio_dev, 3, 0);
}
static void rise_msg_flag()
{
    gpio_pin_set(gpio_dev, 3, 1);
}

static void add_msg_signal(enum gmtrack_channel_p1 ch, uint8_t p2)
{
    if (!g_enable_event_send)
        return;

    struct gmtrack_poll_msg msg;
    msg.type = gmpoll_step;
    msg.len = 0;
    msg.p1 = (uint8_t)ch;
    msg.p2 = p2;

    k_msgq_put(&g_poll_msgq, &msg, K_NO_WAIT);
    rise_msg_flag();
}



/* Watchdog callback */
static void task_wdt_callback(int channel_id, void *user_data)
{
    LOG_ERR("Watchdog expired, Channel: %d, Thread: %s",
            channel_id, k_thread_name_get((k_tid_t)user_data));

    SEND_FATAL_ERROR_WATCHDOG_TIMEOUT();
}

static const char * get_network_msg_type_string(enum network_msg_type type)
{
    switch (type)
    {
    case NETWORK_DISCONNECTED:
        return "NETWORK_DISCONNECTED";
    case NETWORK_CONNECTED:
        return "NETWORK_CONNECTED";
    case NETWORK_MODEM_RESET_LOOP:
        return "NETWORK_MODEM_RESET_LOOP";
    case NETWORK_UICC_FAILURE:
        return "NETWORK_UICC_FAILURE";
    case NETWORK_LIGHT_SEARCH_DONE:
        return "NETWORK_LIGHT_SEARCH_DONE";
    case NETWORK_SEARCH_DONE:
        return "NETWORK_SEARCH_DONE";
    case NETWORK_ATTACH_REJECTED:
        return "NETWORK_ATTACH_REJECTED";
    case NETWORK_PSM_PARAMS:
        return "NETWORK_PSM_PARAMS";
    case NETWORK_EDRX_PARAMS:
        return "NETWORK_EDRX_PARAMS";
    case NETWORK_SYSTEM_MODE_RESPONSE:
        return "NETWORK_SYSTEM_MODE_RESPONSE";
    case NETWORK_CONNECT:
        return "NETWORK_CONNECT";  
    case NETWORK_DISCONNECT:
        return "NETWORK_DISCONNECT";
    case NETWORK_SEARCH_STOP:
        return "NETWORK_SEARCH_STOP";
    case NETWORK_SYSTEM_MODE_SET_LTEM:
        return "NETWORK_SYSTEM_MODE_SET_LTEM";
    case NETWORK_SYSTEM_MODE_SET_NBIOT:
        return "NETWORK_SYSTEM_MODE_SET_NBIOT";
    case NETWORK_SYSTEM_MODE_SET_LTEM_NBIOT:
        return "NETWORK_SYSTEM_MODE_SET_LTEM_NBIOT";
    case NETWORK_SYSTEM_MODE_REQUEST:
        return "NETWORK_SYSTEM_MODE_REQUEST";           
    case NETWORK_LTE_RRC_IDLE:
        return "NETWORK_LTE_RRC_IDLE";
    case NETWORK_LTE_RRC_CONNECTED:
        return "NETWORK_LTE_RRC_CONNECTED";
    case NETWORK_LTE_SLEEP_ENTER:
        return "NETWORK_LTE_SLEEP_ENTER";
    case NETWORK_LTE_SLEEP_EXIT:
        return "NETWORK_LTE_SLEEP_EXIT";
    default:
        return "UNKNOWN_NETWORK_MSG_TYPE";
    }
}
static const char * get_cloud_msg_type_string(enum cloud_msg_type type)
{
    switch (type)
    {
    case CLOUD_DISCONNECTED:
        return "CLOUD_DISCONNECTED";
    case CLOUD_CONNECTED:
        return "CLOUD_CONNECTED";
    case CLOUD_SHADOW_RESPONSE_DESIRED:
        return "CLOUD_SHADOW_RESPONSE_DESIRED";
    case CLOUD_SHADOW_RESPONSE_DELTA:
        return "CLOUD_SHADOW_RESPONSE_DELTA";
    case CLOUD_SHADOW_RESPONSE_EMPTY_DESIRED:
        return "CLOUD_SHADOW_RESPONSE_EMPTY_DESIRED";
    case CLOUD_SHADOW_RESPONSE_EMPTY_DELTA:
        return "CLOUD_SHADOW_RESPONSE_EMPTY_DELTA";
    case CLOUD_PROVISIONED:
        return "CLOUD_PROVISIONED";
    case CLOUD_PAYLOAD_JSON:
        return "CLOUD_PAYLOAD_JSON";
    case CLOUD_SHADOW_SET_REPORTED_CONFIG:
        return "CLOUD_SHADOW_SET_REPORTED_CONFIG";
    case CLOUD_SHADOW_UPDATE_REPORTED_CONFIG:
        return "CLOUD_SHADOW_UPDATE_REPORTED_CONFIG";
    case CLOUD_SHADOW_UPDATE_REPORTED_DEVICE:
        return "CLOUD_SHADOW_UPDATE_REPORTED_DEVICE";
    case CLOUD_SHADOW_GET_DESIRED:
        return "CLOUD_SHADOW_GET_DESIRED";
    case CLOUD_SHADOW_GET_DELTA:    
        return "CLOUD_SHADOW_GET_DELTA";
    case CLOUD_PROVISIONING_REQUEST:
        return "CLOUD_PROVISIONING_REQUEST";        
    default:
        return "UNKNOWN_CLOUD_MSG_TYPE";
    }
}
static const char * get_location_msg_type_string(enum location_msg_type type)
{
    switch (type)
    {
    case LOCATION_SEARCH_STARTED:
        return "LOCATION_SEARCH_STARTED";
    case LOCATION_SEARCH_DONE:
        return "LOCATION_SEARCH_DONE";
    case LOCATION_CLOUD_REQUEST:
        return "LOCATION_CLOUD_REQUEST";
    case LOCATION_AGNSS_REQUEST:
        return "LOCATION_AGNSS_REQUEST";
    case LOCATION_GNSS_DATA:
        return "LOCATION_GNSS_DATA";
    case LOCATION_MODULE_READY:
        return "LOCATION_MODULE_READY";
    case LOCATION_SEARCH_TRIGGER:
        return "LOCATION_SEARCH_TRIGGER";
    case LOCATION_GNSS_SEARCH_TRIGGER:
        return "LOCATION_GNSS_SEARCH_TRIGGER";
    case LOCATION_SEARCH_CANCEL:
        return "LOCATION_SEARCH_CANCEL";
    default:
        return "UNKNOWN_LOCATION_MSG_TYPE";
    }
}
static const char * get_fota_msg_type_string(enum fota_msg_type type)
{
    switch (type)
    {
        case FOTA_DOWNLOAD_FAILED:
            return "FOTA_DOWNLOAD_FAILED";
        case FOTA_DOWNLOAD_TIMED_OUT:
            return "FOTA_DOWNLOAD_TIMED_OUT";
        case FOTA_DOWNLOADING_UPDATE:   
            return "FOTA_DOWNLOADING_UPDATE";
        case FOTA_NO_AVAILABLE_UPDATE:
            return "FOTA_NO_AVAILABLE_UPDATE";
        case FOTA_SUCCESS_REBOOT_NEEDED:
            return "FOTA_SUCCESS_REBOOT_NEEDED";
        case FOTA_IMAGE_APPLY_NEEDED:
            return "FOTA_IMAGE_APPLY_NEEDED";
        case FOTA_DOWNLOAD_CANCELED:
            return "FOTA_DOWNLOAD_CANCELED";
        case FOTA_DOWNLOAD_REJECTED:
            return "FOTA_DOWNLOAD_REJECTED";
        case FOTA_MODULE_READY:
            return "FOTA_MODULE_READY";
        case FOTA_POLL_REQUEST:
            return "FOTA_POLL_REQUEST";
        case FOTA_IMAGE_APPLY:
            return "FOTA_IMAGE_APPLY";
        case FOTA_DOWNLOAD_CANCEL:
            return "FOTA_DOWNLOAD_CANCEL";
        default:
            return "UNKNOWN_FOTA_MSG_TYPE";
    }
}
static const char * get_storage_msg_type_string(enum storage_msg_type type)
{
    switch (type)
    {
        case STORAGE_SET_THRESHOLD:
            return "STORAGE_SET_THRESHOLD";
        case STORAGE_FLUSH:
            return "STORAGE_FLUSH";
        case STORAGE_CLEAR:
            return "STORAGE_CLEAR";
        case STORAGE_BATCH_REQUEST:
            return "STORAGE_BATCH_REQUEST";
        case STORAGE_BATCH_CLOSE:
            return "STORAGE_BATCH_CLOSE";   
        case STORAGE_STATS:
            return "STORAGE_STATS";
        case STORAGE_THRESHOLD_REACHED:
            return "STORAGE_THRESHOLD_REACHED";
        case STORAGE_DATA:
            return "STORAGE_DATA";
        case STORAGE_BATCH_AVAILABLE:
            return "STORAGE_BATCH_AVAILABLE";
        case STORAGE_BATCH_EMPTY:
            return "STORAGE_BATCH_EMPTY";
        case STORAGE_BATCH_ERROR:
            return "STORAGE_BATCH_ERROR";
        case STORAGE_BATCH_BUSY:
            return "STORAGE_BATCH_BUSY";
        default:
            return "UNKNOWN_STORAGE_MSG_TYPE";
    }
}


static void pgps_notify(int type, const char * msg)
{
    LOG_WRN ("Notified location_method_gnss, type: %d msg: %s\r\n", type, msg);
    switch (type) {
        case 0: /* location_method_gnss registered */
            LOG_DBG("location_method_gnss registered");
            break;
        case 1: /* P-GPS prediction requested */
            send_gmtrack_message(GMTRACK_LOCGNSS_PGPS_REQUESTED, 0);
            break;
        case 2: /* P-GPS predictions ready */
            send_gmtrack_message(GMTRACK_LOCGNSS_PGPS_READY, 0);
            break;
        default:
            LOG_WRN("Unknown notification type from location_method_gnss: %d", type);
    }   
}


/* State machine handlers */
static enum smf_state_result state_running_run(void *o)
{
    struct gmtrack_state *state_object = (struct gmtrack_state *)o;

    if (&GMTRACK_CHAN == state_object->chan)
    {
        const struct gmtrack_msg *msg = (const struct gmtrack_msg *)state_object->msg_buf;

        if (msg->type == GMTRACK_REQUEST)
        {
            LOG_INF("Received gmtrack request");

            state_object->current_value++;

            struct gmtrack_msg response = {
                .type = GMTRACK_RESPONSE,
                .value = state_object->current_value};

            int err = zbus_chan_pub(&GMTRACK_CHAN, &response, K_NO_WAIT);
            if (err)
            {
                LOG_ERR("Failed to publish response: %d", err);
                SEND_FATAL_ERROR();
                return SMF_EVENT_PROPAGATE;
            }
        }
        else if (msg->type == GMTRACK_CONFIG_CHG)
        {
            LOG_DBG("Reporting cfg changed to Silabs");
            gmtrack_flush_cfgchg();
        }
        else if (msg->type == GMTRACK_SUSPEND_FLASH)
        {
            gmtrack_flash_enable(false);
        }
        else if (msg->type == GMTRACK_RESUME_FLASH)
        {
            gmtrack_flash_enable(true);
        }
        else if (msg->type == GMTRACK_LOCGNSS_PGPS_REQUESTED ||
                msg->type == GMTRACK_LOCGNSS_PGPS_READY)
        {
            add_msg_signal(gmstep_gmtrack, msg->type);
        }
        else
        {
            LOG_WRN("Unknown message type received on GMTRACK_CHAN: %d", msg->type);
        }
    }
    else if (&network_chan == state_object->chan)
    {
        const struct network_msg *msg = (const struct network_msg *)state_object->msg_buf;
        LOG_WRN("Network message: %s", get_network_msg_type_string(msg->type));

        if (msg->type == NETWORK_DISCONNECTED ||
            msg->type == NETWORK_CONNECTED ||
            msg->type == NETWORK_CONNECT ||
            msg->type == NETWORK_MODEM_RESET_LOOP ||
            msg->type == NETWORK_UICC_FAILURE ||
            msg->type == NETWORK_LIGHT_SEARCH_DONE ||
            msg->type == NETWORK_SEARCH_DONE ||
            msg->type == NETWORK_ATTACH_REJECTED ||
            msg->type == NETWORK_LTE_RRC_IDLE ||
            msg->type == NETWORK_LTE_RRC_CONNECTED ||
            msg->type == NETWORK_LTE_SLEEP_ENTER ||
            msg->type == NETWORK_LTE_SLEEP_EXIT ||
            msg->type == NETWORK_LTE_SLEEP_EXIT_PRE_WARNING)
         {
            add_msg_signal(gmstep_network, msg->type);
         }
    }
    else if (&cloud_chan == state_object->chan)
    {
        const struct cloud_msg *msg = (const struct cloud_msg *)state_object->msg_buf;
        LOG_WRN("Cloud message: %s", get_cloud_msg_type_string(msg->type));
        if (msg->type == CLOUD_CONNECTED ||
            msg->type == CLOUD_DISCONNECTED)
        {
            add_msg_signal(gmstep_cloud, msg->type);
        }
    }
    else if (&location_chan == state_object->chan)
    {
        const struct location_msg *msg = (const struct location_msg *)state_object->msg_buf;
        LOG_WRN("Location message: %s", get_location_msg_type_string(msg->type));

        if (msg->type == LOCATION_MODULE_READY)
        {
            method_gnss_register_notify(pgps_notify);
        }

        if (msg->type == LOCATION_SEARCH_STARTED ||
            msg->type == LOCATION_SEARCH_DONE ||
            msg->type == LOCATION_GNSS_SEARCH_TRIGGER ||
            msg->type == LOCATION_GNSS_DATA )
        {
            add_msg_signal(gmstep_location, msg->type);
        }
    }
    else if (&fota_chan == state_object->chan)
    {
        const struct fota_msg *msg = (const struct fota_msg *)state_object->msg_buf;
        LOG_WRN("FOTA message: %s", get_fota_msg_type_string(msg->type));
         if (msg->type == FOTA_DOWNLOADING_UPDATE ||
             msg->type == FOTA_NO_AVAILABLE_UPDATE ||
                msg->type == FOTA_SUCCESS_REBOOT_NEEDED ||
                msg->type == FOTA_DOWNLOAD_FAILED ||
                msg->type == FOTA_DOWNLOAD_TIMED_OUT ||
                msg->type == FOTA_DOWNLOAD_CANCELED ||
                msg->type == FOTA_DOWNLOAD_REJECTED ||
             msg->type == FOTA_IMAGE_APPLY_NEEDED)
        {
            add_msg_signal(gmstep_fota, msg->type);
        }
    }
    else if (&storage_chan == state_object->chan)
    {
        const struct storage_msg *msg = (const struct storage_msg *)state_object->msg_buf;
        LOG_WRN("Storage message: %s", get_storage_msg_type_string(msg->type));

        if (msg->type == STORAGE_THRESHOLD_REACHED ||
            msg->type == STORAGE_BATCH_CLOSE) 
        {
            add_msg_signal(gmstep_storage, msg->type);
        }
    }
    else
    {
        LOG_WRN("Message received on unknown channel in gmtrack module");
    }

    return SMF_EVENT_PROPAGATE;
}



int gmtrack_uart_disable(int uart)
{
    int err;
    const struct device *dev = uart == 0 ? uart0_dev : uart1_dev;

    if (!device_is_ready(dev))
    {
        LOG_ERR("UART device is not ready");
        return -ENODEV;
    }

    err = pm_device_action_run(dev, PM_DEVICE_ACTION_SUSPEND);
    if (err && (err != -EALREADY))
    {
        LOG_ERR("pm_device_action_run, error: %d", err);
        return err;
    }
    return 0;
}

int gmtrack_uart_enable(int uart)
{
    int err;
    const struct device *dev = uart == 0 ? uart0_dev : uart1_dev;

    if (!device_is_ready(dev))
    {
        LOG_ERR("UART device is not ready");
        return -ENODEV;
    }

    err = pm_device_action_run(dev, PM_DEVICE_ACTION_RESUME);
    if (err && (err != -EALREADY))
    {
        LOG_ERR("pm_device_action_run, error: %d", err);
        return err;
    }
    return 0;
}

int gmtrack_flash_enable(bool enable)
{
    int err;

    if (!device_is_ready(flash_dev)) {
        LOG_ERR("Flash device is not ready");
        return -ENODEV;
    }

    if (enable) {
        err = pm_device_action_run(flash_dev, PM_DEVICE_ACTION_RESUME);
        if (err && (err != -EALREADY))
        {
            LOG_ERR("pm_device_action_run resume, error: %d", err);
            return err;
        }
    }
    else {
        err = pm_device_action_run(flash_dev, PM_DEVICE_ACTION_SUSPEND);
        if (err && (err != -EALREADY))
        {
            LOG_ERR("pm_device_action_run suspend, error: %d", err);
            return err;
        }
    }
    return 0;
}

int gmtrack_send_network_message(enum network_msg_type type)
{
    const struct network_msg msg = {
        .type = type,
    };

    int err = zbus_chan_pub(&network_chan, &msg, K_SECONDS(1));
    if (err)
    {
        LOG_ERR("zbus_chan_pub, error: %d", err);
        return 1;
    }
    return 0;
}


void gmtrack_led_set(int value)
{
    gpio_pin_set_dt(&led, value);
}

static void gp14_change_fun(const struct device *dev, struct gpio_callback *cb, uint32_t pins)
{
    if (gpio_pin_get(gpio_dev, 14)) {
        // activate interrupt on GP14 pin to detect falling edge
        gpio_pin_interrupt_configure(gpio_dev, 14, GPIO_INT_LEVEL_INACTIVE);
        gmtrack_led_set(1);
        gmtrack_uart_enable(0);
        //LOG_DBG("GP14 pin rised");
        //  confirm driving hi GP15
        gpio_pin_set(gpio_dev, 15, 1);
        //send_gmtrack_message(GMTRACK_RESUME_FLASH, 0);

    }
    else {
        // activate interrupt on GP14 pin to detect rising edge
        gpio_pin_interrupt_configure(gpio_dev, 14, GPIO_INT_LEVEL_ACTIVE);

        gpio_pin_set(gpio_dev, 15, 0);
        gmtrack_led_set(0);
        //LOG_DBG("GP14 pin falled");
        gmtrack_uart_disable(0);
        //send_gmtrack_message(GMTRACK_SUSPEND_FLASH, 0);
    }
}

void start_timer_fun(struct k_timer *timer_id)
{
    gmtrack_led_set(0);

    gmtrack_uart_disable(0);

#if DISABLE_UART1_AT_POWERUP
    LOG_DBG("Disabling uart1");
    gmtrack_uart_disable(1);
#endif
    signal_ready();
}

static void gmtrack_init()
{
    LOG_DBG("Gmtrack module initialization");

    int err = gpio_pin_configure(gpio_dev, 3, GPIO_OUTPUT_LOW);
    if (err)
    {
        LOG_ERR("gp3 pin configure failed (%d)\n", err);
        return;
    }

    err = gpio_pin_configure(gpio_dev, 15, GPIO_OUTPUT_LOW);
    if (err)
    {
        LOG_ERR("gp15 pin configure failed (%d)\n", err);
        return;
    }
    err = gpio_pin_configure(gpio_dev, 14, GPIO_ACTIVE_HIGH | GPIO_INPUT);
    if (err)
    {
        LOG_ERR("gp14 pin configure failed (%d)\n", err);
        return;
    }
    err = gpio_pin_interrupt_configure(gpio_dev, 14, /*GPIO_INT_EDGE_RISING | GPIO_INT_EDGE_FALLING*/ GPIO_INT_LEVEL_ACTIVE);
    if (err)
    {
        LOG_ERR("gp14 int configure failed (%d)\n", err);
        return;
    }
    gpio_init_callback(&gp14_event_cb, gp14_change_fun, BIT(14));

    err = gpio_add_callback(gpio_dev, &gp14_event_cb);
    if(err){
        LOG_ERR("gp14 add callback failed (%d)\n",err);
        return;
    }
    GpsParamsTestFill(&g_gpsparams, &g_gpsparams_vld);

    k_timer_start(&g_start_timer, K_MSEC(2000), K_NO_WAIT);

    gmtrack_led_set(1);


}

/* Module task function */
static void gmtrack_task(void)
{
    int err;
    int task_wdt_id;
    const uint32_t wdt_timeout_ms =
        (CONFIG_APP_GMTRACK_WATCHDOG_TIMEOUT_SECONDS * MSEC_PER_SEC);
    const uint32_t execution_time_ms =
        (CONFIG_APP_GMTRACK_MSG_PROCESSING_TIMEOUT_SECONDS * MSEC_PER_SEC);
    const k_timeout_t zbus_wait_ms = K_MSEC(wdt_timeout_ms - execution_time_ms);
    struct gmtrack_state gmtrack_state = {
        .current_value = 0};

    LOG_DBG("Starting gmtrack module task");

    task_wdt_id = task_wdt_add(wdt_timeout_ms, task_wdt_callback, (void *)k_current_get());

    smf_set_initial(SMF_CTX(&gmtrack_state), &states[STATE_RUNNING]);
    gmtrack_init();

    while (true)
    {
        err = task_wdt_feed(task_wdt_id);
        if (err)
        {
            LOG_ERR("Failed to feed watchdog: %d", err);
            SEND_FATAL_ERROR();
            return;
        }

        err = zbus_sub_wait_msg(&gmtrack,
                                &gmtrack_state.chan,
                                gmtrack_state.msg_buf,
                                zbus_wait_ms);
        if (err == -ENOMSG)
        {
            continue;
        }
        else if (err)
        {
            LOG_ERR("Failed to wait for message: %d", err);
            SEND_FATAL_ERROR();
            return;
        }

        err = smf_run_state(SMF_CTX(&gmtrack_state));
        if (err)
        {
            LOG_ERR("Failed to run state machine: %d", err);
            SEND_FATAL_ERROR();
            return;
        }
    }
}




static void msg_poll()
{
    struct gmtrack_poll_msg msg;
    if (k_msgq_get(&g_poll_msgq, &msg, K_NO_WAIT) == 0)
    {
        // check if there are other data available
        if (k_msgq_num_used_get(&g_poll_msgq) == 0)
            release_msg_flag();

        // msg available, send it out
        switch (msg.type)
        {
        case gmpoll_text:
            printf("\x02"
                   "T%s\x03\r\n",
                   msg.data);
            break;
        case gmpoll_config:
            printf("\x02"
                   "C%s\x03\r\n",
                   msg.data);
            break;
        case gmpoll_ready:
            printf("\x02"
                   "R\x03\r\n");
            break;
        case gmpoll_step:
            printf("\x02"
                   "S%d.%d\x03\r\n",
                   msg.p1, msg.p2);    
            break;                   
        default:
            LOG_ERR("Sending Unknown polled message type %d len %d p1 %d p2 %d data %.*s", 
                msg.type, msg.len, msg.p1, msg.p2, msg.len, msg.data );
            printf("\x02"
                   "U\x03\r\n");
            break;
        }
    }
    else
        printf("\x02"
               "E\x03\r\n");

}

static void add_msg(const struct gmtrack_poll_msg *msg)
{
    k_msgq_put(&g_poll_msgq, msg, K_NO_WAIT);
    rise_msg_flag();
}



static int parse_hex_u64(const char *strval, uint64_t *val)
{
    errno = 0;
    char *end;
    *val = strtoull(strval, &end, 16);
    return errno == 0 ? 0 : -1;
}

static void add_text_msg(const char *text)
{
    struct gmtrack_poll_msg msg;
    int ln = strlen(text);
    if (ln > 59)
        ln = 59;
    msg.type = gmpoll_text;
    msg.len = ln;
    memcpy(msg.data, text, ln);
    msg.data[ln] = 0;
    add_msg(&msg);
}

void poll_test_fun(struct k_timer *timer_id)
{
    add_text_msg("Polled message test 1");
    add_text_msg("Polled message test 2");
}

static void msg_cfgpar(const char *name, const char *value)
{
    int val = atoi(value);
    if (GpsParamsSetValue(name, val) != 0)
    {
        printf("\x02"
               "ERR Bad param name\x03\r\n");
        LOG_ERR("Bad param name %s", name);
    }
    printf("\x02"
           "OK\x03\r\n");
}

static int cmd_silmsg(const struct shell *sh, size_t argc, char **argv)
{
    /*LOG_DBG("Received silmsg command with %d args", argc);
    if (argc > 1)
        LOG_DBG("Arg1: %s", argv[1]);*/

    if (argc < 2)
    {
        return 1;
    }
    if (strcmp(argv[1], "hello") == 0)
    {
        printf("\x02"
               "OK hi\x03\r\n");
    }
    else if (strcmp(argv[1], "poll") == 0)
    {
        msg_poll();
    }
    else if (strcmp(argv[1], "testp") == 0)
    {
        printf("\x02"
               "OK Timer started\x03\r\n");
        k_timer_start(&g_test_timer, K_MSEC(5000), K_NO_WAIT);
    }
    else if (strcmp(argv[1], "noresp") == 0)
    {
    }
    else if (strcmp(argv[1], "cfgpar") == 0)
    {
        if (argc != 4)
        {
            printf("\x02"
                   "ERR Bad params\x03\r\n");
        }
        else
            msg_cfgpar(argv[2], argv[3]);
    }
    else if (strcmp(argv[1], "setsled") == 0 && argc == 3)
    {
        LOG_DBG("Received sled update %s", argv[2]);
        if (parse_hex_u64(argv[2], &g_gmtrack_info.sled) != 0)
        {
            LOG_ERR("Sled value not u64 hex string %s\r\n", argv[2]);
            printf("\x02"
                   "ERR Bad len\x03\r\n");
        }
        else
        {
            LOG_DBG("Sled: %llx", g_gmtrack_info.sled);
            printf("\x02"
                   "OK\x03\r\n");
        }
    }
    else if (strcmp(argv[1], "setbatt") == 0 && argc == 3)
    {
        LOG_DBG("Received batt update %s", argv[2]);
        if (sscanf(argv[2], "%d", &g_gmtrack_info.battlevel) != 1)
            printf("\x02"
                   "ERR\x03\r\n");
        else
        {
            LOG_DBG("Sled: %d", g_gmtrack_info.battlevel);
            printf("\x02"
                   "OK\x03\r\n");
        }
    }
    else if (strcmp(argv[1], "setmac") == 0 && argc == 3)
    {
        LOG_DBG("Received mac update %s", argv[2]);
        if (parse_hex_u64(argv[2], &g_gmtrack_info.mac) != 0)
        {
            LOG_ERR("Mac value not u64 hex string %s\r\n", argv[2]);
            printf("\x02"
                   "ERR Bad len\x03\r\n");
        }
        else
        {
            LOG_DBG("Mac: %llx", g_gmtrack_info.mac);
            printf("\x02"
                   "OK\x03\r\n");
        }
    }
    else if (strcmp(argv[1], "setdate") == 0 && argc == 3)
    {
        LOG_DBG("Received date update %s", argv[2]);
        uint32_t sec;
        uint32_t ms;
        for (char *p = argv[2]; *p != 0; p++)
        {
            if (*p == '.')
                *p = ' ';
        }
        if (sscanf(argv[2], "%d %d", &sec, &ms) != 2)
            printf("\x02"
                   "ERR\x03\r\n");
        else
        {
            int64_t date_time_ms = ((int64_t)sec * 1000) + ms;
            date_time_core_store(date_time_ms, DATE_TIME_OBTAINED_EXT);
            LOG_DBG("Ts: %d %d", sec, ms);
        }
        printf("\x02"
               "OK\x03\r\n");
    }
    else if (strcmp(argv[1], "getdate") == 0) {
        int64_t date_time_ms;
        if (date_time_now(&date_time_ms) < 0)
        {
            printf("\x02"
                   "ERR No date\x03\r\n");
        }
        else
        {
            uint32_t sec = date_time_ms / 1000;
            uint32_t ms = date_time_ms % 1000;
            printf("\x02"
                   "OK %d.%d\x03\r\n", sec, ms);
        }
    }

    else if (strcmp(argv[1], "start") == 0)
    {
        printf("\x02"
               "OK\x03\r\n");
    }
    else if (strcmp(argv[1], "fix") == 0)
    {
        printf("\x02"
               "OK\x03\r\n");
    }
    else if (strcmp(argv[1], "connect") == 0)
    {
        gmtrack_send_network_message(NETWORK_CONNECT);
        printf("\x02"
               "OK\x03\r\n");
    }
    else if (strcmp(argv[1], "info") == 0)
    {
        printf("\x02"
               "OK\x03\r\n");
    }
    else
    {
        printf("\x02"
               "ERR Bad command\x03\r\n");
    }
    return 0;
}


static void signal_ready()
{
    struct gmtrack_poll_msg pollm;
    pollm.type = gmpoll_ready;
    pollm.len = 5;
    memcpy(pollm.data, "READY", 5);
    pollm.data[pollm.len] = 0;
    add_msg(&pollm);
    g_enable_event_send = true;
}

static void chg_queue_flush(const char *msg)
{
    struct gmtrack_poll_msg pollm;
    pollm.type = gmpoll_config;
    pollm.len = strlen(msg);
    memcpy(pollm.data, msg, pollm.len);
    pollm.data[pollm.len] = 0;
    add_msg(&pollm);
    printf("Sent cfg change msg to Silabs: %s\r\n", msg);
}

void gmtrack_flush_cfgchg()
{
    char buf[60];
    GpsParamsFlushValids(buf, 60, chg_queue_flush);
	// After flushing the changes, clear the changed flags
    GpsParamsClearChanged();

}





SHELL_CMD_REGISTER(silmsg, NULL, "Silabs msg", cmd_silmsg);



/* Define module thread */
K_THREAD_DEFINE(gmtrack_task_id,
                CONFIG_APP_GMTRACK_THREAD_STACK_SIZE,
                gmtrack_task, NULL, NULL, NULL,
                K_LOWEST_APPLICATION_THREAD_PRIO, 0, 0);

