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

#define DISABLE_UART1_AT_POWERUP 0

void start_timer_fun(struct k_timer *timer_id);
void poll_test_fun(struct k_timer *timer_id);
static void signal_ready();

K_TIMER_DEFINE(g_start_timer, start_timer_fun, NULL);
K_MSGQ_DEFINE(g_poll_msgq, sizeof(struct gmtrack_poll_msg), 32, 4);
K_TIMER_DEFINE(g_test_timer, poll_test_fun, NULL);

const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(DT_ALIAS(led0), gpios);
const struct gpio_dt_spec gp14 = GPIO_DT_SPEC_GET(DT_ALIAS(led1), gpios);
const struct gpio_dt_spec gp15 = GPIO_DT_SPEC_GET(DT_ALIAS(led2), gpios);
const struct gpio_dt_spec gp2 = GPIO_DT_SPEC_GET(DT_ALIAS(sw0), gpios);
const struct gpio_dt_spec gp3 = GPIO_DT_SPEC_GET(DT_ALIAS(led3), gpios);


static struct gpio_callback gp14_event_cb;
static const struct device *const uart0_dev = DEVICE_DT_GET(DT_NODELABEL(uart0));
static const struct device *const uart1_dev = DEVICE_DT_GET(DT_NODELABEL(uart1));

static void flush_cfgchg();

/* Register log module */
LOG_MODULE_REGISTER(gmtrack_module, CONFIG_APP_GMTRACK_LOG_LEVEL);

/* Define module's zbus channel */
ZBUS_CHAN_DEFINE(GMTRACK_CHAN,
                 struct gmtrack_msg,
                 NULL,
                 NULL,
                 ZBUS_OBSERVERS_EMPTY,
                 ZBUS_MSG_INIT(0)
);


/* Register zbus subscriber */
ZBUS_MSG_SUBSCRIBER_DEFINE(gmtrack);

/* Add subscriber to channel */
ZBUS_CHAN_ADD_OBS(GMTRACK_CHAN, gmtrack, 0);

#define MAX_MSG_SIZE sizeof(struct gmtrack_msg)

BUILD_ASSERT(CONFIG_APP_GMTRACK_WATCHDOG_TIMEOUT_SECONDS >
             CONFIG_APP_GMTRACK_MSG_PROCESSING_TIMEOUT_SECONDS,
             "Watchdog timeout must be greater than maximum message processing time");

/* State machine states */
enum trdy_module_state {
    STATE_RUNNING,
};

/* Module state structure */
struct gmtrack_state {
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


/* State machine handlers */
static enum smf_state_result state_running_run(void *o)
{
    struct gmtrack_state *state_object = (struct gmtrack_state *)o;

    if (&GMTRACK_CHAN == state_object->chan) {
        struct gmtrack_msg msg = MSG_TO_GMTRACK_TYPE(state_object->msg_buf);

        if (msg.type == GMTRACK_REQUEST) {
            LOG_INF("Received gmtrack request");

            state_object->current_value++;

            struct gmtrack_msg response = {
                .type = GMTRACK_RESPONSE,
                .value = state_object->current_value
            };

            int err = zbus_chan_pub(&GMTRACK_CHAN, &response, K_NO_WAIT);
            if (err) {
                LOG_ERR("Failed to publish response: %d", err);
                SEND_FATAL_ERROR();
                return SMF_EVENT_PROPAGATE;
            }
        }
        else if (msg.type == GMTRACK_CONFIG_CHG) {
            LOG_DBG("Reporting cfg changed to Silabs");
            flush_cfgchg();
        }
    }

