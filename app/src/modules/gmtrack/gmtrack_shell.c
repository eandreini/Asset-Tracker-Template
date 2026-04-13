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
#include <date_time.h>

#include <zephyr/pm/device.h>
#include <zephyr/pm/device_runtime.h>
#include <zephyr/pm/pm.h>

#include "network.h"
#include "button.h"
#include "gpsparams.h"


LOG_MODULE_DECLARE(gmtrack_module, CONFIG_APP_GMTRACK_LOG_LEVEL);


static const struct device *gpio_dev = DEVICE_DT_GET(DT_NODELABEL(gpio0));
void main_send_timer_expired_cloud_message();
void main_send_timer_expired_sample_data_message();
static void uart_on_fun(struct k_timer *timer_id);



K_TIMER_DEFINE(g_uarton_timer, uart_on_fun, NULL);

static void uart_on_fun(struct k_timer *timer_id)
{
    gmtrack_uart_enable(0);
    gmtrack_uart_enable(1);
    printf("UART0 and UART1 enabled\r\n");
}   


static int send_button_message()
{
    struct button_msg msg;

    msg.button_number = 1;
    msg.type = BUTTON_PRESS_SHORT;

    int err = zbus_chan_pub(&button_chan, &msg, PUB_TIMEOUT);
    if (err)
    {
        LOG_ERR("zbus_chan_pub short press, error: %d", err);
        SEND_FATAL_ERROR();
    }
    return 0;
}


static int cmd_gpio(const struct shell *sh, size_t argc, char **argv)
{
    if (argc == 1)
    {
        printf("gpio mode <2|3|14|15> <i|o|dis|pu|pd|od|os> (input/output/disabled/in pullup/in pulldown/open drain/open source)\n");
        printf("gpio set <2|3|14|15> <0|1|in>\n");
        printf("gpio get <2|3|14|15>\n");
        printf("gpio dump\n");
        return 0;
    }
    int gp = -1;
    if (argc > 2)
    {
        gp = atoi(argv[2]);
        if (gp < 0)
        {
            printf("Invalid GP number - use 2 3 14 or 15\n");
            return 1;
        }
    }

    if (argc == 4 && strcmp(argv[1], "mode") == 0)
    {
        if (strcmp(argv[3], "i") == 0)
        {
            gpio_pin_configure(gpio_dev, gp, GPIO_INPUT);
        }
        else if (strcmp(argv[3], "o") == 0)
        {
            gpio_pin_configure(gpio_dev, gp, GPIO_OUTPUT);
        }
        else if (strcmp(argv[3], "dis") == 0)
        {
            gpio_pin_configure(gpio_dev, gp, GPIO_DISCONNECTED);
        }
        else if (strcmp(argv[3], "pu") == 0)
        {
            gpio_pin_configure(gpio_dev, gp, GPIO_INPUT | GPIO_PULL_UP);
        }
        else if (strcmp(argv[3], "pd") == 0)
        {
            gpio_pin_configure(gpio_dev, gp, GPIO_INPUT | GPIO_PULL_DOWN);
        }
        else if (strcmp(argv[3], "od") == 0)
        {
            gpio_pin_configure(gpio_dev, gp, GPIO_OUTPUT | GPIO_OPEN_DRAIN);
        }
        else if (strcmp(argv[3], "os") == 0)
        {
            gpio_pin_configure(gpio_dev, gp, GPIO_OUTPUT | GPIO_OPEN_SOURCE);
        }
        else
            return 1;
        return 0;
    }
    else if (argc == 4 && strcmp(argv[1], "set") == 0)
    {
        if (argv[3][0] == 'i')
        {
            gpio_pin_configure(gpio_dev, gp, GPIO_INPUT);
        }
        else if (argv[3][0] == '1')
        {
            gpio_pin_set(gpio_dev, gp, 1);
        }
        else
        {
            gpio_pin_set(gpio_dev, gp, 0);
        }
        return 0;
    }
    else if (argc == 3 && strcmp(argv[1], "get") == 0)
    {
        printf("val:   %d\n", gpio_pin_get(gpio_dev, gp));
        return 0;
    }
    else if (argc == 2 && strcmp(argv[1], "dump") == 0)
    {
        printf("GP2:   %d\n", gpio_pin_get(gpio_dev, 2));
        printf("GP3:   %d\n", gpio_pin_get(gpio_dev, 3));
        printf("GP14:  %d\n", gpio_pin_get(gpio_dev, 14));
        printf("GP15:  %d\n", gpio_pin_get(gpio_dev, 15));
        return 0;
    }
    return 1;
}


