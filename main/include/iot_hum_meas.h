#ifndef _IOT_HUM_MEAS_H_
#define _IOT_HUM_MEAS_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "st_dev.h"

typedef struct {
  IOT_CAP_HANDLE *handle;

  double value;
  const char *unit;
} iot_hum_meas_data_t;

iot_hum_meas_data_t *iot_hum_meas_create(IOT_CTX *ctx, const char *component);

void iot_hum_meas_send_value(iot_hum_meas_data_t *iot_data);

#ifdef __cplusplus
}
#endif

#endif
