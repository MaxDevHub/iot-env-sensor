/*
 * Copyright (c) 2025 MaxDevHub (https://github.com/MaxDevHub)
 *
 * This file is licensed under the MIT License.
 * See the LICENSE file in the root of this repository for details.
 */

#ifndef _IOT_TEMPERATURE_MEASURMENT_H_
#define _IOT_TEMPERATURE_MEASURMENT_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "st_dev.h"

typedef struct {
  IOT_CAP_HANDLE *handle;

  double value;
  const char *unit;
} iot_temp_meas_data_t;

iot_temp_meas_data_t *iot_temp_meas_create(IOT_CTX *ctx, const char *component);

void iot_temp_meas_send_value(iot_temp_meas_data_t *iot_data);

#ifdef __cplusplus
}
#endif

#endif