static int cmd_led(const struct shell *sh, size_t argc, char **argv)
{
    if (argc == 2)
    {
        if (strcmp(argv[1], "on") == 0)
            gmtrack_led_set(1);
        else if (strcmp(argv[1], "off") == 0)
            gmtrack_led_set(0);

        return 0;
    }
    return 1;
}
static int cmd_serialtest(const struct shell *sh, size_t argc, char **argv)
{
    int num = -1;
    if (argc == 2)
    {
        if (sscanf(argv[1], "%d", &num) != 1)
        {
            return 1;
        }
    }
    do
    {
        gmtrack_led_set(1);
        printf("The quick brown fox jumps over the lazy dog\r\n");
        gmtrack_led_set(0);
        if (num > 0)
            num--;
    } while (num);
    return 0;
}

static int disable_device(const char *name)
{
    const struct device *dev;
    int ret;

    dev = shell_device_get_binding(name);
    if (dev == NULL)
    {
        printf("Invalid device: %s", name);
        return -ENODEV;
    }

    if (pm_device_runtime_is_enabled(dev))
    {
        printf("Device %s uses runtime PM, use the runtime functions instead",
               dev->name);
        return -EINVAL;
    }

    ret = pm_device_action_run(dev, PM_DEVICE_ACTION_SUSPEND);
    if (ret < 0)
    {
        printf("Device %s error: %d", "suspend", ret);
        return ret;
    }
    return 0;
}


static int cmd_cfgdump(const struct shell *sh, size_t argc, char **argv)
{
    GpsParamsDump();
    return 1;
}

static void chg_flush(const char *msg)
{
    printf("CfgRow: %s\n", msg);
}


static int cmd_cfgchg(const struct shell *sh, size_t argc, char **argv)
{
    char buf[60];

    int rv = GpsParamsFlushChanged(buf, 60, chg_flush);
    printf("%d parameters changed\n", rv);
    gmtrack_flush_cfgchg();
    return 1;
}
static int cmd_u1ena(const struct shell *sh, size_t argc, char **argv)
{
    if (argc != 2)
        return 0;

    if (strcmp(argv[1], "on") == 0)
    {
        gmtrack_uart_enable(1);
#ifdef CONFIG_NRF_MODEM_LIB_TRACE_BACKEND_UART
        err = nrf_modem_lib_trace_level_set(CONFIG_NRF_MODEM_LIB_TRACE_LEVEL_FULL);
        if (err)
        {
            LOG_ERR("nrf_modem_lib_trace_level_set, error: %d", err);
            return err;
        }
#endif

        printf("Uart1 enabled\n");
    }
    else
    {
        gmtrack_uart_disable(1);
        printf("Uart1 disabled\n");
    }
    return 1;
}
static int cmd_u0ena(const struct shell *sh, size_t argc, char **argv)
{
    if (argc != 2)
        return 0;

    if (strcmp(argv[1], "on") == 0)
    {
        gmtrack_uart_enable(0);
        printf("Uart0 enabled\n");
    }
    else
    {
        gmtrack_uart_disable(0);
        printf("Uart0 disabled\n");
    }
    return 1;
}


static int cmd_datetime(const struct shell *sh, size_t argc, char **argv)
{
    int64_t now;
    int rv = date_time_now(&now);
    printf("Current unixtime rv: %d %lld\n", rv, now);
    return 1;
}

static int cmd_send_data(const struct shell *sh, size_t argc, char **argv)
{
    (void)sh;
    (void)argc;
    (void)argv;

    main_send_timer_expired_cloud_message();
    return 1;
}
static int cmd_sample_data(const struct shell *sh, size_t argc, char **argv)
{
    (void)sh;
    (void)argc;
    (void)argv;

    // main_send_timer_expired_sample_data_message();
    send_button_message();

    return 1;
}

static int cmd_network_msg(const struct shell *sh, size_t argc, char **argv)
{
    (void)sh;
    if (argc != 2)
    {
        printf("Usage: network_msg <msg_type>\n");
        printf("msg_type: conn for connect, disc for disconnect\n");
        return 0;
    }
    enum network_msg_type type;
    if (strcmp(argv[1], "conn") == 0)
    {
        type = NETWORK_CONNECT;
    }
    else if (strcmp(argv[1], "disc") == 0)
    {
        type = NETWORK_DISCONNECT;
    }
    else
    {
        printf("Invalid msg_type. Use 'conn' or 'disc'.\n");
        return 0;
    }
    gmtrack_send_network_message(type);
    return 1;
}


static int cmd_udis(const struct shell *sh, size_t argc, char **argv)
{
    if (argc != 2)
    {
        printf("Usage: udis <seconds>\n");
        return 0;
    }
    int seconds = atoi(argv[1]);
    if (seconds <= 0)
    {
        printf("Invalid seconds value. Must be a positive integer.\n");
        return 0;
    }
    gmtrack_uart_disable(0);
    gmtrack_uart_disable(1);

    printf("UART0 and UART1 disabled for %d seconds\n", seconds);
    k_timer_start(&g_uarton_timer, K_SECONDS(seconds), K_NO_WAIT);
    return 1;
}

