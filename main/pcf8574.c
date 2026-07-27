#include "pcf8574.h"

#include "driver/gpio.h"
#include "esp_log.h"

static const char *TAG = "KEY";

static volatile key_evt_t key_event = KEY_NONE;

//--------------------------------------------------
// key state machine
//--------------------------------------------------

typedef struct
{
    uint8_t bit;
    gpio_num_t gpio;

    bool last_state;      // true = released, false = pressed
    bool hold_sent;

    uint32_t press_time;
    uint32_t debounce_time;

} key_t;

static key_t keys[] =
{
    { KEY_UP_BIT   , KEY_UP_GPIO   , true, false, 0, 0 },
    { KEY_DOWN_BIT , KEY_DOWN_GPIO , true, false, 0, 0 },
    { KEY_OK_BIT   , KEY_OK_GPIO   , true, false, 0, 0 },
    { KEY_BACK_BIT , KEY_BACK_GPIO , true, false, 0, 0 },
    { KEY_TRIG_BIT , KEY_TRIG_GPIO , true, false, 0, 0 }
};

#define KEY_COUNT   (sizeof(keys) / sizeof(keys[0]))

//--------------------------------------------------

static uint32_t key_time_ms(void)
{
    return (uint32_t)esp_log_timestamp();
}

//--------------------------------------------------
// Read direct GPIOs and build raw byte
// Active low:
// bit = 1 => released
// bit = 0 => pressed
//--------------------------------------------------

static uint8_t read_keys_gpio_raw(void)
{
    uint8_t raw = 0xFF;

    for (int i = 0; i < KEY_COUNT; i++)
    {
        int level = gpio_get_level(keys[i].gpio);

        if (level == 0)
        {
            raw &= ~(keys[i].bit);
        }
        else
        {
            raw |= keys[i].bit;
        }
    }

    return raw;
}

//--------------------------------------------------

esp_err_t pcf8574_init(void)
{
    gpio_config_t io_conf =
    {
        .pin_bit_mask =
            (1ULL << KEY_UP_GPIO)   |
            (1ULL << KEY_DOWN_GPIO) |
            (1ULL << KEY_OK_GPIO)   |
            (1ULL << KEY_BACK_GPIO) |
            (1ULL << KEY_TRIG_GPIO),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };

    ESP_ERROR_CHECK(gpio_config(&io_conf));

    ESP_LOGI(TAG, "Direct GPIO keys init OK");
    ESP_LOGI(TAG, "UP=%d DOWN=%d OK=%d BACK=%d TRIG=%d",
             KEY_UP_GPIO,
             KEY_DOWN_GPIO,
             KEY_OK_GPIO,
             KEY_BACK_GPIO,
             KEY_TRIG_GPIO);

    return ESP_OK;
}

//--------------------------------------------------
// Keep same API name to minimize project changes
//--------------------------------------------------

esp_err_t pcf8574_read(uint8_t *value)
{
    if (value == NULL)
        return ESP_ERR_INVALID_ARG;

    *value = read_keys_gpio_raw();
    return ESP_OK;
}

//--------------------------------------------------

static void push_event(key_evt_t evt)
{
    if (key_event == KEY_NONE)
    {
        key_event = evt;
    }
}

//--------------------------------------------------

void key_init(void)
{
    key_event = KEY_NONE;

    uint32_t now = key_time_ms();
    uint8_t raw = read_keys_gpio_raw();

    for (int i = 0; i < KEY_COUNT; i++)
    {
        bool released = (raw & keys[i].bit) ? true : false;

        keys[i].last_state = released;
        keys[i].hold_sent = false;
        keys[i].press_time = now;
        keys[i].debounce_time = now;
    }
}

//--------------------------------------------------

void key_scan(void)
{
    uint8_t raw;

    if (pcf8574_read(&raw) != ESP_OK)
        return;

    uint32_t now = key_time_ms();

    for (int i = 0; i < KEY_COUNT; i++)
    {
        bool released = (raw & keys[i].bit) ? true : false;

        //--------------------------------------------------
        // state changed
        //--------------------------------------------------

        if (released != keys[i].last_state)
        {
            if ((now - keys[i].debounce_time) < KEY_DEBOUNCE_MS)
                continue;

            keys[i].debounce_time = now;
            keys[i].last_state = released;

            //----------------------------------------------
            // pressed
            //----------------------------------------------

            if (!released)
            {
                keys[i].press_time = now;
                keys[i].hold_sent = false;
            }

            //----------------------------------------------
            // released
            //----------------------------------------------

            else
            {
                if (!keys[i].hold_sent)
                {
                    switch (keys[i].bit)
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

                        default:
                            break;
                    }
                }
            }
        }

        //--------------------------------------------------
        // hold detection
        //--------------------------------------------------

        if (!released &&
            !keys[i].hold_sent &&
            (now - keys[i].press_time >= KEY_HOLD_MS))
        {
            keys[i].hold_sent = true;

            switch (keys[i].bit)
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
