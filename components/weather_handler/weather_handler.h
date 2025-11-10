#ifndef WEATHER_HANDLER_H
#define WEATHER_HANDLER_H

#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @file weather_data.h
 * @brief Interface para coleta e parsing de dados meteorológicos do Open-Meteo.
 *
 * Este módulo realiza requisições HTTP usando o http_client.c e faz o parsing
 * do JSON retornado, preenchendo uma estrutura de dados com valores numéricos.
 */

/**
 * @brief Estrutura que representa os dados meteorológicos atuais.
 */
typedef struct {
    float temperature;    ///< Temperatura em °C
    float humidity;       ///< Umidade relativa do ar em %
    float precipitation;  ///< Precipitação em mm/h
    int weather_code;     ///< Código WMO do tempo atual
    bool is_day;          ///< Indica se é dia (1) ou noite (0)
} weather_data_t;

/**
 * @brief Obtém e analisa os dados meteorológicos atuais do Open-Meteo.
 *
 * @param latitude     Latitude em graus decimais.
 * @param longitude    Longitude em graus decimais.
 * @param out_data     Ponteiro para a estrutura que receberá os dados.
 *
 * @return
 *  - ESP_OK: sucesso.
 *  - ESP_FAIL: falha na requisição HTTP ou no parsing JSON.
 *  - ESP_ERR_INVALID_ARG: argumentos inválidos.
 */
esp_err_t weather_data_fetch(float latitude, float longitude, weather_data_t *out_data);

/**
 * @brief Converte o código WMO em uma descrição textual curta.
 *
 * @param code Código numérico WMO (ex: 0, 1, 45, 95...).
 * @return Ponteiro para string estática com a descrição correspondente.
 */
const char *weather_data_wmo_description(int code);

#ifdef __cplusplus
}
#endif

#endif  // WEATHER_HANDLER_H