    return SMF_EVENT_PROPAGATE;
}



static int uart_disable(void)
{
	int err;

	if (!device_is_ready(uart0_dev) || !device_is_ready(uart1_dev)) {
		LOG_ERR("UART devices are not ready");
		return -ENODEV;
	}
	//LOG_DBG("suspending UART devices");

	err = pm_device_action_run(uart0_dev, PM_DEVICE_ACTION_SUSPEND);
	if (err && (err != -EALREADY)) {
		LOG_ERR("pm_device_action_run, error: %d", err);
		return err;
	}

    return 0;
}

static int uart_enable(void)
{
	int err;

	if (!device_is_ready(uart0_dev) || !device_is_ready(uart1_dev)) {
		LOG_ERR("UART devices are not ready");
		return -ENODEV;
	}


	err = pm_device_action_run(uart0_dev, PM_DEVICE_ACTION_RESUME);
	if (err && (err != -EALREADY)) {
		LOG_ERR("pm_device_action_run, error: %d", err);
		return err;
	}

	//LOG_DBG("UART devices enabled");
	return 0;
}


static void led_set(int value)
{
	gpio_pin_set_dt(&led, value); 
}

static void gp14_change_fun(const struct device *dev, struct gpio_callback *cb, uint32_t pins)
{
    if (gpio_pin_get_dt(&gp14)) {
        led_set(1);
        uart_enable();
        //LOG_DBG("GP14 pin rised");
        // confirm driving hi GP15
        gpio_pin_set_dt(&gp15, 1);
    }
    else {
        gpio_pin_set_dt(&gp15, 0);
        led_set(0);
        //LOG_DBG("GP14 pin falled");
        uart_disable();
    }
}

void start_timer_fun(struct k_timer *timer_id)
{
    led_set(0);
    uart_disable();


#if DISABLE_UART1_AT_POWERUP
    LOG_DBG("Disabling uart1");
    int err = pm_device_action_run(uart1_dev, PM_DEVICE_ACTION_SUSPEND);
    if (err && (err != -EALREADY)) {
        LOG_ERR("pm_device_action_run, error: %d", err);
    }
#endif
    signal_ready();
}

static void gmtrack_init()
{
    LOG_DBG("Gmtrack module initialization");

    int err = gpio_pin_configure_dt(&gp3, GPIO_OUTPUT_LOW);
    if(err){
        LOG_ERR("gp3 pin configure failed (%d)\n",err);
        return;
    }

    err = gpio_pin_configure_dt(&gp15, GPIO_OUTPUT_LOW);
    if(err){
        LOG_ERR("gp15 pin configure failed (%d)\n",err);
        return;
    }
    err = gpio_pin_configure_dt(&gp14, GPIO_ACTIVE_HIGH | GPIO_INPUT);
    if(err){
        LOG_ERR("gp14 pin configure failed (%d)\n",err);
        return;
    }
    err = gpio_pin_interrupt_configure_dt(&gp14, GPIO_INT_EDGE_RISING | GPIO_INT_EDGE_FALLING);
    if(err){
        LOG_ERR("gp14 int configure failed (%d)\n",err);
        return;
    }
    gpio_init_callback(&gp14_event_cb, gp14_change_fun, BIT(gp14.pin));

    err = gpio_add_callback(gp14.port, &gp14_event_cb);
    if(err){
        LOG_ERR("gp14 add callback failed (%d)\n",err);
        return;
    }

    GpsParamsTestFill();

    k_timer_start(&g_start_timer, K_MSEC(2000), K_NO_WAIT);

    led_set(1);
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
        .current_value = 0
    };

    LOG_DBG("Starting gmtrack module task");

    task_wdt_id = task_wdt_add(wdt_timeout_ms, task_wdt_callback, (void *)k_current_get());

    smf_set_initial(SMF_CTX(&gmtrack_state), &states[STATE_RUNNING]);
    gmtrack_init();
    
    while (true) {
        err = task_wdt_feed(task_wdt_id);
        if (err) {
            LOG_ERR("Failed to feed watchdog: %d", err);
            SEND_FATAL_ERROR();
            return;
        }

        err = zbus_sub_wait_msg(&gmtrack,
                               &gmtrack_state.chan,
                               gmtrack_state.msg_buf,
                               zbus_wait_ms);
        if (err == -ENOMSG) {
            continue;
        } else if (err) {
            LOG_ERR("Failed to wait for message: %d", err);
            SEND_FATAL_ERROR();
            return;
        }

        err = smf_run_state(SMF_CTX(&gmtrack_state));
        if (err) {
            LOG_ERR("Failed to run state machine: %d", err);
            SEND_FATAL_ERROR();
            return;
        }
    }
}


