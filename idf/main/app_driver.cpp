#include <esp_log.h>
#include <esp_timer.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include "driver/gpio.h"

#include <esp_matter.h>
#include <app_priv.h>

using namespace chip::app::Clusters;
using namespace esp_matter;

static const char *TAG = "app_driver";

enum input_mode_t {
    INPUT_MODE_UNKNOWN = 0,
    INPUT_MODE_MOMENTARY,
    INPUT_MODE_LATCHING,
};

static uint16_t s_endpoint_id = 0;
static bool s_on_off = false;
static bool s_hub_callback_active = false;

static input_mode_t s_input_mode = INPUT_MODE_UNKNOWN;
static bool s_switch_closed_raw = false;
static bool s_switch_closed_stable = false;
static int64_t s_switch_change_us = 0;

static bool s_press_active = false;
static int64_t s_press_start_us = 0;
static bool s_last_latching_closed = false;

static bool s_decommission_press_active = false;
static int64_t s_decommission_press_start_us = 0;
static bool s_decommission_triggered = false;

static int64_t now_us(void)
{
    return esp_timer_get_time();
}

static uint32_t elapsed_ms(int64_t start_us)
{
    return (uint32_t)((now_us() - start_us) / 1000);
}

bool app_driver_switch_get_closed()
{
    /* Pull-up: LOW when switch closed to GND => ON */
    return gpio_get_level(SWITCH_GPIO) == 0;
}

static bool app_driver_decommission_pressed()
{
    return gpio_get_level(DECOMMISSION_GPIO) == 0;
}

static void app_driver_set_led(bool on)
{
    gpio_set_level(STATUS_LED_GPIO, on ? 1 : 0);
}

static void app_driver_publish_on_off(bool on)
{
    if (s_endpoint_id == 0 || s_hub_callback_active) {
        return;
    }

    s_on_off = on;
    esp_matter_attr_val_t val = esp_matter_bool(on);
    esp_err_t err = attribute::update(s_endpoint_id, OnOff::Id, OnOff::Attributes::OnOff::Id, &val);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to update OnOff attribute: %d", err);
    } else {
        ESP_LOGI(TAG, "OnOff => %s", on ? "ON" : "OFF");
    }
}

static void app_driver_apply_physical_on_off(bool on)
{
    app_driver_set_led(on);
    app_driver_publish_on_off(on);
}

static void app_driver_toggle_on_off()
{
    bool next = !s_on_off;
    ESP_LOGI(TAG, "Momentary: toggling to %s", next ? "ON" : "OFF");
    app_driver_apply_physical_on_off(next);
}

static void detect_latching_mode()
{
    s_input_mode = INPUT_MODE_LATCHING;
    s_last_latching_closed = s_switch_closed_stable;
    ESP_LOGI(TAG, "Detected: latching switch (OnOff follows physical position)");
    app_driver_apply_physical_on_off(s_switch_closed_stable);
}

static void detect_momentary_mode()
{
    s_input_mode = INPUT_MODE_MOMENTARY;
    ESP_LOGI(TAG, "Detected: momentary switch (each click toggles OnOff)");
}

static void update_switch_reading()
{
    bool reading = app_driver_switch_get_closed();
    int64_t now = now_us();

    if (reading != s_switch_closed_raw) {
        s_switch_closed_raw = reading;
        s_switch_change_us = now;
        return;
    }

    if (elapsed_ms(s_switch_change_us) >= SWITCH_DEBOUNCE_MS && reading != s_switch_closed_stable) {
        s_switch_closed_stable = reading;
    }
}

static void handle_unknown_mode()
{
    int64_t now = now_us();

    if (s_switch_closed_stable) {
        if (!s_press_active) {
            s_press_active = true;
            s_press_start_us = now;
            return;
        }

        if (elapsed_ms(s_press_start_us) >= LATCH_DETECT_MS) {
            s_press_active = false;
            detect_latching_mode();
        }
        return;
    }

    if (!s_press_active) {
        return;
    }

    uint32_t held_ms = elapsed_ms(s_press_start_us);
    s_press_active = false;

    if (held_ms >= SWITCH_DEBOUNCE_MS) {
        detect_momentary_mode();
        app_driver_toggle_on_off();
    }
}

static void handle_momentary_mode()
{
    int64_t now = now_us();

    if (s_switch_closed_stable) {
        if (!s_press_active) {
            s_press_active = true;
            s_press_start_us = now;
        }
        return;
    }

    if (!s_press_active) {
        return;
    }

    if (elapsed_ms(s_press_start_us) >= SWITCH_DEBOUNCE_MS) {
        s_press_active = false;
        app_driver_toggle_on_off();
    }
}

static void handle_latching_mode()
{
    if (s_switch_closed_stable == s_last_latching_closed) {
        return;
    }

    s_last_latching_closed = s_switch_closed_stable;
    ESP_LOGI(TAG, "Latching switch => %s", s_switch_closed_stable ? "ON" : "OFF");
    app_driver_apply_physical_on_off(s_switch_closed_stable);
}

