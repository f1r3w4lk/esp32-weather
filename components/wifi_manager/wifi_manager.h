#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include "stdbool.h"

#ifdef __cplusplus
extern "C" {
#endif

void wifi_manager_init(bool force_config);

#ifdef __cplusplus
}
#endif

#endif  // WIFI_MANAGER_H