static const struct gpio_dt_spec * get_io(const char * param)
{
	int gp = atoi(param);
	switch (gp) {
		case 2:
			return &gp2;
		case 3:
			return &gp3;
		case 14:
			return &gp14;
		case 15:
			return &gp15;
		default:
			return NULL;
	}
	return NULL;
}



static int cmd_gpio(const struct shell *sh, size_t argc, char **argv)
{
	if (argc == 1) {
		printf ("gpio mode <2|3|14|15> <i|o|dis|pu|pd|od|os> (input/output/disabled/in pullup/in pulldown/open drain/open source)\n");
		printf ("gpio set <2|3|14|15> <0|1|in>\n");
		printf ("gpio get <2|3|14|15>\n");
		printf ("gpio dump\n");
		return 0;
	}
	const struct gpio_dt_spec * gp = NULL;
	if (argc > 2) {
		gp = get_io(argv[2]);
		if (gp == NULL) {
			printf ("Invalid GP number - use 2 3 14 or 15\n");
			return 1;
		}
	}

	if (argc==4 && strcmp(argv[1],"mode") == 0) {
		if (strcmp(argv[3],"i") == 0) {
			gpio_pin_configure_dt(gp, GPIO_INPUT);
		}
		else if (strcmp(argv[3],"o") == 0) {
			gpio_pin_configure_dt(gp, GPIO_OUTPUT);
		}
		else if (strcmp(argv[3],"dis") == 0) {
			gpio_pin_configure_dt(gp, GPIO_DISCONNECTED);
		}
		else if (strcmp(argv[3],"pu") == 0) {
			gpio_pin_configure_dt(gp, GPIO_INPUT | GPIO_PULL_UP);
		}
		else if (strcmp(argv[3],"pd") == 0) {
			gpio_pin_configure_dt(gp, GPIO_INPUT | GPIO_PULL_DOWN);
		}
		else if (strcmp(argv[3],"od") == 0) {
			gpio_pin_configure_dt(gp, GPIO_OUTPUT | GPIO_OPEN_DRAIN);
		}
		else if (strcmp(argv[3],"os") == 0) {
			gpio_pin_configure_dt(gp, GPIO_OUTPUT | GPIO_OPEN_SOURCE);
		}
		else
			return 1;
		return 0;
	}
	else if (argc == 4 && strcmp(argv[1],"set") == 0) {
		if (argv[3][0] == 'i') {
			gpio_pin_configure_dt(gp, GPIO_INPUT);
		}
		else if (argv[3][0] == '1') {
			gpio_pin_set_dt(gp, 1);
		}
		else {
			gpio_pin_set_dt(gp, 0);
		}
		return 0;
	}
	else if (argc == 3 && strcmp(argv[1],"get") == 0) {
		printf ("val:   %d\n", gpio_pin_get_dt(gp));
		return 0;
	}
	else if (argc == 2 && strcmp(argv[1],"dump") == 0) {
		printf ("GP2:   %d\n", gpio_pin_get_dt(&gp2));
		printf ("GP3:   %d\n", gpio_pin_get_dt(&gp3));
		printf ("GP14:  %d\n", gpio_pin_get_dt(&gp14));
		printf ("GP15:  %d\n", gpio_pin_get_dt(&gp15));
		return 0;
	}
	return 1;
}


static int cmd_led(const struct shell *sh, size_t argc, char **argv)
{
	if (argc == 2) {
		if (strcmp(argv[1],"on") == 0)
			gpio_pin_set_dt(&led, 1); 
		else if (strcmp(argv[1],"off") == 0)
			gpio_pin_set_dt(&led, 0); 

		return 0;
	}
	return 1;
}
static int cmd_serialtest(const struct shell *sh, size_t argc, char **argv)
{
	int num = -1;
	if (argc == 2) {
		if (sscanf (argv[1], "%d", &num) != 1) {
			return 1;
		}
	}
	do {
		gpio_pin_set_dt(&led, 1); 
		printf ("The quick brown fox jumps over the lazy dog\r\n");
		gpio_pin_set_dt(&led, 0); 
		if (num > 0)
			num--;
	} while (num);
	return 0;
}

