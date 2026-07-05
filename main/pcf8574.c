#include "pcf8574.h"

#include "driver/i2c_master.h"
#include "esp_log.h"

static const char *TAG = "KEY";

static i2c_master_bus_handle_t bus_handle = NULL;
static i2c_master_dev_handle_t dev_handle = NULL;

static volatile key_evt_t key_event = KEY_NONE;

//--------------------------------------------------
// key state machine
//--------------------------------------------------

typedef struct
{
    uint8_t bit;

    bool last_state;
    bool hold_sent;

    uint32_t press_time;
    uint32_t debounce_time;

} key_t;

static key_t keys[] =
{
    { KEY_UP_BIT   , true, false, 0, 0 },
    { KEY_DOWN_BIT , true, false, 0, 0 },
    { KEY_OK_BIT   , true, false, 0, 0 },
    { KEY_BACK_BIT , true, false, 0, 0 },
    { KEY_TRIG_BIT , true, false, 0, 0 }
};

//--------------------------------------------------

static uint32_t key_time_ms(void)
{
    return (uint32_t)(esp_log_timestamp());
}

//--------------------------------------------------

esp_err_t pcf8574_init(void)
{
    i2c_master_bus_config_t bus_cfg =
    {
        .i2c_port = I2C_NUM_0,
        .sda_io_num = GPIO_NUM_8,
        .scl_io_num = GPIO_NUM_21,
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .glitch_ignore_cnt = 7,
        .flags.enable_internal_pullup = true,
    };

    ESP_ERROR_CHECK(
        i2c_new_master_bus(&bus_cfg, &bus_handle));

    i2c_device_config_t dev_cfg =
    {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = PCF8574_ADDR,
        .scl_speed_hz = 100000,
    };

    ESP_ERROR_CHECK(
        i2c_master_bus_add_device(
            bus_handle,
            &dev_cfg,
            &dev_handle));

    ESP_LOGI(TAG, "PCF8574 OK");

    return ESP_OK;
}

//--------------------------------------------------

esp_err_t pcf8574_read(uint8_t *value)
{
    return i2c_master_receive(
        dev_handle,
        value,
        1,
        100);
}

//--------------------------------------------------

static void push_event(key_evt_t evt)
{
    if(key_event == KEY_NONE)
    {
        key_event = evt;
    }
}

//--------------------------------------------------

void key_init(void)
{
    key_event = KEY_NONE;
}

//--------------------------------------------------

void key_scan(void)
{
    uint8_t raw;

    if(pcf8574_read(&raw) != ESP_OK)
        return;

    uint32_t now = key_time_ms();

    for(int i = 0; i < 5; i++)
    {
        bool released = (raw & keys[i].bit) ? true : false;

        //--------------------------------------------------
        // state changed
        //--------------------------------------------------

        if(released != keys[i].last_state)
        {
            if((now - keys[i].debounce_time) < KEY_DEBOUNCE_MS)
                continue;

            keys[i].debounce_time = now;
            keys[i].last_state = released;

            //----------------------------------------------
            // pressed
            //----------------------------------------------

            if(!released)
            {
                keys[i].press_time = now;
                keys[i].hold_sent = false;
            }

            //----------------------------------------------
            // released
            //----------------------------------------------

            else
            {
                if(!keys[i].hold_sent)
                {
                    switch(keys[i].bit)
                    {
                        case KEY_UP_BIT:
                            push_event(KEY_UP);
                            break;

                        case KEY_DOWN_BIT:
                            push_event(KEY_DOWN);
                            break;

                        case KEY_OK_BIT:
                            push_event(KEY_OK);
                            break;

                        case KEY_BACK_BIT:
                            push_event(KEY_BACK);
                            break;

                        case KEY_TRIG_BIT:
                            push_event(KEY_TRIG);
                            break;
                    }
                }
            }
        }

        //--------------------------------------------------
        // hold detection
        //--------------------------------------------------

        if(!released &&
           !keys[i].hold_sent &&
           (now - keys[i].press_time >= KEY_HOLD_MS))
        {
            keys[i].hold_sent = true;

            switch(keys[i].bit)
            {
                case KEY_OK_BIT:
                    push_event(KEY_OK_HOLD);
                    break;

                case KEY_BACK_BIT:
                    push_event(KEY_BACK_HOLD);
                    break;

                default:
                    break;
            }
        }
    }
}

//--------------------------------------------------

key_evt_t key_get(void)
{
    key_evt_t evt = key_event;

    key_event = KEY_NONE;

    return evt;
}