static void app_driver_decommission_feedback()
{
    for (int i = 0; i < 10; i++) {
        app_driver_set_led(true);
        vTaskDelay(pdMS_TO_TICKS(50));
        app_driver_set_led(false);
        vTaskDelay(pdMS_TO_TICKS(50));
    }
}

static void handle_decommission_button()
{
    int64_t now = now_us();

    if (app_driver_decommission_pressed()) {
        if (!s_decommission_press_active) {
            s_decommission_press_active = true;
            s_decommission_press_start_us = now;
            s_decommission_triggered = false;
        } else if (!s_decommission_triggered && elapsed_ms(s_decommission_press_start_us) >= DECOMMISSION_HOLD_MS) {
            s_decommission_triggered = true;
            ESP_LOGW(TAG, "GPIO%d held >= %d ms: factory reset / decommission", DECOMMISSION_GPIO,
                     DECOMMISSION_HOLD_MS);
            app_driver_apply_physical_on_off(false);
            app_driver_decommission_feedback();
            esp_err_t err = esp_matter::factory_reset();
            if (err != ESP_OK) {
                ESP_LOGE(TAG, "factory_reset failed: %d", err);
            }
        }
        return;
    }

    s_decommission_press_active = false;
}

static void handle_switch_input()
{
    update_switch_reading();

    switch (s_input_mode) {
    case INPUT_MODE_UNKNOWN:
        handle_unknown_mode();
        break;
    case INPUT_MODE_MOMENTARY:
        handle_momentary_mode();
        break;
    case INPUT_MODE_LATCHING:
        handle_latching_mode();
        break;
    }
}

static void switch_monitor_task(void *arg)
{
    (void)arg;

    s_switch_closed_raw = app_driver_switch_get_closed();
    s_switch_closed_stable = s_switch_closed_raw;
    s_switch_change_us = now_us();
    s_on_off = s_switch_closed_stable;
    app_driver_set_led(s_on_off);

    /* Already held closed at boot: treat as latching after detect window. */
    if (s_switch_closed_stable) {
        s_press_active = true;
        s_press_start_us = now_us();
    }

    while (true) {
        handle_decommission_button();
        handle_switch_input();
        vTaskDelay(pdMS_TO_TICKS(20));
    }
}

app_driver_handle_t app_driver_switch_init()
{
    gpio_config_t sw_conf = {
        .pin_bit_mask = 1ULL << SWITCH_GPIO,
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };
    ESP_ERROR_CHECK(gpio_config(&sw_conf));

    gpio_config_t decomm_conf = {
        .pin_bit_mask = 1ULL << DECOMMISSION_GPIO,
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };
    ESP_ERROR_CHECK(gpio_config(&decomm_conf));

    gpio_config_t led_conf = {
        .pin_bit_mask = 1ULL << STATUS_LED_GPIO,
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };
    ESP_ERROR_CHECK(gpio_config(&led_conf));

    bool closed = app_driver_switch_get_closed();
    s_on_off = closed;
    app_driver_set_led(closed);
    ESP_LOGI(TAG, "Switch GPIO%d, LED GPIO%d, decommission GPIO%d (init closed=%d)", SWITCH_GPIO, STATUS_LED_GPIO,
             DECOMMISSION_GPIO, closed);

    return (app_driver_handle_t)1;
}

esp_err_t app_driver_start_switch_monitor(uint16_t endpoint_id)
{
    s_endpoint_id = endpoint_id;
    BaseType_t ok = xTaskCreate(switch_monitor_task, "sw_mon", 4096, NULL, 5, NULL);
    return (ok == pdPASS) ? ESP_OK : ESP_FAIL;
}

esp_err_t app_driver_attribute_update(app_driver_handle_t driver_handle, uint16_t endpoint_id, uint32_t cluster_id,
                                      uint32_t attribute_id, esp_matter_attr_val_t *val)
{
    (void)driver_handle;

    if (endpoint_id != s_endpoint_id) {
        return ESP_OK;
    }

    if (cluster_id == OnOff::Id && attribute_id == OnOff::Attributes::OnOff::Id) {
        s_hub_callback_active = true;
        s_on_off = val->val.b;
        app_driver_set_led(val->val.b);
        s_hub_callback_active = false;
        ESP_LOGI(TAG, "Hub OnOff => LED=%s", val->val.b ? "ON" : "OFF");
    }

    return ESP_OK;
}

esp_err_t app_driver_identify_effect()
{
    bool current = gpio_get_level(STATUS_LED_GPIO) != 0;
    for (int i = 0; i < 3; i++) {
        app_driver_set_led(!current);
        vTaskDelay(pdMS_TO_TICKS(150));
        app_driver_set_led(current);
        vTaskDelay(pdMS_TO_TICKS(150));
    }
    return ESP_OK;
}