static void release_msg_flag()
{
    gpio_pin_set_dt(&gp3, 0);
}
static void rise_msg_flag()
{
    gpio_pin_set_dt(&gp3, 1);
}

static void msg_poll()
{
    struct gmtrack_poll_msg msg;
    if (k_msgq_get(&g_poll_msgq, &msg, K_NO_WAIT) == 0) {
        // msg available, send it out
        switch (msg.type) {
            case gmpoll_text:
                printf ("\x02""T%s\x03\r\n", msg.data);
                break;
            case gmpoll_config:
                printf ("\x02""C%s\x03\r\n", msg.data);
                break;
            case gmpoll_ready:
                printf ("\x02""R\x03\r\n");
                break;
            default:
                printf ("\x02""U\x03\r\n");
                break;
        }
    }
    else
        printf ("\x02""E\x03\r\n");

    // check if there are other data available
    if (k_msgq_num_used_get(&g_poll_msgq) == 0)        
        release_msg_flag();
    else
        rise_msg_flag();
}

static void add_msg(const struct gmtrack_poll_msg * msg)
{
    k_msgq_put(&g_poll_msgq, msg, K_NO_WAIT);
    rise_msg_flag();
}

static void add_text_msg(const char * text)
{
    struct gmtrack_poll_msg msg;
    int ln = strlen(text);
    if (ln > 59)
        ln = 59;
    msg.type = gmpoll_text;
    msg.len = ln;
    memcpy (msg.data, text, ln);
    msg.data[ln] = 0;
    add_msg(&msg);
}

void poll_test_fun(struct k_timer *timer_id)
{
    add_text_msg("Polled message test 1");
    add_text_msg("Polled message test 2");
}

static void msg_cfgpar (const char * name, const char * value)
{
    int val = atoi(value);
    if (GpsParamsSetValue(name, val) != 0) {
        printf ("\x02""ERR Bad param name\x03\r\n");
        LOG_ERR("Bad param name %s", name);
    }
    printf ("\x02""OK\x03\r\n");
}

static int cmd_silmsg(const struct shell *sh, size_t argc, char **argv)
{
	if (argc < 2) {
		return 1;
    }
    if (strcmp (argv[1], "hello") == 0) {
        printf ("\x02""OK hi\x03\r\n");
    }
    else if (strcmp (argv[1], "poll") == 0) {
        msg_poll();
    }
    else if (strcmp (argv[1], "testp") == 0) {
        printf ("\x02""OK Timer started\x03\r\n");
        k_timer_start(&g_test_timer, K_MSEC(5000), K_NO_WAIT);
    }
    else if (strcmp (argv[1], "noresp") == 0) {
    }
    else if (strcmp (argv[1], "cfgpar") == 0) {
        if (argc != 4) {
            printf ("\x02""ERR Bad params\x03\r\n");
        }
        else
            msg_cfgpar(argv[2], argv[3]);
    }
    else if (strcmp (argv[1], "setsled") == 0) {
        printf ("\x02""OK\x03\r\n");
    }
    else if (strcmp (argv[1], "setbatt") == 0) {
        printf ("\x02""OK\x03\r\n");
    }
    else if (strcmp (argv[1], "setmac") == 0) {
        printf ("\x02""OK\x03\r\n");
    }
    else if (strcmp (argv[1], "setdate") == 0) {
        printf ("\x02""OK\x03\r\n");
    }
    else if (strcmp (argv[1], "start") == 0) {
        printf ("\x02""OK\x03\r\n");
    }
    else if (strcmp (argv[1], "fix") == 0) {
        printf ("\x02""OK\x03\r\n");
    }
    else if (strcmp (argv[1], "connect") == 0) {
        printf ("\x02""OK\x03\r\n");
    }
    else if (strcmp (argv[1], "info") == 0) {
        printf ("\x02""OK\x03\r\n");
    }
    else if (strcmp (argv[1], "info") == 0) {
        printf ("\x02""OK\x03\r\n");
    }
    else {
        printf ("\x02""ERR Bad command\x03\r\n");
    }
    return 0;
}

