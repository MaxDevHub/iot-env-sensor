#include "iot_temp_meas.h"
#include "caps/iot_caps_helper_temperatureMeasurement.h"
#include "esp_log.h"
#include <stdlib.h>
#include <string.h>

static const char *TAG_LOG = "IOT_TEMP_MEAS";

iot_temp_meas_data_t *iot_temp_meas_create(IOT_CTX *ctx,
                                           const char *component) {
  iot_temp_meas_data_t *iot_data = NULL;

  iot_data = malloc(sizeof(iot_temp_meas_data_t));
  if (!iot_data) {
    ESP_LOGE(TAG_LOG, "Error malloc for iot_temp_meas_data_t");
    return NULL;
  }

  memset(iot_data, 0, sizeof(iot_temp_meas_data_t));

  iot_data->handle = st_cap_handle_init(
      ctx, component, caps_helper_temperatureMeasurement.id, NULL, NULL);
  iot_data->unit = caps_helper_temperatureMeasurement.attr_temperature.unit_C;

  return iot_data;
}

void iot_temp_meas_send_value(iot_temp_meas_data_t *iot_data) {
  int32_t seq_no = 1;
  ST_CAP_SEND_ATTR_NUMBER(
      iot_data->handle,
      caps_helper_temperatureMeasurement.attr_temperature.name, iot_data->value,
      iot_data->unit, NULL, seq_no);

  if (seq_no) {
    ESP_LOGE(TAG_LOG, "Error send value");
  }
}
