#ifndef DISPLAY_ASSETS_H
#define DISPLAY_ASSETS_H

#include <stdint.h>

// ======================= FONT 6x8 =======================
// Store in flash (ROM) to save RAM
// Each character: 6 bytes (5 width + 1 spacing)
extern const uint8_t font6x8[][6];

// ======================= ICONS 24x24 =======================
extern const uint8_t icon_wifi_off[72];
extern const uint8_t icon_wifi[72];
extern const uint8_t icon_sun[72];
extern const uint8_t icon_cloud[72];
extern const uint8_t icon_rain[72];
extern const uint8_t icon_moon[72];
extern const uint8_t icon_wind[72];

#endif  // DISPLAY_ASSETS_H
