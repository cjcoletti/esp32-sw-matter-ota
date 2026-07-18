#pragma once

#include <esp_err.h>
#include <esp_matter.h>
#include "driver/gpio.h"

#if CHIP_DEVICE_CONFIG_ENABLE_THREAD
#include "esp_openthread_types.h"
#endif

#define SWITCH_GPIO GPIO_NUM_14
#define STATUS_LED_GPIO GPIO_NUM_15
#define DECOMMISSION_GPIO GPIO_NUM_8
#define SWITCH_DEBOUNCE_MS 50
#define LATCH_DETECT_MS 800
#define DECOMMISSION_HOLD_MS 5000

typedef void *app_driver_handle_t;

/** Initialize switch (GPIO14), status LED (GPIO15) and decommission button (GPIO8). */
app_driver_handle_t app_driver_switch_init();

/**
 * Read physical switch: closed (LOW) = true/ON, open (HIGH) = false/OFF.
 * Used for initial OnOff when the input looks already latched closed at boot.
 */
bool app_driver_switch_get_closed();

/** Apply OnOff attribute changes from the Matter stack (updates LED only). */
esp_err_t app_driver_attribute_update(app_driver_handle_t driver_handle, uint16_t endpoint_id, uint32_t cluster_id,
                                      uint32_t attribute_id, esp_matter_attr_val_t *val);

/** Start task that monitors GPIO14 (auto-detect momentary/latching) and GPIO8 decommission. */
esp_err_t app_driver_start_switch_monitor(uint16_t endpoint_id);

/** Blink status LED for Identify cluster feedback. */
esp_err_t app_driver_identify_effect();

#if CHIP_DEVICE_CONFIG_ENABLE_THREAD
#define ESP_OPENTHREAD_DEFAULT_RADIO_CONFIG()                                           \
    {                                                                                   \
        .radio_mode = RADIO_MODE_NATIVE,                                                \
    }

#define ESP_OPENTHREAD_DEFAULT_HOST_CONFIG()                                            \
    {                                                                                   \
        .host_connection_mode = HOST_CONNECTION_MODE_NONE,                              \
    }

#define ESP_OPENTHREAD_DEFAULT_PORT_CONFIG()                                            \
    {                                                                                   \
        .storage_partition_name = "nvs", .netif_queue_size = 10, .task_queue_size = 10, \
    }
#endif
