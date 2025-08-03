/*
 * Copyright (c) 2025 MaxDevHub (https://github.com/MaxDevHub)
 *
 * This file is licensed under the MIT License.
 * See the LICENSE file in the root of this repository for details.
 */

#include "bme280.h"
#include "driver/i2c_master.h"
#include "esp_log.h"
#include "i2c_bus.h"
#include "iot_hum_meas.h"
#include "iot_temp_meas.h"
#include "st_dev.h"

#define I2C_MASTER_SDA_IO 4
#define I2C_MASTER_SCL_IO 3
#define I2C_MASTER_FREQ_HZ 100000

static const char *LOG_TAG = "MAIN";

// onboarding_config_start is null-terminated string
extern const uint8_t
    onboarding_config_start[] asm("_binary_onboarding_config_json_start");
extern const uint8_t
    onboarding_config_end[] asm("_binary_onboarding_config_json_end");

// device_info_start is null-terminated string
extern const uint8_t device_info_start[] asm("_binary_device_info_json_start");
extern const uint8_t device_info_end[] asm("_binary_device_info_json_end");

static IOT_CTX iot_ctx = NULL;
static iot_temp_meas_data_t *iot_temp_meas = NULL;
static iot_hum_meas_data_t *iot_hum_meas = NULL;

static bme280_handle_t bme280;
static i2c_bus_handle_t i2c_bus;

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

  bme280 = bme280_create(i2c_bus, BME280_I2C_ADDRESS_DEFAULT);
  bme280_default_init(bme280);
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
    ESP_LOGE(LOG_TAG, "Error create iot_ctx");

  iot_temp_meas = iot_temp_meas_create(iot_ctx, "main");
  iot_hum_meas = iot_hum_meas_create(iot_ctx, "main");
}
