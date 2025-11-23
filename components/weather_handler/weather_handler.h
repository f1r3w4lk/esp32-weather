#ifndef WEATHER_HANDLER_H
#define WEATHER_HANDLER_H

#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @file weather_handler.h
 * @brief Module for retrieving and parsing weather data from Open-Meteo API.
 *
 * Performs HTTPS GET requests using @ref http_client and extracts relevant
 * fields from the JSON response, storing them in a weather_data_t structure.
 */

/**
 * @brief Structure representing current weather data from Open-Meteo.
 */
typedef struct {
    float temperature;    ///< Air temperature in °C
    float humidity;       ///< Relative humidity in %
    float precipitation;  ///< Precipitation rate in mm/h
    int weather_code;     ///< WMO numeric weather condition code
    bool is_day;          ///< True if daytime, false if nighttime
} weather_data_t;

/**
 * @brief Fetch current weather from Open-Meteo API and parse the result.
 *
 * Sends a HTTP request using the given latitude & longitude, and fills the
 * provided structure with the parsed weather parameters.
 *
 * @param latitude     Geographic latitude in decimal degrees.
 * @param longitude    Geographic longitude in decimal degrees.
 * @param out_data     Pointer to a structure where parsed weather data is stored.
 *
 * @return
 *  - ESP_OK                       Success
 *  - ESP_ERR_INVALID_ARG          Invalid parameters
 *  - ESP_FAIL                     Request or parsing failure
 */
esp_err_t weather_data_fetch(float latitude, float longitude, weather_data_t *out_data);

/**
 * @brief Convert a WMO weather code into a short textual description.
 *
 * @param code WMO weather code (e.g. 0, 1, 45, 95...)
 *
 * @return A constant string describing the condition (e.g. "Clear", "Rainy", etc.)
 */
const char *weather_data_wmo_description(int code);

#ifdef __cplusplus
}
#endif

#endif  // WEATHER_HANDLER_H