static int cmd_cfgdump(const struct shell *sh, size_t argc, char **argv)
{
    GpsParamsDump();
    return 1;
}

static void signal_ready()
{
    struct gmtrack_poll_msg pollm;
    pollm.type = gmpoll_ready;
    pollm.len = 5;
    memcpy (pollm.data, "READY", 5);
    pollm.data[pollm.len] = 0;
    add_msg(&pollm);
}

static void chg_queue_flush(const char * msg)
{
    struct gmtrack_poll_msg pollm;
    pollm.type = gmpoll_config;
    pollm.len = strlen(msg);
    memcpy (pollm.data, msg, pollm.len);
    pollm.data[pollm.len] = 0;
    add_msg(&pollm);
}

static void flush_cfgchg()
{
    char buf[60];
    GpsParamsFlushChanged(buf, 60, chg_queue_flush);
}

static void chg_flush(const char * msg)
{
    printf ("CfgRow: %s\n", msg);
}

static int cmd_cfgchg(const struct shell *sh, size_t argc, char **argv)
{
    char buf[60];

    int rv = GpsParamsFlushChanged(buf, 60, chg_flush);
    printf ("%d parameters changed\n", rv);
    flush_cfgchg();
    return 1;
}
static int cmd_u1ena(const struct shell *sh, size_t argc, char **argv)
{
    if (argc != 2)
        return 0;

    if (strcmp(argv[1], "on") == 0) {
        int err = pm_device_action_run(uart1_dev, PM_DEVICE_ACTION_RESUME);
	    if (err && (err != -EALREADY)) {
		    LOG_ERR("pm_device_action_run, error: %d", err);
	    }
        #ifdef CONFIG_NRF_MODEM_LIB_TRACE_BACKEND_UART
        err = nrf_modem_lib_trace_level_set(CONFIG_NRF_MODEM_LIB_TRACE_LEVEL_FULL);
        if (err) {
            LOG_ERR("nrf_modem_lib_trace_level_set, error: %d", err);
            return err;
        }
        #endif

        printf ("Uart1 enabled\n");
    }
    else {
        int err = pm_device_action_run(uart1_dev, PM_DEVICE_ACTION_SUSPEND);
	    if (err && (err != -EALREADY)) {
		    LOG_ERR("pm_device_action_run, error: %d", err);
	    }
        printf ("Uart1 disabled\n");
    }
    return 1;
}


static int cmd_datetime(const struct shell *sh, size_t argc, char **argv)
{
    int64_t now;
    int rv = date_time_now(&now);
    printf ("Current unixtime rv: %d %lld\n", rv, now);
    return 1;
}

/* Define module thread */
K_THREAD_DEFINE(gmtrack_task_id,
                CONFIG_APP_GMTRACK_THREAD_STACK_SIZE,
                gmtrack_task, NULL, NULL, NULL,
                K_LOWEST_APPLICATION_THREAD_PRIO, 0, 0);

SHELL_CMD_REGISTER(led, NULL, "Turn led on and off", cmd_led);
SHELL_CMD_REGISTER(gpio, NULL, "get/set gpio 0/1", cmd_gpio);
SHELL_CMD_REGISTER(serialtest, NULL, "Dump on serial <arg> sentences, infinite if 0 or missing param", cmd_serialtest);
SHELL_CMD_REGISTER(silmsg, NULL, "Silabs msg", cmd_silmsg);
SHELL_CMD_REGISTER(cfgdump, NULL, "Dump config", cmd_cfgdump);
SHELL_CMD_REGISTER(cfgchg, NULL, "Send changed config to SILAB", cmd_cfgchg);
SHELL_CMD_REGISTER(u1ena, NULL, "Enable UART1 (LOG) interface", cmd_u1ena);
SHELL_CMD_REGISTER(datetime, NULL, "Get date and time", cmd_datetime);