static int cmd_suspdev(const struct shell *sh, size_t argc, char **argv)
{
    if (argc == 1)
    {
        disable_device("gd25wb256e3ir@1");
        disable_device("spi@b000");
        disable_device("gpio@842500");
    }
    else if (argc == 2)
    {
        if (strcmp(argv[1], "flash") == 0)
        {
            disable_device("gd25wb256e3ir@1");
        }
        else if (strcmp(argv[1], "spi") == 0)
        {
            disable_device("spi@b000");
        }
        else if (strcmp(argv[1], "gpio") == 0)
        {
            disable_device("gpio@842500");
        }
        else
        {
            disable_device(argv[1]);
        }
    }
    else
    {
        printf("Usage: suspdev [device_name] | flash | spi | gpio\n");
        return 0;
    }
    return 1;
}

static int cmd_flashena(const struct shell *sh, size_t argc, char **argv)
{
    if (argc != 2)
    {
        printf("Usage: flashena <on|off>\n");
        return 0;
    }
    if (strcmp(argv[1], "on") == 0)
    {
        gmtrack_flash_enable(true);
        printf("Flash enabled\n");
    }
    else if (strcmp(argv[1], "off") == 0)
    {
        gmtrack_flash_enable(false);
        printf("Flash disabled\n");
    }
    else
    {
        printf("Invalid argument. Use 'on' or 'off'.\n");
        return 0;
    }
    return 1;
}
static int cmd_sleep(const struct shell *sh, size_t argc, char **argv)
{
    if (argc != 2)
    {
        printf("Usage: sleep <seconds>\n");
        return 0;
    }
    int seconds = atoi(argv[1]);
    if (seconds <= 0)
    {
        printf("Invalid seconds value. Must be a positive integer.\n");
        return 0;
    }
    printf("Sleeping for %d seconds...\n", seconds);
    k_sleep(K_SECONDS(seconds));
    printf("Awake!\n");
    return 1;
}


static int cmd_dcdc(const struct shell *sh, size_t argc, char **argv)
{
    if (argc != 2)
    {
        printf("Usage: dcdc <on|off>\n");
        return 0;
    }
    if (strcmp(argv[1], "on") == 0)
    {
        volatile uint32_t *reg = (uint32_t *)0x40004578;
        printf ("Current value: %d\n", *reg);
        *reg = 1;
        printf("DCDC enabled: %d\n", *reg);
    }
    else if (strcmp(argv[1], "off") == 0)
    {
        volatile uint32_t *reg = (uint32_t *)0x40004578;
        printf ("Current value: %d\n", *reg);
        *reg = 0;
        printf("DCDC disabled: %d\n", *reg);
    }
    else
    {
        printf("Invalid argument. Use 'on' or 'off'.\n");
        return 0;
    }
    return 1;
}



SHELL_CMD_REGISTER(led, NULL, "Turn led on and off", cmd_led);
SHELL_CMD_REGISTER(gpio, NULL, "get/set gpio 0/1", cmd_gpio);
SHELL_CMD_REGISTER(serialtest, NULL, "Dump on serial <arg> sentences, infinite if 0 or missing param", cmd_serialtest);
SHELL_CMD_REGISTER(cfgdump, NULL, "Dump config", cmd_cfgdump);
SHELL_CMD_REGISTER(cfgchg, NULL, "Send changed config to SILAB", cmd_cfgchg);
SHELL_CMD_REGISTER(u0ena, NULL, "Enable UART0 (CMD) interface", cmd_u0ena);
SHELL_CMD_REGISTER(u1ena, NULL, "Enable UART1 (LOG) interface", cmd_u1ena);
SHELL_CMD_REGISTER(datetime, NULL, "Get date and time", cmd_datetime);
SHELL_CMD_REGISTER(udis, NULL, "Disable UART0 and UART1 for <arg> seconds", cmd_udis);
SHELL_CMD_REGISTER(suspdev, NULL, "Suspend device <device_name> | flash | spi | gpio", cmd_suspdev);
SHELL_CMD_REGISTER(sleep, NULL, "Sleep for <arg> seconds", cmd_sleep);
SHELL_CMD_REGISTER(dcdc, NULL, "Enable/disable DCDC", cmd_dcdc);

SHELL_CMD_REGISTER(send_data, NULL, "Send data to cloud", cmd_send_data);
SHELL_CMD_REGISTER(sample_data, NULL, "Sample data", cmd_sample_data);
SHELL_CMD_REGISTER(nw_msg, NULL, "Send network message", cmd_network_msg);
SHELL_CMD_REGISTER(flashena, NULL, "Enable/disable flash", cmd_flashena);
