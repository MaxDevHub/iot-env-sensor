/*
 * Copyright (c) 2025 MaxDevHub (https://github.com/MaxDevHub)
 *
 * This file is licensed under the MIT License.
 * See the LICENSE file in the root of this repository for details.
 */

#include "bme280.h"
#include "button_gpio.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "i2c_bus.h"
#include "iot_button.h"
#include "iot_hum_meas.h"
#include "iot_temp_meas.h"
#include "st_dev.h"

#define I2C_MASTER_SDA_IO 4
#define I2C_MASTER_SCL_IO 3
#define I2C_MASTER_FREQ_HZ 100000

#define CONTROL_BUTTON_PIN 2

static const char *LOG_TAG = "MAIN";

// onboarding_config_start is null-terminated string
extern const uint8_t
    onboarding_config_start[] asm("_binary_onboarding_config_json_start");
extern const uint8_t
    onboarding_config_end[] asm("_binary_onboarding_config_json_end");

// device_info_start is null-terminated string
extern const uint8_t device_info_start[] asm("_binary_device_info_json_start");
extern const uint8_t device_info_end[] asm("_binary_device_info_json_end");

static iot_status_t g_iot_status = IOT_STATUS_IDLE;
static iot_stat_lv_t g_iot_stat_lv;

static TaskHandle_t task_measurment_handle = NULL;

// IOT
static IOT_CTX iot_ctx = NULL;
static iot_temp_meas_data_t *iot_temp_meas = NULL;
static iot_hum_meas_data_t *iot_hum_meas = NULL;

// BME280
static bme280_handle_t bme280 = NULL;
static i2c_bus_handle_t i2c_bus = NULL;

static void bme280_init() {
  i2c_config_t conf = {
      .mode = I2C_MODE_MASTER,
      .sda_io_num = I2C_MASTER_SDA_IO,
      .sda_pullup_en = GPIO_PULLUP_ENABLE,
      .scl_io_num = I2C_MASTER_SCL_IO,
      .scl_pullup_en = GPIO_PULLUP_ENABLE,
      .master.clk_speed = I2C_MASTER_FREQ_HZ,
  };
  i2c_bus = i2c_bus_create(I2C_NUM_0, &conf);

  if (!i2c_bus)
    ESP_LOGE(LOG_TAG, "I2C bus create failed");

  bme280 = bme280_create(i2c_bus, BME280_I2C_ADDRESS_DEFAULT);

  if (!bme280)
    ESP_LOGE(LOG_TAG, "BME280 create failed");

  bme280_default_init(bme280);
  bme280_set_sampling(bme280, BME280_MODE_FORCED, BME280_SAMPLING_X2,
                      BME280_SAMPLING_X16, BME280_SAMPLING_X1,
                      BME280_FILTER_X16, BME280_STANDBY_MS_1000);
}

// Button control device
static button_handle_t button_control = NULL;

static void button_single_click_cb(void *arg, void *usr_data) {
  if (g_iot_status == IOT_STATUS_NEED_INTERACT)
    st_conn_ownership_confirm(iot_ctx, true);
}

static void button_long_press_cb(void *arg, void *usr_data) {
  if (task_measurment_handle != NULL)
    vTaskDelete(task_measurment_handle);

  st_conn_cleanup(iot_ctx, true);
};

static void button_control_init() {
  const button_config_t btn_conf = {.short_press_time = 180,
                                    .long_press_time = 2000};
  const button_gpio_config_t btn_gpio_conf = {.gpio_num = CONTROL_BUTTON_PIN,
                                              .active_level = 1};

  iot_button_new_gpio_device(&btn_conf, &btn_gpio_conf, &button_control);
  if (!button_control)
    ESP_LOGE(LOG_TAG, "Button create failed");

  iot_button_register_cb(button_control, BUTTON_SINGLE_CLICK, NULL,
                         button_single_click_cb, NULL);
  iot_button_register_cb(button_control, BUTTON_LONG_PRESS_START, NULL,
                         button_long_press_cb, NULL);
}

static void task_measurment(void *arg) {
  for (;;) {
    bme280_take_forced_measurement(bme280);

    vTaskDelay(pdMS_TO_TICKS(500));

    float temp = 0.0f, hum = 0.0f;

    bme280_read_temperature(bme280, &temp);
    bme280_read_humidity(bme280, &hum);

    iot_temp_meas->value = (double)temp;
    iot_hum_meas->value = (double)hum;

    iot_temp_meas_send_value(iot_temp_meas);
    iot_hum_meas_send_value(iot_hum_meas);

    vTaskDelay(pdMS_TO_TICKS(1000 * 60 * 5)); // 5 minutes
  }
}

static void iot_status_cb(iot_status_t status, iot_stat_lv_t stat_lv,
                          void *usr_data) {
  g_iot_status = status;
  g_iot_stat_lv = stat_lv;

  ESP_LOGI(LOG_TAG, "IoT callback status: %d, stat_lv: %d", g_iot_status,
           g_iot_stat_lv);

  switch (status) {
  case IOT_STATUS_CONNECTING:
    if (stat_lv == IOT_STAT_LV_DONE && task_measurment_handle != NULL)
      xTaskCreate(task_measurment, "task_measurment", 4096, NULL, 10,
                  &task_measurment_handle);
    break;
  default:
    break;
  }
}

void app_main(void) {
  unsigned char *onboarding_config = (unsigned char *)onboarding_config_start;
  unsigned int onboarding_config_len =
      onboarding_config_end - onboarding_config_start;
  unsigned char *device_info = (unsigned char *)device_info_start;
  unsigned int device_info_len = device_info_end - device_info_start;

  iot_ctx = st_conn_init(onboarding_config, onboarding_config_len, device_info,
                         device_info_len);

  if (!iot_ctx)
    ESP_LOGE(LOG_TAG, "IOT_CTX create failed");

  iot_temp_meas = iot_temp_meas_create(iot_ctx, "main");
  iot_hum_meas = iot_hum_meas_create(iot_ctx, "main");

  bme280_init();
  button_control_init();

  st_conn_start(iot_ctx, iot_status_cb, IOT_STATUS_ALL, NULL, NULL);
}
